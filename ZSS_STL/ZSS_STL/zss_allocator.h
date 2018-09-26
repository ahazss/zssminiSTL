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
		std::set_new_handler(0);                                //使用set_new_handler,获得一个不抛出异常的new
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
		new(p) T1(value);                               //在p所指向的内存空间(预分配),即operator new分配的，创建类型为T1的对象，构造函数接收const T2&的参数
	}

	template <typename T>
	inline void _destroy(T* ptr)
	{
		ptr->~T();                                      //此时需要显式调用T的析构函数，而不可用delete
	}


	template <typename T>
	class allocator
	{
	public:
		typedef T          value_type;
		typedef T*         pointer;
		typedef const T*   const_pointer;
		typedef T&         reference;
		typedef const T&   const_reference;
		typedef size_t     size_type;
		typedef ptrdiff_t  difference_type;


		template <typename U>
		struct rebind
		{
			typedef allocator<U> other;
		};

		allocator()
		{
		}

		template<typename U>
		allocator(const allocator<U>&)
		{
		}


		pointer allocate(size_type n, const void* hint = 0)                 //第二个参数为提示，可利用它来增进区域性??
		{
			return _allocate((difference_type)n, (pointer)0);               //_allocate实际上在这里实现的是operator new
		}

		void deallocate(pointer p, size_type n)
		{
			_deallocate(p);
		}

		void construct(pointer p, const T& value)
		{
			_construct(p, value);                                           //_construct实际上完成了new operator(_allocate已分配好的内存，再调用构造函数)
		}

		void destroy(pointer p)
		{
			_destroy(p);
		}

		//返回x的地址(指针)
		pointer adress(reference x)
		{
			return (pointer)&x;
		}

		const_pointer const_address(const_reference x)
		{
			return (const_pointer)&x;
		}
		
		size_type max_size() const
		{
			return size_type(UINT_MAX / sizeof(T));                             //UNIT_MAX  maximum unsigned int value
		}
	};

} //end of namespace ZSS
