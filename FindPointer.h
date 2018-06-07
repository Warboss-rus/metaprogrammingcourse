#pragma once
#include <utility>
#include <functional>
#include <iterator>

namespace metaprogramming_course
{
template<class Iterator, class Predicate>
auto FindPointer(Iterator begin, Iterator end, Predicate&& pred)
{
	for (; begin != end; ++begin)
	{
		if (std::invoke(std::forward<Predicate>(pred), *begin))
		{
			return &*begin;
		}
	}
	return typename std::iterator_traits<Iterator>::pointer();
}

template<class Range, class Predicate>
auto FindPointer(Range&& range, Predicate&& pred)
{
	return FindPointer(std::begin(range), std::end(range), std::forward<Predicate>(pred));
}
}