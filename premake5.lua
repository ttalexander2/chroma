workspace "Chroma"
    architecture "x64"
    startproject "Sandbox"
    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Chroma/third_party/GLFW/include"
IncludeDir["Glad"] = "Chroma/third_party/Glad/include"
IncludeDir["ImGui"] = "Chroma/third_party/imgui"
IncludeDir["glm"] = "Chroma/third_party/glm"
IncludeDir["stb_image"] = "Chroma/third_party/stb_image"

group "Dependencies"
    include "Chroma/third_party/GLFW"
    include "Chroma/third_party/Glad"
    include "Chroma/third_party/imgui"
    include "Chroma/third_party/glm"
group "" -- end of dependencies


project "Chroma"
    location "Chroma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    pchheader "chromapch.h"
    pchsource "%{prj.name}/src/chromapch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/third_party/stb_image/**.h",
        "%{prj.name}/third_party/stb_image/**.cpp",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/third_party/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"

    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "CHROMA_PLATFORM_WINDOWS",
            "CHROMA_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
        }
    
    filter "configurations:Debug"
        defines "CHROMA_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "on"





project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

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
        "Chroma/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}"
    }

    links
    {
        "Chroma"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "CHROMA_PLATFORM_WINDOWS",
        }
    
    filter "configurations:Debug"
        defines "CHROMA_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "on"


project "Polychrome"
    location "Polychrome"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

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
        "Chroma/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}"
    }

    links
    {
        "Chroma"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "CHROMA_PLATFORM_WINDOWS",
        }
    
    filter "configurations:Debug"
        defines "CHROMA_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "on"
