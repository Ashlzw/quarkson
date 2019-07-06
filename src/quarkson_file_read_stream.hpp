#pragma once
#include <memory>
#include <string>
#include <vector>

#include "quarkson_file_operator.hpp"

namespace quarkson
{

template<typename Encoding = char>
class GenericFileReadStream
{
public:

	typedef typename Encoding Ch;

	GenericFileReadStream(const std::shared_ptr<FileOperator>& fop_ptr) : _fop_ptr(fop_ptr) {}
	~GenericFileReadStream() = default;

	std::basic_string<Ch> GetString()
	{
		uint64_t sz = _fop_ptr->GetFileSize();
		std::vector<Ch> tmp_data(sz);

		_fop_ptr->FileRead(tmp_data.data(), sz / sizeof(Ch));

		return { tmp_data.begin(), tmp_data.end() };
	}

private:
	std::shared_ptr<FileOperator> _fop_ptr;
};

}