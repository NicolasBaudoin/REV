// Cas limites que le main du sujet ne teste pas.
#include "vect2.hpp"
#include <iostream>
#include <sstream>

int main()
{
	vect2				v(3, -4);
	const vect2			c(1, 2);
	std::ostringstream	ss;

	ss << v;                                              // operator<< doit ecrire dans os
	std::cout << "ostringstream : [" << ss.str() << "]" << std::endl;
	std::cout << "c * 3 = " << c * 3 << ", 3 * c = " << 3 * c << std::endl;
	std::cout << "c - v = " << c - v << ", -c = " << -c << std::endl;
	std::cout << "c[0] + c[1] = " << c[0] + c[1] << std::endl;
	std::cout << "(v2(2,2) * 2 == v2(4,4)) = " << (vect2(2, 2) * 2 == vect2(4, 4)) << std::endl;
	vect2	w = v;
	(w += c) -= c;
	std::cout << "chainage (w += c) -= c : " << w << std::endl;
	std::cout << "w++ puis w : " << w++ << " " << w << std::endl;
	return (0);
}
