/* #region Includes */

// C++ Includes
#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// ROOT Includes
#include <ROOT/TThreadedObject.hxx>
#include <ROOT/TTreeProcessorMT.hxx>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TSpline.h>
#include <TStopwatch.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>

// CASort Includes
#include <CASort/CAAddBack.hpp>
#include <CASort/CACalibration.hpp>
#include <CASort/CAConfiguration.hpp>
#include <CASort/CACrosstalkCorrection.hpp>
#include <CASort/CAGainCorrection.hpp>
#include <CASort/CAUtilities.hpp>

// Project Includes
#include "Histograms.hpp"

/* #endregion Includes */

// Main function
int main(int argc, char* argv[])
{

    /* #region Configuration Setup */

    auto args = CAUtilities::ParseArguments(argc, argv);

    // Configure ROOT threading from CLI before creating threaded histograms.
    if (!ROOT::IsImplicitMTEnabled()) ROOT::EnableImplicitMT(args.threadCount);
    ROOT::EnableThreadSafety();

    std::cout << "=============== Welcome to TylerSort! ==================" << std::endl;
    CAUtilities::PrintConfiguration(args);

    Histograms::Initialize(args.mode);

    printf("[INFO] ROOT multithreading enabled with %d threads\n", ROOT::GetThreadPoolSize());

    /* #endregion Configuration Setup */

    /* #region Calibration Setup */

    // Declare calibration variables (populated only for cal/xtcorr modes)
    std::vector<std::function<double(double)>>                               ccGainMatch, cbGainMatch, psGainMatch, ceGainMatch;
    std::vector<std::function<double(double)>>                               ccECalibrate, cbECalibrate, psECalibrate, ceECalibrate;
    std::vector<std::function<std::array<double, 4>(std::array<double, 4>)>> xTalkCorrection;

    if (args.mode == "cal" || args.mode == "xtcorr" || args.mode == "coin")
    {
        auto funcsGainMatch = CAGainCorrection::MakeCorrections(args.gainShiftFile);

        // Cross-talk Correction Functions
        try
        {
            xTalkCorrection = CACrosstalkCorrection::MakeCorrections(args.xtalkFile);
            printf("[INFO] Crosstalk correction functions retrieved\n");
        } catch (const std::exception& e)
        {
            printf("[WARN] Crosstalk correction functions not found, proceeding without crosstalk correction\n");
            xTalkCorrection = std::vector<std::function<std::array<double, 4>(std::array<double, 4>)>>(4, [](std::array<double, 4> x) { return x; });
        }

        // Calibration functions
        for (int det : {1, 3, 5, 7})
        {
            for (int xtal = 1; xtal <= 4; xtal++)
            {
                std::string calFileName = Form("%s/C%iE%i.cal_params.txt", args.calibrationDir.c_str(), det, xtal);
                ccECalibrate.push_back(CACalibration::MakeCalibration(calFileName));
            }
        }
        try
        {
            ccGainMatch = funcsGainMatch.at(0);
            printf("[INFO] Clover Cross Gain Match and Calibration functions retrieved\n");
        } catch (const std::exception& e)
        {
            printf("[WARN] Clover Cross Gain Match functions not found, proceeding without gain matching\n");
            ccGainMatch = std::vector<std::function<double(double)>>(16, [](double x) { return x; });
        }

        for (int det : {1, 2, 3, 5})
        {
            for (int xtal = 1; xtal <= 4; xtal++)
            {
                std::string calFileName = Form("%s/B%iE%i.cal_params.txt", args.calibrationDir.c_str(), det, xtal);
                cbECalibrate.push_back(CACalibration::MakeCalibration(calFileName));
            }
        }
        try
        {
            cbGainMatch = funcsGainMatch.at(1);
            printf("[INFO] Clover Back Gain Match and Calibration functions retrieved\n");
        } catch (const std::exception& e)
        {
            printf("[WARN] Clover Back Gain Match functions not found, proceeding without gain matching\n");
            cbGainMatch = std::vector<std::function<double(double)>>(16, [](double x) { return x; });
        }
    }

    /* #endregion Calibration Setup */

    /* #region Event Loop Setup*/

    auto inputFile = TFile::Open(args.runFileName.c_str());
    if (!inputFile || inputFile->IsZombie()) { throw std::runtime_error(Form("[ERROR] Error opening input file: %s", args.runFileName.c_str())); }
    printf("[INFO] Opened file %s\n", args.runFileName.c_str());

    // Find the first TTree in the file, regardless of name
    std::string treeName;
    for (auto* key : *inputFile->GetListOfKeys())
    {
        if (std::string(static_cast<TKey*>(key)->GetClassName()) == "TTree")
        {
            treeName = key->GetName();
            break;
        }
    }
    if (treeName.empty()) throw std::runtime_error("[ERROR] No TTree found in input file");

    TTree* tree = inputFile->Get<TTree>(treeName.c_str());
    if (!tree) throw std::runtime_error("[ERROR] Error opening TTree '" + treeName + "'");

    ULong64_t nEntries = tree->GetEntries();

    printf("[INFO] Opened TTree \"%s\" and counted %llu events\n", treeName.c_str(), nEntries);

    delete tree;
    inputFile->Close();

    // Atomic counter for processed entries
    std::atomic<uint64_t> processedEntries(0);

    // Start the progress bar in a separate thread
    std::thread progressBarThread(CAUtilities::DisplayProgressBar, std::ref(processedEntries), nEntries);

    printf("[INFO] Processing events...\n");

    // Limit max tasks to reduce TBB work-stealing related race conditions
    // This ensures better alignment between TBB tasks and ROOT's TThreadedObject slots
    ROOT::TTreeProcessorMT::SetTasksPerWorkerHint(1);

    // Create a TTreeReader to read the TTree
    ROOT::TTreeProcessorMT EventProcessor(args.runFileName.c_str(), treeName.c_str());

    /* #endregion Event Loop Setup */

    // Fill Function
    auto fillHistograms = [&](TTreeReader& eventReader) {
        /* #region Set the branch addresses for the TTree */

        // Clover Cross
        TTreeReaderArray<double> cc_amp_val(eventReader, "clover_cross.amplitude");
        TTreeReaderArray<double> cc_cht_val(eventReader, "clover_cross.channel_time");
        TTreeReaderArray<double> cc_mdt_val(eventReader, "clover_cross.module_timestamp");
        TTreeReaderArray<double> cc_plu_val(eventReader, "clover_cross.pileup");
        TTreeReaderArray<double> cc_trt_val(eventReader, "clover_cross.trigger_time");

        // Clover Back
        TTreeReaderArray<double> cb_amp_val(eventReader, "clover_back.amplitude");
        TTreeReaderArray<double> cb_cht_val(eventReader, "clover_back.channel_time");
        TTreeReaderArray<double> cb_mdt_val(eventReader, "clover_back.module_timestamp");
        TTreeReaderArray<double> cb_plu_val(eventReader, "clover_back.pileup");
        TTreeReaderArray<double> cb_trt_val(eventReader, "clover_back.trigger_time");

#if PROCESS_POS_SIG
        // Pos Sig
        TTreeReaderArray<double> ps_amp_val(eventReader, "pos_sig.amplitude");
        TTreeReaderArray<double> ps_cht_val(eventReader, "pos_sig.channel_time");
        TTreeReaderArray<double> ps_mdt_val(eventReader, "pos_sig.module_timestamp");
        TTreeReaderArray<double> ps_plu_val(eventReader, "pos_sig.pileup");
        TTreeReaderArray<double> ps_trt_val(eventReader, "pos_sig.trigger_time");
#endif // PROCESS_POS_SIG

        // CEBR All
        TTreeReaderArray<double> ce_inL_val(eventReader, "cebr_all.integration_long");
        TTreeReaderArray<double> ce_cht_val(eventReader, "cebr_all.channel_time");
        TTreeReaderArray<double> ce_mdt_val(eventReader, "cebr_all.module_timestamp");
        TTreeReaderArray<double> ce_ins_val(eventReader, "cebr_all.integration_short");
        TTreeReaderArray<double> ce_trt_val(eventReader, "cebr_all.trigger_time");

        /* #endregion */

        /* #region Get Histogram pointers*/

        const bool isRaw            = (args.mode == "raw");
        const bool isCal            = (args.mode == "cal" || args.mode == "xtcorr");
        const bool isXtcorr         = (args.mode == "xtcorr");
        const bool isCoin           = (args.mode == "coin");
        const bool needsCalibration = (isCal || isCoin); // coin mode needs calibrated energies to populate hpgeHits

        // Clover Cross thread-local histogram pointers
        std::shared_ptr<TH2D>                cc_amp, cc_cht, cc_plu, cc_trt;
        std::shared_ptr<TH1D>                cc_mdt;
        std::shared_ptr<TH2D>                cc_chE, cc_sum, cc_abE;
        std::shared_ptr<TH1D>                cc_abM;
        std::array<std::shared_ptr<TH2D>, 6> c1_xtk{}, c3_xtk{}, c5_xtk{}, c7_xtk{};

        // Clover Back thread-local histogram pointers
        std::shared_ptr<TH2D>                cb_amp, cb_cht, cb_plu, cb_trt;
        std::shared_ptr<TH1D>                cb_mdt;
        std::shared_ptr<TH2D>                cb_chE, cb_sum, cb_abE;
        std::shared_ptr<TH1D>                cb_abM;
        std::array<std::shared_ptr<TH2D>, 6> b1_xtk{}, b2_xtk{}, b3_xtk{}, b5_xtk{};

        // CeBr thread-local histogram pointers
        std::shared_ptr<TH2D> ce_inl, ce_ins, ce_cht, ce_trt;
        std::shared_ptr<TH1D> ce_mdt;

        // Coincidence thread-local histogram pointers
        std::shared_ptr<TH3I> gege_ggT;
        std::shared_ptr<TH1I> gege_ggM;

        if (isRaw)
        {
            // Clover Cross
            cc_amp = Histograms::cc_amp->GetThreadLocalPtr();
            cc_cht = Histograms::cc_cht->GetThreadLocalPtr();
            // cc_plu = Histograms::cc_plu->GetThreadLocalPtr();
            // cc_mdt = Histograms::cc_mdt->GetThreadLocalPtr();
            // cc_trt = Histograms::cc_trt->GetThreadLocalPtr();
            //  Clover Back
            cb_amp = Histograms::cb_amp->GetThreadLocalPtr();
            cb_cht = Histograms::cb_cht->GetThreadLocalPtr();
            // cb_plu = Histograms::cb_plu->GetThreadLocalPtr();
            // cb_mdt = Histograms::cb_mdt->GetThreadLocalPtr();
            // cb_trt = Histograms::cb_trt->GetThreadLocalPtr();
            //  CeBr Detectors
            ce_inl = Histograms::ce_inl->GetThreadLocalPtr();
            // ce_ins = Histograms::ce_ins->GetThreadLocalPtr();
            ce_cht = Histograms::ce_cht->GetThreadLocalPtr();
            // ce_mdt = Histograms::ce_mdt->GetThreadLocalPtr();
            //  ce_trt = Histograms::ce_trt->GetThreadLocalPtr();
        }
        if (isCal)
        {
            cc_chE = Histograms::cc_chE->GetThreadLocalPtr();
            cc_sum = Histograms::cc_sum->GetThreadLocalPtr();
            cc_abE = Histograms::cc_abE->GetThreadLocalPtr();
            cb_chE = Histograms::cb_chE->GetThreadLocalPtr();
            cb_sum = Histograms::cb_sum->GetThreadLocalPtr();
            cb_abE = Histograms::cb_abE->GetThreadLocalPtr();
        }
        if (isXtcorr)
        {
            cc_abM = Histograms::cc_abM->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) c1_xtk[i] = Histograms::c1_xtk[i]->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) c3_xtk[i] = Histograms::c3_xtk[i]->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) c5_xtk[i] = Histograms::c5_xtk[i]->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) c7_xtk[i] = Histograms::c7_xtk[i]->GetThreadLocalPtr();
            cb_abM = Histograms::cb_abM->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) b1_xtk[i] = Histograms::b1_xtk[i]->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) b2_xtk[i] = Histograms::b2_xtk[i]->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) b3_xtk[i] = Histograms::b3_xtk[i]->GetThreadLocalPtr();
            for (int i = 0; i < 6; i++) b5_xtk[i] = Histograms::b5_xtk[i]->GetThreadLocalPtr();
        }
        if (isCoin)
        {
            gege_ggT = Histograms::gege_ggT->GetThreadLocalPtr();
            gege_ggM = Histograms::gege_ggM->GetThreadLocalPtr();
        }

        std::array<std::array<std::shared_ptr<TH2D>, 6>, 4> cc_xtk = {c1_xtk, c3_xtk, c5_xtk, c7_xtk};
        std::array<std::array<std::shared_ptr<TH2D>, 6>, 4> cb_xtk = {b1_xtk, b2_xtk, b3_xtk, b5_xtk};

        /* #endregion */

        // Loop over the entries in the tree
        while (eventReader.Next())
        {
            if (isRaw)
            {
                // Module Time
                // cc_mdt->Fill(cc_mdt_val[0] * Histograms::kNsPerBin);
                // cb_mdt->Fill(cb_mdt_val[0] * Histograms::kNsPerBin);
                // ce_mdt->Fill(ce_mdt_val[0] * Histograms::kNsPerBin);
                // Trigger Times
                // cc_trt->Fill(cc_trt_val[0] * Histograms::kNsPerBin, 0);
                // cc_trt->Fill(cc_trt_val[1] * Histograms::kNsPerBin, 1);
                // cb_trt->Fill(cb_trt_val[0] * Histograms::kNsPerBin, 0);
                // cb_trt->Fill(cb_trt_val[1] * Histograms::kNsPerBin, 1);
                // ce_trt->Fill(ce_trt_val[0] * Histograms::kNsPerBin, 0);
                // ce_trt->Fill(ce_trt_val[1] * Histograms::kNsPerBin, 1);
            }

            std::vector<std::pair<double, double>> hpgeHits; // Vector to store HPGe hits for coincidence analysis
            // std::vector<std::pair<double, double>> cebrHits;   // Vector to store CeBr hits for coincidence analysis

            // Detector Loop
            for (size_t det = 0; det < 4; det++)
            {
                std::array<double, 4> cc_xtal_E = {NAN, NAN, NAN, NAN}, cb_xtal_E = {NAN, NAN, NAN, NAN};
                std::array<double, 4> cc_xtal_T = {NAN, NAN, NAN, NAN}, cb_xtal_T = {NAN, NAN, NAN, NAN};

                // Crystal Loop
                for (size_t xtal = 0; xtal < 4; xtal++)
                {
                    auto ch = det * 4 + xtal; // Channel number 0-15

                    if (isRaw)
                    {
                        // Clover Cross
                        cc_amp->Fill(cc_amp_val[ch], ch);
                        cc_cht->Fill(cc_cht_val[ch] * Histograms::kNsPerBin, ch);
                        // cc_plu->Fill(cc_plu_val[ch], ch);
                        //  Clover Back
                        cb_amp->Fill(cb_amp_val[ch], ch);
                        cb_cht->Fill(cb_cht_val[ch] * Histograms::kNsPerBin, ch);
                        // cb_plu->Fill(cb_plu_val[ch], ch);
                        //  CeBr Detectors
                        ce_inl->Fill(ce_inL_val[ch], ch);
                        // ce_ins->Fill(ce_ins_val[ch], ch);
                        ce_cht->Fill(ce_cht_val[ch] * Histograms::kNsPerBin, ch);
                    }

                    if (needsCalibration && !std::isnan(cc_amp_val[ch]) && !std::isnan(cc_cht_val[ch]))
                    {
                        double energy   = ccECalibrate[ch](ccGainMatch[ch](cc_amp_val[ch]));
                        double cht      = cc_cht_val[ch] * Histograms::kNsPerBin;
                        cc_xtal_E[xtal] = energy;
                        cc_xtal_T[xtal] = cht;
                        if (isCal)
                        {
                            cc_chE->Fill(energy, ch);
                            cc_sum->Fill(energy, det);
                        }
                    }

                    if (needsCalibration && !std::isnan(cb_amp_val[ch]) && !std::isnan(cb_cht_val[ch]))
                    {
                        double energy   = cbECalibrate[ch](cbGainMatch[ch](cb_amp_val[ch]));
                        double cht      = cb_cht_val[ch] * Histograms::kNsPerBin;
                        cb_xtal_E[xtal] = energy;
                        cb_xtal_T[xtal] = cht;
                        if (isCal)
                        {
                            cb_chE->Fill(energy, ch);
                            cb_sum->Fill(energy, det);
                        }
                    }
                } // End Crystal Loop

                // Clover Cross Add-Back
                if (needsCalibration && std::any_of(cc_xtal_E.begin(), cc_xtal_E.end(), [](double x) { return x > CAAddBack::kAddBackThreshold; }))
                {
                    if (isXtcorr)
                    {
                        unsigned int cc_mult = std::count_if(cc_xtal_E.begin(), cc_xtal_E.end(), [](double x) { return !std::isnan(x); });
                        cc_abM->Fill(cc_mult, det);
                        if (cc_mult == 2) CACrosstalkCorrection::FillXTalkHistograms(cc_xtk[det], cc_xtal_E, cc_xtal_T);
                    }
                    auto energies_corr = xTalkCorrection[4 + det](cc_xtal_E);
                    auto ab_hit        = CAAddBack::GetAddBackHit(energies_corr, cc_xtal_T);
                    hpgeHits.push_back(ab_hit);
                    if (isCal) cc_abE->Fill(ab_hit.first, det);
                }

                // Clover Back Add-Back
                if (needsCalibration && std::any_of(cb_xtal_E.begin(), cb_xtal_E.end(), [](double x) { return x > CAAddBack::kAddBackThreshold; }))
                {
                    if (isXtcorr)
                    {
                        unsigned int cb_mult = std::count_if(cb_xtal_E.begin(), cb_xtal_E.end(), [](double x) { return !std::isnan(x); });
                        cb_abM->Fill(cb_mult, det);
                        if (cb_mult == 2) CACrosstalkCorrection::FillXTalkHistograms(cb_xtk[det], cb_xtal_E, cb_xtal_T);
                    }
                    auto energies_corr = xTalkCorrection[det](cb_xtal_E);
                    auto ab_hit        = CAAddBack::GetAddBackHit(energies_corr, cb_xtal_T);
                    hpgeHits.push_back(ab_hit);
                    if (isCal) cb_abE->Fill(ab_hit.first, det);
                }

            } // End Detector Loop

            // Coincidence Analysis
            if (isCoin)
            {
                // HPGe-HPGe Coincidence
                for (size_t i = 0; i < hpgeHits.size(); i++)
                {
                    for (size_t j = i + 1; j < hpgeHits.size(); j++)
                    {
                        double energy1  = hpgeHits[i].first;
                        double energy2  = hpgeHits[j].first;
                        double timeDiff = hpgeHits[i].second - hpgeHits[j].second;

                        gege_ggT->Fill(energy1, energy2, timeDiff);
                        gege_ggT->Fill(energy2, energy1, timeDiff);
                    }
                }
                gege_ggM->Fill(hpgeHits.size());
                // printf("[DEBUG] Processed event with %zu HPGe hits\n", hpgeHits.size());
            }

            processedEntries++;
        } // End Event Loop
    };

    // Loop over the entries in the TTree and fill the histograms appropriately
    TStopwatch timer;
    timer.Start();
    EventProcessor.Process(fillHistograms);
    timer.Stop();

    progressBarThread.join();

    printf("[INFO] Processed events in %.2f seconds (%.2f events/second)\n", timer.RealTime(), static_cast<double>(processedEntries) / timer.RealTime());

    // Save the histograms to a new ROOT file
    TFile* outfile = new TFile(args.outputFileName.c_str(), "RECREATE");
    if (!outfile || outfile->IsZombie()) { throw std::runtime_error(Form("[ERROR] Error creating output file: %s", args.outputFileName.c_str())); }

    /* #region Write Histograms */

    // Clover Cross Histograms
    auto cc_dir = outfile->mkdir("clover_cross");
    cc_dir->cd();
    if (args.mode == "raw")
    {
        Histograms::cc_amp->Write();
        Histograms::cc_cht->Write();
        // Histograms::cc_plu->Write();
        // Histograms::cc_trt->Write();
        Histograms::cc_mdt->Write();
    }
    if (args.mode == "cal" || args.mode == "xtcorr")
    {
        Histograms::cc_chE->Write();
        Histograms::cc_sum->Write();
        Histograms::cc_abE->Write();
    }
    if (args.mode == "xtcorr")
    {
        Histograms::cc_abM->Write();
        for (int i = 0; i < 6; i++)
        {
            Histograms::c1_xtk[i]->Write();
            Histograms::c3_xtk[i]->Write();
            Histograms::c5_xtk[i]->Write();
            Histograms::c7_xtk[i]->Write();
        }
    }

    // Clover Back Histograms
    auto cb_dir = outfile->mkdir("clover_back");
    cb_dir->cd();
    if (args.mode == "raw")
    {
        Histograms::cb_amp->Write();
        Histograms::cb_cht->Write();
        // Histograms::cb_plu->Write();
        // Histograms::cb_trt->Write();
        Histograms::cb_mdt->Write();
    }
    if (args.mode == "cal" || args.mode == "xtcorr")
    {
        Histograms::cb_chE->Write();
        Histograms::cb_sum->Write();
        Histograms::cb_abE->Write();
    }
    if (args.mode == "xtcorr")
    {
        Histograms::cb_abM->Write();
        for (int i = 0; i < 6; i++)
        {
            Histograms::b1_xtk[i]->Write();
            Histograms::b2_xtk[i]->Write();
            Histograms::b3_xtk[i]->Write();
            Histograms::b5_xtk[i]->Write();
        }
    }

    // CeBr Histograms
    auto ce_dir = outfile->mkdir("cebr_all");
    ce_dir->cd();
    if (args.mode == "raw")
    {
        Histograms::ce_inl->Write();
        Histograms::ce_ins->Write();
        Histograms::ce_cht->Write();
        // Histograms::ce_trt->Write();
        Histograms::ce_mdt->Write();
    }
    if (args.mode == "cal" || args.mode == "xtcorr") { Histograms::ce_chE->Write(); }

    // Coincidence Histograms
    if (args.mode == "coin")
    {
        auto coin_dir = outfile->mkdir("coin");
        coin_dir->cd();
        Histograms::gege_ggT->Write();
        Histograms::gege_ggM->Write();
        // Histograms::cege_ggT->Write();
        // Histograms::cege_ggM->Write();
        // Histograms::cece_ggT->Write();
        // Histograms::cece_ggM->Write();
    }
    outfile->cd();

    /* #endregion */

    printf("[INFO] Saved histograms to file: %s\n", args.outputFileName.c_str());

    outfile->Close();
    delete outfile;

    printf("[INFO] Done!\n");

    return 0;
}