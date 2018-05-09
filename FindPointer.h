#pragma once

namespace metaprogramming_course
{
namespace detail
{
template<class Iterator>
auto GetPointer(Iterator it)
{
	return it._Ptr;
}

template<class T>
auto GetPointer(T* it)
{
	return it;
}
}

template<class Iterator, class Predicate>
auto FindPointer(Iterator begin, Iterator end, Predicate&& pred)
{
	for (; begin != end; ++begin)
	{
		if (std::invoke(pred, *begin))
		{
			return detail::GetPointer(begin);
		}
	}
	return std::iterator_traits<Iterator>::pointer();
}

template<class Range, class Predicate>
auto FindPointer(Range&& range, Predicate&& pred)
{
	return FindPointer(std::begin(range), std::end(range), pred);
}
}