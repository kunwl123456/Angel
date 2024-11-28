#ifndef FIXED_RBTREE_H
#define FIXED_RBTREE_H

#include <iterator>
#include "sk_utility.h"
#include "container/referable_array.h"
/*
namespace sk {
	namespace detail {
#define MIDDLE_ARRAY_BOUND (60000)
#define SMALL_ARRAY_BOUND (250)

		template<size_t N>
		struct rbtree_size_type {
			typedef size_t tree_size_t;
		};

		template<>
		struct rbtree_size_type<MIDDLE_ARRAY_BOUND> {
			typedef uint16_t tree_size_t;
		};

		template<>
		struct rbtree_size_type<SMALL_ARRAY_BOUND> {
			typedef uint8_t tree_size_t;
		};

		template<typename V, size_t N>
		struct rbtree_node {
			enum { size_bound = (N <= SMALL_ARRAY_BOUND) ? SMALL_ARRAY_BOUND : (N > MIDDLE_ARRAY_BOUND ? N : MIDDLE_ARRAY_BOUND) };

			typedef typename rbtree_size_type<size_bound>::tree_size_t tree_size_t;
			static const tree_size_t npos = static_cast<tree_size_t>(-1);
			static const char black = static_cast<char>(0);
			static const char red = static_cast<char>(1);

			char			color;
			tree_size_t		left;
			tree_size_t		right;
			tree_size_t		parent;
			V				value;

			rbtree_node(const V& v) :color(red), left(npos), right(npos), parent(npos), value(v) {}
		};

		template<typename T, BOOL C>
		struct rbtree_iterator {
			typedef T																		tree_type;
			typedef typename T::node														node_type;
			typedef typename T::value_type													value_type;
			typedef ptrdiff_t																difference_type;
			typedef rbtree_iterator<T, C>													self;
			typedef std::bidirectional_iterator_tag											iterator_category;
			typedef typename if_<C, const tree_type*, tree_type*>::type						tree_pointer;
			typedef typename if_<C, const node_type*, node_type*>::type						node_pointer;
			typedef typename if_<C, const value_type*, value_type*>::type					pointer;
			typedef typename if_<C, const value_type&, value_type&>::type					reference;

			tree_pointer t;
			node_pointer n;

			rbtree_iterator():t(NULL),n(NULL){}
			explicit rbtree_iterator(tree_pointer t,tree_pointer n):t(t),n(n){}


			template<BOOL B>
			rbtree_iterator(const rbtree_iterator<T,B>&s):t(s.t),n(s.n){}

			self& operator=(const self& s)
			{
				if (this == &s)
					return *this;
				t = s.t;
				n = s.n;

				return *this;
			}

			reference operator*()  const { return n->value; }
			reference operator->()  const { return &n->value; }

			self& operator++() { n = t->__next(n); return *this; }
			self  operator++(int) { self tmp(*this); n = t->__next(n); return tmp; }
			self& operator--() {
				if (n) n = t->__prev(n);
				else n = t->max();
				return *this;
			}
			self operator--(int) {
				self tmp(*this);
				if (n) n = t->__prev(n);
				else n = t->max();
				return tmp;
			}

			BOOL operator==(const self& x) const { return n == x.n && t == x.t; }
			BOOL operator!=(const self& x) const { return !(*this == x); }

			BOOL operator==(const rbtree_iterator<T, !C>& x) const { return n == x.n && t == x.t; }
			BOOL operator!=(const rbtree_iterator<T, !C>& x) const { return !(*this == x); }
		};
	}

	//template<typename K,typename V,typename F>


};
	*/

#endif //FIXED_RBTREE_H