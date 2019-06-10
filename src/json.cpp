#include <type_traits>

#include <cassert>

#include "json.hpp"

using std::is_integral;
using std::is_floating_point;
#if 1
namespace quarkson {

class json_error : public json_value
{
public:
	virtual const json::json_type type() const { return json::json_type::ERROR; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }
};

class json_object : public json_value
{
	friend json_value;
public:
	explicit json_object(const json::object& obj) : obj_(obj) {};
	explicit json_object(json::object&& obj) : obj_(std::move(obj)) {};

	virtual const json::json_type type() const { return json::json_type::OBJECT; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }

private:
	json::object obj_;
};

class json_array : public json_value
{
	friend json_value;
public:
	explicit json_array(const json::array& arr) : arr_(arr) {};
	explicit json_array(json::array&& arr) : arr_(std::move(arr)) {};

	virtual const json::json_type type() const { return json::json_type::ARRAY; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }

private:
	json::array arr_;
};

class json_string : public json_value
{
	friend json_value;
public:
	explicit json_string(const string & str) : str_(str) {}
	explicit json_string(string && str) : str_(std::move(str)) {}

	virtual const json::json_type type() const { return json::json_type::STRING; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }

private:
	string str_;
};

class json_boolean : public json_value
{
	friend json_value;
public:
	explicit json_boolean(bool b) : b_(b) {};

	virtual const json::json_type type() const { return json::json_type::BOOLEAN; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }
private:
	bool b_;
};

class json_number : public json_value
{
	friend json_value;
public:
	explicit json_number(double num) : num_(num) {}

	virtual const json::json_type type() const { return json::json_type::NUMBER; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }
private:
	double num_;
};

class json_null : public json_value
{
	friend json_value;
public:
	virtual const json::json_type type() const { return json::json_type::NUL; }

	virtual const shared_ptr<json_value> get() { return shared_ptr<json_value>(this); }
};
#endif

const json::object & json_value::get_object() const
{
	assert(dynamic_cast<const json_object *>(this) != nullptr);
	return dynamic_cast<const json_object *>(this)->obj_;
}

const json::array & json_value::get_array() const
{
	assert(dynamic_cast<const json_array *>(this) != nullptr);
	return dynamic_cast<const json_array *>(this)->arr_;
}

const string & json_value::get_string() const
{
	assert(dynamic_cast<const json_string *>(this) != nullptr);
	return dynamic_cast<const json_string *>(this)->str_;
}

double json_value::get_number() const
{
	assert(dynamic_cast<const json_number *>(this) != nullptr);
	return dynamic_cast<const json_number *>(this)->num_;
}

bool json_value::get_bool() const
{
	assert(dynamic_cast<const json_boolean *>(this) != nullptr);
	return dynamic_cast<const json_boolean *>(this)->b_;
}

nullptr_t json_value::get_null() const
{
	assert(dynamic_cast<const json_null *>(this) != nullptr);
	return nullptr;
}

shared_ptr<json_value> quarkson::json_value::error_instance()
{
	return (new json_error())->get();
}

shared_ptr<json_value> json_value::null_instance()
{
	return (new json_null())->get();
}

shared_ptr<json_value> json_value::bool_instance(const bool b)
{
	return (new json_boolean(b))->get();
}

shared_ptr<json_value> quarkson::json_value::number_instance(const double num)
{
	return (new json_number(num))->get();
}

shared_ptr<json_value> json_value::string_instance(const string &str)
{
	return (new json_string(str))->get();
}

shared_ptr<json_value> json_value::string_instance(string &&str)
{
	return (new json_string(str))->get();
}

shared_ptr<json_value> json_value::array_instance(const json::array &arr)
{
	return (new json_array(arr))->get();
}

shared_ptr<json_value> json_value::array_instance(json::array &&arr)
{
	return (new json_array(arr))->get();
}

shared_ptr<json_value> json_value::object_instance(const json::object &obj)
{
	return (new json_object(obj))->get();
}

shared_ptr<json_value> json_value::object_instance(json::object &&obj)
{
	return (new json_object(obj))->get();
}

const json::json_type json::type() const
{
	return data_->type();
}

const json::object & json::get_object() const
{
	return data_->get_object();
}

const json::array & json::get_array() const
{
	return data_->get_array();
}

double json::get_number() const
{
	return data_->get_number();
}

const string & json::get_string() const
{
	return data_->get_string();
}

bool json::get_boolean() const
{
	return data_->get_bool();
}

nullptr_t json::get_null() const
{
	return data_->get_null();
}

void json::convert_to_object_add(const object &obj)
{
	object tmp_obj = obj;
	tmp_obj.insert(std::make_pair("", data_));

	data_ = json_value::object_instance(tmp_obj);
}

void json::convert_to_object_add(object &&obj)
{
	object tmp_obj = obj;
	tmp_obj.insert(std::make_pair("", data_));

	data_ = json_value::object_instance(tmp_obj);
}

}