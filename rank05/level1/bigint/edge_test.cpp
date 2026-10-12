// Cas limites que le main du sujet ne teste pas (mais qu'un correcteur peut tester).
#include "bigint.hpp"
#include <iostream>
#include <sstream>

int main()
{
	bigint	zero, a(42), b(42), c(9), d(10), big(4294967295u);

	std::cout << "0 << 3 = " << (zero << 3) << std::endl;                  // 0, pas 0000
	std::cout << "5 >> 3 = " << (bigint(5) >> 3) << std::endl;              // 0
	std::cout << "42 >> 2 = " << (a >> 2) << std::endl;                     // 0
	std::cout << "a > b = " << (a > b) << ", a >= b = " << (a >= b)         // 0, 1
			  << ", a < b = " << (a < b) << ", a <= b = " << (a <= b) << std::endl;
	std::cout << "9 < 10 = " << (c < d) << ", 10 > 9 = " << (d > c) << std::endl;
	std::cout << "999...9 + 1 = " << (bigint(999999999) + 1) << std::endl;
	std::cout << "UINT_MAX + UINT_MAX = " << (big + big) << std::endl;
	bigint	x(1);
	x <<= 30;
	x += big;
	std::cout << "10^30 + UINT_MAX = " << x << std::endl;
	std::ostringstream	ss;
	ss << a;
	std::cout << "ostringstream : [" << ss.str() << "]" << std::endl;     // << doit ecrire dans os
	std::cout << "(a << (const bigint)2) = " << (a << (const bigint)2) << std::endl;
	return (0);
}
