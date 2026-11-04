# DeliriumT Vocals — Dark Grave / DXTXM

**Target:** Windows 10/11 x64 · FL Studio · VST3  
**Version:** 1.0.0 source package

A six-module vocal effect processor for Darkwave, Gothic Post-Punk, Deathrock and adjacent dark electronic styles.

## Included modules
- EQ: Low / Mid / High gain and mid frequency
- Compressor: Threshold / Ratio / Attack / Release / Makeup
- Chorus: Rate / Depth / Mix
- Delay: Time / Feedback / Mix / Stereo
- Reverb: Decay / Size / Damping / Pre-delay / Mix
- Saturation: Drive / Tone / Mix
- Input / Output trim, per-module enable switches, host state recall and five built-in starting presets.

## Build on Windows
1. Install Visual Studio 2022 with **Desktop development with C++** and the Windows 10/11 SDK.
2. Install CMake 3.22 or newer and Git.
3. Open an x64 Developer PowerShell in this folder.
4. Run:
   ```powershell
   cmake -S . -B build -A x64
   cmake --build build --config Release
   ```
   CMake downloads JUCE 8.0.6 on the first configure (internet required).
5. The VST3 bundle will be produced under `build/DeliriumTVocals_artefacts/Release/VST3/DeliriumT Vocals.vst3`.
6. Copy the `.vst3` bundle to:
   `C:\Program Files\Common Files\VST3`
7. In FL Studio, open **Options > Manage plugins > Find installed plugins**.

## Installer
`Installer/DeliriumT_Vocals.iss` is an Inno Setup 6 script. Build the plugin first, then compile the script with Inno Setup 6. It expects the VST3 bundle at the build path above.

## Important status
This package contains the C++ source, CMake project, preset reference sheets and an Inno Setup installer script. It is **not a precompiled Windows VST3 binary**. A Windows x64 build and DAW validation are required before distribution. The source should be treated as a development release until it has been compiled and tested in FL Studio.

## Branding
Product: DeliriumT Vocals  
Visual identity: DXTXM  
Distributor credit: DARK GRAVE

## Audio note
Use conservative wet levels and gain staging. The compressor, chorus, delay, reverb and saturation are independent. The current EQ is a three-band tone stage with a user-adjustable upper-mid/high crossover. The stereo control offsets the delay read position between channels. The delay/chorus engines are lightweight real-time DSP intended as a starting implementation, not a substitute for a full mastering-grade processor.


## One-click local build helper
On Windows, after installing Visual Studio 2022 C++ workload, CMake, and Git, double-click `BUILD_WINDOWS.bat`. It configures and builds the x64 VST3. Install Inno Setup 6 and compile `Installer/DeliriumT_Vocals.iss` to create the setup EXE.

## Automated Windows build
The `.github/workflows/windows-vst3.yml` workflow can build the Windows VST3 and installer on a GitHub Windows runner. Push this folder to a GitHub repository and run **Actions > Build DeliriumT Vocals (Windows VST3) > Run workflow**. Download the resulting artifact from the workflow run.
