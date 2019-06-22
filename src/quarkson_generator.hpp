#include <memory>
#include <stack>
#include <sstream>

#include "json.hpp"

namespace quarkson {

	const std::string true_str = "true";
	const std::string false_str = "false";
	const std::string null_str = "null";

template<typename OutputStream, typename TargetEncoding>
class Generator
{
public:
	Generator(OutputStream &stream) : _stream(&stream) {}
	Generator(std::shared_ptr<OutputStream> &stream) : _stream(stream) {}

	void Generate(const json& json_data)
	{
		const std::shared_ptr<json_value> p = json_data.get();

		GenerateHelp(p);
	}
private:
	typedef typename TargetEncoding TCh;

	void GenerateHelp(const std::shared_ptr<json_value>& json_ptr)
	{
		TCh comma = ',';
		switch (json_ptr->type())
		{
		case json::json_type::OBJECT:
		{
			TCh left_brace = '{';
			TCh right_brace = '}';
			TCh semicolon = ':';
			_stream->PushBack(left_brace);

			const json::object& objs = json_ptr->get_object();

			size_t obj_size = objs.size();
			size_t i = 0;
			for (auto& obj : objs)
			{
				++i;
				const std::string &key = obj.first;
				
				std::string encoding_str = UnicodeToEncoding(key);
				// process string
				_stream->Append(encoding_str.data(), encoding_str.data() + encoding_str.size());

				_stream->PushBack(semicolon);

				// process value

				GenerateHelp(obj.second);

				if (i != obj_size)
					_stream->PushBack(comma);
			}

			_stream->PushBack(right_brace);
		}
		break;
		case json::json_type::ARRAY:
		{
			TCh left_bracket = '[';
			TCh right_bracket = ']';

			const json::array& items = json_ptr->get_array();

			_stream->PushBack(left_bracket);

			size_t arr_size = items.size();
			size_t i = 0;

			for (auto &item : items)
			{
				++i;
				GenerateHelp(item);

				if (i != arr_size)
					_stream->PushBack(comma);
			}

			_stream->PushBack(right_bracket);
		}
		break;
		case json::json_type::STRING:
		{
			const std::string& str = json_ptr->get_string();

			std::string encoding_str = UnicodeToEncoding(str);
			
			_stream->Append(encoding_str.data(), encoding_str.data() + encoding_str.size());
		}
		break;
		case json::json_type::BOOLEAN:
		{
			if (json_ptr->get_bool())
				_stream->Append(true_str.data(), true_str.data() + true_str.size());
			else
				_stream->Append(false_str.data(), false_str.data() + false_str.size());
		}
		break;
		case json::json_type::NUL:
			_stream->Append(null_str.data(), null_str.data() + null_str.size());
			break;
		case json::json_type::NUMBER:
		{
			std::stringstream ss;
			ss << json_ptr->get_number();
			std::string num_str = ss.str();
			_stream->Append(num_str.data(), num_str.data() + num_str.size());
		}
		break;
		default:
			break;
		}
	}

	std::string UnicodeToEncoding(const std::string& source)
	{
		std::string ret_buf;
		const auto p = source.begin();
		TCh quote = '"';
		//_stream.PushBack(quote);
		ret_buf.push_back(quote);
		for (auto it = source.begin(); it != source.end(); ++it)
		{
			switch (*it)
			{
			case '\"':
				//_stream.PushBack('\\');
				//_stream.PushBack('\"');
				//continue;
			case '\\':
				//_stream.PushBack('\\');
				//_stream.PushBack('\\');
				//continue;
			case '/':
				//_stream.PushBack('\\');
				//_stream.PushBack('/');
				//continue;
			case '\b':
				//_stream.PushBack('\\');
				//_stream.PushBack('\b');
				//continue;
			case '\f':
				//_stream.PushBack('\\');
				//_stream.PushBack('\f');
				//continue;
			case '\n':
				//_stream.PushBack('\\');
				//_stream.PushBack('\n');
				//continue;
			case '\r':
				//_stream.PushBack('\\');
				//_stream.PushBack('\r');
				//continue;
			case '\t':
				ret_buf.push_back('\\');
				ret_buf.push_back(*it);
				continue;
			default:
				{
					if (*it <= 0x7f)
					{
						//_stream.PushBack(*it);
						ret_buf.push_back(*it);
					}
					else
					{
						//_stream.PushBack('u');
						ret_buf.push_back(*it);
						unsigned c = *it;
						unsigned int uni = 0;
						size_t len = 0;
						if (0x80 <= c && c <= 0xbf)
						{
							uni = 0x1f & *it++;
							len = 1;
						}
						else if (0xe0 <= c && c <= 0xef)
						{
							uni = 0x0f & *it++;
							len = 2;
						}
						else if (0xf0 <= c && c <= 0xf8)
						{
							uni = 0x08 & *it++;
							len = 3;
						}

						for (size_t i = 0; it != source.end() && i < len; ++i, ++it)
						{
							uni << 6;
							uni |= 0x3f & *it;
						}

						char tmp_buf[10] = { 0 };
						sprintf_s(tmp_buf, "%x", uni);

						ret_buf.append(tmp_buf, strlen(tmp_buf));
					}
				}
				break;
			}
		}
		//_stream.PushBack(quote);
		ret_buf.push_back(quote);

		return ret_buf;
	}

	std::shared_ptr<OutputStream> _stream;
};

}