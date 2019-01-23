#pragma once
#include"zss_alloc.h"
#include"zss_iterator.h"
#include"zss_uninitialized.h"
#include<algorithm>

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
	typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
	static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }    //buffer_size返回每个缓冲区放多少元素

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
	value_type& operator*() const { return *cur; }
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
	self operator+(difference_type n) const  {
		self tmp = *this;                //必须要新建对象是因为+=给其自身，而+则可能为赋值给别的对象不可以用自身
		return tmp += n;                 //调用+=
	}

	self& operator-=(difference_type n) {
		return *this += -n;
	}
	self operator-(difference_type n) const {
		self tmp = *this;
		return tmp -= n;
	}

	value_type& operator[] (difference_type n) const {
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
	typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
	typedef typename const_iterator::reference const_reference;


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
	
	void fill_initialize(size_type n, const value_type& value);
	void create_map_and_nodes(size_type num_elements);
	T* allocate_node();
	void deallocate_node(pointer p);
	void push_back_aux(const value_type& t);
	void push_front_aux(const value_type& t);
	void pop_back_aux();
	void pop_front_aux();
	void reserve_map_at_back(size_type nodes_to_add = 1) {
		if (nodes_to_add + 1 > map_size - (finish.node - map))
			//如果map尾端的节点备用空间不足，就要充换一个map
			//配置更大的，拷贝原来的，释放原来的
			reallocate_map(nodes_to_add, false);
	}
	void reserve_map_at_front(size_type nodes_to_add = 1) {
		if (nodes_to_add > (start.node - map)) 
			//如果map前端的节点备用空间不足，就要更换一个map
			//配置更大的，拷贝原来的，释放原来的
			reallocate_map(nodes_to_add, true);
	}
	void reallocate_map(size_type nodes_to_add, bool add_at_front);
    iterator insert_aux(iterator position, const value_type& x);

public:
	deque(int n, const value_type& value) :start(), finish(), map(0), map_size(0){
		fill_initialize(n, value);          //n为构造元素个数
	}
	deque():start(),finish(),map(0),map_size(0) {
		fill_initialize(0, T());
	}

	iterator begin() { return start; }
	iterator end() { return finish; }

	value_type& operator[](size_type n) {
		return start[difference_type(n)];   //调用__deque_iterator<>::operator[]
	}

	value_type& front() { return *start; }    //调用__deque_iterator<>::operator*,返回所指值
	value_type& back() {
		return *(finish - 1);             //调用__deque_iterator<>::operator-
	}

	size_type size() const { return finish - start; }
	size_type max_size() const { return size_type(-1); }          //?????????
	bool empty() const { return start == finish; }
	void clear();

	void push_back(const value_type& t) {                         //这里写value_type&会报错，仍不知道原因？？？？？？？？
		if (finish.cur != finish.last - 1) {
			//最后缓冲区尚有一个以上的备用空间
			construct(finish.cur, t);             //直接在备用空间上构造函数
			++finish.cur;
		}
		else                                      //最后缓冲区只剩一个元素备用空间
			push_back_aux(t);
	}                 
	void push_front(const value_type& t) {
		if (start.cur != start.first) {
			//第一缓冲区仍有备用空间
			construct(start.cur - 1, t);
			--start.cur;
		}
		else //第一缓冲区已无备用空间
			push_front_aux(t);
	}

	void pop_back() {
		if (finish.cur != finish.first) {
			//最后缓冲区有一个或更多的元素
			--finish.cur;                 //调整指针相当于排除了最后的元素
			destroy(finish.cur);          //将最后的元素析构
		}
		else
			pop_back_aux();               //指向最后一个元素的下一个，finish.cur指的没有元素，进行缓冲区的释放工作
	}
	void pop_front() {
		if (start.cur != start.last-1) {
			//第一缓冲区有一个或更多的元素
			destroy(start.cur);          //将第一元素析构
			++start.cur;                 //调整指针相当于排除了最后的元素
			
		}
		else
			pop_front_aux();               //指向最后一个元素的下一个，finish.cur指的没有元素，进行缓冲区的释放工作
	}

	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);

	iterator insert(iterator position, const value_type& x);
	iterator insert(iterator position, size_type n, const value_type& x);
};

