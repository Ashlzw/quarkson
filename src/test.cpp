#pragma warning(disable : 4996)

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "json.hpp"
#include "quarkson_parser.hpp"
#include "quarkson_generator.hpp"
#include "quarkson_string_stream.hpp"
#include "quarkson_file_write_stream.hpp"
#include "simple_file_operator.hpp"
#include "quarkson_file_read_stream.hpp"

using std::cout;
using std::endl;

using quarkson::json;
using quarkson::json_value;
using quarkson::parser;
using quarkson::GenericStringBuffer;
using quarkson::Generator;
using quarkson::GenericFileWriteStream;
using quarkson::FileOperator;
using quarkson::SimpleFileOperator;
using quarkson::Parser;
using quarkson::GenericFileReadStream;
using quarkson::FileOperatorMode;


static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual) \
	do \
	{ \
		test_count++; \
		if (equality) \
			test_pass++; \
		else \
		{ \
			std::clog << __FILE__ << ":" << __LINE__ << " expect: " << expect << " actual: " << actual << std::endl; \
			main_ret = 1; \
		} \
	} while (0)

#define EXPECT_EQ_RET_TYPE(expect, actual)	EXPECT_EQ_BASE(expect == actual, static_cast<int>(expect), static_cast<int>(actual))

#define EXPECT_EQ_VALUE_TYPE(expect, actual) EXPECT_EQ_BASE(expect == actual, static_cast<int>(expect), static_cast<int>(actual))

#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE(expect == actual, expect, actual)

#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE(expect == actual, expect, actual)

#define TEST_NUMBER(n, jstr) \
	do \
	{ \
		json j = parser::parse(jstr); \
		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, j.type()); \
		EXPECT_EQ_DOUBLE(n, j.get_number()); \
	} while (0)

#define TEST_STRING(str, jstr) \
	do \
	{ \
		json j = parser::parse(jstr); \
		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, j.type()); \
		EXPECT_EQ_STRING(str, j.get_string()); \
	} while (0)

static void test_parse_null()
{
	json j = parser::parse("null");
	EXPECT_EQ_VALUE_TYPE(json::json_type::NUL, j.type());
}

static void test_parse_true()
{
	json j = parser::parse("true");
	EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, j.type());
}

static void test_parse_false()
{
	json j = parser::parse("false");
	EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, j.type());
}

static void test_parse_number()
{
	TEST_NUMBER(0, "0");
	TEST_NUMBER(-0, "-0");
	TEST_NUMBER(1, "1");
	TEST_NUMBER(-1, "-1");
	TEST_NUMBER(0.99, "0.99");
	TEST_NUMBER(1.99, "1.99");
	TEST_NUMBER(3.1415, "3.1415");

	TEST_NUMBER(1.0000000000000002, "1.0000000000000002");
	TEST_NUMBER(1.0000000000000004, "1.0000000000000004");
	TEST_NUMBER(2, "2");
	TEST_NUMBER(-2, "-2");

	TEST_NUMBER(3, "3");
	TEST_NUMBER(4, "4");
	TEST_NUMBER(5, "5");
	TEST_NUMBER(6, "6");
	TEST_NUMBER(23, "23");
	TEST_NUMBER(1152921504606846974ll, "1152921504606846974");
	TEST_NUMBER(9223372036854775808ull, "9223372036854775808");

	TEST_NUMBER(4.9e-324, "4.9e-324"); // Min. subnormal positive double
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308"); // Max. subnormal double
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308"); // Min. normal positi
	TEST_NUMBER(1.7976931348623157e308, "1.7976931348623157e308"); // Max. Double
}

static void test_parse_string()
{
	TEST_STRING("", "\"\"");
	TEST_STRING("A", "\"A\"");
	TEST_STRING("\"", "\"\\\"\"");
	TEST_STRING("\\", "\"\\\\\"");
	TEST_STRING("/", "\"\\/\"");
	TEST_STRING("\b", "\"\\b\"");
	TEST_STRING("\f", "\"\\f\"");
	TEST_STRING("\n", "\"\\n\"");
	TEST_STRING("\r", "\"\\r\"");
	TEST_STRING("\t", "\"\\t\"");

	TEST_STRING("hello world", "\"hello world\"");

	TEST_STRING("\xE4\xB8\xA5", "\"\\u4E25\"");

	TEST_STRING("\xF0\x90\x80\x80", "\"\\uD800\\uDC00\"");

	TEST_STRING("\xF4\x8F\xBF\xBF", "\"\\uDBFF\\uDFFF\"");
}

