#pragma once
#include <utility>

namespace metaprogramming_course
{
namespace detail
{
template<class Ret, class Args>
class MovableFunctionImplInterface
{
public:
	virtual ~MovableFunctionImplInterface() = default;

	virtual Ret operator()(Args&&) const = 0;
};

template<class T, class Ret, class Args>
class MovableFunctionImpl : public MovableFunctionImplInterface<Ret, Args>
{
public:
	MovableFunctionImpl(T&& data)
		: m_data(std::move(data))
	{
	}

	MovableFunctionImpl(const MovableFunctionImpl& other) = default;
	MovableFunctionImpl(MovableFunctionImpl&& other) = default;

	Ret operator()(Args&& args) const
	{
		return std::apply(m_data, std::forward<Args>(args));
	}
private:
	T m_data;
};

template<class T>
struct ArgsIdentifier;

template <class Return,	class... Args>
struct ArgsIdentifier<Return (Args...)> \
{
	using ReturnType = Return;
	using Arguments = std::tuple<Args...>;
};
}

template <class T>
class MovableFunction
{
	using ReturnType = typename detail::ArgsIdentifier<T>::ReturnType;
	using Args = typename detail::ArgsIdentifier<T>::Arguments;
public:
	MovableFunction() = default;
	MovableFunction(nullptr_t) noexcept {}
	MovableFunction(const MovableFunction& other) 
	{
		Set(other);
	}
	MovableFunction(MovableFunction&& other) = default;

	template<class Fn>
	MovableFunction(Fn&& fn)
	{
		Set(std::forward<Fn>(fn));
	}

	MovableFunction& operator=(const MovableFunction other)
	{
		Set(other);
	}

	MovableFunction& operator=(MovableFunction&& other) = default;

	template<class Fn>
	MovableFunction& operator=(Fn&& fn)
	{
		
	}

	operator bool() const noexcept
	{
		return m_storage;
	}

	template<class... Args>
	ReturnType operator()(Args... args) const
	{
		if (!m_storage)
		{
			throw std::bad_function_call();
		}
		return std::invoke(*m_storage, std::forward_as_tuple(std::forward<Args>(args)...));
	}

private:
	template<class Fn>
	void Set(Fn&& fn)
	{
		m_storage = std::make_unique<detail::MovableFunctionImpl<Fn, ReturnType, Args>>(std::forward<Fn>(fn));
	}

	std::unique_ptr<detail::MovableFunctionImplInterface<ReturnType, Args>> m_storage;
};
}