template<typename T,typename Alloc,size_t BufSize>
inline void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) {
	create_map_and_nodes(n);                //把deque的结构都产生并安排好，n为构造元素结点个数
	map_pointer cur;
	for (cur = start.node; cur < finish.node; ++cur) {
		uninitialized_fill(*cur, *cur + __deque_iterator<T, T&, T*, BufSize>::buffer_size(), value);
	}
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
	//num_elements为要配置的元素结点个数
	//需要已被填充的结点数=(元素个数/每个缓冲区可容纳的元素个数)+1
	//若刚好整除，会多配一个结点（后面用，因为有的已经填满了）
	size_type num_nodes = num_elements / __deque_iterator<T, T&, T*, BufSize>::buffer_size() + 1;

	//一个map最少要管理几个节点。最少8个，最多是"所需节点数+2"（前后各预留一个，扩充时借用）
	map_size = std::max(size_type(8), num_nodes + 2);
	map = map_allocator::allocate(map_size);
	//以上配置出一个“具有map_size个节点”的map,map_allocator分配的每个元素大小为一个T*大小

	//以下令nstart和nfinish指向map所拥有的全部节点的最中央区段
	//(确保可以向前也可以向后扩充，并且头尾扩充能量一样大)
	map_pointer nstart = map + (map_size - num_nodes) / 2;    //num_nodes是要填充的
	map_pointer nfinish = nstart + num_nodes - 1;             //allocate的实际结点个数比当前finish所指的要大
	                                                          //减1是为了保证该空间有元素，而不分配过多缓冲区

	map_pointer cur;
	//为map内的每个现用结点(已申请的num_elements个)配置缓冲区
	//所有缓冲区加起来就是deque的可用空间(最后一个缓冲区可能留有一些剩余)
	for (cur = nstart; cur <= nfinish; ++cur)
		*cur = allocate_node();
	start.set_node(nstart);             //set_node安排好了迭代器对应的node,first,last
	finish.set_node(nfinish);
	start.cur = start.first;
	finish.cur = finish.first + num_elements % __deque_iterator<T, T&, T*, BufSize>::buffer_size();
	//若刚好整除，会多配一个结点
	//此时即令cur指向这多配的一个结点对应的缓冲区的起始处，否则指向最后一个节点下一个
}

