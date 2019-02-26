#pragma once
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING
#include"zss_RB_tree.h"

namespace ZSS {
	template<typename Key, typename Compare = less<Key>, typename Alloc = alloc>
	class set
	{
	public:
		typedef Key key_type;
		typedef Key value_type;
		typedef Compare key_compare;
		typedef Compare value_compare;

	private:
		//以下的identity定义于<zss_function.h>中,定义为:
		/*
		template<typename T>
		struct identity:public unary_function<T,T>{
		   const T& operator()(const T& x) const{ return x; }
		};
		*/
		typedef rb_tree<key_type, value_type, std::identity<value_type>, key_compare, Alloc> rep_type;
		rep_type t;             //采用红黑树来表现set

	public:
		typedef typename rep_type::const_pointer pointer;
		typedef typename rep_type::const_pointer const_pointer;
		typedef typename rep_type::const_reference reference;
		typedef typename rep_type::const_reference const_reference;
		typedef typename rep_type::const_iterator iterator;
		//iterator定义为RB_tree的const_iterator,set的迭代器无法执行写入操作
		//pointer,const_pointer,reference,const_reference都是rb_tree中的const成员

		typedef typename rep_type::const_iterator const_iterator;
		//typedef typename rep_type::const_reverse_iterator reverse_iterator;
		//typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
		typedef typename rep_type::size_type size_type;
		typedef typename rep_type::difference_type difference_type;

		//allocation/deallocation
		//set使用rb_tree的insert_unique(),set不允许相同键值存在
		set() :t(Compare()) {}
		explicit set(const Compare& comp) {}

		template<typename InputIterator>
		set(InputIterator first, InputIterator last) : t(Compare()) { t.insert_unique(first, last); }

		template<typename InputIterator>
		set(InputIterator first, InputIterator last,const Compare& comp) : t(comp) { t.insert_unique(first, last); }

		set(const set<Key, Compare, Alloc>& x) :t(x.t) {}

		set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
			t = x.t;
			return *this;
		}

		//set的所有操作rb-tree均已实现，set仅传递调用
		key_compare key_comp() const { return t.key_comp(); }
		value_compare value_comp() const { return t.key_comp(); }

		iterator begin() const { return t.begin(); }
		iterator end() const { return t.end(); }
		//reverse_iterator rbegin() const { return t.rbegin(); }
		//reverse_iterator rend() const { return t.rend(); }

		bool empty() const { return t.empty(); }
		size_type size() const { return t.size(); }
		size_type max_size() const { return t.max_size(); }
		void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }

		//insert/erase
		typedef std::pair<iterator, bool> pair_iterator_bool;
		std::pair<iterator, bool> insert(const value_type& x) {
			std::pair<typename rep_type::iterator, bool> p = t.insert_equal(x);
			return pair<iterator, bool>(p.first, p, second);
		}
		/*
		iterator insert(iterator position, const value_type& x) {
			typedef typename rep_type::iterator rep_iterator;
			return t.insert_unique((rep_iterator&)position, x);
		}*/
		template<typename InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_unique(first, last);
		}
		/*
		void erase(iterator position)
		{
			typedef typename rep_type::iterator rep_iterator;
			t.__erase((rep_iterator&)position);
		}*/

		void clear() { t.clear(); }

		iterator find(const key_type& x) const { return t.find(x); }
		//size_type count(const key_type& x) const { return t.node_count(x); }
		
		bool operator==(const set& x) { return t == x.t; }
		bool operator!=(const set& x) { return t != x.t; }

	};
}