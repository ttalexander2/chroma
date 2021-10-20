import os
import sys
import subprocess
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen

requiredVulkanVersion = "1.2.182.0"
vulkanDirectory = "./Chroma/third_party/VulkanSDK"

def CheckVulkanSDKDebugLibs():
    shadercdLib = Path(f"{vulkanDirectory}/Lib/shaderc_sharedd.lib")
    
    VulkanSDKDebugLibsURLlist = [
        f"https://sdk.lunarg.com/sdk/download/{requiredVulkanVersion}/windows/VulkanSDK-{requiredVulkanVersion}-DebugLibs.zip",
        f"https://files.lunarg.com/SDK-{requiredVulkanVersion}/VulkanSDK-{requiredVulkanVersion}-DebugLibs.zip"
    ]
    
    if not shadercdLib.exists():
        print(f"\nNo Vulkan SDK debug libs found. (Checked {shadercdLib})")
        vulkanPath = f"{vulkanDirectory}/VulkanSDK-{requiredVulkanVersion}-DebugLibs.zip"
        Utils.DownloadFile(VulkanSDKDebugLibsURLlist, vulkanPath)
        print("Extracting", vulkanPath)
        Utils.UnzipFile(vulkanPath, deleteZipFile=False)
        print(f"Vulkan SDK debug libs installed at {os.path.abspath(vulkanDirectory)}")
    else:
        print(f"\nVulkan SDK debug libs located at {os.path.abspath(vulkanDirectory)}")
    return True

if __name__ == "__main__":
    CheckVulkanSDKDebugLibs()