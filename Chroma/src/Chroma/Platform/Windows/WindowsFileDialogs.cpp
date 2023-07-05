#include "chromapch.h"
#include "Chroma/Utilities/FileDialogs.h"
#include "Chroma/Core/Application.h"

#include <codecvt>

#if _WIN32
#include <commdlg.h>
#include <shobjidl.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace Chroma
{
	std::string FileDialogs::OpenFile(const char *filter, void *nativeHandle)
	{
#if _WIN32
		if (nativeHandle == nullptr)
			nativeHandle = Application::Get().GetWindow().GetNativeWindow();
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(nativeHandle));
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
#endif
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char *filter, void *nativeHandle)
	{
#if _WIN32
		if (nativeHandle == nullptr)
			nativeHandle = Application::Get().GetWindow().GetNativeWindow();
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(nativeHandle));
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
#endif
		return std::string();
	}

	std::string FileDialogs::OpenDirectory(void *nativeHandle)
	{
#if _WIN32

		if (nativeHandle == nullptr)
			nativeHandle = Application::Get().GetWindow().GetNativeWindow();

		IFileOpenDialog *pFileOpen;

		auto hr = CoCreateInstance(CLSID_FileOpenDialog,
				nullptr,
				CLSCTX_ALL,
				IID_IFileOpenDialog,
				reinterpret_cast<void **>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetOptions(FOS_PICKFOLDERS);
			hr = pFileOpen->Show(glfwGetWin32Window(static_cast<GLFWwindow *>(nativeHandle)));

			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						std::wstring ss(pszFilePath);

						// No replacement exists in the standard, so we'll use this.
						// Could use Win32 functions, but Win32 is annoying and i'm lazy
						// ReSharper disable once CppDeprecatedEntity
						return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(ss);
					}
				}
			}
		}
#endif
		return std::string();
	}
}
