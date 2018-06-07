#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <algorithm>
#include <numeric>
#include <memory>
#include "../FindPointer.h"

using namespace metaprogramming_course;

namespace
{
constexpr int VALUE_TO_FIND = 42;

auto CreateDefaultPredicate()
{
	return [](int value) {
		return value == VALUE_TO_FIND;
	};
}
}

TEST_CASE("Finds correct value", "[FindPointer]") {
	std::vector<int> values(100);
	std::iota(values.begin(), values.end(), 0);
	const auto pred = CreateDefaultPredicate();
	const auto ptr = FindPointer(values, pred);
	REQUIRE(ptr);
	CHECK(*ptr == VALUE_TO_FIND);
}

TEST_CASE("Returns nullptr if values was not found", "[FindPointer]") {
	std::vector<int> values(100);
	std::iota(values.begin(), values.end(), 50);
	const auto pred = CreateDefaultPredicate();
	const auto ptr = FindPointer(values, pred);
	CHECK(!ptr);
}

TEST_CASE("Returns first occurence if range has multiple", "[FindPointer]") {
	std::vector<int> values(100);
	values[4] = VALUE_TO_FIND;
	values[7] = VALUE_TO_FIND;
	values[11] = VALUE_TO_FIND;
	values[53] = VALUE_TO_FIND;
	const auto pred = CreateDefaultPredicate();
	const auto ptr = FindPointer(values, pred);
	REQUIRE(ptr);
	CHECK(*ptr == VALUE_TO_FIND);
	const size_t index = ptr - values.data();
	CHECK(index == 4);
}

TEST_CASE("Supports moveable only predicates", "[FindPointer]") {
	std::vector<int> values(100);
	std::iota(values.begin(), values.end(), 0);
	auto valueToFind = std::make_unique<int>(VALUE_TO_FIND);
	auto pred = [desiredValue = std::move(valueToFind)](int value) {
		return value == *desiredValue;
	};
	const auto ptr = FindPointer(values, std::move(pred));
	REQUIRE(ptr);
}

TEST_CASE("Supports moveable only values", "[FindPointer]") {
	std::vector<std::unique_ptr<int>> values;
	for (int i = 0; i < 100; ++i)
	{
		values.emplace_back(std::make_unique<int>(i));
	}
	auto pred = [](auto&& value) {
		return *value == VALUE_TO_FIND;
	};
	const auto ptr = FindPointer(values, pred);
	REQUIRE(ptr);
}

TEST_CASE("Works with C style arrays", "[FindPointer]") {
	int values[100];
	std::iota(std::begin(values), std::end(values), 0);
	const auto pred = CreateDefaultPredicate();
	const auto ptr = FindPointer(values, pred);
	REQUIRE(ptr);
	CHECK(*ptr == VALUE_TO_FIND);
}