workspace "Chroma"
    architecture "x64"

    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-${cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Chroma/third_party/GLFW/include"

include "Chroma/third_party/GLFW"

project "Chroma"
    location "Chroma"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    pchheader "chromapch.h"
    pchsource "%{prj.name}/src/chromapch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/third_party/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "CHROMA_PLATFORM_WINDOWS",
            "CHROMA_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }
    
    filter "configurations:Debug"
        defines "CHROMA_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        optimize "On"





project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Chroma/third_party/spdlog/include",
        "Chroma/src"
    }

    links
    {
        "Chroma"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "CHROMA_PLATFORM_WINDOWS",
        }
    
    filter "configurations:Debug"
        defines "CHROMA_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        optimize "On"
