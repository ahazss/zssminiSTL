#pragma once
#include"zss_alloc.h"
#include"zss_iterator.h"
#include"zss_uninitialized.h"
#include"zss_deque.h"
#define __STL_NULL_TMPL_ARGS <> 

//��dequeΪ�ײ��ṹ�������ͷ�˿���
//���������޸�ĳ��ӿ��γ�����һ�ַ�ò�ĳ�Ϊ�����(adapter)������stack�ᱻ��Ϊcontainer adapter
//stack�������б�����Ϊ�����Բ��ṩ������

template <typename T, typename Sequence = deque<T>>
class queue {
public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	Sequence c;               //�ײ�����

public:
	//����Sequence c�Ĳ������queue�Ĳ���
	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	reference front() { return c.front(); }
	const_reference front() const { return c.front(); }
	reference back() { return c.back(); }
	const_reference back() const { return c.back(); }
	//deque����ͷ�ɽ���stack��β�˽���β�˳������Է���back��ֵ
	void push(const value_type& x) { c.push_back(x); }
	void pop() { c.pop_front(); }
	bool operator== (const queue& x);
	bool operator< (const queue& x);
};

template<typename T, typename Sequence>
inline bool queue<T, Sequence>::operator==(const queue<T, Sequence>& x)
{
	return this->c == x.c;
}
template<typename T, typename Sequence>
inline bool queue<T, Sequence>::operator<(const queue<T, Sequence>& x)
{
	return this->c < x.c;
}

