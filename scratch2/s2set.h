#pragma once

#define S2_USING_SET

#include <cstring>
#include <cstdint>
#include <new>

namespace s2
{
	enum class setexception
	{
		no_such_value,
		duplicate_value,
		index_out_of_range,
		unstable,
	};

	class default_hashers_set
	{
	public:
		static uint64_t hash(const char* key);
		static uint64_t hash(int8_t key);
		static uint64_t hash(int16_t key);
		static uint64_t hash(int32_t key);
		static uint64_t hash(int64_t key);
		static uint64_t hash(uint8_t key);
		static uint64_t hash(uint16_t key);
		static uint64_t hash(uint32_t key);
		static uint64_t hash(uint64_t key);
		static uint64_t hash(float key);
		static uint64_t hash(double key);
	};

	template<typename T>
	class set_entry
	{
	private:
		uint64_t m_hash;
		T m_value;

	public:
		inline uint64_t& hash() { return m_hash; }
		inline T& value() { return m_value; }

		inline const uint64_t& hash() const { return m_hash; }
		inline const T& value() const { return m_value; }
	};

	template<typename T, typename THasher = default_hashers_set>
	class set
	{
	public:
		typedef set_entry<T> entry;

	private:
		entry* m_entries = nullptr;
		size_t m_length = 0;
		size_t m_allocSize = 0;

	public:
		set()
		{
		}

		set(const set& other)
		{
			*this = other;
		}

		~set()
		{
			clear();
			if (m_entries != nullptr) {
				free(m_entries);
			}
		}

		set& operator =(const set& other)
		{
			clear();
			if (m_entries != nullptr) {
				free(m_entries);
				m_entries = nullptr;
				m_allocSize = 0;
			}

			m_length = other.m_length;
			if (m_length > 0) {
				ensure_memory(m_length);
				for (size_t i = 0; i < m_length; i++) {
					new (m_entries + i) entry(other.m_entries[i]);
				}
			}

			return *this;
		}

		void clear()
		{
			for (size_t i = 0; i < m_length; i++) {
				m_entries[i].~entry();
			}
			m_length = 0;
		}

		size_t len() const
		{
			return m_length;
		}

		void add_unsorted(const T& value)
		{
			ensure_memory(m_length + 1);
			entry* ret = new (m_entries + m_length) entry;
			m_length++;
			ret->hash() = THasher::hash(value);
			ret->value() = value;
		}

		void add(const T& value)
		{
			ensure_memory(m_length + 1);

			uint64_t valuehash = THasher::hash(value);

			size_t newIndex = m_length;

			if (m_length > 0) {
				if (valuehash < m_entries[0].hash()) {
					newIndex = 0;
				} else if (valuehash > m_entries[m_length - 1].hash()) {
					newIndex = m_length;
				} else {
					size_t start = 0;
					size_t end = m_length;

					while (true) {
						size_t halfLen = end - start;
						size_t halfIndex = start + halfLen / 2;
						if (halfIndex >= m_length) {
							throw setexception::unstable;
						}
						auto& e = m_entries[halfIndex];

						if (e.hash() == valuehash) {
							throw setexception::duplicate_value;
						}

						if (halfLen == 1) {
							newIndex = halfIndex;
							if (valuehash > e.hash()) {
								newIndex++;
							}
							break;
						} else if (valuehash > e.hash()) {
							start = halfIndex;
						} else if (valuehash < e.hash()) {
							end = halfIndex;
						}
					}
				}

				if (newIndex < m_length) {
					memmove(m_entries + newIndex + 1, m_entries + newIndex, (m_length - newIndex) * sizeof(entry));
				}
			}

			m_length++;

			if (newIndex > 0 && valuehash <= m_entries[newIndex - 1].hash()) {
				throw setexception::unstable;
			}
			if (newIndex < m_length - 1 && valuehash >= m_entries[newIndex + 1].hash()) {
				throw setexception::unstable;
			}

			entry* ret = new (m_entries + newIndex) entry;
			ret->hash() = valuehash;
			ret->value() = value;
		}

