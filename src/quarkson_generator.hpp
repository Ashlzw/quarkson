#include <string>

#include "json.hpp"

namespace quarkson {

template<typename OutputStream>
class generator
{
public:
	static OutputStream generate(const json& j);
public:

private:

};

}