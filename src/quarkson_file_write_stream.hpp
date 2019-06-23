#pragma once
#include <memory>

#include "quarkson_file_operator.hpp"

namespace quarkson
{
template<typename Encoding = char>
class GenericFileWriteStream
{
public:

	typedef typename Encoding Ch;

	GenericFileWriteStream(const std::shared_ptr<FileOperator> &fop_ptr) : _fop_ptr(fop_ptr) {}
	~GenericFileWriteStream() = default;

	void PushBack(Ch c);
	void Append(const Ch* begin, const Ch* end);

private:
	std::shared_ptr<FileOperator> _fop_ptr;
};

template<typename Encoding>
inline void GenericFileWriteStream<Encoding>::PushBack(Ch c)
{
	_fop_ptr->FileWrite(&c, sizeof(Ch));
}

template<typename Encoding>
inline void GenericFileWriteStream<Encoding>::Append(const Ch* begin, const Ch* end)
{
	_fop_ptr->FileWrite(begin, end - begin);
}

}