#pragma once
#include <string>
#include <exception>

#include "File.h"

namespace Chroma
{
	struct File;

	class FileSystem
	{
	public:
		// Analogous to PHYSFS errors
		enum Error
		{
			OK,
			/**< Success; no error.                    */
			OTHER_ERROR,
			/**< Error not otherwise covered here.     */
			OUT_OF_MEMORY,
			/**< Memory allocation failed.             */
			NOT_INITIALIZED,
			/**< PhysicsFS is not initialized.         */
			IS_INITIALIZED,
			/**< PhysicsFS is already initialized.     */
			ARGV0_IS_NULL,
			/**< Needed argv[0], but it is NULL.       */
			UNSUPPORTED,
			/**< Operation or feature unsupported.     */
			PAST_EOF,
			/**< Attempted to access past end of file. */
			FILES_STILL_OPEN,
			/**< Files still open.                     */
			INVALID_ARGUMENT,
			/**< Bad parameter passed to an function.  */
			NOT_MOUNTED,
			/**< Requested archive/dir not mounted.    */
			NOT_FOUND,
			/**< File (or whatever) not found.         */
			SYMLINK_FORBIDDEN,
			/**< Symlink seen when not permitted.      */
			NO_WRITE_DIR,
			/**< No write dir has been specified.      */
			OPEN_FOR_READING,
			/**< Wrote to a file opened for reading.   */
			OPEN_FOR_WRITING,
			/**< Read from a file opened for writing.  */
			NOT_A_FILE,
			/**< Needed a file, got a directory (etc). */
			READ_ONLY,
			/**< Wrote to a read-only filesystem.      */
			CORRUPT,
			/**< Corrupted data encountered.           */
			SYMLINK_LOOP,
			/**< Infinite symbolic link loop.          */
			IO,
			/**< i/o error (hardware failure, etc).    */
			PERMISSION,
			/**< Permission denied.                    */
			NO_SPACE,
			/**< No space (disk full, over quota, etc) */
			BAD_FILENAME,
			/**< Filename is bogus/insecure.           */
			BUSY,
			/**< Tried to modify a file the OS needs.  */
			DIR_NOT_EMPTY,
			/**< Tried to delete dir with files in it. */
			OS_ERROR,
			/**< Unspecified OS-level error.           */
			DUPLICATE,
			/**< Duplicate entry.                      */
			BAD_PASSWORD,
			/**< Bad password.                         */
			APP_CALLBACK /**< Application callback reported error.  */
		};

		enum FileType
		{
			File,
			Directory,
			SymbolicLink,
			Other
		};

		struct FileInfo
		{
			int64_t FileSize;
			int64_t ModifiedTime;
			int64_t CreateTime;
			int64_t AccessTime;
			FileType Type;
			bool ReadOnly;
		};

		class FileSystemException : public std::exception
		{
		public:
			explicit FileSystemException(const char *message) :
				msg_(message)
			{
			}

			explicit FileSystemException(const std::string &message) :
				msg_(message)
			{
			}

			~FileSystemException() noexcept override
			= default;

			const char *what() const noexcept override
			{
				return msg_.c_str();
			}

		protected:
			std::string msg_;
		};


		struct FileIterator
		{
			FileIterator(const std::string &directory);

			std::vector<std::string>::const_iterator begin() { return m_Files.cbegin(); }
			std::vector<std::string>::const_iterator end() { return m_Files.cend(); }

		private:
			std::vector<std::string> m_Files;
		};

		struct RecursiveFileIterator
		{
			RecursiveFileIterator(const std::string &directory);

			std::vector<std::string>::const_iterator begin() { return m_Files.cbegin(); }
			std::vector<std::string>::const_iterator end() { return m_Files.cend(); }

		private:
			std::vector<std::string> m_Files;
		};


		static void Mount(const std::string &path);
		static void Unmount(const std::string &path);
		static void UnmountAll();

		static Error GetLastErrorCode();
		static std::string GetLastError();
		static std::string GetErrorByCode(Error errorCode);

		static bool Exists(const std::string &path);

		static std::string GetBaseDirectory();

		static bool IsDirectory(const std::string &path);
		static bool IsFile(const std::string &path);

		static bool CreateDir(const std::string &path);

		static FileInfo GetFileInfo(const std::string &filePath);
		static int64_t GetFileSize(const std::string &filePath);
		static int64_t GetModifiedTime(const std::string &filePath);
		static int64_t GetCreateTime(const std::string &filePath);
		static int64_t GetAccessTime(const std::string &filePath);
		static FileType GetFileType(const std::string &filePath);
		static bool IsReadonly(const std::string &filePath);
		static const std::string GetFileExtension(const std::string &filePath);
		static bool HasFileExtension(const std::string &filePath, const std::string &extension);

		static std::vector<std::string> GetFileList(const std::string &directory);
		static std::vector<std::string> GetFileListRecursive(const std::string &directory);

		static Chroma::File Open(const std::string &filePath, FileMode mode = FileMode::Read);

		static void SetWriteDirectory(const std::string &directory);

	private:
		static std::vector<std::string> s_Mounted;
	};
}

/* EXAMPLE
try
        {
            Chroma::File file = Chroma::FileSystem::Open("Scenes/Test.chroma");
            CHROMA_CORE_TRACE("Result: {}", file.ReadString());
            file.Seek(0);
            CHROMA_CORE_INFO("{}", file.ReadLine());
            CHROMA_CORE_INFO("{}", file.ReadLine());
            file.Seek(0);

            std::vector<char> buffer;
            buffer.resize(20);

            file.Read<char>(buffer);
            for (auto c : buffer)
            {
                CHROMA_CORE_INFO("{}", (int)c);
            }

            file.Seek(0);
            buffer.resize(file.Length());
            file.Read(buffer);
            file.Close();

            Chroma::FileSystem::SetWriteDirectory("C:\\Users\\Thomas\\source\\repos\\Aesix\\Assets");
            Chroma::File file2 = Chroma::FileSystem::Open("Test2.txt", Chroma::FileMode::Write);
            if (file2.Good())
            {
                file2.Write("Test");
                file2.Flush();
            }
            file2.Close();
        }
        catch (Chroma::FileSystem::FileSystemException e)
        {
            CHROMA_CORE_ERROR("{}", e.what());
        }


        try
        {
            for (const auto& file : Chroma::FileSystem::RecursiveFileIterator(""))
            {
                CHROMA_CORE_WARN("{}", file);
            }
        }
        catch (Chroma::FileSystem::FileSystemException e)
        {
            CHROMA_CORE_ERROR("{}", e.what());
        }
*/
