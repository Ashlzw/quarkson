#pragma once

#include <memory>

class FileOperator
{
public:
	FileOperator();
	~FileOperator();

	virtual void FileOpen() = 0;
	virtual void FileClose() = 0;
	virtual void FileRead(char *buf, size_t buf_len) = 0;
	virtual void FileWrite(const char* buf, size_t buf_len) = 0;
	virtual void FileWriteAsync(const char* buf, size_t buf_len) = 0;
	virtual void FileReadAsync(char* buf, size_t buf_len) = 0;
	virtual void Flush() = 0;

private:

};

FileOperator::FileOperator()
{
}

FileOperator::~FileOperator()
{
}