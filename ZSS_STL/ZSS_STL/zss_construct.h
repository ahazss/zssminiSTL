#pragma once
#include<new.h>

template <typename T1,typename T2>
inline void construct(T1 *p, const T2& value)
{
	new(p) T1(value);                               //在p所指向的内存空间(预分配),即operator new分配的，创建类型为T1的对象，构造函数接收const T2&的参数
}

//destroy第一版本，对一个对象进行处理
template <typename T>
inline void destroy(T* pointer)
{
	pointer->~T();                                  //仅调用析构，未释放
}

//destroy第二版本，接收两个迭代器，处理一个范围内的元素
//判断元素数值型别(value_type) 进而利用_type_traits<> 采取最适当措施(效率)
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
	__destroy(first, last, value_type(first));
}

//判断元素的value_type是否有trivial destructor
template <typename ForwardIterator,typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
	__destroy_aux(first, last, trivial_destructor());
}

//如果trivial_destructor为_false_type，说明该析构函数有用(涉及内存)
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __false_type)
{
	for (; first < last; ++first)
		destroy(&*first);
}

//如果trivial_destructor为_true_type，说明该析构函数没用(不涉及内存)
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

//因为char* wchar_t* 为内置类型，没有析构函数，即不需要__destroy __destroy_aux 进行判断，进行特化以提高效率
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}
