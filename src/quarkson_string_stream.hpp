#include <memory>

namespace quarkson
{

template<typename Encoding = char, typename Allocator = std::allocator<Encoding>>
class GenericStringBuffer
{
	typedef typename Encoding Ch;
public:
	static const size_t kDefaultBufferSize = 1024;

	explicit GenericStringBuffer(const Allocator & alloc) : _alloc(&alloc) {}
	
	GenericStringBuffer(const GenericStringBuffer & rhs) = delete;
	GenericStringBuffer& ioperator = (const GenericStringBuffer & rhs) = delete;

	GenericStringBuffer(GenericStringBuffer && rhs);
	GenericStringBuffer& ioperator = (GenericStringBuffer && rhs);

	void PushBack(const Ch& c);
	void PushBack(Ch&& c);

	void Append(const GenericStringBuffer &);
	void Append(Ch *start, Ch *end);

	void ShrinkToFit();
	void Clear();

	size_t GetSize() const { return _tail - _start };
	size_t GetLength() const { return GetSize() / sizeof(Ch); }

private:
	Allocator* _alloc;
	Ch* _start;
	Ch* _end;
	Ch* _tail;
};

}