# Setting Up

As of current, the engine is deployed on github, and supports windows only.
The latest version of the engine is found here:
<a href="https://github.com/ttalexander2/chroma/releases" target="_blank">https://github.com/ttalexander2/chroma/releases</a>

## System Requirements
- OS: Windows 7 or newer.
- CPU: 64-bit processor. Quad-core or better recommended.
- RAM: Minimum 4GB of ram. 16GB is recommended.
- GPU: Any GPU with OpenGL 4.6 and Vulkan 1.2 support.

## Prerequisites
- Microsoft Visual C++ 2019 Redistributable/Runtime
  - Currently the engine does not provide the C++ runtime required for execution of the engine or applications deployed from the engine.
  - It can either be installed as a part the Visual C++ development tools as a part of a Visual Studio installation, or from a redistributable package found <a href="https://aka.ms/vs/17/release/vc_redist.x64.exe" target="_blank">here</a>.

## Installation
 - Download the application from the github page <a href="https://github.com/ttalexander2/chroma/releases" target="_blank">here</a>.
 - Extract the engine to your preferred directory. The engine does not yet include an installer, so managing the application location and version is up to you.
 - Optionally, install <a href="https://code.visualstudio.com/download" target="_blank">VSCode</a>. The engine features VSCode integration by default.
 - Launch `Polychrome.exe` to get started!

## Building from source
 - If you would prefer, you may build the engine from source.
  - Requires C++20, Visual Studio 2019, C++  Python 3.3+, and Mono
  - All other dependencies are either included within the project, or provided as github submodules.
  - Instructions
      - `git clone --recurse-submodules -j8 https://github.com/ttalexander2/chroma.git`
      - Execute `Setup.bat` to install prerequisites and build the VS Solution. This will check for python versions, as well as install the VulkanSDK.
      - Follow instructions to install prerequisits and verify requirements.
      - Open Solution File.
      - Select Build > Build Solution.
   - Please note, if you experience an error, try building the Chroma.Mono project first.

