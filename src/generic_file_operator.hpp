#pragma once

namespace quarkson
{

class FileOperator
{
public:
	FileOperator() {}
	virtual ~FileOperator() = 0;

	virtual void FileOpen() = 0;
	virtual void FileClose() = 0;
	virtual void FileRead(char *buf, size_t buf_len) = 0;
	virtual void FileWrite(const char* buf, size_t buf_len) = 0;
	virtual int FileWriteAsync(const char* buf, size_t buf_len) = 0;
	virtual int FileReadAsync(char* buf, size_t buf_len) = 0;
	virtual void Flush() = 0;

private:

};

}