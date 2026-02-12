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

// Project Includes
#include "CASort/CAAddBack.hpp"
#include "CASort/CACalibration.hpp"
#include "CASort/CAConfiguration.hpp"
#include "CASort/CACrosstalkCorrection.hpp"
#include "CASort/CAGainCorrection.hpp"
#include "CASort/CAHistograms.hpp"
#include "CASort/CAUtilities.hpp"
/* #endregion Includes */

/* #region DAQ Channel Key */
// Define which DAQ channels correspond to which detectors (C = Clover HPGe, S =
// Single Crystal HPGe, c = CeBr, l = LaBr) see elog for details

// clover_cross
#define C1E1 0
#define C1E2 1
#define C1E3 2
#define C1E4 3
#define C3E1 4
#define C3E2 5
#define C3E3 6
#define C3E4 7
#define C5E1 8
#define C5E2 9
#define C5E3 10
#define C5E4 11
#define C7E1 12
#define C7E2 13
#define C7E3 14
#define C7E4 15

// clover_back
#define B1E1 0
#define B1E2 1
#define B1E3 2
#define B1E4 3
#define B2E1 4
#define B2E2 5
#define B2E3 6
#define B2E4 7
#define B3E1 8
#define B3E2 9
#define B3E3 10
#define B3E4 11
#define B5E1 12
#define B5E2 13
#define B5E3 14
#define B5E4 15

// pos_sig
#define ZDEG 0
#define SB4E1 2
#define B4E1 4
#define B4E2 5
#define B4E3 6
#define B4E4 7

// cebr_all
#define cB 0
#define cC 1
#define cD 2
#define cF 3
#define cG 4
#define cH 5
#define cK 6
#define cO 7
#define cBJ 8
#define cBK 9
#define cBL 10
#define L3 11
#define MPAD 12

static const std::map<std::string, int> cross_channel_map = {
    {"C1E1", C1E1}, {"C1E2", C1E2}, {"C1E3", C1E3}, {"C1E4", C1E4}, {"C3E1", C3E1}, {"C3E2", C3E2}, {"C3E3", C3E3}, {"C3E4", C3E4}, {"C5E1", C5E1}, {"C5E2", C5E2}, {"C5E3", C5E3}, {"C5E4", C5E4}, {"C7E1", C7E1}, {"C7E2", C7E2}, {"C7E3", C7E3}, {"C7E4", C7E4}};
static const std::map<std::string, int> back_channel_map = {
    {"B1E1", B1E1}, {"B1E2", B1E2}, {"B1E3", B1E3}, {"B1E4", B1E4}, {"B2E1", B2E1}, {"B2E2", B2E2}, {"B2E3", B2E3}, {"B2E4", B2E4}, {"B3E1", B3E1}, {"B3E2", B3E2}, {"B3E3", B3E3}, {"B3E4", B3E4}, {"B5E1", B5E1}, {"B5E2", B5E2}, {"B5E3", B5E3}, {"B5E4", B5E4}};
static const std::map<std::string, int> possig_channel_map = {
    {"ZDEG", ZDEG}, {"SB4E1", SB4E1}, {"B4E1", B4E1}, {"B4E2", B4E2}, {"B4E3", B4E3}, {"B4E4", B4E4}};
static const std::map<std::string, int> cebr_channel_map = {
    {"cB", cB}, {"cC", cC}, {"cD", cD}, {"cF", cF}, {"cG", cG}, {"cH", cH}, {"cK", cK}, {"cO", cO}, {"cBJ", cBJ}, {"cBK", cBK}, {"cBL", cBL}, {"L3", L3}, {"MPAD", MPAD}};

/* #endregion DAQ Channel Key*/

