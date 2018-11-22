#pragma once
#include"stl_construct.h"
#include<string>

//uninitialized_copy
template<typename InputIterator,typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
	                              ForwardIterator result)
{
	//firstָ���������ʼλ�ã�lastָ������˽���λ��(ǰ�պ�����)
	//resultָ�������(����ʼ���ռ�)����ʼ��
	return __uninitialized_copy(first, last, result, value_type(result));
}

template<typename InputIterator, typename ForwardIterator,typename T>
ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
	                                 ForwardIterator result,T*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __uninitialized_copy_aux(first, last, result, is_POD());
}

//��fill_n
template<typename InputIterator, typename ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
	                                     ForwardIterator result,__true_type)
{
	return copy(first, last, result);           //���ø߼��㷨copy()
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
	                                     ForwardIterator result,__false_type)
{
	ForwardIterator cur = result;
	//ʡ���쳣�������
	for (; first != last; ++first, ++cur)
	{
		construct(&*cur, *first);
	}
	return cur;
}

//���char*��wchar_t*�����ͱ𣬿��Բ�ȡ����Ч�ʵ�����remove(ֱ���ƶ��ڴ�����)��ִ�и�����Ϊ������Ϊ�ػ��汾
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
    //firstָ�������(����ʼ���ռ�)����ʼ����last��������x��ʾ��ֵ
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
     fill(first, last, x);               //���ɸ߽׺���ִ��
}

template<typename ForwardIterator, typename T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
{
	//ʡ�����쳣����
	ForwardIterator cur = first;
	for (; n > 0; --n, ++cur)
	{
		construct(&*cur, x);
	}
}


template<typename ForwardIterator,typename Size,typename T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
	//������firstָ��Ԥ��ʼ���ռ����ʼ��
	//n��ʾ����ʼ���ռ��С��x��ʾ��ֵ
	//����value_type()ȡ��first��value type
	return __uninitialized_fill_n(first, n, x, value_type(first));
}

template<typename ForwardIterator, typename Size, typename T��typename T1>
ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
	//T1��������value type
	//����Ϊ__type_traits<> ����
	//������ȡ��������first��value type��Ȼ���жϸ��ͱ��Ƿ�ΪPOD�ͱ�
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

//���copy construction��ͬ��assignment����destructor��trivial��������Ч
//�����POD�ͱ�ִ������ת�����º��������ǽ���function template�Ĳ����Ƶ����ƶ���
template<typename ForwardIterator, typename Size, typename T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
	return fill_n(first, n, x);               //���ɸ߽׺���ִ��
}

//fill_n ������<stl_algobase.h>��
/*
template<typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
   for( ; n > 0; --n,++first)
      *first=value;
   return first;
}
*/

//�������POD�ͱ�ִ���������ת�����º��������ǽ���function template�Ĳ����Ƶ����ƶ���
template<typename ForwardIterator, typename Size, typename T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
	//ʡ�����쳣����
	ForwardIterator cur = first;
	for (; n > 0; --n, ++cur)
	{
		construct(&*cur, x);
	}
	return cur;
}


