#pragma once

#define S2_USING_DICT

#include <cstdlib>
#include <cstring>
#include <new>

#ifndef S2_DICT_ALLOC_STEP
#define S2_DICT_ALLOC_STEP 16
#endif

#ifndef S2_DICT_CHECK_FOR_DUPLICATE_KEYS
#define S2_DICT_CHECK_FOR_DUPLICATE_KEYS 1
#endif

namespace s2
{
	enum class fastdictexception
	{
		no_such_key,
		duplicate_key,
		index_out_of_range,
	};

	template<typename TKey, typename TValue>
	class fastdict;

	template<typename TKey, typename TValue>
	class fastdictpair
	{
		friend class fastdict<TKey, TValue>;

	private:
		TKey &m_key;
		TValue &m_value;

	public:
		fastdictpair(TKey &key, TValue &value)
			: m_key(key), m_value(value)
		{
		}

		fastdictpair(const fastdictpair &copy)
			: m_key(copy.m_key), m_value(copy.m_value)
		{
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
	class fastdictiterator
	{
	private:
		typedef fastdict<TKey, TValue> dict_type;

	private:
		dict_type* m_dict;
		size_t m_index;

	public:
		fastdictiterator(dict_type* list, size_t index)
		{
			m_dict = list;
			m_index = index;
		}

		bool operator ==(const fastdictiterator &other)
		{
			return !operator !=(other);
		}

		bool operator !=(const fastdictiterator &other)
		{
			return m_dict != other.m_dict || m_index != other.m_index;
		}

		fastdictiterator &operator ++()
		{
			m_index++;
			return *this;
		}

		fastdictpair<TKey, TValue> operator *()
		{
			return m_dict->get_pair_at(m_index);
		}
	};

	template<typename TKey, typename TValue>
	class fastdict
	{
	public:
		typedef fastdictpair<TKey, TValue> pair;
		typedef fastdictiterator<TKey, TValue> iterator;

	private:
		TKey* m_keys;
		TValue* m_values;
		size_t m_length;
		size_t m_allocSize;

	public:
		fastdict()
		{
			m_keys = nullptr;
			m_values = nullptr;
			m_length = 0;
			m_allocSize = 0;
		}

		fastdict(const fastdict &copy)
			: fastdict()
		{
			size_t copylen = copy.len();
			ensure_memory(copylen);
			for (size_t i = 0; i < copylen; i++) {
				auto &key = copy.m_keys[i];
				auto &value = copy.m_values[i];
				add_pair(key, value);
			}
		}

		fastdict(std::initializer_list<pair> list)
			: fastdict()
		{
			for (pair p : list) {
				add_pair(p.m_key, p.m_value);
			}
		}

		~fastdict()
		{
			clear();
			if (m_keys != nullptr) {
				free(m_keys);
			}
			if (m_values != nullptr) {
				free(m_values);
			}
		}

		fastdict &operator=(const fastdict &copy)
		{
			clear();
			size_t copylen = copy.len();
			ensure_memory(copylen);
			for (size_t i = 0; i < copylen; i++) {
				auto &key = copy.m_keys[i];
				auto &value = copy.m_values[i];
				add_pair(key, value);
			}
			return *this;
		}

		void clear()
		{
			for (size_t i = 0; i < m_length; i++) {
				m_keys[i].~TKey();
				m_values[i].~TValue();
			}
			m_length = 0;
		}

		size_t len() const
		{
			return m_length;
		}

		bool contains_key(const TKey &key) const
		{
			return find_key(key) != -1;
		}

		bool contains_value(const TValue &value) const
		{
			return find_value(value) != -1;
		}

		pair get_pair(const TKey &key)
		{
			int index = find_key(key);
			if (index == -1) {
				throw fastdictexception::no_such_key;
			}
			return pair(m_keys[index], m_values[index]);
		}

		const pair get_pair(const TKey &key) const
		{
			int index = find_key(key);
			if (index == -1) {
				throw fastdictexception::no_such_key;
			}
			return pair(m_keys[index], m_values[index]);
		}

		pair get_pair_at(size_t index)
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			return pair(m_keys[index], m_values[index]);
		}

		const pair get_pair_at(size_t index) const
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			return pair(m_keys[index], m_values[index]);
		}

		TKey& get_key_at(size_t index)
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			return m_keys[index];
		}

