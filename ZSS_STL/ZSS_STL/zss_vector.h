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
	typedef const T*        const_reference;
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

	vector(vector<T>& right)    //拷贝构造函数
	{
		start = data_allocator::allocate(right.capacity());
		finish = uninitialized_copy(right.begin(), right.end(), start);
		end_of_storage = start + right.capacity();
		std::cout << "copy construct" << std::endl;
	}
	vector(vector<T>&& right)   //转移拷贝构造函数
	{
		start = right.begin();
		finish = right.back();
		end_of_storage = (start + right.capacity());

		right.begin() = nullptr;
		right.back() = nullptr;
		right.end_of_storage() = nullptr;
		std::cout << "move construct" << std::endl;
	}
	vector<T>& operator= (const vector<T>& right)   //赋值构造函数
	{
		if (this != &right)
		{
			start = data_allocator::allocate(right.capacity());
			finish = uninitialized_copy(right.begin(), right.end(), start);
			end_of_storage = start + right.capacity();
		}
		std::cout << "copy assignment" << std::endl;
		return *this;
	}
	vector<T>& operator= (vector<T>&& right)        //转移赋值构造函数
	{
		if (this != &right)
		{
			start = right.begin();
			finish = right.back();
			end_of_storage = (start + right.capacity());

			right.begin() = nullptr;
			right.back() = nullptr;
			right.end_of_storage() = nullptr;
		}
		std::cout << "move assignment" << std::endl;
		return *this;
	}

	~vector() {
		destroy(start, finish);                                      //全局函数，在zss_construct.h中(仅析构)
		deallocate();                                                //vector的一个member function(释放内存)
	}
    
	iterator begin() const { return start; }                                       //一般的iterator类型为 vector<xxx>::iterator 
	iterator end() const  { return finish; }                                      //实际上指的还是指针，但此时因为typedef不用自己写类型
	size_type size() const       { return size_type(end() - begin()); }
	size_type capacity() const   { return size_type(end_of_storage - start); }
	bool empty() const           { return begin() == end(); }
	reference front() { return *begin(); }            //第一个元素
	reference back() { return *(end() - 1); }         //最后一个元素(注意end()本身指的是尾后指针)
	
	reference operator[](size_type n) {                                            //编译器根据指针的类型来进行加减运算
		if (n < size())
			return *(begin() + n);
		else {
			std::cout << "out of range" << std::endl;
			exit(1);
		}
	}
	bool operator==(const vector<T>& right) {
		if (size() != right.size())
			return false;
		else {
			for (int i = 0; i < size(); i++){
				if (*(begin() + i) != *(right.begin()+i))
					return false;
			}
			return true;
		}
	}
	bool operator!=(const vector<T>& right) {
		return (!operator==(right));
	}
	bool operator<=(const vector<T>& right){
		if (size() < right.size())
			return true;
		else if (size() > right.size())
			return false;
		else {
			for (int i = 0; i < size(); i++){
				if (*(begin() + i) >*(right.begin() + i))
					return false;
			}
			return true;
		}
	}
	bool operator>=(const vector<T>& right) {
		return !operator<=(right);
	}
	bool operator< (const vector<T>& right) {
		if (size() < right.size())
			return true;
		else if (size() > right.size())
			return false;
		else {
			for (int i = 0; i < size(); i++) {
				if (*(begin() + i) >= *(right.begin() + i))
					return false;
			}
			return true;
		}
	}
	bool operator> (const vector<T>& right) {
		return !operator<(right);
	}

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

	void assign(size_type n, const T& x) {
		uninitialized_fill_n(start, n, x);
	}
	void assign(iterator first, iterator last) {
		uninitialized_copy(first, last, start);
	}
	iterator erase(iterator position) {               //清除某位置上的元素
		if (position + 1 != end())                    //position所指的不是尾元素
			std::copy(position + 1, finish, position);     //后续元素向前移动，copy函数--STL algorithm
		--finish;
		destroy(finish);                              //析构但不释放
		return position;
	}
	iterator erase(iterator first, iterator last) {   //删除[first,last)的元素
		if(last != end())
		    std::copy(last, finish, first); 
		for (int i = 0; i < last - first; i++) {
			--finish;
			destroy(finish);
		}
		return first;
	}
	void clear() { erase(begin(), end()); }

	void resize(size_type new_size) { resize(new_size, T()); }
	void resize(size_type new_size,const T& x);
	void reserve(size_type n);
	void insert(iterator first, const T& x);
	void insert(iterator first, size_type n, const T& x);
	void insert(iterator position, iterator first, iterator last);
	void swap(vector<T>& right) {
		iterator temp_start = start;
		iterator temp_finish = finish;
		start = right.start;
		finish = right.finish;
		right.start = temp_start;
		right.finish = temp_finish;
	}
	
