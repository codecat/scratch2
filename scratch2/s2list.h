#pragma once

#define S2_USING_LIST

#include <cstdlib>
#include <new>

namespace s2
{
	template<typename T>
	class list;

	template<typename T>
	class listiterator
	{
	private:
		typedef list<T> list;

	private:
		list* m_list;
		int m_index;

	public:
		listiterator(list* list, int index)
		{
			m_list = list;
			m_index = index;
		}

		bool operator ==(listiterator &other)
		{
			return !operator !=(other);
		}

		bool operator !=(listiterator &other)
		{
			return m_list != other.m_list || m_index != other.m_index;
		}

		listiterator &operator ++()
		{
			m_index++;
			return *this;
		}

		T &operator *()
		{
			return (*m_list)[m_index];
		}
	};

	template<typename T>
	class list
	{
	public:
		typedef listiterator<T> iterator;

	private:
		T* m_buffer;
		size_t m_length;
		size_t m_allocSize;

	public:
		list()
		{
			m_buffer = nullptr;
			m_length = 0;
			m_allocSize = 0;
		}

		list(const list &copy)
		{
			size_t copylen = copy.len();
			for (size_t i = 0; i < copylen; i++) {
				add(copy[i]);
			}
		}

		~list()
		{
			clear();
			if (m_buffer != nullptr) {
				free(m_buffer);
			}
		}

		void clear()
		{
			for (size_t i = 0; i < m_length; i++) {
				m_buffer[i].~T();
			}
			m_length = 0;
		}

		size_t len() const
		{
			return m_length;
		}

		void add(const T &o)
		{
			ensure_memory(m_length + 1);
			new (m_buffer + m_length) T(o);
			m_length++;
		}

		T &add()
		{
			ensure_memory(m_length + 1);
			T* ret = new (m_buffer + m_length) T;
			m_length++;
			return *ret;
		}

		void remove(int index)
		{
			if (index < 0 || (size_t)index >= m_length) {
				return;
			}
			m_buffer[index].~T();
			m_length--;
		}

		T &operator [](int index)
		{
			return m_buffer[index];
		}

		const T &operator [](int index) const
		{
			return m_buffer[index];
		}

		iterator begin()
		{
			return iterator(this, 0);
		}

		const iterator begin() const
		{
			return iterator(this, 0);
		}

		iterator end()
		{
			return iterator(this, m_length);
		}

		const iterator end() const
		{
			return iterator(this, m_length);
		}

	private:
		void ensure_memory(size_t count)
		{
			if (m_allocSize >= count) {
				return;
			}
			m_buffer = (T*)realloc(m_buffer, count * sizeof(T));
			m_allocSize = count;
		}
	};
}
