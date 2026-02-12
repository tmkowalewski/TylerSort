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
// #include "CASort/CAAddBack.hpp"
// #include "CASort/CACrosstalkCorrection.hpp"
#include "CASort/CAUtilities.hpp"

/* #endregion Includes */

// Main function
int main(int argc, char* argv[])
{

    constexpr size_t kDigitizerChannels = 16;
    constexpr size_t kDigitizerBins = 2 << 16;
    constexpr double kNsPerBin = 0.098;
    const unsigned int kThreads = std::thread::hardware_concurrency();
    constexpr double kEnergyPerBin = 0.25;
    constexpr double kMaxEnergy = 10000;
    constexpr double kXTalkEnergyPerBin = 1;
    constexpr double kXTalkMaxEnergy = 6000;

    // Enable implicit multithreading BEFORE creating TThreadedObject instances
    ROOT::EnableImplicitMT(kThreads);
    printf("[INFO] Enabled implicit MT with %d threads\n", kThreads);

    auto cc_amp = ROOT::TThreadedObject<TH2D>("cc_amp", "Clover Cross Amplitude (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
    auto cc_cht = ROOT::TThreadedObject<TH2D>("cc_cht", "Clover Cross Channel Time (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins * kNsPerBin, kDigitizerChannels, 0, kDigitizerChannels);
    auto cc_plu = ROOT::TThreadedObject<TH2D>("cc_plu", "Clover Cross Pile-Up;Pile-Up Multiplicity;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
    auto cc_mdt = ROOT::TThreadedObject<TH1D>("cc_mdt", "Clover Cross Module Time;Time (ns);Counts/Bin", kDigitizerBins, 0, kDigitizerBins * kNsPerBin);
    auto cc_trt = ROOT::TThreadedObject<TH2D>("cc_trt", "Clover Cross Trigger Time;Time (ns);Trigger ID;Counts/Bin", kDigitizerBins, 0, kDigitizerBins * kNsPerBin, 2, 0, 2);

    // // Calibrated Hists
    // auto cc_xtE = ROOT::TThreadedObject<TH2D>("cc_xtE", "Clover Cross Energy;Energy (keV);Channel;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels, 0, kDigitizerChannels);

    // // Sum Hists
    // auto cc_sum = ROOT::TThreadedObject<TH2D>("cc_sum", "Clover Cross Energy (Detector Sum);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels / 4, 0, kDigitizerChannels / 4);

    // // Addback Hists
    // auto cc_abE = ROOT::TThreadedObject<TH2D>("cc_abE", "Clover Cross Energy (Detector Addback);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels / 4, 0, kDigitizerChannels / 4);
    // auto cc_abM = ROOT::TThreadedObject<TH2D>("cc_abM", "Clover Cross Addback Multiplicity;Multiplicity;Counts/Bin", 4, 1, 5, kDigitizerChannels / 4, 0, kDigitizerChannels / 4);

    // std::array<ROOT::TThreadedObject<TH2D>, 6> c1_xtk = {
    //     ROOT::TThreadedObject<TH2D>("C1_xtk_E1E2", "C1 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy),
    //     ROOT::TThreadedObject<TH2D>("C1_xtk_E1E3", "C1 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy),
    //     ROOT::TThreadedObject<TH2D>("C1_xtk_E1E4", "C1 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy),
    //     ROOT::TThreadedObject<TH2D>("C1_xtk_E2E3", "C1 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy),
    //     ROOT::TThreadedObject<TH2D>("C1_xtk_E2E4", "C1 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy),
    //     ROOT::TThreadedObject<TH2D>("C1_xtk_E3E4", "C1 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy)};

    // // Raw Hists
    // auto cb_amp = ROOT::TThreadedObject<TH2D>("cb_amp", "Clover Back Amplitude (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
    // auto cb_cht = ROOT::TThreadedObject<TH2D>("cb_cht", "Clover Back Channel Time (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, kDigitizerChannels, 0, kDigitizerChannels);
    // auto cb_plu = ROOT::TThreadedObject<TH2D>("cb_plu", "Clover Back Pile-Up;Pile-Up Multiplicity;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
    // auto cb_mdt = ROOT::TThreadedObject<TH1D>("cb_mdt", "Clover Back Module Time;Time (ns);Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin);
    // auto cb_trt = ROOT::TThreadedObject<TH2D>("cb_trt", "Clover Back Trigger Time;Time (ns);Trigger ID;Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, 2, 0, 2);

    // // Calibrated Hists
    // auto cb_xtE = ROOT::TThreadedObject<TH2D>("cb_xtE", "Clover Back Energy;Energy (keV);Channel;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels, 0, kDigitizerChannels);

    // // Sum Hists
    // auto cb_sum = ROOT::TThreadedObject<TH2D>("cb_sum", "Clover Back Energy (Detector Sum);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels / 4, 0, kDigitizerChannels / 4);

    // // Addback Hists
    // auto cb_abE = ROOT::TThreadedObject<TH2D>("cb_abE", "Clover Back Energy (Detector Addback);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels / 4, 0, kDigitizerChannels / 4);
    // auto cb_abM = ROOT::TThreadedObject<TH2D>("cb_abM", "Clover Back Addback Multiplicity;Multiplicity;Counts/Bin", 4, 1, 5, kDigitizerChannels / 4, 0, kDigitizerChannels / 4);

    // Parse command line arguments

    auto infile = TFile::Open(argv[1], "READ");
    if (!infile || infile->IsZombie())
    {
        throw std::runtime_error(Form("[ERROR] Error opening input file: %s", argv[1]));
    }
    printf("[INFO] Opened file %s\n", argv[1]);

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

    printf("[INFO] Processing events with %d threads...\n", kThreads);

    // Create a TTreeReader to read the TTree
    ROOT::TTreeProcessorMT EventProcessor(argv[1], "clover");

    // Fill Function
    auto fillHistograms = [&](TTreeReader& event_reader)
    {
        TTreeReaderArray<double> cc_amp_val(event_reader, "clover_cross.amplitude");
        TTreeReaderArray<double> cc_cht_val(event_reader, "clover_cross.channel_time");
        TTreeReaderArray<double> cc_mdt_val(event_reader, "clover_cross.module_timestamp");
        TTreeReaderArray<double> cc_plu_val(event_reader, "clover_cross.pileup");
        TTreeReaderArray<double> cc_trt_val(event_reader, "clover_cross.trigger_time");

        // TTreeReaderArray<double> cb_amp_val(event_reader, "clover_back.amplitude");
        // TTreeReaderArray<double> cb_cht_val(event_reader, "clover_back.channel_time");
        // TTreeReaderArray<double> cb_mdt_val(event_reader, "clover_back.module_timestamp");
        // TTreeReaderArray<double> cb_plu_val(event_reader, "clover_back.pileup");
        // TTreeReaderArray<double> cb_trt_val(event_reader, "clover_back.trigger_time");

        auto cc_amp_ptr = cc_amp.Get();
        auto cc_cht_ptr = cc_cht.Get();
        auto cc_plu_ptr = cc_plu.Get();
        auto cc_trt_ptr = cc_trt.Get();
        auto cc_mdt_ptr = cc_mdt.Get();
        // auto cc_xtE_ptr = cc_xtE.Get();
        // auto cc_sum_ptr = cc_sum.Get();
        // auto cc_abE_ptr = cc_abE.Get();
        // auto cc_abM_ptr = cc_abM.Get();
        // std::array<std::shared_ptr<TH2D>, 6> c1_xtk_ptrs;
        // for (size_t i = 0; i < c1_xtk_ptrs.size(); ++i)
        //     c1_xtk_ptrs[i] = c1_xtk[i].Get();

        // auto cb_amp_ptr = cb_amp.Get();
        // auto cb_cht_ptr = cb_cht.Get();
        // auto cb_plu_ptr = cb_plu.Get();
        // auto cb_trt_ptr = cb_trt.Get();
        // auto cb_mdt_ptr = cb_mdt.Get();
        // auto cb_xtE_ptr = cb_xtE.Get();
        // auto cb_sum_ptr = cb_sum.Get();
        // auto cb_abE_ptr = cb_abE.Get();
        // auto cb_abM_ptr = cb_abM.Get();

        // Loop over the entries in the tree
        while (event_reader.Next())
        {
            cc_mdt_ptr->Fill(cc_mdt_val[0] * kNsPerBin);
            cc_trt_ptr->Fill(cc_trt_val[0] * kNsPerBin, 0);
            cc_trt_ptr->Fill(cc_trt_val[1] * kNsPerBin, 1);

            // cb_mdt_ptr->Fill(cb_mdt_val[0] * kNsPerBin);
            // cb_trt_ptr->Fill(cb_trt_val[0] * kNsPerBin, 0);
            // cb_trt_ptr->Fill(cb_trt_val[1] * kNsPerBin, 1);

            for (size_t det = 0; det < 4; det++)
            {
                // std::array<double, 4> cc_xtal_E = {NAN, NAN, NAN, NAN}, cb_xtal_E = {NAN, NAN, NAN, NAN};
                // std::array<double, 4> cc_xtal_T = {NAN, NAN, NAN, NAN}, cb_xtal_T = {NAN, NAN, NAN, NAN};

                // Crystal Loop
                for (size_t xtal = 0; xtal < 4; xtal++)
                {
                    auto ch = det * 4 + xtal; // Channel number 0-15

                    // Raw Histograms
                    cc_amp_ptr->Fill(cc_amp_val[ch], ch);
                    cc_cht_ptr->Fill(cc_cht_val[ch], ch);
                    cc_plu_ptr->Fill(cc_plu_val[ch], ch);
                    // cb_amp_ptr->Fill(cb_amp_val[ch], ch);
                    // cb_cht_ptr->Fill(cb_cht_val[ch], ch);
                    // cb_plu_ptr->Fill(cb_plu_val[ch], ch);

                    // // Calibrated Histograms
                    // if (!std::isnan(cc_amp_val[ch]) &&
                    //     !std::isnan(cc_cht_val[ch]))
                    // {
                    //     // std::cout << "Channel: " << ch << ", ";
                    //     double energy = cc_amp_val[ch] * 0.16; // Gain-match, then calibrate
                    //     double cht = cc_cht_val[ch] * kNsPerBin;
                    //     cc_xtE_ptr->Fill(energy, ch);
                    //     cc_cht_ptr->Fill(cht, ch);
                    //     cc_sum_ptr->Fill(energy, det); // ch / 4 is the detector number
                    //     cc_xtal_E[xtal] = energy;
                    //     cc_xtal_T[xtal] = cht;
                    // }

                    // if (!std::isnan(cb_amp_val[ch]) &&
                    //     !std::isnan(cb_cht_val[ch]))
                    // {
                    //     // std::cout << "Channel: " << ch << ", ";
                    //     double energy = cb_amp_val[ch] * 0.16; // Gain-match, then calibrate
                    //     double cht = cb_cht_val[ch] * kNsPerBin;
                    //     cb_xtE_ptr->Fill(energy, ch);
                    //     cb_cht_ptr->Fill(cht, ch);
                    //     cb_sum_ptr->Fill(energy, det); // ch / 4 is the detector number
                    //     cb_xtal_E[xtal] = energy;
                    //     cb_xtal_T[xtal] = cht;
                    // }
                }

                // // Add-Back Histograms
                // cc_abE_ptr->Fill(CAAddBack::GetAddBackEnergy(cc_xtal_E, cc_xtal_T), det);
                // unsigned int mult = std::count_if(cc_xtal_E.begin(), cc_xtal_E.end(), [](double e)
                //                                   { return !std::isnan(e); });
                // cc_abM_ptr->Fill(mult, det);
                // if (mult == 2 && det == 0)
                // {
                //     CACrosstalkCorrection::FillXTalkHistograms(c1_xtk_ptrs, cc_xtal_E, cc_xtal_T);
                // }

                // cb_abE_ptr->Fill(CAAddBack::GetAddBackEnergy(cb_xtal_E, cb_xtal_T), det);
                // mult = std::count_if(cb_xtal_E.begin(), cb_xtal_E.end(), [](double e)
                //                      { return !std::isnan(e); });
                // cb_abM_ptr->Fill(mult, det);
            }
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
    TFile* outfile = new TFile("out.root", "RECREATE");

    cc_amp.Merge()->Write();
    cc_cht.Merge()->Write();
    cc_plu.Merge()->Write();
    cc_trt.Merge()->Write();
    cc_mdt.Merge()->Write();

    printf("Saved histograms to file: out.root\n");

    outfile->Close();
    delete outfile;

    printf("Done!\n");

    return 0;
}