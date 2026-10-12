#include "searchable_array_bag.hpp"

searchable_array_bag::searchable_array_bag() : array_bag() {}
searchable_array_bag::searchable_array_bag(const searchable_array_bag &o) : array_bag(o) {}
searchable_array_bag::~searchable_array_bag() {}

searchable_array_bag	&searchable_array_bag::operator=(const searchable_array_bag &o)
{
	array_bag::operator=(o);
	return (*this);
}

/* recherche lineaire dans data[0..size-1] */
bool	searchable_array_bag::has(int value) const
{
	for (int i = 0; i < size; i++)
		if (data[i] == value)
			return (true);
	return (false);
}
