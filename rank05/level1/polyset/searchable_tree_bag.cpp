#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag() : tree_bag() {}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag &o) : tree_bag(o) {}
searchable_tree_bag::~searchable_tree_bag() {}

searchable_tree_bag	&searchable_tree_bag::operator=(const searchable_tree_bag &o)
{
	tree_bag::operator=(o);
	return (*this);
}

/* recherche dans un ABR : plus petit -> gauche, plus grand -> droite */
bool	searchable_tree_bag::has(int value) const
{
	node	*cur = tree;

	while (cur)
	{
		if (value == cur->value)
			return (true);
		cur = (value < cur->value) ? cur->l : cur->r;
	}
	return (false);
}
