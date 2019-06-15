#include <memory>
#include "json.hpp"

namespace quarkson {

template<typename OutputStream>
class Generator
{
public:
	Generator(OutputStream &stream) : _stream(&stream) {}
	Generator(std::shared_ptr<OutputStream> &stream) : _stream(stream) {}

	void Generate(const json&);
private:
	std::shared_ptr<OutputStream> _stream;
};

}