static void test_parse_array()
{
	{
		json j = parser::parse("[]");
		EXPECT_EQ_VALUE_TYPE(json::json_type::ARRAY, j.type());
	}

	{
		json j = parser::parse("[1, true, false, null, \"hello\"]");

		EXPECT_EQ_VALUE_TYPE(json::json_type::ARRAY, j.type());
		json::array arr = j.get_array();
		EXPECT_EQ_DOUBLE(1, arr[0]->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, arr[1]->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, arr[2]->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUL, arr[3]->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, arr[4]->type());
		EXPECT_EQ_STRING("hello", arr[4]->get_string());
	}

	{
		json j = parser::parse("[1.23, [ true, false ], null]");

		EXPECT_EQ_VALUE_TYPE(json::json_type::ARRAY, j.type());
		json::array arr = j.get_array();
		EXPECT_EQ_DOUBLE(1.23, arr[0]->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::ARRAY, arr[1]->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, arr[1]->get_array()[0]->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, arr[1]->get_array()[1]->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUL, arr[2]->type());
	}
}

static void test_parse_object()
{
	{
		json j = parser::parse("{}");
		EXPECT_EQ_VALUE_TYPE(json::json_type::OBJECT, j.type());
	}

	{
		json j = parser::parse("{ \"precision\": \"zip\", \"Latitude\": 37.7668, \"Longitude\": -122.3959, \"Address\": \"\", \"City\": \"SAN FRANCISCO\", \"State\": \"CA\", \"Zip\": 94107, \"Country\": \"US\" }");
		EXPECT_EQ_VALUE_TYPE(json::json_type::OBJECT, j.type());

		json::object objs = j.get_object();

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs.find("precision")->second->type());
		EXPECT_EQ_STRING("zip", objs.find("precision")->second->get_string());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs.find("Latitude")->second->type());
		EXPECT_EQ_DOUBLE(37.7668, objs.find("Latitude")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs.find("Longitude")->second->type());
		EXPECT_EQ_DOUBLE(-122.3959, objs.find("Longitude")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs.find("Address")->second->type());
		EXPECT_EQ_STRING("", objs.find("Address")->second->get_string());

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs.find("City")->second->type());
		EXPECT_EQ_STRING("SAN FRANCISCO", objs.find("City")->second->get_string());

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs.find("State")->second->type());
		EXPECT_EQ_STRING("CA", objs.find("State")->second->get_string());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs.find("Zip")->second->type());
		EXPECT_EQ_DOUBLE(94107, objs.find("Zip")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs.find("Country")->second->type());
		EXPECT_EQ_STRING("US", objs.find("Country")->second->get_string());
	}

	//{        
	//	"Image": {            
	//		"Width":  800, 
	//		"Height" : 600, 
	//		"Title" : "View from 15th Floor", 
	//		"Thumbnail" : {                
	//			"Url":    "http://www.example.com/image/481989943", 
	//			"Height" : 125, 
	//			"Width" : 100            
	//		}, 
	//		"Animated" : false, 
	//		"IDs" : [116, 943, 234, 38793]          
	//	}      
	//}

	{
		json j = parser::parse("{ \"Image\": { \"Width\": 800, \"Height\": 600, \"Title\": \"View from 15th Floor\", \"Thumbnail\": { \"Url\": \"http:\\/\\/www.example.com\\/image\\/481989943\", \"Height\": 125, \"Width\": 100 }, \"Animated\" : false, \"IDs\": [116, 943, 234, 38793] } }");
		EXPECT_EQ_VALUE_TYPE(json::json_type::OBJECT, j.type());

		json::object objs = j.get_object();

		EXPECT_EQ_VALUE_TYPE(json::json_type::OBJECT, objs.find("Image")->second->type());
		json::object objs2 = objs.find("Image")->second->get_object();

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs2.find("Width")->second->type());
		EXPECT_EQ_DOUBLE(800, objs2.find("Width")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs2.find("Height")->second->type());
		EXPECT_EQ_DOUBLE(600, objs2.find("Height")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs2.find("Title")->second->type());
		EXPECT_EQ_STRING("View from 15th Floor", objs2.find("Title")->second->get_string());

		EXPECT_EQ_VALUE_TYPE(json::json_type::OBJECT, objs2.find("Thumbnail")->second->type());
		json::object objs3 = objs2.find("Thumbnail")->second->get_object();

		EXPECT_EQ_VALUE_TYPE(json::json_type::STRING, objs3.find("Url")->second->type());
		EXPECT_EQ_STRING("http://www.example.com/image/481989943", objs3.find("Url")->second->get_string());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs3.find("Height")->second->type());
		EXPECT_EQ_DOUBLE(125, objs3.find("Height")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, objs3.find("Width")->second->type());
		EXPECT_EQ_DOUBLE(100, objs3.find("Width")->second->get_number());

		EXPECT_EQ_VALUE_TYPE(json::json_type::BOOLEAN, objs2.find("Animated")->second->type());

		EXPECT_EQ_VALUE_TYPE(json::json_type::ARRAY, objs2.find("IDs")->second->type());
		json::array jarr = objs2.find("IDs")->second->get_array();
		int arr[4] = { 116, 943, 234, 38793 };
		for (size_t i = 0; i < 4; i++)
		{
			EXPECT_EQ_VALUE_TYPE(json::json_type::NUMBER, jarr[i]->type());
			EXPECT_EQ_DOUBLE(arr[i], jarr[i]->get_number());
		}
	}
}

