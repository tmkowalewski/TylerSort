# TylerSort

A multi-threaded ROOT-based histogram sorting program for HIGS NRF experiments. It reads event data from a ROOT TTree, applies optional calibrations and cross-talk corrections, and writes histograms to an output ROOT file.

## Detectors

| Detector | Description |
|---|---|
| **Clover Cross** (`cc`) | HPGe clover detector (cross orientation) — 4 detectors × 4 crystals |
| **Clover Back** (`cb`) | HPGe clover detector (back orientation) — 4 detectors × 4 crystals |
| **CeBr** (`ce`) | CeBr₃ scintillator detectors — 16 channels |

## Dependencies

- [ROOT](https://root.cern/) (with multithreading support) — `root-config` must be on `PATH`
- [CASort](https://github.com/) — installed to `~/.local/`
  - `CAAddBack`, `CACalibration`, `CAConfiguration`, `CACrosstalkCorrection`, `CAGainCorrection`, `CAUtilities`
- [Boost](https://www.boost.org/) — `boost_program_options`

## Building

```bash
# Release build
make

# Install to ~/.local/bin
make install

# Remove build artifacts
make clean
```

## Usage

```
TylerSort [options] <input.root> <output.root>
```

The processing mode is selected via CLI options parsed by `CAUtilities::ParseArguments`. Three modes are supported:

| Mode | Description |
|---|---|
| `raw` | Fill raw ADC amplitude, channel time, pile-up, and trigger-time histograms |
| `cal` | Apply gain matching and energy calibration; fill calibrated energy and add-back histograms |
| `xtcorr` | All of `cal`, plus fill cross-talk correction matrices for each detector pair |

### Calibration files

When running in `cal` or `xtcorr` mode the following files are required:

| Option | Description |
|---|---|
| `--caldir` | Directory containing per-crystal calibration parameter files (`C<det>E<xtal>.cal_params.txt` for Clover Cross, `B<det>E<xtal>.cal_params.txt` for Clover Back) |
| `--gsfile` | Gain-shift correction file (`.cags`) |
| `--xtalkfile` | *(optional)* Cross-talk correction file; if absent, cross-talk correction is skipped |

### Example

```bash
TylerSort --caldir=data/energy_calibration \
          --gsfile=data/gain_correction/70Ge_default.cags \
          data/trees/root_data_70Ge_run208.mvmelst.bin_tree.root \
          data/hists/test/test.hists.root
```

## Output Structure

Histograms are written into subdirectories of the output ROOT file:

```
output.root
├── clover_cross/
│   ├── (raw)    cc_amp, cc_cht, cc_plu, cc_trt, cc_mdt
│   ├── (cal)    cc_chE, cc_sum, cc_abE
│   └── (xtcorr) cc_abM, c1_xtk[0-5], c3_xtk[0-5], c5_xtk[0-5], c7_xtk[0-5]
├── clover_back/
│   ├── (raw)    cb_amp, cb_cht, cb_plu, cb_trt, cb_mdt
│   ├── (cal)    cb_chE, cb_sum, cb_abE
│   └── (xtcorr) cb_abM, b1_xtk[0-5], b2_xtk[0-5], b3_xtk[0-5], b5_xtk[0-5]
└── cebr_all/
    ├── (raw)    ce_inl, ce_ins, ce_cht, ce_trt, ce_mdt
    └── (cal)    ce_chE
```

### Histogram parameters

| Constant | Value | Description |
|---|---|---|
| `kMaxEnergy` | 10 000 keV | Energy axis upper limit |
| `kXTalkMaxEnergy` | 6 000 keV | Energy axis upper limit for cross-talk matrices |
| `kEnergyPerBin` | 0.5 keV | Bin width for 1D/2D energy histograms |
| `kXTalkEnergyPerBin` | 5.0 keV | Bin width for 2D cross-talk histograms |
| `kNsPerBin` | 0.098 ns | ADC-bin to nanosecond conversion factor |
| `kDigitizerBins` | 65 536 (16-bit) | Raw ADC range |
| `kDigitizerChannels` | 16 | Number of digitizer channels per module |

## Project Structure

```
TylerSort/
├── src/
│   └── TylerSort.cpp      # Main program
├── include/
│   └── Histograms.hpp     # Histogram declarations and initialization
├── bin/                   # Compiled executable (created by make)
├── obj/                   # Object files (created by make)
├── Makefile
└── 70Ge_default.cags      # Default gain-shift correction file
```