		const TKey& get_key_at(size_t index) const
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			return m_keys[index];
		}

		TValue& get_value_at(size_t index)
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			return m_values[index];
		}

		const TValue& get_value_at(size_t index) const
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			return m_values[index];
		}

		void add(const TKey &key)
		{
#if S2_DICT_CHECK_FOR_DUPLICATE_KEYS
			if (contains_key(key)) {
				throw fastdictexception::duplicate_key;
			}
#endif
			add_pair(key);
		}

		void add(const TKey &key, const TValue &value)
		{
#if S2_DICT_CHECK_FOR_DUPLICATE_KEYS
			if (contains_key(key)) {
				throw fastdictexception::duplicate_key;
			}
#endif
			add_pair(key, value);
		}

		void set(const TKey &key, const TValue &value)
		{
			int index = find_key(key);
			if (index == -1) {
				add_pair(key, value);
				return;
			}
			m_values[index] = value;
		}

		void remove(const TKey &key)
		{
			int index = find_key(key);
			if (index == -1) {
				throw fastdictexception::no_such_key;
			}
			remove_at(index);
		}

		void remove_at(size_t index)
		{
			if (index >= m_length) {
				throw fastdictexception::index_out_of_range;
			}
			m_keys[index].~TKey();
			m_values[index].~TValue();
			if (index != m_length - 1) {
				memmove(m_keys + index, m_keys + index + 1, (m_length - index - 1) * sizeof(TKey));
				memmove(m_values + index, m_values + index + 1, (m_length - index - 1) * sizeof(TValue));
			}
			m_length--;
		}

		TValue &operator [](const TKey &key)
		{
			int index = find_key(key);
			if (index == -1) {
				return m_values[add_pair(key)];
			}
			return m_values[index];
		}

		const TValue &operator [](const TKey &key) const
		{
			int index = find_key(key);
			if (index == -1) {
				throw fastdictexception::no_such_key;
			}
			return m_values[index];
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

		int find_key(const TKey &key) const
		{
			for (size_t i = 0; i < m_length; i++) {
				if (m_keys[i] == key) {
					return (int)i;
				}
			}
			return -1;
		}

		int find_value(const TValue &value) const
		{
			for (size_t i = 0; i < m_length; i++) {
				if (m_values[i] == value) {
					return (int)i;
				}
			}
			return -1;
		}

		void ensure_memory(size_t count)
		{
			if (m_allocSize >= count) {
				return;
			}

			if (count % S2_DICT_ALLOC_STEP > 0) {
				count += (count % S2_DICT_ALLOC_STEP);
			}

			m_keys = (TKey*)realloc(m_keys, count * sizeof(TKey));
			m_values = (TValue*)realloc(m_values, count * sizeof(TValue));
			m_allocSize = count;
		}

		template<typename TFunc>
		void sort_key(TFunc func)
		{
#if defined(_MSC_VER)
			qsort_s(m_keys, m_length, sizeof(TKey), [](void* context, const void* a, const void* b) {
				return (*(TFunc*)context)(*(TKey*)a, *(TKey*)b);
			}, &func);
#else
			qsort_r(m_keys, m_length, sizeof(TKey), [](const void* a, const void* b, void* context) {
				return (*(TFunc*)context)(*(TKey*)a, *(TKey*)b);
			}, &func);
#endif
		}

		template<typename TFunc>
		void sort_value(TFunc func)
		{
#if defined(_MSC_VER)
			qsort_s(m_keys, m_length, sizeof(TValue), [](void* context, const void* a, const void* b) {
				return (*(TFunc*)context)(*(TValue*)a, *(TValue*)b);
			}, &func);
#else
			qsort_r(m_keys, m_length, sizeof(TKey), [](const void* a, const void* b, void* context) {
				return (*(TFunc*)context)(*(TValue*)a, *(TValue*)b);
			}, &func);
#endif
		}

	private:
		size_t add_pair(const TKey &key)
		{
			ensure_memory(m_length + 1);
			size_t newIndex = m_length;
			new (m_keys + newIndex) TKey(key);
			new (m_values + newIndex) TValue;
			m_length++;
			return newIndex;
		}

		size_t add_pair(const TKey &key, const TValue &value)
		{
			ensure_memory(m_length + 1);
			size_t newIndex = m_length;
			new (m_keys + newIndex) TKey(key);
			new (m_values + newIndex) TValue(value);
			m_length++;
			return newIndex;
		}
	};
}
