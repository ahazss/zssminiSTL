#pragma once


//simple_allocΪalloc�ṩ�ṹ��ʹ�������Ľӿڷ���STL���
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

//��һ��������
#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl;exit(1)   //���崦�����ķ�ʽ
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
		if (result == 0) result = oom_malloc(n);
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

//�ڶ���������
//template��instû���ã���һ�������ڶ��̣߳��ݲ�����
enum { __ALIGN = 8 };             //С��������ϵ��߽�
enum { __MAX_BYTES = 128 };       //С�����������
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };    //free-lists����


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
	//static void* reallocate(void *p, size_t old_sz, size_t new_sz);
}; 

typedef __default_alloc_template alloc; //�ڶ���������