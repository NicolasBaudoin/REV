#include "set.hpp"

set::set(searchable_bag &bag) : _bag(bag) {}
set::set(const set &o) : _bag(o._bag) {}
set::~set() {}

/* une reference ne peut pas etre "re-pointee" : on garde notre sac */
set	&set::operator=(const set &o)
{
	(void)o;
	return (*this);
}

void	set::insert(int value)
{
	if (!_bag.has(value))
		_bag.insert(value);
}

void	set::insert(int *values, int count)
{
	for (int i = 0; i < count; i++)
		insert(values[i]);
}

bool	set::has(int value) const { return (_bag.has(value)); }
void	set::print() const { _bag.print(); }
void	set::clear() { _bag.clear(); }

searchable_bag			&set::get_bag() { return (_bag); }
const searchable_bag	&set::get_bag() const { return (_bag); }
