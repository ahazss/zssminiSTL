#include"zss_alloc.h"


// malloc_alloc out - of - memory handling
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


										//static data number�Ķ������ʼֵ�趨
char* __default_alloc_template::start_free = 0;

char* __default_alloc_template::end_free = 0;

size_t __default_alloc_template::heap_size = 0;

typename __default_alloc_template::obj* volatile
__default_alloc_template::free_list[__NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };              //free_list��һ��ָ�����飬������ÿһ��Ԫ�ض�Ϊobj*ָ��

void* __default_alloc_template::allocate(size_t n)
{
	obj* volatile* my_free_list;
	obj* result;
	//����128bytes�͵��õ�һ��������
	if (n > (size_t)__MAX_BYTES)
	{
		return (malloc_alloc::allocate(n));
	}

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

void __default_alloc_template::deallocate(void *p, size_t n)
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


void* __default_alloc_template::refill(size_t n)
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
char* __default_alloc_template::chunk_alloc(size_t size, int& nobjs)
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
		nobjs = bytes_left / size;
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
		if (start_free == 0)
		{
			//heap�ռ䲻�㣬malloc()ʧ��
			int i;
			obj* volatile * my_free_list, *p;
			//��������������Ѱ����û�б�n��Ŀ��п�
			for (i = size; i <= __MAX_BYTES; i += __ALIGN)
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