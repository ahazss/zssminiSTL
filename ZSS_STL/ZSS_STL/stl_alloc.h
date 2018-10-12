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
//��'template'�Ͳ��������ڷ��ͱ����inst��ûʲô�ã������instһ��Ϊ����
//SGIʹ��malloc����::operator new �������ڴ�����һ��ԭ�������C++��δ�ṩ��Ӧrealloc()���ڴ����ò���
template <int inst>
class __malloc_alloc_template
{
private:
	//���¾����ڴ����ڴ治������ oom:out of memory
	static void* oom_malloc(size_t);
	static void* oom_realloc(void *, size_t);
	static void (*__malloc_alloc_oom_handler)();    //����ָ��

public:
	//������ʵ�������ã����Ծ�Ϊ��̬����
	static void* allocate(size_t n)
	{
		void* result = malloc(n);                   //��һ��������ֱ����malloc() malloc(size_t n) 
		                                            //��������Ϊvoid* ����Ϊvoid*��ǿ��ת��Ϊ�������͵�ָ��
		//�����޷���������ʱ������oom_malloc()
		if (result == 0) result == oom_malloc(n);
		return result;
	}

	static void deallocate(void* p,size_t )
	{
		free(p);                                    //��һ��������ֱ��ʹ��free()
	}

	static void* reallocate(void* p, size_t new_sz)
	{
		void* result = realloc(p, new_sz);          //��һ��������ֱ��ʹ��realloc()
		//�����޷���������ʱ������oom_realloc
		if (result == 0) result = oom_realloc(p, new_sz);
		return result;
	}

	//����C++��set_new_handler() ��ͨ����ָ���Լ���out-of-memory handler
	//����ֱ����C++ new-handler���ƣ���Ϊ����ʹ��::operator new �������ڴ�
	//C++ new-handler�����ǣ������Ҫ��ϵͳ���ڴ����������޷�������ʱ������һ������ָ���ĺ���
	//����::operator new�޷�������������bad_alloc�쳣״̬ǰ�����ȵ����ɿͶ�ָ���Ĵ�������(new-handler)
	static void (* set_malloc_handler(void (*f)()))()
	{
		void(*old)() = _malloc_alloc_oom_handler;
		_malloc_alloc_oom_handler = f;
		return (old);
	}
};

//malloc_alloc out-of-memory handling
//��ֵΪ0���ȿͻ����趨
template <int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
	void(*my_alloc_handler)();
	void *result;

	for (;;)
	{
		//���ϳ����ͷţ����ã����ͷţ�������...
		my_alloc_handler = __malloc_alloc_oom_handler;
		if (my_alloc_handler == 0) { __THROW_BAD_ALLOC; }
		(*my_alloc_handler)();                             //�ٴε��ã���ͼ�ͷ������ط����ڴ�
		result = malloc(n);                                //�ٴγ��������ڴ�
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
		//���ϳ����ͷţ����ã����ͷţ�������...
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler)();                            //�ٴε��ã���ͼ�ͷ��ڴ�
		result = realloc(p, n);
		if (result)      return result;
	}
}

//ֱ�ӽ�����instָ��Ϊ0
typedef __malloc_alloc_template<0>  malloc_alloc;


//�ڶ���������
//template��instû���ã���һ�������ڶ��̣߳��ݲ�����
enum {__ALIGN=8};             //С��������ϵ��߽�
enum {__MAX_BYTES=128};       //С�����������
enum {__NFREELISTS=__MAX_BYTES/__ALIGN};    //free-lists����

template <bool threads,int inst>
class __default_alloc_template
{
private:
	union obj {
		union obj* free_list_link;
		char client_data[1];
	};
	
	//ROUND_UP()��bytes�ϵ���8�ı���
	//~(__ALIGNΪ-1)Ϊ�������룬+__ALIGN-1��Ϊ��ȷ���ϵ�
	static size_t ROUND_UP(size_t bytes)
	{
		return ((bytes)+__ALIGN - 1 & ~(__ALIGN - 1));       
	}

	//16��free-lists volatileȷ��������ֵ�ķ��� �����ÿһ��Ԫ�ؾ���ָ��
	static obj* volatile free_list[__NFREELISTS];
	
	//���������С������ʹ�õ�n��free_list
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);   //ǰ������ȡ8�ı������ټ�1��Ϊ�Ǳ�
	}

	//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free list
	static void* refill(size_t n);

	//����һ���ռ䣬������nobjs����СΪ"size"������
	//�������nobjs �����鲻���㣬nobjs���ܻή��
	static char* chunk_alloc(size_t size, int &nobjs);

	//Chunk allocation state
	static char *start_free;              //�ڴ����ʼλ�ã�ֻ��chunk_alloc()�б仯
	static char *end_free;                //�ڴ�ؽ���λ�ã�ֻ��chunk_alloc()�б仯
	static size_t heap_size;              //��¼�ڴ���Ѿ���ϵͳ�����˶����ڴ�


public:
	static void* allocate(size_t n);                                      //�������ı�׼�ӿ�
	static void deallocate(void *p, size_t n);
	static void* reallocate(void *p, size_t old_sz, size_t new_sz);
}; 

