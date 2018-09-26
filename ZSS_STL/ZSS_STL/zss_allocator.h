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
		std::set_new_handler(0);                                //ʹ��set_new_handler,���һ�����׳��쳣��new
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
		new(p) T1(value);                               //��p��ָ����ڴ�ռ�(Ԥ����),��operator new����ģ���������ΪT1�Ķ��󣬹��캯������const T2&�Ĳ���
	}

	template <typename T>
	inline void _destroy(T* ptr)
	{
		ptr->~T();                                      //��ʱ��Ҫ��ʽ����T��������������������delete
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


		pointer allocate(size_type n, const void* hint = 0)                 //�ڶ�������Ϊ��ʾ����������������������??
		{
			return _allocate((difference_type)n, (pointer)0);               //_allocateʵ����������ʵ�ֵ���operator new
		}

		void deallocate(pointer p, size_type n)
		{
			_deallocate(p);
		}

		void construct(pointer p, const T& value)
		{
			_construct(p, value);                                           //_constructʵ���������new operator(_allocate�ѷ���õ��ڴ棬�ٵ��ù��캯��)
		}

		void destroy(pointer p)
		{
			_destroy(p);
		}

		//����x�ĵ�ַ(ָ��)
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
