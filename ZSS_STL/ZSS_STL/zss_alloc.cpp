#include"zss_alloc.h"


// malloc_alloc out - of - memory handling
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


										//static data number的定义与初始值设定
char* __default_alloc_template::start_free = 0;

char* __default_alloc_template::end_free = 0;

size_t __default_alloc_template::heap_size = 0;

typename __default_alloc_template::obj* volatile
__default_alloc_template::free_list[__NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };              //free_list是一个指针数组，数组中每一个元素都为obj*指针

void* __default_alloc_template::allocate(size_t n)
{
	obj* volatile* my_free_list;
	obj* result;
	//大于128bytes就调用第一级配置器
	if (n > (size_t)__MAX_BYTES)
	{
		return (malloc_alloc::allocate(n));
	}

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

void __default_alloc_template::deallocate(void *p, size_t n)
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


void* __default_alloc_template::refill(size_t n)
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
char* __default_alloc_template::chunk_alloc(size_t size, int& nobjs)
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
		nobjs = bytes_left / size;
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
		if (start_free == 0)
		{
			//heap空间不足，malloc()失败
			int i;
			obj* volatile * my_free_list, *p;
			//在自由链表中搜寻还有没有比n大的空闲块
			for (i = size; i <= __MAX_BYTES; i += __ALIGN)
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