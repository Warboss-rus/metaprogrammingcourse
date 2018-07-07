#include "catch.hpp"
#include "../MovableFunction.h"

using namespace metaprogramming_course;
using namespace std;

namespace
{
constexpr char DEFAULT_STRING_ARGUMENT[] = "string";
bool FREE_FUNCTION_CALLED = false;

int* FreeFunction(const std::string& str, bool boolVal)
{
	CHECK(str == DEFAULT_STRING_ARGUMENT);
	CHECK(boolVal);
	FREE_FUNCTION_CALLED = true;
	return nullptr;
}
}

TEST_CASE("Constructs from function pointer", "[Movable function]") {
	MovableFunction<int*(const std::string&, bool)> f = FreeFunction;
	int* result = f(std::string(DEFAULT_STRING_ARGUMENT), true);
	CHECK(FREE_FUNCTION_CALLED);
	CHECK(!result);
}

TEST_CASE("Constructs from lambda", "[Movable function]") {
	bool called = false;
	MovableFunction<int*(const std::string&, bool)> f = [&](const std::string& str, bool boolVal) {
		CHECK(str == DEFAULT_STRING_ARGUMENT);
		CHECK(boolVal);
		called = true;
		return nullptr;
	};
	int* result = f(std::string(DEFAULT_STRING_ARGUMENT), true);
	CHECK(called);
	CHECK(!result);
}

TEST_CASE("Constructs from bind", "[Movable function]") {
	struct Temp
	{
		int* FunctionToCall(const std::string& str, bool boolVal)
		{
			CHECK(str == DEFAULT_STRING_ARGUMENT);
			CHECK(boolVal);
			called = true;
			return nullptr;
		}
		bool called = false;
	} temp;
	MovableFunction<int*(const std::string&, bool)> f = std::bind(&Temp::FunctionToCall, &temp, std::placeholders::_1, std::placeholders::_2);
	int* result = f(std::string(DEFAULT_STRING_ARGUMENT), true);
	CHECK(temp.called);
	CHECK(!result);
}

TEST_CASE("Constructs from a movable only lambda", "[Movable function]") {
	bool called = false;
	auto expectedStringArgument = std::make_unique<std::string>(DEFAULT_STRING_ARGUMENT);
	auto func = [expectedStr = std::move(expectedStringArgument), &called](const std::string& str, bool boolVal) {
		CHECK(str == *expectedStr);
		CHECK(boolVal);
		called = true;
		return nullptr;
	};
	MovableFunction<int*(const std::string&, bool)> f = std::move(func);
	int* result = f(std::string(DEFAULT_STRING_ARGUMENT), true);
	CHECK(called);
	CHECK(!result);
}

TEST_CASE("Throws if called when empty", "[Movable function]") {
	MovableFunction<int*(const std::string&, bool)> f;
	CHECK_THROWS_AS(f(std::string(DEFAULT_STRING_ARGUMENT), true), std::bad_function_call);
}

TEST_CASE("Can be moved with a movable only lambda", "[Movable function]") {
	bool called = false;
	auto expectedStringArgument = std::make_unique<std::string>(DEFAULT_STRING_ARGUMENT);
	auto func = [expectedStr = std::move(expectedStringArgument), &called](const std::string& str, bool boolVal) {
		CHECK(str == *expectedStr);
		CHECK(boolVal);
		called = true;
		return nullptr;
	};
	MovableFunction<int*(const std::string&, bool)> f = std::move(func);
	auto f2 = std::move(f);
	int* result = f2(std::string(DEFAULT_STRING_ARGUMENT), true);
	CHECK(called);
	CHECK(!result);
	CHECK_THROWS_AS(f(std::string(DEFAULT_STRING_ARGUMENT), true), std::bad_function_call);
}

TEST_CASE("Supports functors returning void", "[Movable function]") {
	bool called = false;
	MovableFunction<void(const std::string&, bool)> f = [&](const std::string& str, bool boolVal) {
		CHECK(str == DEFAULT_STRING_ARGUMENT);
		CHECK(boolVal);
		called = true;
	};
	CHECK_NOTHROW(f(std::string(DEFAULT_STRING_ARGUMENT), true));
	CHECK(called);
}

TEST_CASE("Supports functors without arguments", "[Movable function]") {
	bool called = false;
	MovableFunction<int*()> f = [&]() {
		called = true;
		return nullptr;
	};
	int* result = f();
	CHECK(called);
	CHECK(!result);
}