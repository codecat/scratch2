#include "structs.h"

int _numFooInstances = 0;
int _numBarInstances = 0;

Foo::Foo()
{
	_numFooInstances++;
	num = 0;
}

Foo::Foo(const Foo &copy)
{
	_numFooInstances++;
	num = copy.num;
}

Foo::~Foo()
{
	_numFooInstances--;
}

Bar::Bar()
{
	_numBarInstances++;
}

Bar::Bar(const Bar &copy)
{
	_numBarInstances++;
	a = copy.a;
	b = copy.b;
	c = copy.c;
	d = copy.d;
}

Bar::~Bar()
{
	_numBarInstances--;
}