template<typename T, typename Alloc, size_t BufSize>
inline T* deque<T,Alloc,BufSize>::allocate_node() 
{ 
	return data_allocator::allocate(__deque_iterator<T, T&, T*, BufSize>::buffer_size());
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::deallocate_node(pointer p)
{
	return data_allocator::deallocate(p, __deque_iterator<T, T&, T*, BufSize>::buffer_size());
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t)
{
	//当缓冲区只剩一个备用元素空间时才会被调用
	value_type t_copy = t;
	reserve_map_at_back();                           //若符合某种条件必须重新换一个map
	*(finish.node + 1) = allocate_node();            //配置一个新节点(缓冲区),更改指针所指向的内容
	construct(finish.cur, t_copy);                   //还剩一个备用元素空间
	finish.set_node(finish.node + 1);
	finish.cur = finish.first;
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t)
{
	//当第一个缓冲区没有任何备用空间时才会被调用
	value_type t_copy = t;
	reserve_map_at_front();                          //若符合某一个条件必须重新换一个map(start.node已经到了map的头)
	*(start.node - 1) = allocate_node();             //配置一个新节点(缓冲区)，更改指针所指向的内容
	start.set_node(start.node - 1);
	start.cur = start.last - 1;
	construct(start.cur, t_copy);
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::pop_back_aux()
{
	//当finish.cur==finish.first才会被调用
	deallocate_node(finish.first);     //释放缓冲区
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	destroy(finish.cur);               //不用deallocate结点，整个缓冲区进行释放
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::pop_front_aux()
{
	destroy(start.cur);               //将第一缓冲区的第一个元素析构
	deallocate_node(start.first);     //释放第一缓冲区
	start.set_node(start.node + 1);
	start.cur = start.first;          //指向下一个缓冲区的第一个元素
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front)
{
	size_type old_num_nodes = finish.node - start.node + 1;
	size_type new_num_nodes = old_num_nodes + nodes_to_add;

	map_pointer new_nstart;
	if (map_size > 2 * new_num_nodes) {
		new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		//nstart = map + (map_size - num_nodes) / 2; new_num_nodes增大了
		if (new_nstart < start.node)
			std::copy(start.node, finish.node + 1, new_nstart);                             //往前移，给后面扩充结点
		else
			std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);    //往后移，给前面扩充结点
	}
	else {
		size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
		map_pointer new_map = map_allocator::allocate(new_map_size);
		new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		//把原map内容拷贝过来
		std::copy(start.node, finish.node + 1, new_nstart);
		//释放原map
		map_allocator::deallocate(map, map_size);
		map = new_map;
		map_size = new_map_size;
	}
	//重新定义迭代器start和finish
	start.set_node(new_nstart);
	finish.set_node(new_nstart + old_num_nodes - 1);
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::clear()
{
	//deque的初始状态保留了一个缓冲区，clear()之后也应该保留一个缓冲区
	//以下针对头和尾以外的每一个缓冲区(均为满的)
	for (map_pointer node = start.node + 1; node < finish.node; ++node) {
		//将缓冲区的所有元素析构
		destroy(*node, *node + __deque_iterator<T, T&, T*, BufSize>::buffer_size());                  //为node指向的缓冲区内容的值
		data_allocator::deallocate(*node, __deque_iterator<T, T&, T*, BufSize>::buffer_size());       //开辟的空间
	}

	if (start.node != finish.node) {                            //至少有头尾两个缓冲区
		destroy(start.cur, start.last);                         //将头缓冲区的所有元素析构
		destroy(finish.first, finish.cur);                      //将尾缓冲区的所有元素析构
		data_allocator::deallocate(finish.first, __deque_iterator<T, T&, T*, BufSize>::buffer_size());       //释放尾缓冲区，将头缓冲区保留
	}
	else                                                        //只有一个缓冲区
		destroy(start.cur, finish.cur);                         //析构元素并保留缓冲区
	finish = start;                                             //调整状态
}

template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T,Alloc,BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator pos)
{
	//清除pos所指的元素
	iterator next = pos;
	++next;
	difference_type index = pos - start;         //清除点之前的元素个数
	if (index < (size() >> 1)) {                 //如果清除点之前的元素少，靠前,>>1按位右移一位，除以2
		std::copy_backward(start, pos, next);
		pop_front();
	}
	else {                                       //清除点后元素更多，靠后
		copy(next, finish, pos);
		pop_back();
	}
	return start + index;                        //重载的+ pos后的一个元素
}

template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
{
	if (first == start && last == finish) {
		clear();
		return finish;
	}
	else {
		difference_type n = last - first;       //清除区间的长度
		difference_type elems_before = first - start;    //清除区间前方元素个数
		if (elems_before < (size() - n) / 2) {           //前方的元素较少
			std::copy_backward(start, first, last);      //向后移动前方元素，覆盖清除区间,start向后移动n
			iterator new_start = start + n;              //标记deque新起点
			destroy(start, new_start);                   //将冗余元素释放
			//将冗余的缓冲区释放
			for (map_pointer cur = start.node; cur < new_start.node; ++cur)
				data_allocator::deallocate(*cur, __deque_iterator<T, T&, T*, BufSize>::buffer_size());
			start = new_start;
		}
		else {                                            //后方的元素较少
			std::copy(last, finish, first);
			iterator new_finish = finish - n;             //标记deque新尾点
			destroy(new_finish, finish);                  //将冗余元素释放
			for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
				data_allocator::deallocate(*cur, __deque_iterator<T, T&, T*, BufSize>::buffer_size());
			finish = new_finish;
		}
		return start + elems_before;
	}
}
template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert(iterator position, size_type n, const value_type& x)
{
	for (int i = 0; i < n; i++) {
		insert(position, x);
		position++;
	}
}
template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert(iterator position, const value_type& x)
{
	if (position.cur == start.cur) {         //如果插入点是deque最前端
		push_front(x);
		return start;
	}
	else if (position.cur == finish.cur) {       //如果插入点是deque最尾端
		push_back(x);
		iterator tmp = finish;
		--tmp;
		return tmp;
	}
	else {
		return insert_aux(position, x);
	}
}

template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert_aux(iterator position, const value_type& x)
{
	difference_type index = position - start;
	value_type x_copy = x;
	if (index < size() / 2) {                 //插入点前元素较少(向前挤)
		push_front(front());                  //构造与start的值相同的结点在最前面
		iterator front1 = start;
		//元素移动
		++front1;
		iterator front2 = front1;
		++front2;
		position = start + index;                  //要插入的位置
		iterator pos1 = position;
		++pos1;
		std::copy(front2, pos1, front1);
	}
	else {
		push_back(back());
		iterator back1 = finish;
		--back1;
		iterator back2 = back1;
		--back2;
		position= start + index;
		std::copy_backward(position, back2, back1);    //元素移动
	}
	*position = x_copy;            //在插入点上设置新值
	return position;
}


}