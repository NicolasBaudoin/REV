#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <string>
#include <iostream>

/*
** On stocke le nombre en string de chiffres, poids fort en premier : 1337 -> "1337".
** Invariant : jamais de zero de tete (sauf "0" lui-meme).
*/
class bigint
{
	private:
		std::string	_d;
		unsigned int	to_uint() const;

	public:
		bigint();
		bigint(unsigned int n);
		bigint(const bigint &o);
		bigint &operator=(const bigint &o);
		~bigint();

		const std::string	&str() const;

		bigint	operator+(const bigint &o) const;
		bigint	&operator+=(const bigint &o);
		bigint	&operator++();			// ++b
		bigint	operator++(int);		// b++

		bigint	operator<<(unsigned int n) const;
		bigint	operator>>(unsigned int n) const;
		bigint	&operator<<=(unsigned int n);
		bigint	&operator>>=(unsigned int n);
		bigint	operator<<(const bigint &n) const;
		bigint	operator>>(const bigint &n) const;
		bigint	&operator<<=(const bigint &n);
		bigint	&operator>>=(const bigint &n);

		bool	operator==(const bigint &o) const;
		bool	operator!=(const bigint &o) const;
		bool	operator<(const bigint &o) const;
		bool	operator>(const bigint &o) const;
		bool	operator<=(const bigint &o) const;
		bool	operator>=(const bigint &o) const;
};

std::ostream	&operator<<(std::ostream &os, const bigint &b);

#endif