static void test_generator()
{
	json j = parser::parse("{ \"Image\": { \"Width\": 800, \"Height\": 600, \"Title\": \"View from 15th Floor\", \"Thumbnail\": { \"Url\": \"http:\\/\\/www.example.com\\/image\\/481989943\", \"Height\": 125, \"Width\": 100 }, \"Animated\" : false, \"IDs\": [116, 943, 234, 38793] } }");
#if 1
	GenericStringBuffer<> *generate_ret = new GenericStringBuffer<>();

	Generator<GenericStringBuffer<>, char> generator = Generator<GenericStringBuffer<>, char>(std::shared_ptr<GenericStringBuffer<>>(generate_ret));

	generator.Generate(j);

	std::string ret = generate_ret->ToString();

	std::cout << ret << std::endl;

#endif
#if 0
	Generator<GenericFileWriteStream<>, char> generator(std::shared_ptr<GenericFileWriteStream<>>(new GenericFileWriteStream<>(SimpleFileOperator::CreateSimpleFileOperator("F:\\wangyi\\json\\test.json"))));

	generator.Generate(j);
#endif // 0

	return;
}

static void test_file_read_parse(size_t mark, size_t cnt)
{

#if 1
	{
		double min_time = -1;
		double max_time = -1;
		auto total_start = std::chrono::system_clock::now();
		for (size_t i = 0; i < mark; ++i)
		{
			for (size_t j = 0; j < cnt; ++j)
			{
				auto j_start = std::chrono::system_clock::now();
				Parser<GenericFileReadStream<>, char> parse_test(std::shared_ptr<GenericFileReadStream<>>(new GenericFileReadStream<>(SimpleFileOperator::CreateSimpleFileOperator("F:\\wangyi\\test_data\\json_data\\json_benchmark-master\\data\\canada.json", FileOperatorMode::FILE_READ))));
				parse_test.Parse();
				auto j_end = std::chrono::system_clock::now();
				std::chrono::duration<double> elapsed_seconds = j_end - j_start;
				min_time = min_time < 0 ? elapsed_seconds.count() : std::min(min_time, elapsed_seconds.count());
				max_time = max_time < 0 ? elapsed_seconds.count() : std::max(min_time, elapsed_seconds.count());
			}
		}
		auto total_end = std::chrono::system_clock::now();
		std::chrono::duration<double> large_total_seconds = total_end - total_start;
		std::chrono::duration<double> large_average_elapsed_seconds = (total_end - total_start) / (mark * cnt);

		FILE *fp = fopen("F:\\wangyi\\test_data\\json_data\\json_benchmark-master\\data\\result.large.txt", "wb");
		char large_tmp[1000] = { 0 };
		sprintf(large_tmp, "Large File Read : In %d times read\r\nMin Elapsed Time : %f\r\nMin Elapsed Time : %f\r\nAverage Elapsed Time : %f\r\nTotal Elapsed Time : %f", cnt, min_time, max_time, large_average_elapsed_seconds, large_total_seconds.count());
		fwrite(large_tmp, sizeof(char), strlen(large_tmp), fp);
		fclose(fp);
	}

	{
		typedef std::string::value_type char_t;
		typedef std::istreambuf_iterator<char_t> iterator_t;

		std::ifstream ifs("F:\\wangyi\\test_data\\json_data\\json_benchmark-master\\data\\canada.json");
		std::string json_str = std::string(iterator_t(ifs), iterator_t());

		double min_time = -1;
		double max_time = -1;
		auto total_start = std::chrono::system_clock::now();
		for (size_t i = 0; i < mark; ++i)
		{
			for (size_t j = 0; j < cnt; ++j)
			{
				auto j_start = std::chrono::system_clock::now();
				parser::parse(json_str);
				auto j_end = std::chrono::system_clock::now();
				std::chrono::duration<double> elapsed_seconds = j_end - j_start;
				min_time = min_time < 0 ? elapsed_seconds.count() : std::min(min_time, elapsed_seconds.count());
				max_time = max_time < 0 ? elapsed_seconds.count() : std::max(min_time, elapsed_seconds.count());
			}
		}
		auto total_end = std::chrono::system_clock::now();
		std::chrono::duration<double> total_seconds = total_end - total_start;
		std::chrono::duration<double> average_elapsed_seconds = (total_end - total_start) / (mark * cnt);

		FILE* fp = fopen("F:\\wangyi\\test_data\\json_data\\json_benchmark-master\\data\\result.memory.large.txt", "wb");
		char large_tmp[1000] = { 0 };
		sprintf(large_tmp, "Large File Read : In %d times read\r\nMin Elapsed Time : %f\r\nMin Elapsed Time : %f\r\nAverage Elapsed Time : %f\r\nTotal Elapsed Time : %f", cnt, min_time, max_time, average_elapsed_seconds, total_seconds.count());
		fwrite(large_tmp, sizeof(char), strlen(large_tmp), fp);
		fclose(fp);
	}

	{
		std::vector<std::string> jsons;
		std::ifstream ifs("F:\\wangyi\\test_data\\json_data\\json_benchmark-master\\data\\one-json-per-line.jsons");
		for (std::string line; std::getline(ifs, line); )
			jsons.push_back(line);

		double min_time = -1;
		double max_time = -1;
		auto total_start = std::chrono::system_clock::now();
		for (size_t i = 0; i < mark; ++i)
		{
			for (size_t j = 0; j < cnt; ++j)
			{
				for (const auto& s : jsons)
				{
					auto j_start = std::chrono::system_clock::now();
					parser::parse(s);
					auto j_end = std::chrono::system_clock::now();
					std::chrono::duration<double> elapsed_seconds = j_end - j_start;
					min_time = min_time < 0 ? elapsed_seconds.count() : std::min(min_time, elapsed_seconds.count());
					max_time = max_time < 0 ? elapsed_seconds.count() : std::max(min_time, elapsed_seconds.count());
				}
			}
		}
		auto total_end = std::chrono::system_clock::now();
		std::chrono::duration<double> small_total_seconds = total_end - total_start;
		std::chrono::duration<double> small_average_elapsed_seconds = (total_end - total_start) / (mark * cnt * jsons.size());

		FILE* fp1 = fopen("F:\\wangyi\\test_data\\json_data\\json_benchmark-master\\data\\result.small.txt", "wb");
		char small_tmp[1000] = { 0 };
		sprintf(small_tmp, "Large File Read : In %d times read\r\nMin Elapsed Time : %f\r\nMin Elapsed Time : %f\r\nAverage Elapsed Time : %f\r\nTotal Elapsed Time : %f", cnt, min_time, max_time, small_average_elapsed_seconds, small_total_seconds.count());
		fwrite(small_tmp, sizeof(char), strlen(small_tmp), fp1);
		fclose(fp1);
	}
#endif // 1


}

static void test_parse()
{
	test_parse_null();
#if 1
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_array();
	test_parse_object();
#endif // 0
}

std::size_t max_marks = 2; // 10;
std::size_t max_cnt = 1000;

int main()
{
	//test_parse();

	//std::cout << test_pass << "/" << test_count << " (" << std::setprecision(3) << test_pass * 100.0 / test_count << ") passed" << std::endl;

	//test_generator();

	//system("PAUSE");

	test_file_read_parse(max_marks, max_cnt);

	return main_ret;
}