#pragma once

#define S2_USING_FUNC

namespace s2
{
	template<typename Result, typename ...Args>
	struct func_abstract
	{
		virtual Result operator()(Args... args) = 0;
		virtual func_abstract *clone() const = 0;
		virtual ~func_abstract() {}
	};

	template<typename Func, typename Result, typename ...Args>
	class func_concrete : public func_abstract<Result, Args...>
	{
	private:
		Func m_func;

	public:
		func_concrete(const Func &x)
			: m_func(x)
		{
		}

		virtual Result operator()(Args... args)
		{
			return m_func(args...);
		}

		virtual func_concrete* clone() const
		{
			return new func_concrete(m_func);
		}
	};

	template<typename Func>
	struct func_filter
	{
		typedef Func type;
	};

	template<typename Result, typename ...Args>
	struct func_filter<Result(Args...)>
	{
		typedef Result(*type)(Args...);
	};

	template<typename signature>
	class func;

	template<typename Result, typename ...Args>
	class func<Result(Args...)>
	{
	private:
		func_abstract<Result, Args...>* m_func;

	public:
		func()
		{
			m_func = nullptr;
		}

		template<typename Func>
		func(const Func &x)
		{
			m_func = new func_concrete<typename func_filter<Func>::type, Result, Args...>(x);
		}

		func(const func &rhs)
		{
			m_func = nullptr;
			if (rhs.m_func != nullptr) {
				m_func = rhs.m_func->clone();
			}
		}

		func(decltype(nullptr))
		{
			m_func = nullptr;
		}

		~func()
		{
			if (m_func != nullptr) {
				delete m_func;
			}
		}

		func &operator=(const func &rhs)
		{
			if (&rhs != this && rhs.m_func != nullptr)
			{
				auto temp = rhs.m_func->clone();
				if (m_func != nullptr) {
					delete m_func;
				}
				m_func = temp;
			}
			return *this;
		}

		func &operator=(decltype(nullptr))
		{
			m_func = nullptr;
			return *this;
		}

		template<typename Func>
		func &operator=(const Func &x)
		{
			auto temp = new func_concrete<typename func_filter<Func>::type, Result, Args...>(x);
			if (m_func != nullptr) {
				delete m_func;
			}
			m_func = temp;
			return *this;
		}

		bool operator==(decltype(nullptr)) const
		{
			return m_func == nullptr;
		}

		bool operator!=(decltype(nullptr)) const
		{
			return m_func != nullptr;
		}

		Result operator()(Args... args) const
		{
			if (m_func == nullptr) {
				return Result();
			}
			return (*m_func)(args...);
		}
	};
}
