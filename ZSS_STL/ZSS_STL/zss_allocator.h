#pragma once
#include<new>                   //placement new
#include<cstddef>               //ptrdiff_t,size_t
#include<cstdlib>               //exit()
#include<climits>               //UNIT_MAX
#include<iostream>              //cerr

namespace ZSS
{
	//���ÿռ�
	template <typename T>
	inline T* _allocate(ptrdiff_t size, T*)                //size*sizeof(T)����Ҫ������ڴ��С
	{ 
		set_new_handler(0);                                //ж��new_handler��ʹoperator new����һ�ε��õ�ʱ����Ϊnew_handlerΪ���׳��ڴ������쳣��
		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
		if (tmp == 0)                                      //tmp��ͨ������operator new�õ���һ��ָ�룬ָ����Ҫ��С���ڴ�
		{
			cerr << "out of memory" << endl;
			exit(1);
		}
		return tmp;
	}

	//�黹��ǰ���õĿռ�
	template <typename T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);
	}

	//��ͬ��new(const void* ) p T(x) 
	template <typename T1,typename T2>
	inline void _construct(T1 *p, const T2& value)
	{
		new(p) T1(value);                               //��p��ָ����ڴ�ռ�(Ԥ����),��������ΪT1�Ķ��󣬹��캯������const T2&�Ĳ���
	}

	template <typename T>
	online void _destroy(T* ptr)
	{
		ptr->~T();                                      //��ʱ��Ҫ��ʽ����T��������������������delete
	}

}
