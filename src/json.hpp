#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using std::string;
using std::vector;
using std::unordered_map;
using std::shared_ptr;

namespace quarkson {

class json_value;

class json
{
public:
	using array = vector<shared_ptr<json_value>>;
	using object = unordered_map<string, shared_ptr<json_value>>;

	enum class json_type
	{
		OBJECT = 1,
		ARRAY = 2,
		NUMBER = 4,
		STRING = 8,
		BOOLEAN = 16,
		NUL = 32,
		ERROR = 64
	};

	json() = default;

	json(const shared_ptr<json_value> & data) : data_(data) {}

	json(shared_ptr<json_value> && data) : data_(std::move(data)) {}

	const json_type type() const;

	const object & get_object() const;

	const array & get_array() const;
	
	double get_number() const;
	
	const string & get_string() const;

	bool get_boolean() const;

	nullptr_t get_null() const;

	void convert_to_object_add(const object &);
	void convert_to_object_add(object&&);

	const shared_ptr<json_value> get() const { return data_; }

private:
	shared_ptr<json_value> data_;
};

class json_value
{
public:
	json_value() = default;
	virtual ~json_value() = default;

	virtual const json::json_type type() const = 0;

	virtual const shared_ptr<json_value> get() = 0;

	const json::object & get_object() const;

	const json::array & get_array() const;

	const string & get_string() const;

	double get_number() const;

	bool get_bool() const;

	nullptr_t get_null() const;

	static shared_ptr<json_value> null_instance();
	static shared_ptr<json_value> bool_instance(const bool);
	static shared_ptr<json_value> number_instance(const double);
	static shared_ptr<json_value> string_instance(const string &);
	static shared_ptr<json_value> string_instance(string &&);
	static shared_ptr<json_value> array_instance(const json::array &);
	static shared_ptr<json_value> array_instance(json::array&&);
	static shared_ptr<json_value> object_instance(const json::object &);
	static shared_ptr<json_value> object_instance(json::object&&);
	static shared_ptr<json_value> error_instance();
};

}