#pragma once


//simple_alloc为alloc提供结构，使配置器的接口符合STL规格
template<typename T,typename Alloc>
class simple_alloc 
{
public:
	static T* allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
	}
	static T* allocate(void)
	{
		return (T*)Alloc::allocate(sizeof(T));
	}
	static void deallocate(T* p, size_t n)
	{
		if ( 0 != n )
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
#   define __THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl;exit(1)   //定义处理错误的方式
#endif

//malloc-based allocator 通常比default alloc速度慢
//一般而言是thread-safe 并对于空间运用比较高效
//无'template'型参数，至于非型别参数inst则没什么用，这里的inst一定为常数
//SGI使用malloc而非::operator new 来配置内存其中一个原因可能是C++并未提供相应realloc()的内存配置操作
template <int inst>
class __malloc_alloc_template
{
private:
	//以下均用于处理内存不足的情况 oom:out of memory
	static void* oom_malloc(size_t);
	static void* oom_realloc(void *, size_t);
	static void (*__malloc_alloc_oom_handler)();    //函数指针

public:
	//不创建实例即可用，所以均为静态方法
	static void* allocate(size_t n)
	{
		void* result = malloc(n);                   //第一级配置器直接用malloc() malloc(size_t n) 
		                                            //返回类型为void* 是因为void*可强制转换为任意类型的指针
		//以下无法满足需求时，改用oom_malloc()
		if (result == 0) result = oom_malloc(n);
		return result;
	}

	static void deallocate(void* p,size_t )
	{
		free(p);                                    //第一级配置器直接使用free()
	}

	static void* reallocate(void* p, size_t new_sz)
	{
		void* result = realloc(p, new_sz);          //第一级配置器直接使用realloc()
		//以下无法满足需求时，改用oom_realloc
		if (result == 0) result = oom_realloc(p, new_sz);
		return result;
	}

	//仿真C++的set_new_handler() 可通过它指定自己的out-of-memory handler
	//不能直接用C++ new-handler机制，因为并非使用::operator new 来配置内存
	//C++ new-handler机制是：你可以要求系统在内存配置需求无法被满足时，调用一个你所指定的函数
	//即在::operator new无法完成任务而丢出bad_alloc异常状态前，会先调用由客端指定的处理例程(new-handler)
	static void (* set_malloc_handler(void (*f)()))()
	{
		void(*old)() = _malloc_alloc_oom_handler;
		_malloc_alloc_oom_handler = f;
		return (old);
	}
};

//第二级配置器
//template中inst没有用，第一参数用于多线程，暂不讨论
enum { __ALIGN = 8 };             //小型区块的上调边界
enum { __MAX_BYTES = 128 };       //小型区块的上限
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };    //free-lists个数


class __default_alloc_template
{
private:
	union obj {
		union obj* free_list_link;
		char client_data[1];
	};
	
	//ROUND_UP()将bytes上调至8的倍数
	//~(__ALIGN为-1)为对其掩码，+__ALIGN-1是为了确保上调
	static size_t ROUND_UP(size_t bytes)
	{
		return ((bytes)+__ALIGN - 1 & ~(__ALIGN - 1));       
	}

	//16个free-lists volatile确保对特殊值的访问 数组的每一个元素均是指针
	static obj* volatile free_list[__NFREELISTS];
	
	//根据区块大小，决定使用第n号free_list
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);   //前面向上取8的倍数，再减1变为角标
	}

	//返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
	static void* refill(size_t n);

	//配置一大块空间，可容纳nobjs个大小为"size"的区块
	//如果配置nobjs 个区块不方便，nobjs可能会降低
	static char* chunk_alloc(size_t size, int &nobjs);

	//Chunk allocation state
	static char *start_free;              //内存池起始位置，只在chunk_alloc()中变化
	static char *end_free;                //内存池结束位置，只在chunk_alloc()中变化
	static size_t heap_size;              //记录内存池已经向系统申请了多大的内存


public:
	static void* allocate(size_t n);                                      //配置器的标准接口
	static void deallocate(void *p, size_t n);
	//static void* reallocate(void *p, size_t old_sz, size_t new_sz);
}; 

typedef __default_alloc_template alloc; //第二级配置器