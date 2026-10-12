#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

class vect2
{
	private:
		int	_v[2];

	public:
		vect2();
		vect2(int x, int y);
		vect2(const vect2 &o);
		vect2 &operator=(const vect2 &o);
		~vect2();

		int		operator[](int i) const;	// lecture sur un const vect2
		int		&operator[](int i);			// ecriture : v[1] = 12

		vect2	&operator+=(const vect2 &o);
		vect2	&operator-=(const vect2 &o);
		vect2	&operator*=(int k);
		vect2	operator+(const vect2 &o) const;
		vect2	operator-(const vect2 &o) const;
		vect2	operator*(int k) const;		// v * 2
		vect2	operator-() const;			// -v

		vect2	&operator++();				// ++v
		vect2	operator++(int);			// v++
		vect2	&operator--();
		vect2	operator--(int);

		bool	operator==(const vect2 &o) const;
		bool	operator!=(const vect2 &o) const;
};

vect2			operator*(int k, const vect2 &v);	// 3 * v (hors classe !)
std::ostream	&operator<<(std::ostream &os, const vect2 &v);

#endif
