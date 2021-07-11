workspace "Chroma"
    architecture "x64"
    startproject "Polychrome"
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
IncludeDir["entt"] = "Chroma/third_party/entt"
IncludeDir["FMOD"] = "Chroma/third_party/fmod"
IncludeDir["lua"] = "Chroma/third_party/lua"
IncludeDir["yaml"] = "Chroma/third_party/yaml-cpp/include"

group "Dependencies"
    include "Chroma/third_party/GLFW"
    include "Chroma/third_party/Glad"
    include "Chroma/third_party/imgui"
    include "Chroma/third_party/glm"
    include "Chroma/third_party/lua"
    include "Chroma/third_party/yaml-cpp"
group "" -- end of dependencies


project "Chroma"
    location "Chroma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
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
        "%{prj.name}/third_party/entt/**.hpp",
        "%{prj.name}/third_party/lua/**.h",
        "%{prj.name}/third_party/yaml-cpp/include/yaml-cpp/**.h"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLM_FORCE_MESSAGES",
        "GLM_FORCE_INTRINSICS",
        "GLM_FORCE_INLINE",
        "GLM_FORCE_ALIGNED_GENTYPES"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/third_party/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/misc/cpp",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.yaml}"

    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        "yaml-cpp"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "CHROMA_PLATFORM_WINDOWS",
            "CHROMA_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        links
        {
            "Chroma/lib/Windows/x64/fmodL_vc.lib",
            "Chroma/lib/Windows/x64/fmodstudioL_vc.lib"
        }
    
    filter "configurations:Debug"
        defines 
        {
            "CHROMA_DEBUG"
        }
        runtime "Debug"
        symbols "On"
        optimize "On"


    filter "configurations:Release"
        defines 
        {
            "CHROMA_RELEASE"
        }
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "On"


    filter { "files:**.hpp" }
        optimize "Speed"
    
    filter { "files:**.inl" }
        optimize "Speed"




project "Runtime"
    location "Runtime"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
        "%{prj.name}/third_party/yaml-cpp/include/yaml-cpp/**.h"
    }

    includedirs
    {
        "Chroma/third_party/spdlog/include",
        "Chroma/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.yaml}"

    }

    links
    {
        "Chroma"
    }


    postbuildcommands
    {
        "{ECHO} Copying ../Chroma/lib/Windows/x64/fmodstudioL.dll to %{cfg.targetdir}",
        "{COPY} ../Chroma/lib/Windows/x64/fmodstudioL.dll %{cfg.targetdir}",
        "{ECHO} Copying ../Chroma/lib/Windows/x64/fmodL.dll to %{cfg.targetdir}",
        "{COPY} ../Chroma/lib/Windows/x64/fmodL.dll %{cfg.targetdir}",
        "{ECHO} Copying assets to %{cfg.targetdir}/assets",
        "{COPY} assets %{cfg.targetdir}/assets",
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
        symbols "On"
        optimize "Debug"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "On"


project "Polychrome"
    location "Polychrome"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
        "%{prj.name}/third_party/yaml-cpp/include/yaml-cpp/**.h"
    }

    includedirs
    {
        "Chroma/third_party/spdlog/include",
        "Chroma/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/misc/cpp",
        "%{IncludeDir.entt}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.yaml}"

    }

    links
    {
        "Chroma"
    }


    postbuildcommands
    {
        "{ECHO} Copying ../Chroma/lib/Windows/x64/fmodstudioL.dll to %{cfg.targetdir}",
        "{COPY} ../Chroma/lib/Windows/x64/fmodstudioL.dll %{cfg.targetdir}",
        "{ECHO} Copying ../Chroma/lib/Windows/x64/fmodL.dll to %{cfg.targetdir}",
        "{COPY} ../Chroma/lib/Windows/x64/fmodL.dll %{cfg.targetdir}",
        "{ECHO} Copying assets to %{cfg.targetdir}/assets",
        "{COPY} assets %{cfg.targetdir}/assets",
    }

    defines
    {
        CHROMA_EDITOR
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
        symbols "On"
        optimize "Debug"

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "On"