#pragma once
#include<new>                   //placement new
#include<cstddef>               //ptrdiff_t,size_t
#include<cstdlib>               //exit()
#include<climits>               //UNIT_MAX
#include<iostream>              //cerr

namespace ZSS
{
	//配置空间
	template <typename T>
	inline T* _allocate(ptrdiff_t size, T*)                //size*sizeof(T)是需要分配的内存大小
	{ 
		set_new_handler(0);                                //卸载new_handler，使operator new在下一次调用的时候，因为new_handler为空抛出内存申请异常。
		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
		if (tmp == 0)                                      //tmp是通过调用operator new得到的一个指针，指向需要大小的内存
		{
			cerr << "out of memory" << endl;
			exit(1);
		}
		return tmp;
	}

	//归还先前配置的空间
	template <typename T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);
	}

	//等同于new(const void* ) p T(x) 
	template <typename T1,typename T2>
	inline void _construct(T1 *p, const T2& value)
	{
		new(p) T1(value);                               //在p所指向的内存空间(预分配),创建类型为T1的对象，构造函数接收const T2&的参数
	}

	template <typename T>
	online void _destroy(T* ptr)
	{
		ptr->~T();                                      //此时需要显式调用T的析构函数，而不可用delete
	}

}
