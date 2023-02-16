## Chroma Engine

[![Build](https://github.com/ttalexander2/chroma/actions/workflows/build.yml/badge.svg)](https://github.com/ttalexander2/chroma/actions/workflows/build.yml)
[![Documentation](https://github.com/ttalexander2/chroma/actions/workflows/main.yml/badge.svg)](https://github.com/ttalexander2/chroma/actions/workflows/main.yml)

A small C++ Game Engine and Editor, designed for creating games in 2D. It was started as a way to learn more about game technology, computer graphics, and game engine design. It takes ideas and inspiration from other open-source engines, and is designed to be lightweight and easily modifiable.

At the moment, only Windows building is supported. Support for other platforms will be implemented at a later date.

**This project is a work-in-progress, and will likely see breaking changes! Use at your own risk!**

![](https://raw.githubusercontent.com/ttalexander2/chroma/master/docs/screencap.png)

---


#### Prerequisites
- Visual C++ Redistributable 2019

#### Building
##### Windows
- Requires C++20, Visual Studio 2019, Python 3.3+, and Mono
- All other dependencies are either included within the project, or provided as github submodules.
- Instructions
    - `git clone --recurse-submodules -j8 https://github.com/ttalexander2/chroma.git`
    - Execute `Setup.bat` to install prerequisites and build the VS Solution. This will check for python versions, as well as install the VulkanSDK.
    - Follow instructions to install prerequisits and verify requirements.
    - Open Solution File.
    - Select Build > Build Solution.
 - Please note, if you experience an error, try building the Chroma.Mono project first.
     
##### Mac & Linux
- Currently does not support building on mac/linux, but support is on the way!

----
#### Notes
- Engine is currently single threaded, plans for a multi-threaded renderer as well as a job system may be implemented in the future.
- Audio is implemented using FMOD Studio, plans for a non-fmod audio backend will be implemented in the future.
- Only OpenGL renderer is implemented, a Vulkan backend is currently in the works.

---
#### Documentation

Please see the [Documentation Page](https://ttalexander2.github.io/chroma/) for more information and resources.
