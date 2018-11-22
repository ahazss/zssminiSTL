#pragma once
#include"stl_construct.h"
#include<string>

//uninitialized_copy
template<typename InputIterator,typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
	                              ForwardIterator result)
{
	//first指向输入端起始位置，last指向输入端结束位置(前闭后开区间)
	//result指向输出端(欲初始化空间)的起始处
	return __uninitialized_copy(first, last, result, value_type(result));
}

template<typename InputIterator, typename ForwardIterator,typename T>
ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
	                                 ForwardIterator result,T*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __uninitialized_copy_aux(first, last, result, is_POD());
}

//见fill_n
template<typename InputIterator, typename ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
	                                     ForwardIterator result,__true_type)
{
	return copy(first, last, result);           //调用高级算法copy()
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
	                                     ForwardIterator result,__false_type)
{
	ForwardIterator cur = result;
	//省略异常处理机制
	for (; first != last; ++first, ++cur)
	{
		construct(&*cur, *first);
	}
	return cur;
}

//针对char*和wchar_t*两种型别，可以采取更具效率的做法remove(直接移动内存内容)来执行复制行为，以下为特化版本
inline char* uninitialized_copy(const char* first, const char* last,
	                            char* result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last,
	                               wchar_t* result)
{
	memmove(result, first, sizeof(wchar_t)*(last - first));
	return result + (last - first);
}

template<typename ForwardIterator,typename T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last,
	                     const T& x)
{
    //first指向输出端(欲初始化空间)的起始处，last结束处，x表示初值
	__uninitialized_fill(first, last, x, value_tyoe(first));
}

template<typename ForwardIterator, typename T,typename T1>
void __uninitialized_fill(ForwardIterator first, ForwardIterator last,
	const T& x, T1*)
{
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	return __uninitialized_fill_aux(first, n, x, is_POD());
}

template<typename ForwardIterator, typename T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
{
     fill(first, last, x);               //交由高阶函数执行
}

template<typename ForwardIterator, typename T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
{
	//省略了异常处理
	ForwardIterator cur = first;
	for (; n > 0; --n, ++cur)
	{
		construct(&*cur, x);
	}
}


template<typename ForwardIterator,typename Size,typename T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
	//迭代器first指向预初始化空间的起始处
	//n表示欲初始化空间大小，x表示初值
	//利用value_type()取出first的value type
	return __uninitialized_fill_n(first, n, x, value_type(first));
}

template<typename ForwardIterator, typename Size, typename T，typename T1>
ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
	//T1在这里是value type
	//以下为__type_traits<> 技法
	//首先萃取出迭代器first的value type，然后判断该型别是否为POD型别
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

//如果copy construction等同于assignment而且destructor是trivial，以下有效
//如果是POD型别，执行流程转到以下函数，这是藉由function template的参数推导机制而得
template<typename ForwardIterator, typename Size, typename T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
	return fill_n(first, n, x);               //交由高阶函数执行
}

//fill_n 定义在<stl_algobase.h>中
/*
template<typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
   for( ; n > 0; --n,++first)
      *first=value;
   return first;
}
*/

//如果不是POD型别，执行流程则会转到以下函数，这是藉由function template的参数推导机制而得
template<typename ForwardIterator, typename Size, typename T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
	//省略了异常处理
	ForwardIterator cur = first;
	for (; n > 0; --n, ++cur)
	{
		construct(&*cur, x);
	}
	return cur;
}


