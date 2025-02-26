// reference.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <format>
class Number
{
public:
	Number();
	Number(int a, int b);
	~Number();
	Number operator++();
	Number operator++(int);
	friend Number operator +(const Number& lhs, const Number& rhs);
	friend Number operator -(const Number& lhs, const Number& rhs);
	friend Number operator *(const Number& lhs, const Number& rhs);
	friend Number operator /(const Number& lhs, const Number& rhs);
	friend Number operator +=(Number& lhs, const Number& rhs);
	friend Number operator -=(Number& lhs, const Number& rhs);
	friend Number operator *= (Number& lhs, const Number& rhs);
	friend Number operator /=(Number& lhs, const Number& rhs);
	friend struct std::formatter<Number>;
private:
	int a, b;
};
template<>
struct std::formatter<Number> {
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}
	auto format(const Number& num, std::format_context& ctx)
		const {
		return
			std::format_to(ctx.out(),
				"Number(a :{}, b: {})", num.a, num.b);
	}
};
