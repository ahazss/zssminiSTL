#pragma once
#ifdef __USE_MALLOC
//��allocΪ��һ��������
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
#else
//��allocΪ�ڶ���������
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
#endif

//simple_allocΪalloc�ṩ�ṹ��ʹ�������Ľӿڷ���STL���
template<typename T,typename Alloc>
class simple_alloc 
{
public:
	static T *allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
	}
	static T*allocate(void)
	{
		return (T*)Alloc::allocate(sizeof(T));
	}
	static void deallocate(T* p, size_t n)
	{
		if 0 != n
		   Alloc::deallocate(p, n * sizeof(T));
	}
	static void deallocate(T* p)
	{
		Alloc::deallocate(p, sizeof(T));
	}
};

//��һ��������
#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC cerr<<"out of memory"<<endl;exit(1)   //���崦�����ķ�ʽ
#endif

//malloc-based allocator ͨ����default alloc�ٶ���
//һ�������thread-safe �����ڿռ����ñȽϸ�Ч
//��'template'�Ͳ��������ڷ��ͱ����inst��ûʲô��
template <int inst>
class __malloc_alloc_template
{
private:
	//���¾�Ϊ����ָ�룬���ڴ����ڴ治������ oom:out of memory
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();

public:
	static void* allocate(size_t n)
	{
		void* result = malloc(n);                   //��һ��������ֱ����malloc()
	}
};