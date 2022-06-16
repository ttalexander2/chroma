#include "chromapch.h"
#include "FileSystem.h"

#include <physfs.h>


namespace Chroma
{

    std::vector<std::string> FileSystem::_mounted;

    void FileSystem::Mount(const std::string& path)
    {
        CHROMA_CORE_INFO("Mounted: {}", path);
        int result = PHYSFS_mount(path.c_str(), NULL, 1);
        if (result == 0)
            throw FileSystemException(GetLastError());
        else
            _mounted.push_back(path);
    }

    void FileSystem::Unmount(const std::string& path)
    {
        int result = PHYSFS_unmount(path.c_str());
        if (result == 0)
            throw FileSystemException(GetLastError());
    }

    void FileSystem::UnmountAll()
    {
        bool success = true;
        for (auto& path : _mounted)
        {
            int result = PHYSFS_unmount(path.c_str());
            if (result == 0)
                success = false;
        }

        if (!success)
            throw FileSystemException(GetLastError());
    }


    std::string FileSystem::GetBaseDirectory()
	{
		return PHYSFS_getBaseDir();

    }
    FileSystem::Error FileSystem::GetLastErrorCode()
    {
        return (FileSystem::Error)PHYSFS_getLastErrorCode();
    }

    std::string FileSystem::GetLastError()
    {
        return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    std::string FileSystem::GetErrorByCode(FileSystem::Error errorCode)
    {
        return PHYSFS_getErrorByCode((PHYSFS_ErrorCode)errorCode);
    }

    bool FileSystem::Exists(const std::string& path)
    {
        return PHYSFS_exists(path.c_str()) != 0;
    }

    bool FileSystem::IsDirectory(const std::string& path)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(path.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
            return false;
        }
            
        return (statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY);
    }

    bool FileSystem::IsFile(const std::string& path)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(path.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
            return false;
        }
        return (statbuf.filetype == PHYSFS_FILETYPE_REGULAR);
	}

	bool FileSystem::CreateDir(const std::string &path)
	{
		return PHYSFS_mkdir(path.c_str()) != 0;
	}

    FileSystem::FileInfo FileSystem::GetFileInfo(const std::string& path)
    {
        FileInfo out;
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(path.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        out.AccessTime = statbuf.accesstime;
        out.CreateTime = statbuf.createtime;
        out.FileSize = statbuf.filesize;
        out.ModifiedTime = statbuf.modtime;
        out.Type = (FileSystem::FileType)statbuf.filetype;
        out.ReadOnly = statbuf.readonly != 0;
        return out;
    }

    int64_t FileSystem::GetFileSize(const std::string& filePath)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(filePath.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        return statbuf.filesize;

    }

    int64_t FileSystem::GetModifiedTime(const std::string& filePath)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(filePath.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        return statbuf.modtime;
    }

    int64_t FileSystem::GetCreateTime(const std::string& filePath)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(filePath.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        return statbuf.createtime;
    }

    int64_t FileSystem::GetAccessTime(const std::string& filePath)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(filePath.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        return statbuf.accesstime;
    }

    FileSystem::FileType FileSystem::GetFileType(const std::string& filePath)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(filePath.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        return (FileSystem::FileType)statbuf.filetype;
    }

    bool FileSystem::IsReadonly(const std::string& filePath)
    {
        PHYSFS_Stat statbuf;
        if (PHYSFS_stat(filePath.c_str(), &statbuf) == 0)
        {
            throw FileSystemException(GetLastError());
        }
        return statbuf.readonly != 0;
    }

    const std::string FileSystem::GetFileExtension(const std::string& filePath)
    {
        size_t pos = filePath.find_last_of('.');
        if (pos == std::string::npos)
            return "";

        return filePath.substr(pos);
    }

    bool FileSystem::HasFileExtension(const std::string& filePath, const std::string& extension)
    {
        return GetFileExtension(filePath) == extension;
    }

    std::vector<std::string> FileSystem::GetFileList(const std::string& directory)
    {
        std::vector<std::string> out;
        char** rc = PHYSFS_enumerateFiles(directory.c_str());
        if (rc == NULL)
        {
            throw FileSystemException(GetLastError());
            return out;
        }

        char** i;
        for (i = rc; *i != NULL; i++)
        {
            if (FileSystem::IsFile(*i))
                out.push_back(*i);
        }
        PHYSFS_freeList(rc);
        return out;
    }

    std::vector<std::string> FileSystem::GetFileListRecursive(const std::string& directory)
    {
        std::vector<std::string> out;
        char** rc = PHYSFS_enumerateFiles(directory.c_str());
        if (rc == NULL)
        {
            throw FileSystemException(GetLastError());
            return out;
        }

        char** i;
        for (i = rc; *i != NULL; i++)
        {
            std::string dir = directory + "/";
            if (directory.empty())
                dir = "";
            if (FileSystem::IsFile(dir + *i))
                out.push_back(dir +  *i);
            else if (FileSystem::IsDirectory(dir + *i))
            {
                auto recr = GetFileListRecursive(dir + *i);
                out.insert(out.end(), recr.begin(), recr.end());
            }
                
        }
        PHYSFS_freeList(rc);

        return out;
    }

    Chroma::File FileSystem::Open(const std::string& filePath, FileMode mode)
    {
        PHYSFS_File* file = NULL;
        if (mode == FileMode::Read)
        {
             file = PHYSFS_openRead(filePath.c_str());
        }
        else if (mode == FileMode::Write)
        {
            file = PHYSFS_openWrite(filePath.c_str());
        }
        else if (mode == FileMode::Append)
        {
            file = PHYSFS_openAppend(filePath.c_str());
        }
        Chroma::File res = Chroma::File(file);
        res.m_Path = filePath;
        return res;
    }

    void FileSystem::SetWriteDirectory(const std::string& directory)
    {
        if (PHYSFS_setWriteDir(directory.c_str()) == 0)
            throw FileSystemException(GetLastError());
    }


    FileSystem::FileIterator::FileIterator(const std::string& directory)
    {
        m_Files = FileSystem::GetFileList(directory);
    }

    FileSystem::RecursiveFileIterator::RecursiveFileIterator(const std::string& directory)
    {
        m_Files = FileSystem::GetFileListRecursive(directory);
    }

}

