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
	static T* allocate(size_t n)
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
		if (result == 0) result == oom_malloc(n);
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

//malloc_alloc out-of-memory handling
//初值为0，等客户端设定
template <int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
	void(*my_alloc_handler)();
	void *result;

	for (;;)
	{
		//不断尝试释放，配置，再释放，再配置...
		my_alloc_handler = __malloc_alloc_oom_handler;
		if (my_alloc_handler == 0) { __THROW_BAD_ALLOC; }
		(*my_alloc_handler)();                             //再次调用，企图释放其他地方的内存
		result = malloc(n);                                //再次尝试配置内存
		if (result)     return result;
	}
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
	void(*my_malloc_handler)();
	void* result;

	for (;;)
	{
		//不断尝试释放，配置，再释放，再配置...
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler)();                            //再次调用，企图释放内存
		result = realloc(p, n);
		if (result)      return result;
	}
}

//直接将参数inst指定为0
typedef __malloc_alloc_template<0>  malloc_alloc;


//第二级配置器
//template中inst没有用，第一参数用于多线程，暂不讨论
enum {__ALIGN=8};             //小型区块的上调边界
enum {__MAX_BYTES=128};       //小型区块的上限
enum {__NFREELISTS=__MAX_BYTES/__ALIGN};    //free-lists个数

template <bool threads,int inst>
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
	static void* reallocate(void *p, size_t old_sz, size_t new_sz);
}; 

//static data number的定义与初始值设定
template<bool threads,int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
__default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =      //free_list是一个指针数组，数组中每一个元素都为obj*指针
[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::allocate(size_t n)
{
	obj* volatile* my_free_list;
	obj* result;
	//大于128bytes就调用第一级配置器
	if (n > (size_t)__MAX_BYTES)
	    { return (malloc_alloc::allocate(n)); }

	//寻找16个free lists中适当的一个
	my_free_list = free_list + FREELIST_INDEX(n);          //向上取变为8的(n+1)倍，n为角标,得到一个指向free_list[n]指针
	result = *my_free_list;                                //result是一个obj*指针
	if (result == 0)                                       //这个结点下面没有挂内存，则就要去内存池中申请
	{
		void *r = refill(ROUND_UP(n));                     //向内存池申请(申请大小调为8的整数倍)
		return r;
	}
	//调整free list
	*my_free_list = result->free_list_link;                //目前的理解是free list是一个16的数组，总共有16个自由链表，每一个链表有若干个内存相同的结点
	return (result);                                       //my_free_list实际上是数组元素地址，解引用并给它赋值为链表的下一个结点地址(上一个已被用)
}

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
{
	obj *q = (obj*)p;
	obj* volatile *my_free_list;

	////如果n大于自由链表中结点所能挂的最大内存块，则就直接调用一级配置器的释放函数
	if (n > (size_t)__MAX_BYTES)
	{
		malloc_alloc::deallocate(p, n);
		return;
	}

	//寻找对应的free list
	my_free_list = free_list + FREELIST_INDEX(n);
	//调整free list，回收区块(将p插到第一块之前)
	q->free_list_link = *my_free_list;
	*my_free_list = q;                      //数组元素更新为新插入的地址
}

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
     //返回一个大小为n的对象
	int nobjs = 20;
	//调用chunk_alloc(),尝试取得nobjs个区块作为free list的新节点
	//参数nobjs是pass by reference
	char* chunk = chunk_alloc(n, nobjs);          //因为现在链表中没有，所以要想内存池中申请，多余的再挂到自由链表下面
	obj* volatile *my_free_list;
	obj* result;
	obj* current_obj, *next_obj;
	int i;

	//如果只获得一个区块，这个区块就分配给调用者用，free list无新节点
	if (nobjs == 1)  return(chunk);
	//否则就准备调整free list，纳入新节点
	my_free_list = free_list + FREELIST_INDEX(n);

	//以下在chunk空间内建立free list
	result = (obj*)chunk;                  //申请的第一个对象作为返回值(chunk 首地址+obj*类型)
	*my_free_list = (obj*)(chunk + n);     //第二个对象的地址放到链表中
	next_obj = *my_free_list;
	
	//将free list对应链表的各结点串连起来
	for (i = 1;; i++)
	{
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n);
		if (i == nobjs - 1)  //链表中该有的总结点数
		{
			current_obj->free_list_link = 0;
			break;
		}
		else
		{
			current_obj->free_list_link = next_obj;
		}
	}
	return result;
}

//内存池,size已上调为8的倍数
template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size int& nobjs)
{
	char* result;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free;               //内存池剩余空间

	if (bytes_left >= total_bytes)
	{
		//内存池剩余空间完全满足需求量
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else if (bytes_left >= size)
	{
		//内存池剩余空间不能完全满足需求量，但足够供应一个及以上的区块
		nobjs = bytes_left / n;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else
	{
		//内存池剩余的内存连一个区块的大小都无法提供
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);       //向系统申请内存的转换值
		if (bytes_left > 0)
		{
			//利用内存池零头
			//首先寻找适当的free list
			obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
			//调整free list，将内存池的残余空间编入
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}

		//配置heap空间，用来补充内存池
		start_free = (char*)malloc(bytes_to_get);
		if (statrt_free == 0)
		{
			//heap空间不足，malloc()失败
			int i;
			obj* volatile * my_free_list, *p;
			//在自由链表中搜寻还有没有比n大的空闲块
			for (i = size; i <= MAX_BYTES; i += __ALIGN)
			{
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (p != 0)
				{
					//free list内尚有未用区块 调整free list以释出未用块
					*my_free_list = p->free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					//递归调用自己，修正nobjs
					return (chunk_alloc(size, nobjs));
				}
			}
			//到处都没有内存可用了，调用第一级配置器，看看out-of-memory能不能处理
			end_free = 0;
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);            //会抛出异常或解决内存不足问题
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		//递归调用自己，修正nobjs
		return (chunk_alloc(size, nobjs));
	}
}