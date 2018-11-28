#pragma once
#include"zss_alloc.h"
#include"zss_construct.h"
#include"zss_uninitialized.h"

namespace ZSS {

template<typename T, typename Alloc=alloc>                     //alloc为第一级或第二级配置器
class vector {
public:
    //vector的嵌套型别定义
	typedef T               value_type;
	typedef value_type*     pointer;
	typedef value_type*     iterator;
	typedef value_type&     reference;
	typedef size_t          size_type;                         //在32位系统中size_t为4字节，64位系统中为8字节
	typedef ptrdiff_t       difference_type;

protected:
	//simple_alloc是SGI STL的空间配置器,见zss_alloc.h
	typedef simple_alloc<value_type, Alloc>  data_allocator;
	iterator start;                           //表示目前使用空间的头，实际为指针
	iterator finish;                          //表示目前使用空间的尾
	iterator end_of_storage;                  //表示目前可用空间的尾

	void insert_aux(iterator position, const T& x);

	void deallocate() {
		if (start)
			data_allocator::deallocate(start, end_of_storage - start);      //此处也为指针的减法，此处调用应该在元素已经析构之后
	}

	void fill_initialize(size_type n, const T& value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

public:
	iterator begin() { return start; }                                       //一般的iterator类型为 vector<xxx>::iterator 
	iterator end()   { return finish; }                                      //实际上指的还是指针，但此时因为typedef不用自己写类型
	size_type size() const       { return size_type(end() - begin()); }
	size_type capacity() const { return size_type(end_of_storage() - begin()); }
	bool empty() const           { return begin() == end(); }
	reference operator[](size_type n) { return *(begin() + n); }                      //编译器根据指针的类型来进行加减运算

	vector() :start(0), finish(0), end_of_storage(0) { }
	vector(size_type n, const T& value)              { fill_initialize(n, value); }
	vector(int n, const T& value)                    { fill_initialize(n, value); }
	vector(long n, const T& value)                   { fill_initialize(n, value); }   //为什么要把开辟数组的大小都转换为size_t，用long不会装不下吗？？？
	explicit vector(size_type n)                     { fill_initialize(n, T()); }

	vector(iterator first, iterator last) {
		start = data_allocator::allocate(last-first);
		uninitialized_copy(first, last, start);
		finish = start + (last - first);
		end_of_storage = finish;
	}

	~vector() {
		destroy(start, finish);                                      //全局函数，在zss_construct.h中(仅析构)
		deallocate();                                                //vector的一个member function(释放内存)
	}

	reference front() { return *begin(); }            //第一个元素
	reference back() { return *(end() - 1); }         //最后一个元素(注意end()本身指的是尾后指针)
	void push_back(const T& x) {                      //将元素插至最尾端
		if (finish != end_of_storage) {               //仍有内存，不需要扩容
			construct(finish, x);                     //全局函数，见zss_construct.h(在已有内存上构造)
			++finish;                                 //相应类型的指针的递增
		}
		else
			insert_aux(end(), x);                     //vector的member function(超过了capacity)
	}

	void pop_back() {                                 //将尾端元素取出
		--finish;                                     //finish本身是尾后指针
		destroy(finish);                              //见zss_construct.h 仅调用析构，未释放内存(所以capacity不变)
	}

	iterator erase(iterator position) {               //清除某位置上的元素
		if (position + 1 != end())                    //position所指的不是尾元素
			std::copy(position + 1, finish, position);     //后续元素向前移动，copy函数--STL algorithm
		--finish;
		destroy(finish);                              //析构但不释放
		return position;
	}

	iterator erase(iterator first, iterator last) {   //删除[first,last)的元素
		if(last + 1 != end())
		    std::copy(last + 1, finish, first);
		for (int i = 0; i < last - first; i++) {
			--finish;
			destroy(finish);
		}
		return first;
	}
	
	void resize(size_type new_size) { resize(new_size, T()); }           //resize定义？？？？？
	void clear() { erase(begin(), end()); }
	//void insert(iterator first, const T& x);
	void insert(iterator first, size_type n, const T& x);
	//void insert(iterator first, iterator bfirst, iterator afirst);

protected:
	//配置空间并填满内容
	iterator allocate_and_fill(size_type n, const T& x) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
};

}
