#pragma once
#include "quarkson_file_operator.hpp"

#include <memory>

#include <cstdlib>
#include <cstdio>

namespace quarkson
{
class SimpleFileOperator : public FileOperator 
{
public:

	static std::shared_ptr<FileOperator> CreateSimpleFileOperator(const char* file_path);

	SimpleFileOperator() : fp(0) {};

	void FileOpen(const char* file_path);
	void FileClose();
	void FileRead(void* buf, size_t buf_len);
	void FileWrite(const void* buf, size_t buf_len);
	int FileReadAsync(void* buf, size_t buf_len);
	int FileWriteAsync(const void* buf, size_t buf_len);
	void Flush();

	~SimpleFileOperator() { FileClose(); }

private:
	FILE* fp;
};
}