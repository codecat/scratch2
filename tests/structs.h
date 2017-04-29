extern int _numFooInstances;
extern int _numBarInstances;

class Foo
{
public:
	int num;

public:
	Foo();
	Foo(const Foo &copy);
	~Foo();
};

class Bar
{
public:
	int a;
	int b;
	int c;
	int d;

public:
	Bar();
	Bar(const Bar &copy);
	~Bar();
};
