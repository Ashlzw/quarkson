#pragma once

#include "json.hpp"

namespace quarkson {

class parser
{
public:
	static json parse(const string&);
	static json parse(const string&, const string&);
public:
	parser(const string &str) : s(str.c_str()), p(str.c_str()) 
	{
		int a = 0;
	}

	shared_ptr<json_value> parse_value();
	shared_ptr<json_value> parse_object();
	shared_ptr<json_value> parse_array();
	shared_ptr<json_value> parse_string();
	shared_ptr<json_value> parse_number();
	shared_ptr<json_value> parse_literal();

	bool isdigit1to9(char ch) { return ch >= '1' ? (ch <= '9' ? true : false) : false; }

	const char * parse_hex4(const char * p, unsigned int &uni);

	const string encode_utf8(unsigned int uni);

	void skip_space() { for (; *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'; ++p); }

	const char *s;
	const char *p;
};

}