		template<typename TComparable = T>
		void remove(const TComparable& value)
		{
			int index = index_of(value);
			if (index == -1) {
				throw setexception::no_such_value;
			}
			remove_at(index);
		}

		void remove_at(size_t index)
		{
			if (index >= m_length) {
				throw setexception::index_out_of_range;
			}
			m_entries[index].~entry();
			if (index != m_length - 1) {
				memmove(m_entries + index, m_entries + index + 1, (m_length - index - 1) * sizeof(entry));
			}
			m_length--;
		}

		template<typename TComparable = T>
		bool contains(const TComparable& value) const
		{
			return index_of(value) != -1;
		}

		entry& at(size_t index)
		{
			if (index >= m_length) {
				throw setexception::index_out_of_range;
			}
			return m_entries[index];
		}

		const entry& at(size_t index) const
		{
			if (index >= m_length) {
				throw setexception::index_out_of_range;
			}
			return m_entries[index];
		}

		void sort()
		{
			qsort(m_entries, m_length, sizeof(entry), [](const void* pa, const void* pb) {
				auto a = (entry*)pa;
				auto b = (entry*)pb;
				if (a->hash() < b->hash()) {
					return -1;
				} else if (a->hash() > b->hash()) {
					return 1;
				}
				return 0;
			});
		}

		void ensure_memory(size_t count)
		{
			if (m_allocSize >= count) {
				return;
			}

			size_t resize = m_allocSize + m_allocSize / 2;
			if (resize < SIZE_MAX && resize > count) {
				count = resize;
			}

			m_entries = (entry*)realloc(m_entries, count * sizeof(entry));
			m_allocSize = count;
		}

		template<typename TComparable = T>
		int index_of(const TComparable& value) const
		{
			if (m_length == 0) {
				return -1;
			}

			size_t start = 0;
			size_t end = m_length;

			uint64_t valuehash = THasher::hash(value);

			while (true) {
				size_t halfLen = end - start;
				size_t halfIndex = start + halfLen / 2;
				if (halfIndex >= m_length) {
					throw setexception::unstable;
				}
				auto& e = m_entries[halfIndex];

				if (valuehash == e.hash()) {
					return (int)halfIndex;
				} else if (halfLen == 1) {
					return -1;
				} else if (valuehash > e.hash()) {
					start = halfIndex;
				} else if (valuehash < e.hash()) {
					end = halfIndex;
				}
			}

			return -1;
		}
	};
}

#ifdef S2_IMPL

uint64_t s2::default_hashers_set::hash(const char* key)
{
	// This is a modified MurmurHash64A by Austin Appleby

	const uint64_t seed = 0x2f97bc371e161991llu;
	const uint64_t m = 0xc6a4a7935bd1e995llu;
	const int r = 47;

	int len = (int)strlen(key);

	uint64_t h = seed ^ (len * m);

	const uint64_t* data = (const uint64_t*)key;
	const uint64_t* end = data + (len / 8);

	while (data != end) {
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char* data2 = (const unsigned char*)data;

	switch (len & 7) {
	case 7: h ^= uint64_t(data2[6]) << 48;
	case 6: h ^= uint64_t(data2[5]) << 40;
	case 5: h ^= uint64_t(data2[4]) << 32;
	case 4: h ^= uint64_t(data2[3]) << 24;
	case 3: h ^= uint64_t(data2[2]) << 16;
	case 2: h ^= uint64_t(data2[1]) << 8;
	case 1: h ^= uint64_t(data2[0]);
		h *= m;
	}

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

uint64_t s2::default_hashers_set::hash(int8_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(int16_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(int32_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(int64_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(uint8_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(uint16_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(uint32_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers_set::hash(uint64_t key) { return key; }
uint64_t s2::default_hashers_set::hash(float key) { return (uint64_t) * (uint32_t*)&key; }
uint64_t s2::default_hashers_set::hash(double key) { return *(uint64_t*)&key; }

#endif
