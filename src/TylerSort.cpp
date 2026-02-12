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

/* #endregion Includes */

// Main function
int main(int argc, char* argv[])
{

    constexpr size_t kDigitizerChannels = 16;
    constexpr size_t kDigitizerBins = 2 << 16; // 16-bit digitizer
    constexpr double kNsPerBin = 0.098;        // 62.5 ps per bin for 16-bit digitizer at 1 GHz sampling rate

    auto cc_amp = ROOT::TThreadedObject<TH2D>("cc_amp", "Clover Cross Amplitude (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
    auto cc_cht = ROOT::TThreadedObject<TH2D>("cc_cht", "Clover Cross Channel Time (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins * kNsPerBin, kDigitizerChannels, 0, kDigitizerChannels);
    auto cc_plu = ROOT::TThreadedObject<TH2D>("cc_plu", "Clover Cross Pile-Up;Pile-Up Multiplicity;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
    auto cc_mdt = ROOT::TThreadedObject<TH1D>("cc_mdt", "Clover Cross Module Time;Time (ns);Counts/Bin", kDigitizerBins, 0, kDigitizerBins * kNsPerBin);
    auto cc_trt = ROOT::TThreadedObject<TH2D>("cc_trt", "Clover Cross Trigger Time;Time (ns);Trigger ID;Counts/Bin", kDigitizerBins, 0, kDigitizerBins * kNsPerBin, 2, 0, 2);

    printf("[INFO] Processing events...\n");

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

        auto cc_amp_ptr = cc_amp.Get();
        auto cc_cht_ptr = cc_cht.Get();
        auto cc_plu_ptr = cc_plu.Get();
        auto cc_trt_ptr = cc_trt.Get();
        auto cc_mdt_ptr = cc_mdt.Get();

        // Loop over the entries in the tree
        while (event_reader.Next())
        {
            cc_mdt_ptr->Fill(cc_mdt_val[0] * kNsPerBin);
            cc_trt_ptr->Fill(cc_trt_val[0] * kNsPerBin, 0);
            cc_trt_ptr->Fill(cc_trt_val[1] * kNsPerBin, 1);

            // Detector Loop
            for (size_t ch = 0; ch < 16; ch++)
            {
                cc_amp_ptr->Fill(cc_amp_val[ch], ch);
                cc_cht_ptr->Fill(cc_cht_val[ch], ch);
                cc_plu_ptr->Fill(cc_plu_val[ch], ch);
            }
        }
    };

    // Loop over the entries in the TTree and fill the histograms appropriately
    TStopwatch timer;
    timer.Start();
    EventProcessor.Process(fillHistograms);
    timer.Stop();

    printf("[INFO] Processed events in %.2f seconds\n", timer.RealTime());

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