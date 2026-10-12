#include "vect2.hpp"

vect2::vect2() { _v[0] = 0; _v[1] = 0; }
vect2::vect2(int x, int y) { _v[0] = x; _v[1] = y; }
vect2::vect2(const vect2 &o) { *this = o; }
vect2::~vect2() {}

vect2	&vect2::operator=(const vect2 &o)
{
	_v[0] = o._v[0];
	_v[1] = o._v[1];
	return (*this);
}

int		vect2::operator[](int i) const { return (_v[i]); }
int		&vect2::operator[](int i) { return (_v[i]); }

/* on ecrit les "op=" et on derive les autres a partir d'eux */
vect2	&vect2::operator+=(const vect2 &o) { _v[0] += o._v[0]; _v[1] += o._v[1]; return (*this); }
vect2	&vect2::operator-=(const vect2 &o) { _v[0] -= o._v[0]; _v[1] -= o._v[1]; return (*this); }
vect2	&vect2::operator*=(int k) { _v[0] *= k; _v[1] *= k; return (*this); }

vect2	vect2::operator+(const vect2 &o) const { vect2 r(*this); return (r += o); }
vect2	vect2::operator-(const vect2 &o) const { vect2 r(*this); return (r -= o); }
vect2	vect2::operator*(int k) const { vect2 r(*this); return (r *= k); }
vect2	vect2::operator-() const { return (vect2(-_v[0], -_v[1])); }

vect2	&vect2::operator++() { return (*this += vect2(1, 1)); }
vect2	&vect2::operator--() { return (*this -= vect2(1, 1)); }
vect2	vect2::operator++(int) { vect2 old(*this); ++(*this); return (old); }
vect2	vect2::operator--(int) { vect2 old(*this); --(*this); return (old); }

bool	vect2::operator==(const vect2 &o) const { return (_v[0] == o._v[0] && _v[1] == o._v[1]); }
bool	vect2::operator!=(const vect2 &o) const { return (!(*this == o)); }

vect2	operator*(int k, const vect2 &v) { return (v * k); }

std::ostream	&operator<<(std::ostream &os, const vect2 &v)
{
	return (os << "{" << v[0] << ", " << v[1] << "}");	/* os, PAS std::cout */
}
