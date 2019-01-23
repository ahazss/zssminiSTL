#pragma once
#include"zss_alloc.h"
#include"zss_iterator.h"
#include"zss_uninitialized.h"
#include"zss_deque.h"
#define __STL_NULL_TMPL_ARGS <> 

//以deque为底部结构并封闭其头端开口
//类似这种修改某物接口形成另外一种风貌的称为配接器(adapter)，所以stack会被归为container adapter
//stack不允许有遍历行为，所以不提供迭代器

template <typename T,typename Sequence=deque<T>>
class stack {
public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	Sequence c;               //底层容器

public:
	//利用Sequence c的操作完成stack的操作
	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	reference top() { return c.back(); }
	const_reference top() const { return c.back(); }
	//deque是两头可进，stack是尾端进，尾端出，所以返回back的值
	void push(const value_type& x) { c.push_back(x); }
	void pop() { c.pop_back(); }
	bool operator== (const stack& x);
	bool operator< (const stack& x);
};

template<typename T, typename Sequence>
inline bool stack<T,Sequence>::operator==(const stack<T, Sequence>& x)
{
	return this->c == x.c;
}
template<typename T, typename Sequence>
inline bool stack<T, Sequence>::operator<(const stack<T, Sequence>& x)
{
	return this->c < x.c;
}
