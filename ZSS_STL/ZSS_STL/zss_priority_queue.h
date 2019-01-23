#pragma once
#include"zss_vector.h"
#include"zss_algorithm.h"
using namespace ZSS;

//û��ʵ�ֿ��Զ���ȽϷ�ʽ��
//ȱʡ�����max_heap��Ϊ�ײ��������γ�max_heap
template <typename T, typename Sequence=vector<T>, typename Compare=std::less<typename Sequence::value_type>>
class priority_queue {
public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	Sequence c;
	Compare comp;
	
public:
	priority_queue() : c() { }
	explicit priority_queue(const Compare& x): c(), comp(x) { }

	//�����õ���make_heap(),push_heap(),pop_heap()���Ƿ����㷨
	/*
    template <typename InputIterator>
	priority_queue(InputIterator first,InputIterator last, const Compare& x)
		: c(first, last), comp(x) {  make_heap(c.begin(), c.end(),comp); }
		*/
	template <typename InputIterator>
	priority_queue(InputIterator first, InputIterator last)
		: c(first, last) { make_heap(c.begin(), c.end()); }
	
	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	reference top() { return c.front(); }
	const_reference top() const { return c.front(); }
	void push(const value_type& x) {
		c.push_back(x);
		push_heap(c.begin(), c.end());
	}
	void pop() {
		pop_heap(c.begin(), c.end());
		c.pop_back();
	}

};