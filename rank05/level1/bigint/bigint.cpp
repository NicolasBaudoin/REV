#include "bigint.hpp"

bigint::bigint() : _d("0") {}

bigint::bigint(unsigned int n)
{
	if (n == 0)
		_d = "0";
	while (n > 0)
	{
		_d.insert(_d.begin(), char('0' + n % 10));
		n /= 10;
	}
}

bigint::bigint(const bigint &o) : _d(o._d) {}

bigint	&bigint::operator=(const bigint &o)
{
	if (this != &o)
		_d = o._d;
	return (*this);
}

bigint::~bigint() {}

const std::string	&bigint::str() const { return (_d); }

unsigned int	bigint::to_uint() const
{
	unsigned int	n = 0;

	for (size_t i = 0; i < _d.size(); i++)
		n = n * 10 + (_d[i] - '0');
	return (n);
}

/* addition "a la main" : de droite a gauche, avec retenue */
bigint	bigint::operator+(const bigint &o) const
{
	bigint	res;
	int		i = _d.size() - 1;
	int		j = o._d.size() - 1;
	int		carry = 0;
	int		sum;

	res._d = "";
	while (i >= 0 || j >= 0 || carry)
	{
		sum = carry;
		if (i >= 0)
			sum += _d[i--] - '0';
		if (j >= 0)
			sum += o._d[j--] - '0';
		res._d.insert(res._d.begin(), char('0' + sum % 10));
		carry = sum / 10;
	}
	return (res);
}

bigint	&bigint::operator+=(const bigint &o) { return (*this = *this + o); }
bigint	&bigint::operator++() { return (*this += bigint(1)); }

bigint	bigint::operator++(int)
{
	bigint	old(*this);

	++(*this);
	return (old);
}

/* << n : ajoute n zeros a droite (sauf si on vaut 0 -> pas de zeros de tete !) */
bigint	bigint::operator<<(unsigned int n) const
{
	bigint	res(*this);

	if (_d != "0")
		res._d.append(n, '0');
	return (res);
}

/* >> n : enleve les n derniers chiffres (tout -> "0") */
bigint	bigint::operator>>(unsigned int n) const
{
	bigint	res(*this);

	if (n >= _d.size())
		res._d = "0";
	else
		res._d.erase(_d.size() - n);
	return (res);
}

bigint	&bigint::operator<<=(unsigned int n) { return (*this = *this << n); }
bigint	&bigint::operator>>=(unsigned int n) { return (*this = *this >> n); }
bigint	bigint::operator<<(const bigint &n) const { return (*this << n.to_uint()); }
bigint	bigint::operator>>(const bigint &n) const { return (*this >> n.to_uint()); }
bigint	&bigint::operator<<=(const bigint &n) { return (*this = *this << n.to_uint()); }
bigint	&bigint::operator>>=(const bigint &n) { return (*this = *this >> n.to_uint()); }

/* comparaison : d'abord la longueur, puis ordre alphabetique (meme longueur) */
bool	bigint::operator<(const bigint &o) const
{
	if (_d.size() != o._d.size())
		return (_d.size() < o._d.size());
	return (_d < o._d);
}

bool	bigint::operator==(const bigint &o) const { return (_d == o._d); }
bool	bigint::operator!=(const bigint &o) const { return (!(*this == o)); }
bool	bigint::operator>(const bigint &o) const { return (o < *this); }
bool	bigint::operator<=(const bigint &o) const { return (!(o < *this)); }
bool	bigint::operator>=(const bigint &o) const { return (!(*this < o)); }

std::ostream	&operator<<(std::ostream &os, const bigint &b)
{
	return (os << b.str());
}
