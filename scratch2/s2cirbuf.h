#pragma once

#define S2_USING_CIRBUF

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace s2
{
	enum class cirbufexception
	{
		not_enough_capacity,
		not_enough_bytes,
		new_capacity_wont_fit,
		invalid_capacity,
	};

	struct cirbuf_mapping
	{
		struct pair
		{
			uint8_t* buffer;
			size_t size;
		};

		struct iterator
		{
		private:
			const cirbuf_mapping* m_map;
			const pair* m_pair;

		public:
			inline iterator(const cirbuf_mapping* map, const pair* pair)
			{
				m_map = map;
				m_pair = pair;
			}

			inline bool operator ==(const iterator& other) { return !operator !=(other); }
			inline bool operator !=(const iterator& other) { return m_map != other.m_map || m_pair != other.m_pair; }

			inline iterator& operator ++()
			{
				if (m_pair == &m_map->first && m_map->rest.buffer != nullptr) {
					m_pair = &m_map->rest;
				} else {
					m_pair = nullptr;
				}
				return *this;
			}

			inline const pair& operator *() { return *m_pair; }
		};

		pair first;
		pair rest;

		inline iterator begin() const { return iterator(this, &first); }
		inline iterator end() const { return iterator(this, nullptr); }

		inline size_t size() const { return first.size + rest.size; }

		inline void write_from(const uint8_t* buffer)
		{
			memcpy(first.buffer, buffer, first.size);
			if (rest.buffer != nullptr) {
				memcpy(rest.buffer, buffer + first.size, rest.size);
			}
		}

		inline void read_to(uint8_t* buffer) const
		{
			memcpy(buffer, first.buffer, first.size);
			if (rest.buffer != nullptr) {
				memcpy(buffer + first.size, rest.buffer, rest.size);
			}
		}
	};

	class cirbuf
	{
	private:
		uint8_t* m_buffer;
		uint8_t* m_bufferEnd;
		size_t m_capacity;
		size_t m_size;
		uint8_t* m_head;
		uint8_t* m_tail;

	public:
		inline cirbuf(size_t capacity)
		{
			if (capacity == 0) {
				throw cirbufexception::invalid_capacity;
			}
			m_buffer = (uint8_t*)malloc(capacity);
			m_bufferEnd = m_buffer + capacity;
			m_capacity = capacity;
			m_size = 0;
			m_head = m_tail = m_buffer;
		}

		inline cirbuf(const cirbuf&) = delete;
		inline ~cirbuf() { free(m_buffer); }

		inline size_t capacity() const { return m_capacity; }
		inline size_t size() const { return m_size; }

		inline size_t tell_head() const { return m_head - m_buffer; }
		inline size_t tell_tail() const { return m_tail - m_buffer; }

		// Changes the capacity of the buffer, keeping its contents.
		inline void set_capacity(size_t capacity)
		{
			if (capacity < m_size) {
				throw cirbufexception::new_capacity_wont_fit;
			} else if (capacity == 0) {
				throw cirbufexception::invalid_capacity;
			}

			auto newBuffer = (uint8_t*)malloc(capacity);
			peek(newBuffer, m_size);

			free(m_buffer);
			m_buffer = newBuffer;
			m_bufferEnd = newBuffer + capacity;
			m_capacity = capacity;

			m_tail = newBuffer;
			m_head = newBuffer + m_size;
			if (m_head == m_bufferEnd) {
				m_head = m_buffer;
			}
		}

		// Ensure the capacity of the buffer is at least the given capacity.
		inline void ensure_capacity(size_t capacity)
		{
			if (capacity > m_capacity) {
				// Resize in chunks to avoid re-allocating too often
				size_t resize = m_capacity + m_capacity / 2;
				if (resize < SIZE_MAX && resize > capacity) {
					capacity = resize;
				}

				set_capacity(capacity);
			}
		}

		// Ensure there's enough capacity for the given size.
		// This is the same as b.ensure_capacity(b.size() + size).
		inline void ensure_capacity_for_push(size_t size) { ensure_capacity(m_size + size); }

		// Allocates space on the buffer and returns the pointers and sizes to write to.
		// 
		// Be careful when using this in combination with indeterminate streams: you're expected to write to *all* bytes of
		// the returned map. For example, writing less than `first.size` bytes to `first.buffer` and then writing some bytes
		// to `rest.buffer` means the data will be fragmented with garbage data! You should only use this function when you
		// are certain you are going to be able to write all the necessary bytes.
		inline cirbuf_mapping push_map(size_t size)
		{
			if (m_size + size > m_capacity) {
				throw cirbufexception::not_enough_capacity;
			}

			cirbuf_mapping ret;
			ret.first.buffer = m_head;

			size_t available = m_bufferEnd - m_head;
			if (size > available) {
				ret.first.size = available;
				ret.rest.buffer = m_buffer;
				ret.rest.size = size - available;
				m_head = m_buffer + ret.rest.size;
			} else {
				ret.first.size = size;
				ret.rest.buffer = nullptr;
				ret.rest.size = 0;
				m_head += size;
			}

			if (m_head == m_bufferEnd) {
				m_head = m_buffer;
			}

			m_size += size;

			return ret;
		}

		// Peeks bytes from the buffer and returns the pointers and sizes to read from.
		inline cirbuf_mapping peek_map(size_t size) const
		{
			if (size > m_size) {
				throw cirbufexception::not_enough_bytes;
			}

			cirbuf_mapping ret;
			ret.first.buffer = m_tail;

			size_t available = m_bufferEnd - m_tail;
			if (size > available) {
				ret.first.size = available;
				ret.rest.buffer = m_buffer;
				ret.rest.size = size - available;
			} else {
				ret.first.size = size;
				ret.rest.buffer = nullptr;
				ret.rest.size = 0;
			}

			return ret;
		}

		// Takes bytes from the buffer and returns the pointers and sizes to read from.
		// 
		// While usually less relevant, the warning for `push_map` also counts for `take_map`. You are expected to use all
		// bytes returned in the map.
		inline cirbuf_mapping take_map(size_t size)
		{
			cirbuf_mapping ret = peek_map(size);
			if (ret.rest.buffer != nullptr) {
				m_tail = m_buffer + ret.rest.size;
			} else {
				m_tail += ret.first.size;
			}

			if (m_tail == m_bufferEnd) {
				m_tail = m_buffer;
			}

			m_size -= size;

			return ret;
		}

		// Copies the given buffer onto the head.
		inline void push(const uint8_t* buffer, size_t size) { push_map(size).write_from(buffer); }

		// Pushes 1 byte to the buffer.
		inline void push(uint8_t byte) { push(&byte, sizeof(uint8_t)); }

		// Copies the given number of bytes from the tail into buffer.
		inline void take(uint8_t* buffer, size_t size) { take_map(size).read_to(buffer); }

		// Takes 1 byte from the buffer.
		inline uint8_t take()
		{
			uint8_t ret;
			take(&ret, sizeof(uint8_t));
			return ret;
		}

		// Copies the given number of bytes from the tail into buffer, but does not take the bytes off the buffer.
		inline void peek(uint8_t* buffer, size_t size) const { peek_map(size).read_to(buffer); }

		// Peeks 1 byte from the buffer.
		inline uint8_t peek() const
		{
			uint8_t ret;
			peek(&ret, sizeof(uint8_t));
			return ret;
		}
	};

	template<typename T>
	class cirbuf_typed
	{
	private:
		cirbuf m_buffer;

	public:
		inline cirbuf_typed(size_t capacity) : m_buffer(capacity * sizeof(T)) {}
		inline cirbuf_typed(const cirbuf_typed&) = delete;

		inline size_t size() const { return m_buffer.size() / sizeof(T); }
		inline size_t tell_head() const { return m_buffer.tell_head() / sizeof(T); }
		inline size_t tell_tail() const { return m_buffer.tell_tail() / sizeof(T); }

		inline void push(const T* buffer, size_t count) { m_buffer.push((uint8_t*)buffer, count * sizeof(T)); }
		inline void push(const T& obj) { m_buffer.push((const uint8_t*)&obj, sizeof(T)); }

		inline void take(T* buffer, size_t count) { m_buffer.take((uint8_t*)buffer, count * sizeof(T)); }
		inline void take(T& obj) { m_buffer.take((uint8_t*)&obj, sizeof(T)); }
		inline T take()
		{
			T ret{};
			take(ret);
			return ret;
		}

		inline void peek(T* buffer, size_t count) { m_buffer.peek((uint8_t*)buffer, count * sizeof(T)); }
		inline void peek(T& obj) { m_buffer.peek((uint8_t*)&obj, sizeof(T)); }
		inline T peek()
		{
			T ret{};
			peek(ret);
			return ret;
		}
	};
}
