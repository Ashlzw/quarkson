#pragma once

namespace quarkson
{

enum class FileOperatorMode
{
	FILE_READ = 0,
	FILE_WRITE,
	FILE_APPEND,
};

class FileOperator
{
public:
	FileOperator() {}
	virtual ~FileOperator() {}

	virtual void FileOpen(const char *file_path, FileOperatorMode) = 0;
	virtual void FileClose() = 0;
	virtual void FileRead(void *buf, size_t buf_len) = 0;
	virtual void FileWrite(const void* buf, size_t buf_len) = 0;
	virtual int FileWriteAsync(const void* buf, size_t buf_len) = 0;
	virtual int FileReadAsync(void* buf, size_t buf_len) = 0;
	virtual unsigned long long GetFileSize() const = 0;
	virtual void Flush() = 0;

private:

};

}