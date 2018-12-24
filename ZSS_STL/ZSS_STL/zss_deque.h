#pragma once
#include"zss_iterator.h"
#include"zss_uninitialized.h"

namespace ZSS {

inline size_t __deque_buf_size(size_t n, size_t sz)
{
	//n是个数，若n不为0，由用户自己定义，若为0，则buffer使用默认大小512bytes
	//如果sz(元素大小)小于512，传回512/sz，如果sz不小于512，传回1
	//buffer是缓冲区大小，n是每个缓冲区可以放多少个元素
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template<typename T,typename Ref,typename Ptr,size_t BufSiz>
struct __deque_iterator {
	typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
	typedef __deque_iterator<T, const T&, const T*, Bufsiz> const_iterator;
	static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

	typedef random_access_iterator_tag iterator_category;    // random迭代器，有+n,-n的功能
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T** map_pointer;
	
	typedef __deque_iterator self;

	//保持与容器的联结
	T* cur;                 //此迭代器所指的缓冲区的current元素
	T* first;               //此迭代器所指的缓冲区的头
	T* last;                //此迭代器所指的缓冲区的尾
	map_pointer node;       //指向管控中心

	//使用set_node跳一个缓冲区
	void set_node(map_pointer new_node) {
		node = new_node;                //node是指调用set_node的那个迭代器的node移一下(后或前移)
		first = *new_node;              //更新当前迭代器的first和last指针
		last = first + difference_type(buffer_size());
	}
	
	//重载运算子
	reference operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); }        //指向数据的指针
	difference_type operator- (const self& x) const {            //求this-x的距离
		return difference_type(buffer_size())*(node - x.node - 1) + (cur - first) + (x.last - x.cur);
	}
	
	self& operator++() {             //对自身进行了累加，所以返回自身的引用
		++cur;
		if (cur == last) {           //如果已到达了缓冲区的尾端
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	self operator++(int) {           //返回一个新的对象
		self tmp = *this;
		++*this;                     //this是指向迭代器自身的指针
		return tmp;
	}
	self& operator--() {
		if (cur == first) {          //若已到所在缓冲区的头端
			set_node(node - 1);      //就切换至前一个元素(缓冲区)的最后一个元素
			cur = last;
		}
		--cur;                       //切换至前一个元素
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}

	//以下实现随机存取，迭代器可直接跳跃n个距离
	self& operator+=(difference_type n) {
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size()))
			//目标在同一缓冲区内
			cur += n;
		else {
			//目标位置不在同一缓冲区内
			difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) :
				-difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			//切换至正确的元素
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}
	self& operator+(difference_type n) const {
		self tmp = *this;                //必须要新建对象是因为+=给其自身，而+则可能为赋值给别的对象不可以用自身
		return tmp += n;                 //调用+=
	}

	self& operator-=(difference_type n) const {
		return *this += -n;
	}
	self& operator-(difference_type n) const {
		self tmp = *this;
		return tmp -= n;
	}

	reference operator[] (difference_type n) const {
		return *(*this + n);     //最外层的*为重载的*，返回*cur(即当前指针的值)
	}
	bool operator==(const self& x) const { return cur == x.cur; }    //两个cur所指向的地址是否相等
	bool operator!=(const self& x) const { return !(*this == x); }   //调用重载的==
	bool operator<(const self& x) const {
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
};


//size_t指缓冲区大小，默认值0表示将使用512bytes缓冲区
template<typename T,typename Alloc=alloc,size_t BufSiz=0>
class deque 
{
public:
	typedef T value_type;
	typedef T& reference;
	typedef T* pointer;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef __deque_iterator<T, T&, T*, BufSiz> iterator;

protected:
	//专属空间配置器，大小为一个元素大小
	typedef simple_alloc<value_type, Alloc> data_allocator;
	//专属空间配置器，大小为一个指针大小
	typedef simple_alloc<pointer, Alloc> map_allocator;
	//元素指针的指针
	typedef pointer* map_pointer;    //T**

	iterator start;
	iterator finish;

	map_pointer map;                 //指向map，map是一块连续空间，其中的每个元素
	                                 //都是一个指针(称为节点)指向一块缓冲区
	size_type map_size;              //map内可容纳多少指针
	
	void fill_initialize(size_type n, const reference value);
	void create_map_and_nodes(size_type num_elements);
	

public:
	deque(int n, const reference value&) :start(), finish(), map(0), map_size(0){
		fill_initialize(n, value);
	}

	iterator begin() { return start; }
	iterator end() { return finish; }

	reference operator[](size_type n) {
		return start[difference_type(n)];   //调用__deque_iterator<>::operator[]
	}

	reference front() { return *start; }    //调用__deque_iterator<>::operator[],返回所指值
	reference back() {
		iterator *(finish - 1);             //调用__deque_iterator<>::operator-
	}

	size_type size() const { return finish - start; }
	size_type max_size() const { return size_type(-1); }          //?????????
	bool empty() const { return start == finish; }
};

template<typename T,typename Alloc,size_t BufSize>
inline void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const reference value) {
	create_map_and_nodes(n);                //把deque的结构都产生并安排好
	map_pointer cur;
	for (cur = start.node; cur < finish.node; ++cur) {
		uninitialized_fill(*cur, *cur + buffer_size(), value);
	}
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
	//需要结点数=(元素个数、每个缓冲区可容纳的元素个数)+1
	//若刚好整除，会多配一个结点（后面用，因为有的已经填满了）
	size_type num_nodes = num_elements / buffer_size() + 1;

	//一个map最少要管理几个节点。最少8个，最多是"所需节点数+2"（前后各预留一个，扩充时借用）
	map_size = std::max(initial_map_size(), num_nodes + 2);
	map = map_allocator::allocate(map_size);
	//以上配置出一个“具有map_size个节点”的map

	//以下令nstart和nfinish指向map所拥有的全部节点的最中央区段
	//(确保可以向前也可以向后扩充，并且头尾扩充能量一样大)
	map_pointer nstart = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = nstart + num_node - 1;

	map_pointer cur;
	//为map内的每个现用结点配置缓冲区，所有缓冲区加起来就是deque的可用空间(最后一个缓冲区可能留有一些剩余)
	for (cur = nstart; cur <= nfinish; ++cur)
		*cur = allocate_note();
	start.set_node(nstart);
	finish.set_node(nfinish);
	start.cur = start.first;
	finish.cur = finish.first + num_elements % buffer_size();
	//若刚好整除，会多配一个结点
	//此时即令cur指向这多配的一个结点对应的缓冲区的起始处
}

}