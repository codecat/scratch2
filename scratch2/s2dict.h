#pragma once

#define S2_USING_DICT

#include <cstdlib>
#include <cstring>
#include <new>

#ifndef S2_DICT_ALLOC_STEP
#define S2_DICT_ALLOC_STEP 16
#endif

namespace s2
{
	enum class dictexception
	{
		no_such_key,
		duplicate_key,
		index_out_of_range,
	};

	template<typename TKey, typename TValue>
	class dict;

	template<typename TKey, typename TValue>
	class dictpair
	{
		friend class dict<TKey, TValue>;

	private:
		TKey m_key;
		TValue m_value;

	public:
		dictpair(const TKey &key)
		{
			m_key = key;
		}

		dictpair(const TKey &key, const TValue &value)
		{
			m_key = key;
			m_value = value;
		}

		dictpair(const dictpair &copy)
		{
			m_key = copy.m_key;
			m_value = copy.m_value;
		}

		TKey &key()
		{
			return m_key;
		}

		const TKey &key() const
		{
			return m_key;
		}

		TValue &value()
		{
			return m_value;
		}

		const TValue &value() const
		{
			return m_value;
		}
	};

	template<typename TKey, typename TValue>
	class dictiterator
	{
	private:
		typedef dict<TKey, TValue> dict_type;

	private:
		dict_type* m_dict;
		int m_index;

	public:
		dictiterator(dict_type* list, int index)
		{
			m_dict = list;
			m_index = index;
		}

		bool operator ==(const dictiterator &other)
		{
			return !operator !=(other);
		}

		bool operator !=(const dictiterator &other)
		{
			return m_dict != other.m_dict || m_index != other.m_index;
		}

		dictiterator &operator ++()
		{
			m_index++;
			return *this;
		}

		dictpair<TKey, TValue> &operator *()
		{
			return m_dict->get_pair_at(m_index);
		}
	};

	template<typename TKey, typename TValue>
	class dict
	{
	public:
		typedef dictpair<TKey, TValue> pair;
		typedef dictiterator<TKey, TValue> iterator;

	private:
		pair* m_pairs;
		size_t m_length;
		size_t m_allocSize;

	public:
		dict()
		{
			m_pairs = nullptr;
			m_length = 0;
			m_allocSize = 0;
		}

		dict(const dict &copy)
		{
			//TODO
		}

		~dict()
		{
			clear();
			if (m_pairs != nullptr) {
				free(m_pairs);
			}
		}

		void clear()
		{
			for (size_t i = 0; i < m_length; i++) {
				m_pairs[i].~pair();
			}
			m_length = 0;
		}

		size_t len() const
		{
			return m_length;
		}

		int index_of(const TKey &key) const
		{
			for (size_t i = 0; i < m_length; i++) {
				if (m_pairs[i].m_key == key) {
					return i;
				}
			}
			return -1;
		}

		bool contains_key(const TKey &key) const
		{
			return find_key(key) != nullptr;
		}

		bool contains_value(const TValue &value) const
		{
			return find_value(value) != nullptr;
		}

		pair &get_pair(const TKey &key)
		{
			pair* p = find_key(key);
			if (p == nullptr) {
				throw dictexception::no_such_key;
			}
			return *p;
		}

		const pair &get_pair(const TKey &key) const
		{
			pair* p = find_key(key);
			if (p == nullptr) {
				throw dictexception::no_such_key;
			}
			return *p;
		}

		pair &get_pair_at(size_t index)
		{
			if (index >= m_length) {
				throw dictexception::index_out_of_range;
			}
			return m_pairs[index];
		}

		const pair &get_pair_at(size_t index) const
		{
			if (index >= m_length) {
				throw dictexception::index_out_of_range;
			}
			return m_pairs[index];
		}

		pair &add(const TKey &key)
		{
			if (contains_key(key)) {
				throw dictexception::duplicate_key;
			}
			return add_pair(key);
		}

		pair &add(const TKey &key, const TValue &value)
		{
			if (contains_key(key)) {
				throw dictexception::duplicate_key;
			}
			return add_pair(key, value);
		}

		void set(const TKey &key, const TValue &value)
		{
			pair* p = find_key(key);
			if (p == nullptr) {
				add_pair(key, value);
				return;
			}
			p->m_key = value;
		}

		void remove(const TKey &key)
		{
			int index = index_of(key);
			if (index == -1) {
				throw dictexception::no_such_key;
			}
			remove_at(index);
		}

		void remove_at(size_t index)
		{
			if (index >= m_length) {
				throw dictexception::index_out_of_range;
			}
			m_pairs[index].~pair();
			if (index != m_length - 1) {
				memmove(m_pairs + index, m_pairs + index + 1, (m_length - index - 1) * sizeof(pair));
			}
			m_length--;
		}

		TValue &operator [](const TKey &key)
		{
			pair* p = find_key(key);
			if (p == nullptr) {
				return add_pair(key).m_value;
			}
			return p->m_value;
		}

		const TValue &operator [](const TKey &key) const
		{
			pair* p = find_key(key);
			if (p == nullptr) {
				throw dictexception::no_such_key;
			}
			return p->m_value;
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
		pair &add_pair(const TKey &key)
		{
			ensure_memory(m_length + 1);
			pair* ret = new (m_pairs + m_length) pair(key);
			m_length++;
			return *ret;
		}

		pair &add_pair(const TKey &key, const TValue &value)
		{
			ensure_memory(m_length + 1);
			pair* ret = new (m_pairs + m_length) pair(key, value);
			m_length++;
			return *ret;
		}

		pair* find_key(const TKey &key) const
		{
			for (size_t i = 0; i < m_length; i++) {
				if (m_pairs[i].m_key == key) {
					return &m_pairs[i];
				}
			}
			return nullptr;
		}

		pair* find_value(const TValue &value) const
		{
			for (size_t i = 0; i < m_length; i++) {
				if (m_pairs[i].m_value == value) {
					return &m_pairs[i];
				}
			}
			return nullptr;
		}

		void ensure_memory(size_t count)
		{
			if (m_allocSize >= count) {
				return;
			}

			if (count % S2_DICT_ALLOC_STEP > 0) {
				count += (count % S2_DICT_ALLOC_STEP);
			}

			m_pairs = (pair*)realloc(m_pairs, count * sizeof(pair));
			m_allocSize = count;
		}
	};
}
