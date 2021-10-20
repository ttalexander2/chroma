## Chroma

[![Build](https://github.com/ttalexander2/chroma/actions/workflows/build.yml/badge.svg)](https://github.com/ttalexander2/chroma/actions/workflows/build.yml)
[![Documentation](https://github.com/ttalexander2/chroma/actions/workflows/main.yml/badge.svg)](https://github.com/ttalexander2/chroma/actions/workflows/main.yml)

A small 2D C++ Game Engine and Editor, designed in order to providing simple tools for creating games in 2D. It is designed to be lightweight, cross-platform, and easily modifiable.

**This project is a working progress, and will likely see breaking changes! Use at your own risk!**

---

#### Building
##### Windows
- Requires C++20, Visual Studio 2019, and VulkanSDK
- Uses premake build system
- All other dependencies are either included within the project, or provided as github submodules.
- Instructions
    - `git clone --recurse-submodules -j8 https://github.com/ttalexander2/chroma.git`
    - Execute `Setup.bat` to generate Visual Studio Solution.
    - Open Solution File.
    - Select Build > Build Solution.
     
##### Mac & Linux
- Currently does not support building on mac/linux, but support is on the way!

----
#### Notes
- Currently single threaded, plans for a multi-threaded renderer in the future.
- Audio is implemented using FMOD Studio, plans for a non-fmod audio backend may be implemted in the future.
- Only OpenGL renderer is implemented, plans for DirectX and Vulkan renderer are in the works.

---
#### Usage

Here is a quite simple application to draw a single square on the screen:
```cpp

#include "Chroma.h"
using namespace Chroma;

class App : public Application
{
public:
    void Init() override;               // Called on application start
    void OnEvent(Event& e) override;    // Called once per event
    void Update(Time time) override;    // Called once per frame

    void Draw(Time time) override       // Called once per frame
    {

        Chroma::RenderCommand::SetClearColor({ 0.0f, 0.0f , 0.0f , 1.0f });
		Chroma::RenderCommand::Clear();

        static Chroma::OrthographicCameraController cameraController(1920.0f / 1080.0f);
        Chroma::Renderer2D::BeginScene(cameraController.GetCamera());

		Chroma::Renderer2D::DrawQuad({ 0.5, 0.5 }, { 0.9, 0.9 }, {1,1,1,1});

		Chroma::Renderer2D::EndScene();
    }

    // This provides the engine with your application as the entry point 
    // is controlled by the engine.
    Application* CreateApplication()
    {
        return new App();
    }
}

```

The engine provides scene data structures which interface with the editor which this example does not make use of. The engine is designed to provide a suite of systems and structures to build games, but none of it is required in order to build a game.

---
#### Documentation

Please see our [Documentation Page](https://ttalexander2.github.io/chroma/) for more information and resources.
