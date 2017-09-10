#pragma once

#if defined(__APPLE__) && !defined(_XOPEN_SOURCE)
#error When using s2fiber.h on MacOS, you need to build with -D_XOPEN_SOURCE!
#endif

#define S2_USING_FIBER

namespace s2
{
	class fiber
	{
	public:
		typedef void(*func_type)(fiber&);
		typedef void* fiber_type;

	private:
#ifndef _MSC_VER
		void* m_stack;
#endif
		fiber_type m_fiber;
		fiber_type m_fiberParent;
		func_type m_func;
		bool m_finished;

		void* m_userdata;

	public:
		fiber(const func_type &func);
		~fiber();

		void clear();

		void yield();
		bool resume();

		void userdata(void* p);
		void* userdata();

		bool isfinished();

		void internal_execute();
	};
}

#ifdef S2_IMPL
#include <cstdio>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <cstdlib>
#include <ucontext.h>
#endif

namespace s2
{
#ifdef _MSC_VER
	static VOID CALLBACK fiberroutine(LPVOID p)
	{
		((fiber*)p)->internal_execute();
	}
#else
	static void fiberroutine(s2::fiber* p)
	{
		p->internal_execute();
	}
#endif
}

s2::fiber::fiber(const func_type &func)
{
#ifdef _MSC_VER
	if (!IsThreadAFiber()) {
		ConvertThreadToFiber(nullptr);
	}

	m_fiber = CreateFiber(0, fiberroutine, this);
	m_fiberParent = GetCurrentFiber();
#else
	m_fiber = new ucontext_t;
	m_fiberParent = new ucontext_t;

	getcontext((ucontext_t*)m_fiber);
	((ucontext_t*)m_fiber)->uc_stack.ss_sp = m_stack = malloc(SIGSTKSZ);
	((ucontext_t*)m_fiber)->uc_stack.ss_size = SIGSTKSZ;
	((ucontext_t*)m_fiber)->uc_link = (ucontext_t*)m_fiberParent;
	makecontext((ucontext_t*)m_fiber, (void(*)())fiberroutine, 1, this);
#endif

	m_func = func;
	m_finished = false;

	m_userdata = nullptr;
}

s2::fiber::~fiber()
{
	clear();
}

void s2::fiber::clear()
{
	if (m_fiber != nullptr) {
#ifdef _MSC_VER
		DeleteFiber(m_fiber);
#else
		free(m_stack);
		delete (ucontext_t*)m_fiber;
#endif
		m_fiber = nullptr;
	}

#ifndef _MSC_VER
	if (m_fiberParent != nullptr) {
		delete (ucontext_t*)m_fiberParent;
	}
#endif
	m_fiberParent = nullptr;
}

void s2::fiber::yield()
{
#ifdef _MSC_VER
	if (GetCurrentFiber() != m_fiber) {
#else
	if (false) { //
#endif
		printf("s2::fiber::yield() must be called from itself!\n");
		return;
	}

#ifdef _MSC_VER
	SwitchToFiber(m_fiberParent);
#else
	swapcontext((ucontext_t*)m_fiber, (ucontext_t*)m_fiberParent);
#endif
}

bool s2::fiber::resume()
{
	if (m_fiber == nullptr) {
		return false;
	}

#ifdef _MSC_VER
	if (GetCurrentFiber() != m_fiberParent) {
#else
	if (false) { //
#endif
		printf("s2::fiber::resume() must be called from its parent fiber!\n");
		return false;
	}

#ifdef _MSC_VER
	SwitchToFiber(m_fiber);
#else
	swapcontext((ucontext_t*)m_fiberParent, (ucontext_t*)m_fiber);
#endif

	if (m_finished) {
		clear();
	}
	return true;
}

void s2::fiber::userdata(void* p)
{
	m_userdata = p;
}

void* s2::fiber::userdata()
{
	return m_userdata;
}

bool s2::fiber::isfinished()
{
	return m_finished;
}

void s2::fiber::internal_execute()
{
#ifdef _MSC_VER
	if (GetCurrentFiber() != m_fiber) {
#else
	if (false) { //
#endif
		printf("s2::fiber::internal_execute() must be called from itself!\n");
		return;
	}
	m_func(*this);
	m_finished = true;
	yield();
}
#endif
