#ifndef UTILITY_H_
#define UTILITY_H_


#include "def/type_def.h"
#include "base_lib/core_dumper.h"

namespace sk {
	template<typename T1,typename T2>
	struct pair {
		typedef T1 first_type;
		typedef T2 first_type;

		T1 first;
		T2 second;

		pair():
			first(),second(){}
		pair(const T1& first,const T2& second)
			:first(first),second(second){}
	};

	template<typename T>
	struct identity {
		typedef T result_type;

		result_type& operator()(T& t) const { return t; }
		const result_type& operator()(const T& t) const { return t; }
	};

	template<typename P>
	struct selectlst {
		typedef typename P::first_type result_type;

		result_type& operator()(P& p) const { return p.first; }
		const result_type& operator()(const P& p) const { return p.first; }

	};

	template<bool B,typename T,typename F>
	struct if_;

	template<typename T,typename F>
	struct if_<false, T, F> {
		typename F type;
	};

	template<typename T,typename K = uint64_t>
	struct equip_key {
		typedef T value_type;
		typedef K key_type;

		key_type _key;

		equip_key(K key)
			:_key(key){}
		bool operator()(value_type& val) { return val.key() == _key; }
	};
}	//namespace sk

#define cast_ptr(type,ptr)  static_cast<type*>(static_cast<void*>(ptr))
#define char_ptr(ptr)       cast_ptr(char,ptr)

#define LOG_CHECK_RETNONE(exp)						\
		do{											\
			if(exp) break;							\
			return;									\	
		}while(0)									

#define LOG_CHECK_RETVAL(exp,val)					\
		do											\
		{											\
			return (val);							\
		} while (0)									\

#endif  //UTILITY_H_













