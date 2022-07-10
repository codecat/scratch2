#pragma once

#define S2_USING_HASHTABLE

#include <cstring>
#include <cstdint>
#include <new>

namespace s2
{
	enum class hashtableexception
	{
		no_such_key,
		duplicate_key,
		index_out_of_range,
		unstable,
	};

	class default_hashers
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

	template<typename TKey, typename TValue, typename THasher = default_hashers>
	class hashtable
	{
	public:
		struct entry
		{
			uint64_t hash;
			TKey key;
			TValue value;
		};

		class iterator
		{
		private:
			typedef hashtable<TKey, TValue> hashtable_type;

		private:
			hashtable_type* m_hashtable;
			size_t m_index;

		public:
			iterator(hashtable_type* hashtable, size_t index)
			{
				m_hashtable = hashtable;
				m_index = index;
			}

			bool operator ==(const iterator& other)
			{
				return !operator !=(other);
			}

			bool operator !=(const iterator& other)
			{
				return m_hashtable != other.m_hashtable || m_index != other.m_index;
			}

			iterator& operator ++()
			{
				m_index++;
				return *this;
			}

			entry& operator *()
			{
				return m_hashtable->m_entries[m_index];
			}
		};

	private:
		entry* m_entries = nullptr;
		size_t m_length = 0;
		size_t m_allocSize = 0;

	public:
		hashtable()
		{
		}

		hashtable(const hashtable& other)
		{
			*this = other;
		}

		~hashtable()
		{
			clear();
			if (m_entries != nullptr) {
				free(m_entries);
			}
		}

		hashtable& operator =(const hashtable& other)
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

		TValue& add_unsorted(const TKey& key)
		{
			ensure_memory(m_length + 1);
			entry* ret = new (m_entries + m_length) entry;
			m_length++;
			ret->hash = THasher::hash(key);
			ret->key = key;
			return ret->value;
		}

		TValue& add(const TKey& key)
		{
			ensure_memory(m_length + 1);

			uint64_t keyhash = THasher::hash(key);

			size_t newIndex = m_length;

			if (m_length > 0) {
				if (keyhash < m_entries[0].hash) {
					newIndex = 0;
				} else if (keyhash > m_entries[m_length - 1].hash) {
					newIndex = m_length;
				} else {
					size_t start = 0;
					size_t end = m_length;

					while (true) {
						size_t halfLen = end - start;
						size_t halfIndex = start + halfLen / 2;
						if (halfIndex >= m_length) {
							throw hashtableexception::unstable;
						}
						auto& e = m_entries[halfIndex];

						if (e.hash == keyhash) {
							throw hashtableexception::duplicate_key;
						}

						if (halfLen == 1) {
							newIndex = halfIndex;
							if (keyhash > e.hash) {
								newIndex++;
							}
							break;
						} else if (keyhash > e.hash) {
							start = halfIndex;
						} else if (keyhash < e.hash) {
							end = halfIndex;
						}
					}
				}

				if (newIndex < m_length) {
					memmove(m_entries + newIndex + 1, m_entries + newIndex, (m_length - newIndex) * sizeof(entry));
				}
			}

			m_length++;

			if (newIndex > 0 && keyhash <= m_entries[newIndex - 1].hash) {
				throw hashtableexception::unstable;
			}
			if (newIndex < m_length - 1 && keyhash >= m_entries[newIndex + 1].hash) {
				throw hashtableexception::unstable;
			}

			entry* ret = new (m_entries + newIndex) entry;
			ret->hash = keyhash;
			ret->key = key;
			return ret->value;
		}

		void add(const TKey& key, const TValue& value, bool sort = true)
		{
			if (sort) {
				add(key) = value;
			} else {
				add_unsorted(key) = value;
			}
		}

		void set(const TKey& key, const TValue& value)
		{
			int index = index_of(key);
			if (index == -1) {
				add(key) = value;
				return;
			}
			m_entries[index].value = value;
		}

		void remove(const TKey& key)
		{
			int index = index_of(key);
			if (index == -1) {
				throw hashtableexception::no_such_key;
			}
			remove_at(index);
		}

		void remove_at(size_t index)
		{
			if (index >= m_length) {
				throw hashtableexception::index_out_of_range;
			}
			m_entries[index].~entry();
			if (index != m_length - 1) {
				memmove(m_entries + index, m_entries + index + 1, (m_length - index - 1) * sizeof(entry));
			}
			m_length--;
		}

		bool contains(const TKey& key) const
		{
			return index_of(key) != -1;
		}

		bool get(const TKey& key, TValue& value) const
		{
			int index = index_of(key);
			if (index == -1) {
				return false;
			}

			value = m_entries[index].value;
			return true;
		}

		entry& at(size_t index)
		{
			if (index >= m_length) {
				throw hashtableexception::index_out_of_range;
			}
			return m_entries[index];
		}

		const entry& at(size_t index) const
		{
			if (index >= m_length) {
				throw hashtableexception::index_out_of_range;
			}
			return m_entries[index];
		}

		TValue& operator [](const TKey& key)
		{
			int index = index_of(key);
			if (index == -1) {
				return add(key);
			}
			return m_entries[index].value;
		}

		const TValue& operator [](const TKey& key) const
		{
			int index = index_of(key);
			if (index == -1) {
				throw hashtableexception::no_such_key;
			}
			return m_entries[index].value;
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

		void sort()
		{
			qsort(m_entries, m_length, sizeof(entry), [](const void* pa, const void* pb) {
				auto a = (entry*)pa;
				auto b = (entry*)pb;
				if (a->hash < b->hash) {
					return -1;
				} else if (a->hash > b->hash) {
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

		int index_of(const TKey& key) const
		{
			if (m_length == 0) {
				return -1;
			}

			size_t start = 0;
			size_t end = m_length;

			uint64_t keyhash = THasher::hash(key);

			while (true) {
				size_t halfLen = end - start;
				size_t halfIndex = start + halfLen / 2;
				if (halfIndex >= m_length) {
					throw hashtableexception::unstable;
				}
				auto& e = m_entries[halfIndex];

				if (keyhash == e.hash) {
					return (int)halfIndex;
				} else if (halfLen == 1) {
					return -1;
				} else if (keyhash > e.hash) {
					start = halfIndex;
				} else if (keyhash < e.hash) {
					end = halfIndex;
				}
			}

			return -1;
		}
	};
}

#ifdef S2_IMPL

uint64_t s2::default_hashers::hash(const char* key)
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

uint64_t s2::default_hashers::hash(int8_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(int16_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(int32_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(int64_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(uint8_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(uint16_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(uint32_t key) { return (uint64_t)key; }
uint64_t s2::default_hashers::hash(uint64_t key) { return key; }
uint64_t s2::default_hashers::hash(float key) { return (uint64_t) * (uint32_t*)&key; }
uint64_t s2::default_hashers::hash(double key) { return *(uint64_t*)&key; }

#endif
