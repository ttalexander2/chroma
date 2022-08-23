
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "Chroma/third_party/GLFW/include"
IncludeDir["Glad"] = "Chroma/third_party/Glad/include"
IncludeDir["ImGui"] = "Chroma/third_party/imgui"
IncludeDir["PhysicsFS"] = "Chroma/third_party/physfs/src"
IncludeDir["glm"] = "Chroma/third_party/glm"
IncludeDir["stb_image"] = "Chroma/third_party/stb_image"
IncludeDir["stb_truetype"] = "Chroma/third_party/stb_truetype"
IncludeDir["entt"] = "Chroma/third_party/entt"
IncludeDir["FMOD"] = "Chroma/third_party/fmod"
IncludeDir["yaml"] = "Chroma/third_party/yaml-cpp/include"
IncludeDir["cute_headers"] = "Chroma/third_party/cute_headers"
IncludeDir["concurrentqueue"] = "Chroma/third_party/concurrentqueue"
IncludeDir["readerwriterqueue"] = "Chroma/third_party/readerwriterqueue"
IncludeDir["ImGuizmo"] = "Chroma/third_party/ImGuizmo"
IncludeDir["fmt"] = "Chroma/third_party/fmt/include"
IncludeDir["mono"] = "Chroma/third_party/mono/include"
IncludeDir["nfd"] = "Chroma/third_party/nativefiledialog-extended/src/include"
IncludeDir["shaderc"] = "%{wks.location}/Chroma/third_party/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Chroma/third_party/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["miniz"] = "%{wks.location}/Chroma/third_party/miniz"
IncludeDir["box2d"] = "%{wks.location}/Chroma/third_party/box2d/include/box2d"
IncludeDir["msdfgen"] = "%{wks.location}/Chroma/third_party/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlasgen"] = "%{wks.location}/Chroma/third_party/msdf-atlas-gen"
IncludeDir["better_enums"] = "%{wks.location}/Chroma/third_party/better-enums"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Bin"] = "%{VULKAN_SDK}/Bin"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Chroma/third_party/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/Chroma/third_party/VulkanSDK/Bin"

Library = {}
Library["mono"] = "%{wks.location}/Chroma/third_party/mono/lib/mono-2.0-sgen.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
