#include <memory>
#include <iostream>

namespace quarkson
{

template<typename Encoding = char, typename Allocator = std::allocator<Encoding>>
class GenericStringBuffer
{
	typedef typename Encoding Ch;
public:
	static const size_t kDefaultBufferSize = 1024;

	explicit GenericStringBuffer(const Allocator & alloc) : _alloc(&alloc) {}
	~GenericStringBuffer()
	{
		FreeMemory();
	}

	GenericStringBuffer(const GenericStringBuffer & rhs) = delete;
	GenericStringBuffer& ioperator = (const GenericStringBuffer & rhs) = delete;

	GenericStringBuffer(GenericStringBuffer&& rhs)
	{
		FreeMemory();
		_start = rhs._start;
		_end = rhs._end;
		_tail = rhs._tail;
		rhs->SetNull();
	}

	GenericStringBuffer& operator = (GenericStringBuffer&& rhs)
	{
		if (this != &rhs)
		{
			FreeMemory();
			_start = rhs._start;
			_end = rhs._end;
			_tail = rhs._tail;
			rhs->SetNull();
		}
	}

	void PushBack(const Ch& c)
	{
		if (0 < RemainSpace())
		{
			*_end = c;
		}
		else
		{
			ExtendMemory();
			*_end = c;
		}
	}
	//void PushBack(Ch&& c);

	void Append(const GenericStringBuffer &append_buf)
	{
		while (RemainSpace() < append_buf.GetCapacity()) ExtendMemory();

		memcpy(_end, append_buf._start, append_buf.GetCapacity());
	}

	void Append(const Ch* start, const Ch* end)
	{
		while (RemainSpace() < (end - start)) ExtendMemory();

		memcpy(_end, append_buf._start, append_buf.GetCapacity());
	}

	void ShrinkToFit()
	{
		if (GetCapacity() != GetSize())
		{
			ResizeMemory(GetSize());
		}
	}

	void Clear()
	{
		_end = _start;
		_tail = _start;
	}

	size_t GetCapacity() const { return _tail - _start };
	size_t GetSize() const { return _end - _start };
	size_t GetLength() const { return GetCapacity() / sizeof(Ch); }

	size_t RemainSpace() const { return _tail - _end - 1; }

private:
	void SetNull()
	{
		_start = nullptr;
		_end = nullptr;
		_tail = nullptr;
	}

	void FreeMemory()
	{
		Ch* p = _start;
		size_t n = GetCapacity();
		SetNull();
		_alloc->deallocate(p, n);
	}

	void ExtendMemory()
	{
		size_t current_size = GetCapacity() == 0 ? 1 : GetCapacity();
		if (current_size)
		{
			size_t next_size = current_size << 1;
			ResizeMemory(next_size);
		}
	}

	void ResizeMemory(size_t next_size)
	{
		Ch* p = _alloc.allocate(next_size);
		try
		{
			FreeMemory();
			_start = p;
			_end = p + current_size;
			_tail = p + next_size;
		}
		catch (const std::exception& e)
		{
			std::cout << e.what();
			_alloc.deallocate(p, next_size);
		}
	}

	Allocator* _alloc;
	Ch* _start;
	Ch* _end;
	Ch* _tail;
};

}