#pragma once
#include<new.h>

template <typename T1,typename T2>
inline void construct(T1 *p, const T2& value)
{
	new(p) T1(value);                               //��p��ָ����ڴ�ռ�(Ԥ����),��operator new����ģ���������ΪT1�Ķ��󣬹��캯������const T2&�Ĳ���
}

//destroy��һ�汾����һ��������д���
template <typename T>
inline void destroy(T* pointer)
{
	pointer->~T();                                  //������������δ�ͷ�
}

//destroy�ڶ��汾����������������������һ����Χ�ڵ�Ԫ��
//�ж�Ԫ����ֵ�ͱ�(value_type) ��������_type_traits<> ��ȡ���ʵ���ʩ(Ч��)
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
	__destroy(first, last, value_type(first));
}

//�ж�Ԫ�ص�value_type�Ƿ���trivial destructor
template <typename ForwardIterator,typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
	__destroy_aux(first, last, trivial_destructor());
}

//���trivial_destructorΪ_false_type��˵����������������(�漰�ڴ�)
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __false_type)
{
	for (; first < last; ++first)
		destroy(&*first);
}

//���trivial_destructorΪ_true_type��˵������������û��(���漰�ڴ�)
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

//��Ϊchar* wchar_t* Ϊ�������ͣ�û������������������Ҫ__destroy __destroy_aux �����жϣ������ػ������Ч��
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}