protected:
	//配置空间并填满内容
	iterator allocate_and_fill(size_type n, const T& x) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
};

template<typename T, typename Alloc>
inline void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
	//不懂为什么要判断finish和out关系，不是在push_back()中已经判断过了才调用的吗？？？？？
	if (finish != end_of_storage) {          //还有备用空间
											 //在备用空间起始处构造一个函数，并以vector最后一个元素值为其初值
		construct(finish, *(finish - 1));
		++finish;
		T x_copy = x;
		std::copy_backward(position, finish - 2, finish - 1);
		*position = x_copy;
	}
	else {                                   //已无备用空间
											 //如果原大小为0，则配置1
											 //如果原大小不为0，则配置原大小的两倍，前半段用来放置原数据，后半段用来放置新元素
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;

		iterator new_start = data_allocator::allocate(len);
		iterator new_finish = new_start;
		try {
			new_finish = uninitialized_copy(start, position, new_start);      //返回的是复制完后结束位置的下一位
			construct(new_finish, x);
			++new_finish;
			//将原vector的备用空间的内容也拷贝过来，这个position和finish不是已经重合了吗，不懂用途
			//new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch (...) {
			destroy(new_start, new_finish);                      //若一旦出问题，则把已经构造好的全部释放
			data_allocator::deallocate(new_start, len);
			throw;
		}

		//析构并释放原vector
		destroy(begin(), end());
		deallocate();

		//调整迭代器，指向新的vector
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;

	}
}

template<typename T,typename Alloc>
inline void vector<T, Alloc>::insert(iterator position, const T& x) {
	insert(position, 1, x);
}

