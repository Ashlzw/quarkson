#pragma warning(disable : 4996)
#include "simple_file_operator.hpp"

#include <exception>

std::shared_ptr<quarkson::FileOperator> quarkson::SimpleFileOperator::CreateSimpleFileOperator(const char *file_path)
{
	std::shared_ptr<FileOperator> fp(new SimpleFileOperator());

	fp->FileOpen(file_path);

	return fp;
}

void quarkson::SimpleFileOperator::FileOpen(const char* file_path)
{
	fp = fopen(file_path, "wb");

	if (!fp)
		throw std::exception("file open fail");
}

void quarkson::SimpleFileOperator::FileClose()
{
	//if (fp)
	//{
	//	int rc = fclose(fp);
	//	int errno_code = errno;
	//	if (!rc)
	//		throw std::exception("file close fail");
	//}

	if (fp && fclose(fp))
		throw std::exception("file close fail");
	fp = NULL;
}

void quarkson::SimpleFileOperator::FileRead(void* buf, size_t buf_len)
{
	if (fp)
		fread(buf, sizeof(char), buf_len, fp);
}

void quarkson::SimpleFileOperator::FileWrite(const void* buf, size_t buf_len)
{
	if (fp)
		fwrite(buf, sizeof(char), buf_len, fp);
}

int quarkson::SimpleFileOperator::FileWriteAsync(const void* buf, size_t buf_len)
{
	if (fp)
		FileWrite(buf, buf_len);
	return 0;
}

int quarkson::SimpleFileOperator::FileReadAsync(void* buf, size_t buf_len)
{
	if (fp)
		FileRead(buf, buf_len);
	return 0;
}

void quarkson::SimpleFileOperator::Flush()
{
	if (fp)
		fflush(fp);
}
