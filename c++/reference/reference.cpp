// reference.cpp: 定义应用程序的入口点。
//

#include "reference.h"
#include <print>

using namespace std;
void test_after_plus()
{

	Number n1{ 1, 2 }, n2{ 3,4 };
	auto temp = n1++;
	println("temp is {} , n1 is {}", temp, n1);
}
void test_new()
{
	auto pNumber = new Number();
	delete pNumber;
}

int main()
{
	test_new();
	return 0;
}

Number::Number() : a{ 0 }, b{ 0 }
{
}

Number::Number(int a, int b) : a{ a }, b{ b }
{
}

Number::~Number()
{
}

Number Number::operator++()
{
	a++;
	b++;
	return *this;
}

Number Number::operator++(int)
{
	auto temp = *this;
	++a;
	++b;
	return temp;
}

Number operator+(const Number& lhs, const Number& rhs)
{
	Number ans;
	ans.a = lhs.a + rhs.a;
	ans.b = lhs.b + rhs.b;
	return ans;
}

Number operator-(const Number& lhs, const Number& rhs)
{
	Number ans;
	ans.a = lhs.a - rhs.a;
	ans.b = lhs.b - rhs.b;
	return ans;
}

Number operator*(const Number& lhs, const Number& rhs)
{
	Number ans;
	ans.a = lhs.a * rhs.a;
	ans.b = lhs.b * rhs.b;
	return ans;
}

Number operator/(const Number& lhs, const Number& rhs)
{
	Number ans;
	ans.a = lhs.a / rhs.b;
	ans.b = lhs.b / rhs.b;
	return ans;
}

Number operator+=(Number& lhs, const Number& rhs)
{
	lhs.a += rhs.a;
	lhs.b += rhs.b;
	return lhs;
}

Number operator-=(Number& lhs, const Number& rhs)
{
	lhs.a -= rhs.a;
	lhs.b -= rhs.b;
	return lhs;
}

Number operator*=(Number& lhs, const Number& rhs)
{
	lhs.a *= rhs.a;
	lhs.b *= rhs.b;
	return lhs;
}

Number operator/=(Number& lhs, const Number& rhs)
{
	lhs.a /= rhs.a;
	lhs.b /= rhs.b;
	return lhs;
}
