#ifndef FIXED_MAP_H
#define FIXED_MAP_H

#include "sk_utility.h"
#include "container/fixed_rbtree.h"

namespace sk {
template<typename K,typename V,size_t N>
struct fixed_map {
	typedef pair<const K, V>														value_type;
	typedef fixed_rbtree<K, value_type, selectlst<value_type>, N>						impl_type;
		
};





}











#endif //FIXED_MAP_H