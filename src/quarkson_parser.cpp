#include "quarkson_parser.hpp"

#include <cstring>
#include <cctype>
#include <cmath>
 
namespace quarkson {

json quarkson::parser::parse(const string &s)
{
	string err;
	json tmp = parse(s, err);
	return tmp;
}

json quarkson::parser::parse(const string &s, const string &err)
{
	parser p(s);
	shared_ptr<json_value> jv = p.parse_value();
	json j = json(jv);
	return j;
}

shared_ptr<json_value> quarkson::parser::parse_value()
{
	skip_space();
	switch (*p)
	{
	case 't': 
	case 'f':
	case 'n':
		return parse_literal();
	case '\"':
		return parse_string();
	case '[':
		return parse_array();
	case '{':
		return parse_object();
	case '\0':
		return shared_ptr<json_value>();
	default:
		return parse_number();
	}

	return nullptr;
}

shared_ptr<json_value> quarkson::parser::parse_object()
{
	json::object obj;

	skip_space();
	if (*p++ == '{')
		skip_space();
	else
		return json_value::error_instance();

	if (*p == '}')
	{
		++p;
		return json_value::object_instance(obj);
	}
	else
	{
		while (*p)
		{
			string key = parse_string()->get_string();

			skip_space();
			if (*p++ == ':')
				skip_space();
			else
				return json_value::error_instance();

			shared_ptr<json_value> value = parse_value();
		
			obj.insert(std::make_pair(std::move(key), std::move(value)));
			skip_space();
			char c = *p++;

			if (c == ',')
				skip_space();
			else if (c == '}')
				return json_value::object_instance(obj);
			else
				return json_value::error_instance();
		}
	}

	return json_value::error_instance();
}

shared_ptr<json_value> parser::parse_array()
{
	json::array arr;

	skip_space();
	if (*p++ == '[')
		skip_space();
	else
		return json_value::error_instance();

	if (*p == ']')
	{
		++p;
		return json_value::array_instance(arr);
	}
	else
		while (*p)
		{
			arr.push_back(parse_value());
			skip_space();
			char c = *p++;

			if (c == ',')
				skip_space();
			else if (c == ']')
				return json_value::array_instance(arr);
			else
				return json_value::error_instance();
		}

	return json_value::error_instance();
}

shared_ptr<json_value> quarkson::parser::parse_string()
{
	const char *c = p;
	if (*c == '\"') ++c;
	else return json_value::error_instance();

	if (*c == '\"')
	{
		++c;
		p = c;
		return json_value::string_instance("");
	}
	string str;

	while (*c)
	{
		if (*c == '\\')
		{
			++c;
			switch (*c)
			{
			case '\"':
				str.push_back('\"');
				++c;
				continue;
			case '\\':
				str.push_back('\\');
				++c;
				continue;
			case '/':
				str.push_back('/');
				++c;
				continue;
			case 'b':
				str.push_back('\b');
				++c;
				continue;
			case 'f':
				str.push_back('\f');
				++c;
				continue;
			case 'n':
				str.push_back('\n');
				++c;
				continue;
			case 'r':
				str.push_back('\r');
				++c;
				continue;
			case 't':
				str.push_back('\t');
				++c;
				continue;
			case 'u':
				++c;
				unsigned int uni = 0;
				c = parse_hex4(c, uni);
				if (c == nullptr)
					return json_value::error_instance();
				if (uni >= 0xD800 && uni <= 0xDBFF) 
				{
					unsigned int uni2 = 0;
					if (*c++ != '\\')
						return json_value::error_instance();
					if (*c++ != 'u')
						return json_value::error_instance();
					if (!(c = parse_hex4(c, uni2)))
						return json_value::error_instance();
					uni = ((uni - 0xD800) << 10 | (uni2 - 0xDC00)) + 0x10000;
				}
				str += encode_utf8(uni);
			}
		}
		else if (*c == '\"')
		{
			++c;
			p = c;
			return json_value::string_instance(str);
		}
		else
			str.push_back(*c++);
	}
	return json_value::error_instance();
}

shared_ptr<json_value> quarkson::parser::parse_number()
{
	const char *c = p;
	char *e;
	int64_t i64 = 0;
	bool is_miuns = false;
	bool use_double = false;
	if (*c == '-')
	{
		++c;
		is_miuns = true;
	}

	if (*c == '0') ++c;
	else if (isdigit1to9(*c))
	{
		do
		{
			i64 = i64 * 10 + static_cast<unsigned>(*c - '0');
			++c;

			if (i64 > MAX_SIGNAL_LL || *c == '.')
			{
				use_double = true;
				break;
			}
		} while (isdigit(*c));
	}
	else return json_value::error_instance();

	if (*c == '.')
		use_double = true;

	if (use_double)
	{
		if (*c == '.')
		{
			do
			{
				++c;
			} while (isdigit(*c));
		}

		if (*c != 'e' && *c != 'E')
		{
			errno = 0;
			double num = std::strtod(p, &e);
			if (errno != ERANGE)
			{
				p = e;
				return json_value::number_instance(num);
			}
			else
				return json_value::error_instance();
		}
		else
		{
			do
			{
				++c;
			} while (isdigit(*c) || *c == '+' || *c == '-');
		}

		errno = 0;
		double num = std::strtod(p, &e);
		if (errno != ERANGE)
		{
			p = e;
			return json_value::number_instance(num);
		}
		else
			return json_value::error_instance();
	}
	else
	{
		p = c;
		return is_miuns ? json_value::number_instance(-i64) : json_value::number_instance(i64);
	}
}

shared_ptr<json_value> quarkson::parser::parse_literal()
{
	const char *c = p;
	switch (*c)
	{
	case 'n':
		if (strncmp(c, "null", 4) == 0)
		{
			c += 4;
			p = c;
			return json_value::null_instance();
		}
		break;
	case 't':
		if (strncmp(c, "true", 4) == 0)
		{
			c += 4;
			p = c;
			return json_value::bool_instance(true);
		}
		break;
	case 'f':
		if (strncmp(c, "false", 5) == 0)
		{
			c += 5;
			p = c;
			return json_value::bool_instance(false);
		}
		break;
	}

	return json_value::error_instance();
}

const char * quarkson::parser::parse_hex4(const char * p, unsigned int &uni)
{
	uni = 0;
	for (size_t i = 0; i < 4; ++i)
	{
		char ch = *p++;
		uni <<= 4;
		if (ch >= '0' && ch <= '9') uni |= ch - '0';
		else if (ch >= 'A' && ch <= 'F') uni |= ch - 'A' + 10;
		else if (ch >= 'a' && ch <= 'f') uni |= ch - 'a' + 10;
		else return nullptr;
	}
	return p;
}

const string quarkson::parser::encode_utf8(unsigned int uni)
{
	string str;
	if (uni <= 0x7F)
		str.push_back(uni);
	else if (uni <= 0x7FF)
	{
		str.push_back(0xC0 | ((uni >> 6) & 0xFF));
		str.push_back(0x80 | uni);
	}
	else if (uni <= 0xFFFF)
	{
		str.push_back(0xE0 | ((uni >> 12) & 0xFF));
		str.push_back(0x80 | ((uni >> 6) & 0x3F));
		str.push_back(0x80 | (uni & 0x3F));
	}
	else if (uni <= 0x10FFFF)
	{
		str.push_back(0xF0 | ((uni >> 18) & 0xFF));
		str.push_back(0x80 | ((uni >> 12) & 0x3F));
		str.push_back(0x80 | ((uni >> 6) & 0x3F));
		str.push_back(0x80 | (uni & 0x3F));
	}

	return str;
}

}