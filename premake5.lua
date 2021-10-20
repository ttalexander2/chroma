workspace "Chroma"
    architecture "x64"
    startproject "Polychrome"
    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }

    flags
	{
		"MultiProcessorCompile"
	}



outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Dependencies.lua"

group "Dependencies"
    include "Chroma/third_party/GLFW"
    include "Chroma/third_party/Glad"
    include "Chroma/third_party/imgui"
    include "Chroma/third_party/glm"
    include "Chroma/third_party/lua"
group "" -- end of dependencies


project "Chroma"
    location "Chroma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")
    pchheader "chromapch.h"
    pchsource "%{prj.name}/src/chromapch.cpp"

    dependson { "Chroma.Mono" }

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",

        "%{prj.name}/third_party/yaml-cpp/src/**.cpp",
		"%{prj.name}/third_party/yaml-cpp/src/**.h",
		"%{prj.name}/third_party/yaml-cpp/include/**.h",

        "%{prj.name}/third_party/ImGuizmo/**.cpp"

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
        "%{prj.name}/third_party",
        "%{prj.name}/third_party/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/misc/cpp",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.stb_truetype}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.yaml}",
        "%{IncludeDir.cute_headers}",
        "%{IncludeDir.concurrentqueue}",
        "%{IncludeDir.readerwriterqueue}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Sol2}",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.mono}",
        "%{IncludeDir.VulkanSDK}"

    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        "lua",
        "%{Library.mono}"
    }

    filter "files:Chroma/third_party/ImGuizmo/**.cpp or files:Chroma/third_party/yaml-cpp/src/**.cpp"
        flags { "NoPCH" }

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
        links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}


    filter "configurations:Release"
        defines 
        {
            "CHROMA_RELEASE"
        }
        runtime "Release"
        optimize "On"
        links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "On"
        links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}


    filter { "files:**.hpp" }
        optimize "Speed"
    
    filter { "files:**.inl" }
        optimize "Speed"

    configuration "vs*"
        buildoptions { "/bigobj" }    -- Lua bindings made me do this :(


project "Polychrome"
    location "Polychrome"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    dependson { "Chroma.Mono" }

    links
    {
        "Chroma"
    }

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "Chroma/src",
        "Chroma/third_party/spdlog/include",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/misc/cpp",
        "%{IncludeDir.entt}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.yaml}",
        "%{IncludeDir.concurrentqueue}",
        "%{IncludeDir.readerwriterqueue}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Sol2}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.mono}"
    }


    postbuildcommands
    {
        "{ECHO} Copying %{wks.location}Chroma/lib/Windows/x64/fmodstudioL.dll to %{cfg.targetdir}",
        "{COPYFILE} %{wks.location}Chroma/lib/Windows/x64/fmodstudioL.dll %{cfg.targetdir}",
        "{ECHO} Copying %{wks.location}Chroma/lib/Windows/x64/fmodL.dll to %{cfg.targetdir}",
        "{COPYFILE} %{wks.location}Chroma/lib/Windows/x64/fmodL.dll %{cfg.targetdir}",
        "{ECHO} Copying assets to %{cfg.targetdir}/assets",
        "{COPYDIR} assets %{cfg.targetdir}/assets",
        "{ECHO} Copying mono dependencies to %{cfg.targetdir}",
        "{COPYDIR} %{prj.location}mono %{cfg.targetdir}/mono",
        "{ECHO} Copying %{wks.location}Chroma/third_party/mono/bin/mono-2.0-sgen.dll to %{cfg.targetdir}",
        '{COPYFILE} "%{wks.location}Chroma/third_party/mono/bin/mono-2.0-sgen.dll" "%{cfg.targetdir}"',
        "{ECHO} Copying imgui.ini to %{cfg.targetdir}",
        "{COPYFILE} imgui.ini %{cfg.targetdir}",
        "{COPYFILE} Chroma.Mono.dll %{cfg.targetdir}"
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
        prebuildcommands {
            "{ECHO} Building Chroma.Mono.dll with mcs...",
            "%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -debug -target:library -nostdlib -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/*.cs",
        }
        postbuildcommands
		{
			"{COPYDIR} \"%{LibraryDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\""
		}

    filter "configurations:Release"
        defines "CHROMA_RELEASE"
        runtime "Release"
        optimize "On"
        prebuildcommands {
            "{ECHO} Building Chroma.Mono.dll with mcs...",
            "%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -target:library -nostdlib -optimize -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/*.cs",
        }
        postbuildcommands{
            "{ECHO} Copying Vulkan dlls...",
            "{ECHO} \"%{LibraryDir.VulkanSDK_Bin}/shaderc_shared.dll\" %{cfg.targetdir}",
            "{COPYFILE} \"%{LibraryDir.VulkanSDK_Bin}/shaderc_shared.dll\" %{cfg.targetdir}",
            "{ECHO} \"%{LibraryDir.VulkanSDK_Bin}/spirv-cross-c-shared.dll\" %{cfg.targetdir}",
            "{COPYFILE} \"%{LibraryDir.VulkanSDK_Bin}/spirv-cross-c-shared.dll\" %{cfg.targetdir}",
            "{ECHO} \"%{LibraryDir.VulkanSDK_Bin}/SPIRV-Tools-shared.dll\" %{cfg.targetdir}",
            "{COPYFILE} \"%{LibraryDir.VulkanSDK_Bin}/SPIRV-Tools-shared.dll\" %{cfg.targetdir}"
        }

    filter "configurations:Dist"
        defines "CHROMA_DIST"
        runtime "Release"
        optimize "On"
        prebuildcommands {
            "{ECHO} Building Chroma.Mono.dll with mcs...",
            '{ECHO}"%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -target:library -nostdlib -optimize -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/*.cs"',
            "%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -target:library -nostdlib -optimize -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/*.cs",
        }
        postbuildcommands{
            "{ECHO} Copying Vulkan dlls...",
            "{ECHO} \"%{LibraryDir.VulkanSDK_Bin}/shaderc_shared.dll\" %{cfg.targetdir}",
            "{COPYFILE} \"%{LibraryDir.VulkanSDK_Bin}/shaderc_shared.dll\" %{cfg.targetdir}",
            "{ECHO} \"%{LibraryDir.VulkanSDK_Bin}/spirv-cross-c-shared.dll\" %{cfg.targetdir}",
            "{COPYFILE} \"%{LibraryDir.VulkanSDK_Bin}/spirv-cross-c-shared.dll\" %{cfg.targetdir}",
            "{ECHO} \"%{LibraryDir.VulkanSDK_Bin}/SPIRV-Tools-shared.dll\" %{cfg.targetdir}",
            "{COPYFILE} \"%{LibraryDir.VulkanSDK_Bin}/SPIRV-Tools-shared.dll\" %{cfg.targetdir}"
        }



project "Runtime"
    location "Runtime"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs
    {
        "Chroma/third_party/spdlog/include",
        "Chroma/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.yaml}",
        "%{IncludeDir.Sol2}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.entt}"

    }

    links
    {
        "Chroma"
    }


    postbuildcommands
    {
        "{ECHO} Copying ../Chroma/lib/Windows/x64/fmodstudioL.dll to %{cfg.targetdir}",
        "{COPYFILE} ../Chroma/lib/Windows/x64/fmodstudioL.dll %{cfg.targetdir}",
        "{ECHO} Copying ../Chroma/lib/Windows/x64/fmodL.dll to %{cfg.targetdir}",
        "{COPYFILE} ../Chroma/lib/Windows/x64/fmodL.dll %{cfg.targetdir}",
        "{ECHO} Copying assets to %{cfg.targetdir}/assets",
        "{COPYDIR} assets %{cfg.targetdir}/assets",
        "{ECHO} Copying ../Chroma/third_party/mono/bin/mono-2.0-sgen.dll to %{cfg.targetdir}",
        '{COPYFILE} "../Chroma/third_party/mono/bin/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
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




project "Chroma.Mono"
    location "Script-Core"
    kind "None"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Script-Core/Source/**.cs",
    }

    filter "configurations:Debug"
        buildcommands {
            "{ECHO} Building Chroma.Mono.dll with mcs...",
            "%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -debug -target:library -nostdlib -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/**.cs",
        }

    filter "configurations:Release"
        buildcommands {
            "{ECHO} Building Chroma.Mono.dll with mcs...",
            "%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -target:library -nostdlib -optimize -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/**.cs",
        }

    filter "configurations:Dist"
        buildcommands {
            "{ECHO} Building Chroma.Mono.dll with mcs...",
            "%{prj.location}mono/bin/mono.exe %{prj.location}mono/lib/mono/4.5/mcs.exe -target:library -nostdlib -optimize -out:%{wks.location}bin/" .. outputdir .. "/%{prj.name}/Chroma.Mono.dll -r:%{prj.location}mono/lib/mono/4.5/mscorlib.dll,%{prj.location}mono/lib/mono/4.5/System.dll,%{prj.location}mono/lib/mono/4.5/System.Core.dll -recurse:%{wks.location}Script-Core/Source/**.cs",
        }