template<typename T, typename Alloc>
inline void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
	if (n != 0) {
		if (size_type(end_of_storage - finish) >= n) {
			//备用空间大于等于"新增元素"
			T x_copy = x;
			//以下计算插入点之后的现有元素个数
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				//插入点之后的现有元素个数大于新增元素个数
				//不可直接移动，如 0 1 2 3_ _,要把从0开始的向后移两位，按次序复制会覆盖掉2的值，则导致2没有办法复制
				////而以下思路为 0 1 2 3 _ _ -> 0 1 2 3 2 3 -> 0 1 0 1 2 3 -> x x 0 1 2 3 
				uninitialized_copy(finish - n, finish, finish);       //先将后n个元素后移n位(或许效率更高？？)
				finish += n;                                          //将vector尾端标记后移n位
				std::copy_backward(position, old_finish, finish);          //copy与copy_backward的功能几乎一样，copy_backward(first,last,result)将
																	  //元素[first,last-1]拷贝到[result-(last-first),result),从最后一个元素开始复制
																	  //此时剩下的elems_after-n位到位
				std::fill(position, position + n, x_copy);
			}
			else {
				//插入点后的现有元素个数小于等于新增元素个数
				uninitialized_fill_n(finish,(n - elems_after), x_copy);   //第二个参数的含义为size
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				std::fill(position, old_finish, x_copy);
				//这样做可能是出于效率的考虑，在finish到新的finish+n-elems_after中的元素是只开辟了内存的，所以此时用un..的效率更高(考虑不同型别)
				//而un..不适用于已经有元素的填充，所以将已经有元素的和没哟元素的分开
			}
		}
		else {
			//备用空间小于新增元素个数(要分配额外的内存)
			const size_type old_size = size();
			const size_type len = old_size > n ? 2 * old_size : old_size + n;
			//以下配置新的vector空间
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				//首先将旧vector的插入点之前的元素复制到新空间
				new_finish = uninitialized_copy(start, position, new_start);
				//再将新增元素（初值均为n）填入新空间
				new_finish = uninitialized_fill_n(new_finish, n, x);
				//再将旧vector的插入点之后的元素复制到新空间
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			//清除并释放旧的vector
			destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

template<typename T, typename Alloc>
inline void vector<T, Alloc>::insert(iterator position,iterator first,iterator last)
{
	size_type n = last - first;
	if (n != 0) {
		if (size_type(end_of_storage - finish) >= n) {
			//备用空间大于等于"新增元素"
			//以下计算插入点之后的现有元素个数
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				//插入点之后的现有元素个数大于新增元素个数
				//不可直接移动，如 0 1 2 3_ _,要把从0开始的向后移两位，按次序复制会覆盖掉2的值，则导致2没有办法复制
				uninitialized_copy(finish - n, finish, finish);       //先将后n个元素后移n位
				finish += n;                                          //将vector尾端标记后移n位
				std::copy_backward(position, old_finish, finish);          //copy与copy_backward的功能几乎一样，copy_backward(first,last,result)将
																		   //元素[first,last-1)拷贝到[result-(last-first),result),从最后一个元素开始复制
																		   //此时剩下的elems_after-n位到位
				std::copy(first, last, position);
			}
			else {
				//插入点后的现有元素个数小于等于新增元素个数
				std::copy(position, finish, (position + n));
				finish += n ;
				std::copy(first, last, position);
			}
		}
		else {
			//备用空间小于新增元素个数(要分配额外的内存)
			const size_type old_size = size();
			const size_type len = old_size > n ? 2 * old_size : old_size + n;
			//以下配置新的vector空间
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				//首先将旧vector的插入点之前的元素复制到新空间
				new_finish = uninitialized_copy(start, position, new_start);
				//再将新增元素（初值均为n）填入新空间
				new_finish = uninitialized_copy(first, last, new_finish);
				//再将旧vector的插入点之后的元素复制到新空间
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			//清除并释放旧的vector
			destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

template<typename T, typename Alloc>
inline void vector<T, Alloc>::resize(size_type new_size, const T& x) {
	//改变大小(而非capacity)，可扩大（扩大的值均为x），可缩小(删)
	size_type old_size = size();
	if (old_size > new_size) {
		//缩小
		for (int i = new_size; i < old_size; i++) {
			destroy(begin() + i);
		}
		finish = begin() +new_size;
	}
	else {
		//扩大
		if (new_size < capacity()){
			//不需要开辟新内存
			finish=uninitialized_fill_n(finish, (new_size - old_size), x);
		}
		else {
			//需要开辟新内存
			size_type len = 2 * old_size > new_size ? 2 * old_size : new_size;
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			//将原vector元素拷贝
			new_finish = uninitialized_copy(start, finish, new_start);
			new_finish = uninitialized_fill_n(new_finish, (new_size - old_size), x);
			//删除原vector
			destroy(start, finish);
			deallocate();
			//更新first和last,end_of_range
			start = new_start;
			finish = new_finish;
			end_of_storage = start + len;
		}
	}
}

template<typename T, typename Alloc>
inline void vector<T, Alloc>::reserve(size_type n)
{
	if (n < capacity())
		return;
	else {
		iterator new_start = data_allocator::allocate(n);
		iterator new_finish = new_start;
		new_finish = uninitialized_copy(start, finish, new_start);
		destroy(start, finish);
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = start + n;

	}

}
}