//static data number�Ķ������ʼֵ�趨
template<bool threads,int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
__default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =      //free_list��һ��ָ�����飬������ÿһ��Ԫ�ض�Ϊobj*ָ��
[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::allocate(size_t n)
{
	obj* volatile* my_free_list;
	obj* result;
	//����128bytes�͵��õ�һ��������
	if (n > (size_t)__MAX_BYTES)
	    { return (malloc_alloc::allocate(n)); }

	//Ѱ��16��free lists���ʵ���һ��
	my_free_list = free_list + FREELIST_INDEX(n);          //����ȡ��Ϊ8��(n+1)����nΪ�Ǳ�,�õ�һ��ָ��free_list[n]ָ��
	result = *my_free_list;                                //result��һ��obj*ָ��
	if (result == 0)                                       //����������û�й��ڴ棬���Ҫȥ�ڴ��������
	{
		void *r = refill(ROUND_UP(n));                     //���ڴ������(�����С��Ϊ8��������)
		return r;
	}
	//����free list
	*my_free_list = result->free_list_link;                //Ŀǰ�������free list��һ��16�����飬�ܹ���16����������ÿһ�����������ɸ��ڴ���ͬ�Ľ��
	return (result);                                       //my_free_listʵ����������Ԫ�ص�ַ�������ò�������ֵΪ�������һ������ַ(��һ���ѱ���)
}

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
{
	obj *q = (obj*)p;
	obj* volatile *my_free_list;

	////���n�������������н�����ܹҵ�����ڴ�飬���ֱ�ӵ���һ�����������ͷź���
	if (n > (size_t)__MAX_BYTES)
	{
		malloc_alloc::deallocate(p, n);
		return;
	}

	//Ѱ�Ҷ�Ӧ��free list
	my_free_list = free_list + FREELIST_INDEX(n);
	//����free list����������(��p�嵽��һ��֮ǰ)
	q->free_list_link = *my_free_list;
	*my_free_list = q;                      //����Ԫ�ظ���Ϊ�²���ĵ�ַ
}

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
     //����һ����СΪn�Ķ���
	int nobjs = 20;
	//����chunk_alloc(),����ȡ��nobjs��������Ϊfree list���½ڵ�
	//����nobjs��pass by reference
	char* chunk = chunk_alloc(n, nobjs);          //��Ϊ����������û�У�����Ҫ���ڴ�������룬������ٹҵ�������������
	obj* volatile *my_free_list;
	obj* result;
	obj* current_obj, *next_obj;
	int i;

	//���ֻ���һ�����飬�������ͷ�����������ã�free list���½ڵ�
	if (nobjs == 1)  return(chunk);
	//�����׼������free list�������½ڵ�
	my_free_list = free_list + FREELIST_INDEX(n);

	//������chunk�ռ��ڽ���free list
	result = (obj*)chunk;                  //����ĵ�һ��������Ϊ����ֵ(chunk �׵�ַ+obj*����)
	*my_free_list = (obj*)(chunk + n);     //�ڶ�������ĵ�ַ�ŵ�������
	next_obj = *my_free_list;
	
	//��free list��Ӧ����ĸ���㴮������
	for (i = 1;; i++)
	{
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n);
		if (i == nobjs - 1)  //�����и��е��ܽ����
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

//�ڴ��,size���ϵ�Ϊ8�ı���
template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size int& nobjs)
{
	char* result;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free;               //�ڴ��ʣ��ռ�

	if (bytes_left >= total_bytes)
	{
		//�ڴ��ʣ��ռ���ȫ����������
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else if (bytes_left >= size)
	{
		//�ڴ��ʣ��ռ䲻����ȫ���������������㹻��Ӧһ�������ϵ�����
		nobjs = bytes_left / n;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else
	{
		//�ڴ��ʣ����ڴ���һ������Ĵ�С���޷��ṩ
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);       //��ϵͳ�����ڴ��ת��ֵ
		if (bytes_left > 0)
		{
			//�����ڴ����ͷ
			//����Ѱ���ʵ���free list
			obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
			//����free list�����ڴ�صĲ���ռ����
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}

		//����heap�ռ䣬���������ڴ��
		start_free = (char*)malloc(bytes_to_get);
		if (statrt_free == 0)
		{
			//heap�ռ䲻�㣬malloc()ʧ��
			int i;
			obj* volatile * my_free_list, *p;
			//��������������Ѱ����û�б�n��Ŀ��п�
			for (i = size; i <= MAX_BYTES; i += __ALIGN)
			{
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (p != 0)
				{
					//free list������δ������ ����free list���ͳ�δ�ÿ�
					*my_free_list = p->free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					//�ݹ�����Լ�������nobjs
					return (chunk_alloc(size, nobjs));
				}
			}
			//������û���ڴ�����ˣ����õ�һ��������������out-of-memory�ܲ��ܴ���
			end_free = 0;
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);            //���׳��쳣�����ڴ治������
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		//�ݹ�����Լ�������nobjs
		return (chunk_alloc(size, nobjs));
	}
}