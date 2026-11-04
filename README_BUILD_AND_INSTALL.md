# DeliriumT Vocals — Windows x64 build package

This repository builds the **VST3 plugin and Windows installer** on a GitHub-hosted
Windows runner. It does not contain a precompiled binary.

## Build and download (no local compiler required)

1. Sign in to GitHub and create a **new private repository**.
2. Extract this ZIP. Upload the *contents* of the extracted folder to the repository
   root (the `.github` folder must be at the root, not inside another folder).
3. Open the repository's **Actions** tab.
4. Select **Build DeliriumT Vocals (Windows VST3)**.
5. Click **Run workflow** and confirm.
6. Wait for the workflow to finish successfully.
7. Open the completed run and download the artifact named
   `DeliriumT-Vocals-Windows-x64`.

The artifact contains the `.vst3` bundle and
`DeliriumT_Vocals_Windows_x64_Setup.exe`.

## Install in FL Studio

1. Download and extract the artifact ZIP.
2. Run `DeliriumT_Vocals_Windows_x64_Setup.exe` as a normal Windows installer.
3. Open FL Studio → Options → Manage plugins → Find installed plugins.
4. Search for **DeliriumT Vocals** under VST3.

## Local build (optional)

Requires Windows 10/11 x64, Visual Studio 2022 with Desktop development with C++,
CMake 3.22+, Git, and Inno Setup 6. Run `BUILD_WINDOWS.bat`.

## Project details

- Format: VST3 x64 (recommended; no legacy VST2 SDK needed)
- DAW target: FL Studio
- Effects: EQ, compressor, chorus, delay, reverb, saturation
- Branding: DXTXM / Dark Grave

**Important:** A successful GitHub Actions build confirms compilation and packaging,
but the resulting plugin should still be opened and tested in FL Studio on a Windows
machine before public distribution.
