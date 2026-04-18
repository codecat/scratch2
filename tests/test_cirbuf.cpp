#include <s2cirbuf.h>

#include <s2test.h>

void test_cirbuf()
{
	s2::test_group("cirbuf");

	s2::cirbuf buf(5);
	S2_TEST(buf.size() == 0);

	buf.push(1);
	S2_TEST(buf.size() == 1);

	buf.push(2);
	buf.push(3);

	S2_TEST(buf.size() == 3);
	S2_TEST(buf.tell_head() == 3);
	S2_TEST(buf.tell_tail() == 0);

	S2_TEST(buf.take() == 1);
	S2_TEST(buf.size() == 2);

	S2_TEST(buf.take() == 2);
	S2_TEST(buf.size() == 1);

	S2_TEST(buf.take() == 3);
	S2_TEST(buf.size() == 0);

	S2_TEST(buf.tell_head() == 3);
	S2_TEST(buf.tell_tail() == 3);

	uint8_t four[] = { 1, 2, 3, 4 };
	buf.push(four, sizeof(four));
	S2_TEST(buf.size() == 4);
	S2_TEST(buf.tell_head() == 2);

	memset(four, 0, sizeof(four));
	buf.take(four, sizeof(four));

	S2_TEST(buf.size() == 0);
	S2_TEST(buf.tell_tail() == buf.tell_head());
	S2_TEST(four[0] == 1 && four[1] == 2 && four[2] == 3 && four[3] == 4);

	buf.push(123);
	S2_TEST(buf.peek() == 123);
	S2_TEST(buf.take() == 123);

	buf.push(1);
	buf.push(2);
	buf.push(3);
	S2_TEST(buf.size() == 3);
	buf.set_capacity(3);
	S2_TEST(buf.size() == 3);
	S2_TEST(buf.capacity() == 3);
	S2_TEST(buf.tell_head() == 0);
	S2_TEST(buf.take() == 1);
	S2_TEST(buf.take() == 2);
	S2_TEST(buf.take() == 3);
	S2_TEST(buf.tell_tail() == 0);

	s2::cirbuf_typed<uint32_t> numbuf(5);
	S2_TEST(numbuf.size() == 0);

	numbuf.push(1);
	S2_TEST(numbuf.size() == 1);

	numbuf.push(2);
	numbuf.push(3);

	S2_TEST(numbuf.size() == 3);
	S2_TEST(numbuf.tell_head() == 3);
	S2_TEST(numbuf.tell_tail() == 0);

	S2_TEST(numbuf.take() == 1);
	S2_TEST(numbuf.size() == 2);

	S2_TEST(numbuf.take() == 2);
	S2_TEST(numbuf.size() == 1);

	S2_TEST(numbuf.take() == 3);
	S2_TEST(numbuf.size() == 0);

	S2_TEST(numbuf.tell_head() == 3);
	S2_TEST(numbuf.tell_tail() == 3);

	uint32_t numfour[] = { 1, 2, 3, 4 };
	numbuf.push(numfour, 4);
	S2_TEST(numbuf.size() == 4);
	S2_TEST(numbuf.tell_head() == 2);

	memset(numfour, 0, sizeof(numfour));
	numbuf.take(numfour, 4);

	S2_TEST(numbuf.size() == 0);
	S2_TEST(numbuf.tell_tail() == numbuf.tell_head());
	S2_TEST(four[0] == 1 && four[1] == 2 && four[2] == 3 && four[3] == 4);

	numbuf.push(123);
	S2_TEST(numbuf.peek() == 123);
	S2_TEST(numbuf.take() == 123);

	s2::cirbuf buf2(10);
	S2_TEST(buf2.capacity() == 10);
	buf2.ensure_capacity_for_push(10);
	S2_TEST(buf2.capacity() >= 10);
	buf2.ensure_capacity_for_push(11);
	S2_TEST(buf2.capacity() >= 11);
	buf2.ensure_capacity_for_push(15);
	S2_TEST(buf2.capacity() >= 15);
	buf2.ensure_capacity(30);
	S2_TEST(buf2.capacity() >= 30);

	s2::cirbuf buf3(3);
	int n = 0;
	for (auto& pair : buf3.push_map(1)) {
		pair.buffer[0] = 1;
		S2_TEST(pair.size == 1);
		n++;
	}
	S2_TEST(n == 1);
	S2_TEST(buf3.take() == 1);
	S2_TEST(buf3.tell_head() == 1);

	auto map = buf3.push_map(3);
	S2_TEST(map.first.size == 2);
	S2_TEST(map.rest.size == 1);
	n = 0;
	for (auto& pair : map) {
		n++;
	}
	S2_TEST(n == 2);

	map = buf3.take_map(3);
	S2_TEST(map.first.size == 2);
	S2_TEST(map.rest.size == 1);
	n = 0;
	for (auto& pair : map) {
		n++;
	}
	S2_TEST(n == 2);

	n = 0;
	for (auto& p : buf3.peek_map(0)) {
		S2_TEST(p.size == 0);
		n++;
	}
	S2_TEST(n == 1);
}
