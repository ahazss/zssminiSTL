#pragma once
#ifdef __USE_MALLOC
//令alloc为第一级配置器
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
#else
//令alloc为第二级配置器
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
#endif

//simple_alloc为alloc提供结构，使配置器的接口符合STL规格
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

//第一级配置器
#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC cerr<<"out of memory"<<endl;exit(1)   //定义处理错误的方式
#endif

//malloc-based allocator 通常比default alloc速度慢
//一般而言是thread-safe 并对于空间运用比较高效
//无'template'型参数，至于非型别参数inst则没什么用
template <int inst>
class __malloc_alloc_template
{
private:
	//以下均为函数指针，用于处理内存不足的情况 oom:out of memory
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();

public:
	static void* allocate(size_t n)
	{
		void* result = malloc(n);                   //第一级配置器直接用malloc()
	}
};