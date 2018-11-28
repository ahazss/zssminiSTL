#pragma once
#include"zss_alloc.h"
#include"zss_construct.h"
#include"zss_uninitialized.h"

namespace ZSS {

template<typename T, typename Alloc=alloc>                     //allocΪ��һ����ڶ���������
class vector {
public:
    //vector��Ƕ���ͱ���
	typedef T               value_type;
	typedef value_type*     pointer;
	typedef value_type*     iterator;
	typedef value_type&     reference;
	typedef size_t          size_type;                         //��32λϵͳ��size_tΪ4�ֽڣ�64λϵͳ��Ϊ8�ֽ�
	typedef ptrdiff_t       difference_type;

protected:
	//simple_alloc��SGI STL�Ŀռ�������,��zss_alloc.h
	typedef simple_alloc<value_type, Alloc>  data_allocator;
	iterator start;                           //��ʾĿǰʹ�ÿռ��ͷ��ʵ��Ϊָ��
	iterator finish;                          //��ʾĿǰʹ�ÿռ��β
	iterator end_of_storage;                  //��ʾĿǰ���ÿռ��β

	void insert_aux(iterator position, const T& x);

	void deallocate() {
		if (start)
			data_allocator::deallocate(start, end_of_storage - start);      //�˴�ҲΪָ��ļ������˴�����Ӧ����Ԫ���Ѿ�����֮��
	}

	void fill_initialize(size_type n, const T& value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

public:
	iterator begin() { return start; }                                       //һ���iterator����Ϊ vector<xxx>::iterator 
	iterator end()   { return finish; }                                      //ʵ����ָ�Ļ���ָ�룬����ʱ��Ϊtypedef�����Լ�д����
	size_type size() const       { return size_type(end() - begin()); }
	size_type capacity() const { return size_type(end_of_storage() - begin()); }
	bool empty() const           { return begin() == end(); }
	reference operator[](size_type n) { return *(begin() + n); }                      //����������ָ������������мӼ�����

	vector() :start(0), finish(0), end_of_storage(0) { }
	vector(size_type n, const T& value)              { fill_initialize(n, value); }
	vector(int n, const T& value)                    { fill_initialize(n, value); }
	vector(long n, const T& value)                   { fill_initialize(n, value); }   //ΪʲôҪ�ѿ�������Ĵ�С��ת��Ϊsize_t����long����װ�����𣿣���
	explicit vector(size_type n)                     { fill_initialize(n, T()); }

	vector(iterator first, iterator last) {
		start = data_allocator::allocate(last-first);
		uninitialized_copy(first, last, start);
		finish = start + (last - first);
		end_of_storage = finish;
	}

	~vector() {
		destroy(start, finish);                                      //ȫ�ֺ�������zss_construct.h��(������)
		deallocate();                                                //vector��һ��member function(�ͷ��ڴ�)
	}

	reference front() { return *begin(); }            //��һ��Ԫ��
	reference back() { return *(end() - 1); }         //���һ��Ԫ��(ע��end()����ָ����β��ָ��)
	void push_back(const T& x) {                      //��Ԫ�ز�����β��
		if (finish != end_of_storage) {               //�����ڴ棬����Ҫ����
			construct(finish, x);                     //ȫ�ֺ�������zss_construct.h(�������ڴ��Ϲ���)
			++finish;                                 //��Ӧ���͵�ָ��ĵ���
		}
		else
			insert_aux(end(), x);                     //vector��member function(������capacity)
	}

	void pop_back() {                                 //��β��Ԫ��ȡ��
		--finish;                                     //finish������β��ָ��
		destroy(finish);                              //��zss_construct.h ������������δ�ͷ��ڴ�(����capacity����)
	}

	iterator erase(iterator position) {               //���ĳλ���ϵ�Ԫ��
		if (position + 1 != end())                    //position��ָ�Ĳ���βԪ��
			std::copy(position + 1, finish, position);     //����Ԫ����ǰ�ƶ���copy����--STL algorithm
		--finish;
		destroy(finish);                              //���������ͷ�
		return position;
	}

	iterator erase(iterator first, iterator last) {   //ɾ��[first,last)��Ԫ��
		if(last + 1 != end())
		    std::copy(last + 1, finish, first);
		for (int i = 0; i < last - first; i++) {
			--finish;
			destroy(finish);
		}
		return first;
	}
	
	void resize(size_type new_size) { resize(new_size, T()); }           //resize���壿��������
	void clear() { erase(begin(), end()); }
	//void insert(iterator first, const T& x);
	void insert(iterator first, size_type n, const T& x);
	//void insert(iterator first, iterator bfirst, iterator afirst);

protected:
	//���ÿռ䲢��������
	iterator allocate_and_fill(size_type n, const T& x) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
};

}