// Main function
int main(int argc, char* argv[])
{
    /* #region Configuration Setup */

    auto args = CAUtilities::ParseArguments(argc, argv);

    std::cout << "=============== Welcome to TylerSort! ==================" << std::endl;
    CAUtilities::PrintConfiguration(args);

    // ROOT multithreading is initialized in CAConfiguration.hpp static initializer
    printf("[INFO] ROOT multithreading enabled with %d threads\n", ROOT::GetThreadPoolSize());

    /* #endregion Configuration Setup */

    /* #region Calibration Setup */

    // Gain match functions
    std::vector<std::function<double(double)>> cc_E_gmp, cb_E_gmp, ps_E_gmp, ce_T_gmp;
    auto gain_shift_data = CAGainCorrection::MakeCorrections(args.gainShiftDir, args.runNumber);

    // Cross-talk Correction Functions
    std::vector<std::function<std::array<double, 4>(std::array<double, 4>)>> cc_xtalk_corr, cb_xtalk_corr;

    // Calibraration functions
    std::vector<std::function<double(double)>> cc_E_cal, cb_E_cal, ps_E_cal, ce_E_cal;

#if PROCESS_CLOVER_CROSS
    for (int det : {1, 3, 5, 7})
    {
        for (int xtal = 1; xtal <= 4; xtal++)
        {
            std::string cal_filename = Form("%s/C%iE%i.cal_params.txt",
                                            args.calibrationDir.c_str(), det, xtal);
            cc_E_cal.push_back(CACalibration::MakeCalibration(cal_filename));
        }
    }
    try
    {
        cc_E_gmp = gain_shift_data.at(0);
        printf("[INFO] Clover Cross Gain Match and Calibration functions retrieved\n");
    }
    catch (const std::exception& e)
    {
        printf("[WARN] Clover Cross Gain Match functions not found, proceeding without gain matching\n");
        cc_E_gmp = std::vector<std::function<double(double)>>(16, [](double x)
                                                              { return x; });
    }
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
    for (int det : {1, 2, 3, 5})
    {
        for (int xtal = 1; xtal <= 4; xtal++)
        {
            std::string cal_filename = Form("%s/B%iE%i.cal_params.txt",
                                            args.calibrationDir.c_str(), det, xtal);
            cb_E_cal.push_back(CACalibration::MakeCalibration(cal_filename));
        }
    }
    try
    {
        cb_E_gmp = gain_shift_data.at(1);
        printf("[INFO] Clover Back Gain Match and Calibration functions retrieved\n");
    }
    catch (const std::exception& e)
    {
        printf("[WARN] Clover Back Gain Match functions not found, proceeding without gain matching\n");
        cb_E_gmp = std::vector<std::function<double(double)>>(16, [](double x)
                                                              { return x; });
    }
#endif // PROCESS_CLOVER_BACK

    /* #endregion Calibration Setup */

    /* #region Event Loop Setup*/

    auto infile = TFile::Open(args.runFileName.c_str());
    if (!infile || infile->IsZombie())
    {
        throw std::runtime_error(Form("[ERROR] Error opening input file: %s", args.runFileName.c_str()));
    }
    printf("[INFO] Opened file %s\n", args.runFileName.c_str());

    // Peak at the TTree to get the number of events
    TTree* tree;
    infile->GetObject("clover", tree);
    if (!tree)
    {
        throw std::runtime_error("[ERROR] Error opening TTree");
    }

    ULong64_t n_entries = tree->GetEntries();

    printf("[INFO] Opened TTree \"clover\" and counted %llu events\n", n_entries);

    delete tree;
    infile->Close();

    // Atomic counter for processed entries
    std::atomic<uint64_t> processedEntries(0);

    // Start the progress bar in a separate thread
    std::thread progressBarThread(CAUtilities::DisplayProgressBar, std::ref(processedEntries), n_entries);

    printf("[INFO] Processing events...\n");

    // Limit max tasks to reduce TBB work-stealing related race conditions
    // This ensures better alignment between TBB tasks and ROOT's TThreadedObject slots
    ROOT::TTreeProcessorMT::SetTasksPerWorkerHint(1);

    // Create a TTreeReader to read the TTree
    ROOT::TTreeProcessorMT EventProcessor(args.runFileName.c_str(), "clover");

    /* #endregion Event Loop Setup */

    // Fill Function
    auto fillHistograms = [&](TTreeReader& event_reader)
    {
    /* #region Set the branch addresses for the TTree */

#if PROCESS_CLOVER_CROSS
        TTreeReaderArray<double> cc_amp_val(event_reader, "clover_cross.amplitude");
        TTreeReaderArray<double> cc_cht_val(event_reader, "clover_cross.channel_time");
        TTreeReaderArray<double> cc_mdt_val(event_reader, "clover_cross.module_timestamp");
        TTreeReaderArray<double> cc_plu_val(event_reader, "clover_cross.pileup");
        TTreeReaderArray<double> cc_trt_val(event_reader, "clover_cross.trigger_time");
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
        TTreeReaderArray<double> cb_amp_val(event_reader, "clover_back.amplitude");
        TTreeReaderArray<double> cb_cht_val(event_reader, "clover_back.channel_time");
        TTreeReaderArray<double> cb_mdt_val(event_reader, "clover_back.module_timestamp");
        TTreeReaderArray<double> cb_plu_val(event_reader, "clover_back.pileup");
        TTreeReaderArray<double> cb_trt_val(event_reader, "clover_back.trigger_time");
#endif // PROCESS_CLOVER_BACK

#if PROCESS_POS_SIG
        TTreeReaderArray<double> ps_amp_val(event_reader, "pos_sig.amplitude");
        TTreeReaderArray<double> ps_cht_val(event_reader, "pos_sig.channel_time");
        TTreeReaderArray<double> ps_mdt_val(event_reader, "pos_sig.module_timestamp");
        TTreeReaderArray<double> ps_plu_val(event_reader, "pos_sig.pileup");
        TTreeReaderArray<double> ps_trt_val(event_reader, "pos_sig.trigger_time");
#endif // PROCESS_POS_SIG

#if PROCESS_CEBR_ALL
        TTreeReaderArray<double> ce_inl_val(event_reader, "cebr_all.integration_long");
        TTreeReaderArray<double> ce_cht_val(event_reader, "cebr_all.channel_time");
        TTreeReaderArray<double> ce_mdt_val(event_reader, "cebr_all.module_timestamp");
        TTreeReaderArray<double> ce_ins_val(event_reader, "cebr_all.integration_short");
        TTreeReaderArray<double> ce_trt_val(event_reader, "cebr_all.trigger_time");
#endif // PROCESS_CEBR_ALL

        /* #endregion */

        /* #region Get Histogram pointers*/

        // Use histograms defined in CAHistograms.hpp

#if PROCESS_CLOVER_CROSS
        auto cc_amp = CAHistograms::cc_amp.GetThreadLocalPtr();
        auto cc_cht = CAHistograms::cc_cht.GetThreadLocalPtr();
        auto cc_plu = CAHistograms::cc_plu.GetThreadLocalPtr();
        auto cc_trt = CAHistograms::cc_trt.GetThreadLocalPtr();
        auto cc_mdt = CAHistograms::cc_mdt.GetThreadLocalPtr();
        auto cc_xtE = CAHistograms::cc_xtE.GetThreadLocalPtr();
        auto cc_sum = CAHistograms::cc_sum.GetThreadLocalPtr();
        auto cc_abE = CAHistograms::cc_abE.GetThreadLocalPtr();
        auto cc_abM = CAHistograms::cc_abM.GetThreadLocalPtr();
        auto c1_xtk = std::array<std::shared_ptr<TH2D>, 6>{
            CAHistograms::c1_xtk[0].GetThreadLocalPtr(),
            CAHistograms::c1_xtk[1].GetThreadLocalPtr(),
            CAHistograms::c1_xtk[2].GetThreadLocalPtr(),
            CAHistograms::c1_xtk[3].GetThreadLocalPtr(),
            CAHistograms::c1_xtk[4].GetThreadLocalPtr(),
            CAHistograms::c1_xtk[5].GetThreadLocalPtr(),
        };
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
        auto cb_amp = CAHistograms::cb_amp.GetThreadLocalPtr();
        auto cb_cht = CAHistograms::cb_cht.GetThreadLocalPtr();
        auto cb_plu = CAHistograms::cb_plu.GetThreadLocalPtr();
        auto cb_trt = CAHistograms::cb_trt.GetThreadLocalPtr();
        auto cb_mdt = CAHistograms::cb_mdt.GetThreadLocalPtr();
        auto cb_xtE = CAHistograms::cb_xtE.GetThreadLocalPtr();
        auto cb_sum = CAHistograms::cb_sum.GetThreadLocalPtr();
        auto cb_abE = CAHistograms::cb_abE.GetThreadLocalPtr();
        auto cb_abM = CAHistograms::cb_abM.GetThreadLocalPtr();
#endif // PROCESS_CLOVER_BACK

#if PROCESS_POS_SIG

#endif // PROCESS_POS_SIG

#if PROCESS_CEBR_ALL

#endif // PROCESS_CEBR_ALL

        /* #endregion */

        // Loop over the entries in the tree
        while (event_reader.Next())
        {

// Module Time
#if PROCESS_CLOVER_CROSS
            if (cc_mdt_val.GetSize() > 0)
                cc_mdt->Fill(cc_mdt_val.At(0) * CAHistograms::kNsPerBin);
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
            if (cb_mdt_val.GetSize() > 0)
                cb_mdt->Fill(cb_mdt_val.At(0) * CAHistograms::kNsPerBin);
#endif // PROCESS_CLOVER_BACK

// Trigger Times
#if PROCESS_CLOVER_CROSS
            if (cc_trt_val.GetSize() > 1)
            {
                cc_trt->Fill(cc_trt_val.At(0) * CAHistograms::kNsPerBin, 0);
                cc_trt->Fill(cc_trt_val.At(1) * CAHistograms::kNsPerBin, 1);
            }
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
            if (cb_trt_val.GetSize() > 1)
            {
                cb_trt->Fill(cb_trt_val.At(0) * CAHistograms::kNsPerBin, 0);
                cb_trt->Fill(cb_trt_val.At(1) * CAHistograms::kNsPerBin, 1);
            }
#endif // PROCESS_CLOVER_BACK

            // Main Loop

            // Detector Loop
            for (size_t det = 0; det < 4; det++)
            {
                std::array<double, 4> cc_xtal_E = {NAN, NAN, NAN, NAN}, cb_xtal_E = {NAN, NAN, NAN, NAN};
                std::array<double, 4> cc_xtal_T = {NAN, NAN, NAN, NAN}, cb_xtal_T = {NAN, NAN, NAN, NAN};

                // Crystal Loop
                for (size_t xtal = 0; xtal < 4; xtal++)
                {
                    auto ch = det * 4 + xtal; // Channel number 0-15

                    // Raw Histograms
#if PROCESS_CLOVER_CROSS
                    // Bounds check to prevent memory corruption
                    if (ch < cc_amp_val.GetSize())
                    {
                        cc_amp->Fill(cc_amp_val[ch], ch);
                        cc_cht->Fill(cc_cht_val[ch], ch);
                        cc_plu->Fill(cc_plu_val[ch], ch);
                    }
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
                    // Bounds check to prevent memory corruption
                    if (ch < cb_amp_val.GetSize())
                    {
                        cb_amp->Fill(cb_amp_val[ch], ch);
                        cb_cht->Fill(cb_cht_val[ch], ch);
                        cb_plu->Fill(cb_plu_val[ch], ch);
                    }
#endif // PROCESS_CLOVER_BACK

// Calibrated Histograms
#if PROCESS_CLOVER_CROSS
                    if (ch < cc_amp_val.GetSize() &&
                        !std::isnan(cc_amp_val[ch]) &&
                        !std::isnan(cc_cht_val[ch]))
                    {
                        // std::cout << "Channel: " << ch << ", ";
                        double energy = cc_E_cal[ch](cc_E_gmp[ch](cc_amp_val[ch])); // Gain-match, then calibrate
                        double cht = cc_cht_val[ch] * CAHistograms::kNsPerBin;
                        cc_xtE->Fill(energy, ch);
                        cc_cht->Fill(cht, ch);
                        cc_sum->Fill(energy, det); // ch / 4 is the detector number
                        cc_xtal_E[xtal] = energy;
                        cc_xtal_T[xtal] = cht;
                    }
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
                    if (ch < cb_amp_val.GetSize() &&
                        !std::isnan(cb_amp_val[ch]) &&
                        !std::isnan(cb_cht_val[ch]))
                    {
                        // std::cout << "Channel: " << ch << ", ";
                        double energy = cb_E_cal[ch](cb_E_gmp[ch](cb_amp_val[ch])); // Gain-match, then calibrate
                        double cht = cb_cht_val[ch] * CAHistograms::kNsPerBin;
                        cb_xtE->Fill(energy, ch);
                        cb_cht->Fill(cht, ch);
                        cb_sum->Fill(energy, det); // ch / 4 is the detector number
                        cb_xtal_E[xtal] = energy;
                        cb_xtal_T[xtal] = cht;
                    }
#endif // PROCESS_CLOVER_BACK
                }

// Add-Back Histograms
#if PROCESS_CLOVER_CROSS
                cc_abE->Fill(CAAddBack::GetAddBackEnergy(cc_xtal_E, cc_xtal_T), det);
                unsigned int mult = std::count_if(cc_xtal_E.begin(), cc_xtal_E.end(), [](double e)
                                                  { return !std::isnan(e); });
                cc_abM->Fill(mult, det);
                if (mult == 2 && det == 0)
                {
                    CACrosstalkCorrection::FillXTalkHistograms(c1_xtk, cc_xtal_E, cc_xtal_T);
                }
#endif // PROCESS_CLOVER_CROSS

#if PROCESS_CLOVER_BACK
                cb_abE->Fill(CAAddBack::GetAddBackEnergy(cb_xtal_E, cb_xtal_T), det);
                mult = std::count_if(cb_xtal_E.begin(), cb_xtal_E.end(), [](double e)
                                     { return !std::isnan(e); });
                cb_abM->Fill(mult, det);
#endif // PROCESS_CLOVER_BACK
            }

#if PROCESS_POS_SIG

#endif // PROCESS_POS_SIG

#if PROCESS_CEBR_ALL

#endif // PROCESS_CEBR_ALL

            processedEntries++;
        }
    };

    // Loop over the entries in the TTree and fill the histograms appropriately
    TStopwatch timer;
    timer.Start();
    EventProcessor.Process(fillHistograms);
    timer.Stop();

    progressBarThread.join();

    printf("[INFO] Processed events in %.2f seconds (%.2f events/second)\n",
           timer.RealTime(),
           static_cast<double>(processedEntries) / timer.RealTime());

    // Save the histograms to a new ROOT file
    TFile* outfile = new TFile(args.outputFileName.c_str(), "RECREATE");
    if (!outfile || outfile->IsZombie())
    {
        throw std::runtime_error(Form("[ERROR] Error creating output file: %s", args.outputFileName.c_str()));
    }

/* #region Write Histograms */

// Use histograms defined in CAHistograms.hpp

// Clover Cross Histograms
#if PROCESS_CLOVER_CROSS
    auto cc_dir = outfile->mkdir("clover_cross");
    cc_dir->cd();
    CAHistograms::cc_amp.Write();
    CAHistograms::cc_cht.Write();
    CAHistograms::cc_plu.Write();
    CAHistograms::cc_trt.Write();
    CAHistograms::cc_mdt.Write();
    CAHistograms::cc_xtE.Write();
    CAHistograms::cc_sum.Write();
    CAHistograms::cc_abE.Write();
    CAHistograms::cc_abM.Write();
    for (int i = 0; i < 6; i++)
    {
        CAHistograms::c1_xtk[i].Write();
    }
    outfile->cd();
#endif // PROCESS_CLOVER_CROSS

// Clover Back Histograms
#if PROCESS_CLOVER_BACK
    auto cb_dir = outfile->mkdir("clover_back");
    cb_dir->cd();
    CAHistograms::cb_amp.Write();
    CAHistograms::cb_cht.Write();
    CAHistograms::cb_plu.Write();
    CAHistograms::cb_trt.Write();
    CAHistograms::cb_mdt.Write();
    CAHistograms::cb_xtE.Write();
    CAHistograms::cb_sum.Write();
    CAHistograms::cb_abE.Write();
    CAHistograms::cb_abM.Write();
    outfile->cd();
#endif // PROCESS_CLOVER_BACK

// Positive Signal Histograms
#if PROCESS_POS_SIG

#endif // PROCESS_POS_SIG

    // CeBr All Histograms
#if PROCESS_CEBR_ALL

#endif // PROCESS_CEBR_ALL

    /* #endregion */

    printf("Saved histograms to file: %s\n", args.outputFileName.c_str());

    outfile->Close();
    delete outfile;

    printf("Done!\n");

    return 0;
}