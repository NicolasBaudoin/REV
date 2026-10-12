#pragma once

#include "searchable_bag.hpp"

/*
** set = un "wrapper" autour d'un searchable_bag existant (on garde une REFERENCE,
** on ne copie pas le sac) : insert n'ajoute que si !has().
*/
class set
{
	private:
		searchable_bag	&_bag;
		set();								// pas de set sans sac

	public:
		set(searchable_bag &bag);
		set(const set &o);					// partage le meme sac
		set &operator=(const set &o);
		~set();

		void	insert(int value);
		void	insert(int *values, int count);
		bool	has(int value) const;
		void	print() const;
		void	clear();

		searchable_bag			&get_bag();
		const searchable_bag	&get_bag() const;
};
