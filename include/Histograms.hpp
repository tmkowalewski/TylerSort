#ifndef CAHISTOGRAMS_HPP
#define CAHISTOGRAMS_HPP

// C++ Includes
#include <array>
#include <memory>
#include <string>

// ROOT Includes
#include <ROOT/TThreadedObject.hxx>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

// Project Includes
#include "CASort/TCAHistogram.hpp"

namespace Histograms
{
// Constants
inline constexpr double       kMaxEnergy         = 10000.0;   // Maximum energy for histograms in keV
inline constexpr double       kXTalkMaxEnergy    = 6000.0;    // Maximum energy for cross-talk histograms in keV
inline constexpr double       kEnergyPerBin      = 0.50;      // Energy per bin in keV
inline constexpr double       kXTalkEnergyPerBin = 5.0;       // Coarser binning for 2D cross-talk histograms
inline constexpr double       kCoinEnergyPerBin  = 10.0;      // Energy per bin in coincidence histograms
inline constexpr double       kCoinMaxTime       = 200.0;     // Maximum time for coincidence histograms in ns
inline constexpr double       kCoinTimePerBin    = 5.0;       // Time in ns per bin in coincidence histograms
inline constexpr double       kNsPerBin          = 0.098;     // Conversion factor from bin to nanoseconds
inline constexpr unsigned int kDigitizerBins     = 1u << 16u; // Number of bins in the digitizer (16-bit)
inline constexpr unsigned int kDigitizerChannels = 16u;       // Number of channels in digitizer

// --- Raw histograms (mode == "raw") ---
// Clover Cross
inline std::unique_ptr<TCAHistogram<TH2D>> cc_amp; // Amplitude histogram for Clover Cross module
inline std::unique_ptr<TCAHistogram<TH2D>> cc_cht; // Channel Time histogram for Clover Cross module
// inline std::unique_ptr<TCAHistogram<TH2D>> cc_plu; // Pile-Up histogram for Clover Cross module
inline std::unique_ptr<TCAHistogram<TH1D>> cc_mdt; // Module Time histogram for Clover Cross module
// inline std::unique_ptr<TCAHistogram<TH2D>> cc_trt; // Trigger Time histogram for Clover Cross module

//  Clover Back
inline std::unique_ptr<TCAHistogram<TH2D>> cb_amp; // Amplitude histogram for Clover Back module
inline std::unique_ptr<TCAHistogram<TH2D>> cb_cht; // Channel Time histogram for Clover Back module
// inline std::unique_ptr<TCAHistogram<TH2D>> cb_plu; // Pile-Up histogram for Clover Back module
inline std::unique_ptr<TCAHistogram<TH1D>> cb_mdt; // Module Time histogram for Clover Back module
// inline std::unique_ptr<TCAHistogram<TH2D>> cb_trt; // Trigger Time histogram for Clover Back module

//  CeBr Detectors
inline std::unique_ptr<TCAHistogram<TH2D>> ce_inl; // Integration Long histogram for CeBr detectors
inline std::unique_ptr<TCAHistogram<TH2D>> ce_ins; // Integration Short histogram for CeBr detectors
inline std::unique_ptr<TCAHistogram<TH2D>> ce_cht; // Channel Time histogram for CeBr detectors
inline std::unique_ptr<TCAHistogram<TH1D>> ce_mdt; // Module Time histogram for CeBr detectors
// inline std::unique_ptr<TCAHistogram<TH2D>> ce_trt; // Trigger Time histogram for CeBr detectors

// --- Calibrated histograms (mode == "cal" or "xtcorr") ---
// Clover Cross
inline std::unique_ptr<TCAHistogram<TH2D>> cc_chE; // Channel Energy histogram for Clover Cross module
inline std::unique_ptr<TCAHistogram<TH2D>> cc_sum; // Sum Energy histogram for detectors in the Clover Cross module
inline std::unique_ptr<TCAHistogram<TH2D>> cc_abE; // Add-back Energy histogram for detectorsClover Cross module

// Clover Back
inline std::unique_ptr<TCAHistogram<TH2D>> cb_chE; // Channel Energy histogram for Clover Back module
inline std::unique_ptr<TCAHistogram<TH2D>> cb_sum; // Sum Energy histogram for detectors in the Clover Back module
inline std::unique_ptr<TCAHistogram<TH2D>> cb_abE; // Add-back Energy histogram for detectors in the Clover Back module

// CeBr Detectors
inline std::unique_ptr<TCAHistogram<TH2D>> ce_chE; // Channel Energy histogram for CeBr detectors

// --- Cross-talk correction histograms (mode == "xtcorr") ---
// Clover Cross
inline std::unique_ptr<TCAHistogram<TH1D>>                cc_abM; // Add-back Multiplicity histogram for detectors in the Clover Cross module
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> c1_xtk; // Cross-talk correction histograms for C1
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> c3_xtk; // Cross-talk correction histograms for C3
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> c5_xtk; // Cross-talk correction histograms for C5
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> c7_xtk; // Cross-talk correction histograms for C7
// Clover Back
inline std::unique_ptr<TCAHistogram<TH1D>>                cb_abM; // Add-back Multiplicity histogram for detectors in the Clover Back module
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> b1_xtk; // Cross-talk correction histograms for B1
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> b2_xtk; // Cross-talk correction histograms for B2
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> b3_xtk; // Cross-talk correction histograms for B3
inline std::array<std::unique_ptr<TCAHistogram<TH2D>>, 6> b5_xtk; // Cross-talk correction histograms for B5

// --- Coincidence histograms (mode == "coin") ---
inline std::unique_ptr<TCAHistogram<TH3I>> gege_ggT; // HPGe-HPGe Coincidence histogram (any HPGe in coincidence with any other HPGe)
inline std::unique_ptr<TCAHistogram<TH1I>> gege_ggM; // HPGe-HPGe Coincidence Multiplicity histogram (any HPGe in coincidence with any other HPGe)

inline std::unique_ptr<TCAHistogram<TH3I>> cege_ggT; // CeBr-HPGe Coincidence histogram (any CeBr in coincidence with any HPGe)
inline std::unique_ptr<TCAHistogram<TH1I>> cege_ggM; // CeBr-HPGe Coincidence Multiplicity histogram (any CeBr in coincidence with any HPGe)

inline std::unique_ptr<TCAHistogram<TH3I>> cece_ggT; // CeBr-CeBr Coincidence histogram (any CeBr in coincidence with any other CeBr)
inline std::unique_ptr<TCAHistogram<TH1I>> cece_ggM; // CeBr-CeBr Coincidence Multiplicity histogram (any CeBr in coincidence with any other CeBr)

inline void Initialize(const std::string& mode)
{
    if (mode == "raw")
    {
        // Clover Cross
        cc_amp =
            std::make_unique<TCAHistogram<TH2D>>("cc_amp", "Clover Cross Amplitude (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
        cc_cht = std::make_unique<TCAHistogram<TH2D>>("cc_cht", "Clover Cross Channel Time (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, kDigitizerChannels, 0,
                                                      kDigitizerChannels);
        // cc_plu = std::make_unique<TCAHistogram<TH2D>>(
        //     "cc_plu", "Clover Cross Pile-Up;Pile-Up Multiplicity;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins,
        //     kDigitizerChannels, 0, kDigitizerChannels);
        cc_mdt = std::make_unique<TCAHistogram<TH1D>>("cc_mdt", "Clover Cross Module Time;Time (ns);Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin);
        // cc_trt =
        //     std::make_unique<TCAHistogram<TH2D>>("cc_trt", "Clover Cross Trigger Time;Time (ns);Trigger ID;Counts/Bin",
        //                                          kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, 2, 0, 2);
        // Clover Back
        cb_amp =
            std::make_unique<TCAHistogram<TH2D>>("cb_amp", "Clover Back Amplitude (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
        cb_cht = std::make_unique<TCAHistogram<TH2D>>("cb_cht", "Clover Back Channel Time (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, kDigitizerChannels, 0,
                                                      kDigitizerChannels);
        // cb_plu = std::make_unique<TCAHistogram<TH2D>>(
        //     "cb_plu", "Clover Back Pile-Up;Pile-Up Multiplicity;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins,
        //     kDigitizerChannels, 0, kDigitizerChannels);
        cb_mdt = std::make_unique<TCAHistogram<TH1D>>("cb_mdt", "Clover Back Module Time;Time (ns);Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin);
        // cb_trt =
        //     std::make_unique<TCAHistogram<TH2D>>("cb_trt", "Clover Back Trigger Time;Time (ns);Trigger ID;Counts/Bin",
        //                                          kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, 2, 0, 2);
        // CeBr Detetectors
        ce_inl =
            std::make_unique<TCAHistogram<TH2D>>("ce_inl", "CeBr Integration Long (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
        ce_ins =
            std::make_unique<TCAHistogram<TH2D>>("ce_ins", "CeBr Integration Short (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, kDigitizerBins, kDigitizerChannels, 0, kDigitizerChannels);
        ce_cht = std::make_unique<TCAHistogram<TH2D>>("ce_cht", "CeBr Channel Time (Raw Data);ADC;Channel;Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, kDigitizerChannels, 0,
                                                      kDigitizerChannels);
        ce_mdt = std::make_unique<TCAHistogram<TH1D>>("ce_mdt", "CeBr Module Time;Time (ns);Counts/Bin", kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin);
        // ce_trt = std::make_unique<TCAHistogram<TH2D>>("ce_trt", "CeBr Trigger Time;Time (ns);Trigger ID;Counts/Bin",
        //                                               kDigitizerBins, 0, (kDigitizerBins)*kNsPerBin, 2, 0, 2);
    } else if (mode == "cal")
    {
        // Clover Cross
        cc_chE =
            std::make_unique<TCAHistogram<TH2D>>("cc_chE", "Clover Cross Energy;Energy (keV);Channel;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels, 0, kDigitizerChannels);
        cc_sum = std::make_unique<TCAHistogram<TH2D>>("cc_sum", "Clover Cross Energy (Detector Sum);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy,
                                                      kDigitizerChannels / 4, 0, kDigitizerChannels / 4);
        cc_abE = std::make_unique<TCAHistogram<TH2D>>("cc_abE", "Clover Cross Energy (Detector Addback);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy,
                                                      kDigitizerChannels / 4, 0, kDigitizerChannels / 4);
        // Clover Back
        cb_chE =
            std::make_unique<TCAHistogram<TH2D>>("cb_chE", "Clover Back Energy;Energy (keV);Channel;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels, 0, kDigitizerChannels);
        cb_sum = std::make_unique<TCAHistogram<TH2D>>("cb_sum", "Clover Back Energy (Detector Sum);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels / 4,
                                                      0, kDigitizerChannels / 4);
        cb_abE = std::make_unique<TCAHistogram<TH2D>>("cb_abE", "Clover Back Energy (Detector Addback);Energy (keV);Detector;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy,
                                                      kDigitizerChannels / 4, 0, kDigitizerChannels / 4);
        // CeBr Detectors
        ce_chE = std::make_unique<TCAHistogram<TH2D>>("ce_chE", "CeBr Energy;Energy (keV);Channel;Counts/Bin", kMaxEnergy / kEnergyPerBin, 0, kMaxEnergy, kDigitizerChannels, 0, kDigitizerChannels);

    } else if (mode == "xtcorr")
    {
        // Clover Cross cross-talk
        cc_abM = std::make_unique<TCAHistogram<TH1D>>("cc_abM", "Clover Cross Addback Multiplicity;Multiplicity;Counts/Bin", 4, 1, 5);
        c1_xtk = {std::make_unique<TCAHistogram<TH2D>>("C1_xtk_E1E2", "C1 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C1_xtk_E1E3", "C1 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C1_xtk_E1E4", "C1 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C1_xtk_E2E3", "C1 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C1_xtk_E2E4", "C1 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C1_xtk_E3E4", "C1 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        c3_xtk = {std::make_unique<TCAHistogram<TH2D>>("C3_xtk_E1E2", "C3 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C3_xtk_E1E3", "C3 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C3_xtk_E1E4", "C3 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C3_xtk_E2E3", "C3 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C3_xtk_E2E4", "C3 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C3_xtk_E3E4", "C3 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        c5_xtk = {std::make_unique<TCAHistogram<TH2D>>("C5_xtk_E1E2", "C5 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C5_xtk_E1E3", "C5 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C5_xtk_E1E4", "C5 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C5_xtk_E2E3", "C5 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C5_xtk_E2E4", "C5 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C5_xtk_E3E4", "C5 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        c7_xtk = {std::make_unique<TCAHistogram<TH2D>>("C7_xtk_E1E2", "C7 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C7_xtk_E1E3", "C7 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C7_xtk_E1E4", "C7 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C7_xtk_E2E3", "C7 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C7_xtk_E2E4", "C7 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("C7_xtk_E3E4", "C7 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        // Clover Back cross-talk
        cb_abM = std::make_unique<TCAHistogram<TH1D>>("cb_abM", "Clover Back Addback Multiplicity;Multiplicity;Counts/Bin", 4, 1, 5);
        b1_xtk = {std::make_unique<TCAHistogram<TH2D>>("B1_xtk_E1E2", "B1 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B1_xtk_E1E3", "B1 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B1_xtk_E1E4", "B1 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B1_xtk_E2E3", "B1 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B1_xtk_E2E4", "B1 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B1_xtk_E3E4", "B1 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        b2_xtk = {std::make_unique<TCAHistogram<TH2D>>("B2_xtk_E1E2", "B2 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B2_xtk_E1E3", "B2 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B2_xtk_E1E4", "B2 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B2_xtk_E2E3", "B2 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B2_xtk_E2E4", "B2 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B2_xtk_E3E4", "B2 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        b3_xtk = {std::make_unique<TCAHistogram<TH2D>>("B3_xtk_E1E2", "B3 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B3_xtk_E1E3", "B3 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B3_xtk_E1E4", "B3 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B3_xtk_E2E3", "B3 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B3_xtk_E2E4", "B3 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B3_xtk_E3E4", "B3 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
        b5_xtk = {std::make_unique<TCAHistogram<TH2D>>("B5_xtk_E1E2", "B5 Cross-Talk E1 by E2;E1;E2", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B5_xtk_E1E3", "B5 Cross-Talk E1 by E3;E1;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B5_xtk_E1E4", "B5 Cross-Talk E1 by E4;E1;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B5_xtk_E2E3", "B5 Cross-Talk E2 by E3;E2;E3", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B5_xtk_E2E4", "B5 Cross-Talk E2 by E4;E2;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy),
                  std::make_unique<TCAHistogram<TH2D>>("B5_xtk_E3E4", "B5 Cross-Talk E3 by E4;E3;E4", kXTalkMaxEnergy / kXTalkEnergyPerBin, 0, kXTalkMaxEnergy, kXTalkMaxEnergy / kXTalkEnergyPerBin, 0,
                                                       kXTalkMaxEnergy)};
    } else if (mode == "coin")
    {
        gege_ggT = std::make_unique<TCAHistogram<TH3I>>("gege_ggT", "HPGe-HPGe Coincidence;Energy (keV);Energy (keV);Time (ns);Counts/Bin", kMaxEnergy / kCoinEnergyPerBin, 0, kMaxEnergy,
                                                        kMaxEnergy / kCoinEnergyPerBin, 0, kMaxEnergy, 2 * kCoinMaxTime / kCoinTimePerBin, -kCoinMaxTime, kCoinMaxTime);
        gege_ggM = std::make_unique<TCAHistogram<TH1I>>("gege_ggM", "HPGe-HPGe Coincidence Multiplicity;Multiplicity;Counts", 10, 0.5, 10.5);

        cege_ggT = std::make_unique<TCAHistogram<TH3I>>("cege_ggT", "CeBr-CeBr Coincidence;Energy (keV);Energy (keV);Counts/Bin", kMaxEnergy / kCoinEnergyPerBin, 0, kMaxEnergy,
                                                        kMaxEnergy / kCoinEnergyPerBin, 0, kMaxEnergy, 2 * kCoinMaxTime / kCoinTimePerBin, -kCoinMaxTime, kCoinMaxTime);
        cege_ggM = std::make_unique<TCAHistogram<TH1I>>("cege_ggM", "CeBr-HPGe Coincidence Multiplicity;Multiplicity;Counts", 10, 0.5, 10.5);

        cece_ggT = std::make_unique<TCAHistogram<TH3I>>("cece_ggT", "CeBr-HPGe Coincidence;Energy (keV);Energy (keV);Counts/Bin", kMaxEnergy / kCoinEnergyPerBin, 0, kMaxEnergy,
                                                        kMaxEnergy / kCoinEnergyPerBin, 0, kMaxEnergy, 2 * kCoinMaxTime / kCoinTimePerBin, -kCoinMaxTime, kCoinMaxTime);
        cece_ggM = std::make_unique<TCAHistogram<TH1I>>("cece_ggM", "CeBr-CeBr Coincidence Multiplicity;Multiplicity;Counts", 10, 0.5, 10.5);
    }
}
} // namespace Histograms

#endif // CAHISTOGRAMS_HPP