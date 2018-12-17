#pragma once
#include"zss_alloc.h"
#include"zss_construct.h"

namespace ZSS {

template<typename T>
struct __list_node {
	//双向链表
	typedef __list_node<T>* node_pointer;
	node_pointer prev;
	node_pointer next;
	T data;
};

template<typename T, typename Ref, typename Ptr>
struct __list_iterator {
	typedef __list_iterator<T, T&, T*>  iterator;            //__list_iterator 的针对原生指针的偏特化版本
	typedef __list_iterator<T, Ref, Ptr>  self;              //名称区别？？？

	//iterator 基本型别
	typedef bidirectional_iterator_tag iterator_category;    //双向iterator，没有+n,-n的功能
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef __list_node<T>*  link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	link_type node;                                          //该迭代器对应结点指针

	//constructor
	__list_iterator() {}
	__list_iterator(link_type x) :node(x) {}                 //指向结点的指针         //????? 分别在什么情况下使用
	__list_iterator(const iterator& x) :node(x.node) {}      //迭代器

	bool operator==(const self& x) const { return node == x.node; }
	bool operator!=(const self& x) const { return node != x.node; }
	//对迭代器取值
	reference operator* () const { return (*node).data; }
	
	//迭代器的成员存取(member access)运算子
	pointer operator->() const { return &(operator* ()); }           //迭代器a a->b等价于 *(a).b  在这里是返回数据的地址 ?

	self& operator++ () {                            //前置++
		node = (link_type)((*node).next);
		return *this;
	}

	self& operator++(int) {                          //后置++
		self tmp = *this;
		++*this;
		return tmp;
	}

	self& operator--() {                             //前置--
		node = (link_type)((*node).prev);
		return *this;
	}

	self& operator--(int) {                          //后置--
		self tmp = *this;
		--*this;
		return tmp;
	}
};

template <typename T, typename Alloc=alloc>
class list {
	//双向循环链表
protected:
	typedef __list_node<T> list_node;                       //不能直接定义在public中，因为__list_node<T>是要隐藏的？？
	typedef simple_alloc<list_node, Alloc>  list_node_allocator;   //专属空间配置器，每次配置一个结点大小
	                                                               //list_node_allocator(n) 表示配置n个结点空间

public:
	typedef T                         value_type;
	typedef value_type*               pointer;
	typedef value_type&               reference;
	typedef size_t                    size_type;                         //在32位系统中size_t为4字节，64位系统中为8字节
	typedef ptrdiff_t                 difference_type;
	typedef list_node*                link_type;
	typedef __list_iterator<T,T&,T*>  iterator;                          //iterator类型，定义了++，--前置后置，'解引用'(返回data)，              

	list() { empty_initialize(); }                             //产生一个空链表
	template<typename iter>
	list(iter first, iter last)                          //复制[first,last)的内容给list
	{                       
		empty_initialize();
		insert(node, first, last);
	}
	list(size_type n, const T& x)                              //n个值为x的链表
	{                 
		empty_initialize();
		insert(node, n, x);
	}
	~list(){
		clear();
	}

	void assign(size_type n,const T& x)
	{
		insert(node, n, x);
	}
	template<typename iter>
	void assign(iter first, iter last)
	{
		insert(node, first, last);
	}

	iterator begin() const { return (link_type)(node->next); }    //这里的->是结点类中的不是迭代器中的
	iterator end() const  { return node; }                        //返回类型为link_node*,会调用迭代器的构造函数,end为尾后元素
	bool empty() const { return node->next == node; }
	size_type size() const {
		size_type result = 0;
		result=distance(begin(), end());                       //定义在zss_iterator中，根据迭代器类型进行不同处理
		return result;
	}
	void clear();                                              //清除所有节点(整个链表)
	//取头结点的内容(元素值)
	reference front() { return *begin(); }
	reference back() { return *(--end()); }

	void push_front(const T& x) { insert(begin(), x); }        //插入一个节点作为头结点
	void push_back(const T& x) { insert(end(), x); }           //以push_back加入元素时，此函数内部调用insert()
	void pop_front() { erase(begin()); }                        //移除头结点
	void pop_back() {                                           //移除尾结点 尾结点自减然后上面的元素被释放
		iterator tmp = end();
		erase(--tmp);                                           
	}

	iterator insert(iterator position, const T& x)             //插入结点在position之前
	{           
		//在0 1 2 3 4 中3的位置插入99 结果为 0 1 2 99 3 4
		//因为是在position插元素前，不用担心更新尾指针
		link_type newnode = create_node(x);
		newnode->next = position.node;
		newnode->prev = position.node->prev;
		position.node->prev->next = newnode;
		position.node->prev = newnode;
		return newnode;                                        //这里的iterator应该是会调用迭代器的构造函数
	}   
	iterator insert(iterator position, size_type n, const T& x)
	{
		iterator result = position;
		for (int i = 0; i < n; i++) {
			result=insert(position, x);
		}
		return result;
	}
	iterator insert(iterator position, iterator first, iterator last)
	{
		link_type result=nullptr;
		while (first != last)
		{
			link_type newnode = create_node(*first);
			first++;
			result = newnode;
			newnode->next = position.node;
			newnode->prev = position.node->prev;
			position.node->prev->next = newnode;
			position.node->prev = newnode;
		}
		return result;
	}
	iterator insert(iterator position, pointer first, pointer last)
	{
		link_type result=nullptr;
		while (first != last)
		{
			link_type newnode = create_node(*first);
			result = newnode;
			first++;
			newnode->next = position.node;
			newnode->prev = position.node->prev;
			position.node->prev->next = newnode;
			position.node->prev = newnode;
		}
		return result;
	}

	iterator erase(iterator position)                          //移除迭代器position所指结点
	{
		link_type next_node = link_type(position.node->next);
		link_type prev_node = link_type(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);                            //只释放结点，迭代器不需要专门释放(没有开辟额外空间)自己释放
		return iterator(next_node);                             //调用iterator构造函数，返回一个新迭代器地址
	}
	iterator erase(iterator first, iterator last) {
		while (first != last) 
		{
			erase(first);
			first++;
		}
		return last;
	}

	void resize(size_type n,const T& x=T()) {
		if (n > size()) {
			//扩容
			size_type diff = n - size();
			while (diff != 0) {
                insert(node, x);
				diff--;
			}
		}
		else if (n < size()) {
			//保留前n个元素和node
			size_type diff = size() - n;
			link_type del = node;
			while (diff != 0) {
				del=del->prev;
				--diff;
			}
			erase(del, node);
		}
		else  return;
	}

	void swap(list<T>& right){                                  //仅交换头结点
		link_type node1 = node;
		node = right.node;
		right.node = node1;
	}
	void remove(const T& x);                                    //将值为x的所有元素删除
	void unique();                                              //移除数值相同的连续元素至只剩一个
	//这里的splice还有&&重载
	void splice(iterator position, list<T>& right);             //接和操作，将right接到position之前
	void splice(iterator position, iterator i);                 //接和操作，将i所指元素接到position之前
	void splice(iterator position, iterator first, iterator last);  //接和操作，将first到last元素接到position之前

	void sort();                                                    //排序，因为list为bidirectional_iterator，而STL的sort算法只接受RandomAccessIterator
	                                                                //所以list必须有自己的sort函数
	void merge(list<T>& x);                                         //merge将x合并到*this上，两个list内容都必须先经过递增排列
	void reverse();                                                 //将*this的内容逆向重置


protected:
	link_type node;                      //一个指针即可(环状双向链表)
	                                     //让指针node指向刻意置于尾端的一个空白结点，node便可成为last迭代器
	//配置一个结点并传回
	link_type get_node() { return list_node_allocator::allocate(); }
	//释放一个结点                                    
	void put_node(link_type p) { list_node_allocator::deallocate(p); }
	//产生(配置并构造)一个结点，带有元素值
	link_type create_node(const T& x) {
		link_type p = get_node();
		construct(&p->data, x);                  //全局函数,因为是把x的值给结点的data，所以传过去的地址为data的地址
		                                         //->优先级高于& 运算子->的调用方式即为这样？？？？？？
		return p;
	}
	//销毁(析构并释放)一个结点 
	void destroy_node(link_type p) {
		destroy(&p->data);                       //析构函数针对结点中的data部分
		put_node(p);                             //释放一个结点
	}

	void empty_initialize() {
		node = get_node();               //配置一个结点空间，令node指向它
		node->next = node;               //头尾均指向自己，不设元素值
		node->prev = node;
	}
	void transfer(iterator position, iterator first, iterator last);    
};

template<typename T, typename Alloc>
inline void list<T, Alloc>::clear()
{
	link_type cur = (link_type)node->next;
	while (cur != node)
	{
		link_type del = cur;
		cur = cur->next;                //这里只要不断向后迭代并删除就行，因为是清除所有元素
		destroy_node(del);              //不需要维持双向链接，反而会使效率变低
	}
	//恢复node初始状态
	node->next = node;
	node->prev = node;
}

template<typename T, typename Alloc>
inline void list<T, Alloc>::remove(const T& x)
{
	//所有值等于x的元素都删除
	iterator first = begin();
	iterator last = end();
	while (first != last)                         //这里的!=是重载过的，比较node地址
	{
		iterator next = first;
		++next;
		if (*first == x) erase(first);            //earse释放了node，构建iterator，iterator改变，即此时first里面的地址为无效地址 
		first = next;                             //用next给first赋值的含义为使fisrt指向同一个地址，&first和&next在整个过程中是不会改变的	                                    
	}
}

template<typename T, typename Alloc>
inline void list<T, Alloc>::unique() {
	iterator first = begin();
	iterator last = end();
	if (first == last) return;                    //空链表
	iterator next = first;
	while ((++next) != last)
	{
		if (*first == *next)  erase(next);
		else first = next;
		next = first;                             //恢复next
	}
}

template<typename T,typename Alloc>
inline void list<T, Alloc>::transfer(iterator position, iterator first, iterator last)
{
	//将[first,last)内的所有元素移动到position之前
	if (position != last)
	{
		(*(link_type((*last.node).prev))).next = position.node;
		(*(link_type((*first.node).prev))).next = last.node;
		(*(link_type((*position.node).prev))).next = first.node;
		link_type tmp = link_type((*position.node).prev);
		(*position.node).prev = (*last.node).prev;
		(*last.node).prev = (*first.node).prev;
		(*first.node).prev = tmp;
	}
}

template<typename T,typename Alloc>
inline void list<T, Alloc>::splice(iterator position, list<T>& right)
{
	//将list插到position之前
	if(!right.empty())
	     transfer(position, right.begin(), right.end());
}
template<typename T, typename Alloc>
inline void list<T, Alloc>::splice(iterator position, iterator i)
{
	//将一个元素插到position之前
	iterator j = i;
	++j;
	if (position == i || position == j) return;
	transfer(position, i, j);
}
template<typename T, typename Alloc>
inline void list<T, Alloc>::splice(iterator position, iterator first, iterator last)
{
	if (first != last)
		transfer(position, first, last);
}

template<typename T,typename Alloc>
inline void list<T, Alloc>::sort()
{
	//采用quick sort进行快速排序
	//如果是空链表或仅有一个元素，就不进行任何操作，使用size()==0/1判断较慢
	if (node->next == node || node->next->next == node)
		return;

	//一些新的lists作为中介数据存放区
	list<T, Alloc> carry;
	list<T, Alloc> counter[64];
	int fill = 0;
	while (!empty())
	{
		carry.splice(carry.begin(), begin());
		int i = 0;
		while (i < fill && !counter[i].empty())
		{
			counter[i].merge(carry);
			carry.swap(counter[i++]);          //先用i，再进行++
		}
		carry.swap(counter[i]);
		if (i == fill) ++fill;
	}

	for (int i = 1; i < fill; ++i)
		counter[i].merge(counter[i - 1]);
	swap(counter[fill - 1]);

}

template<typename T,typename Alloc>
inline void list<T, Alloc>::merge(list<T>& x)
{
	//将两个list按大小顺序合并至this链中
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();

	//前提是两个list都已经过递增排序
	while (first1 != last1 && first2 != last2)
	{
		if (*first2 < *first1) {
			iterator next = first2;
			transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
	}
	//如果是2先处理完，1的就留着，1先处理完，则2的要链入1，确保其结果完整性
	if (first2 != last2) 
		transfer(last1, first2, last2);
}

template<typename T,typename Alloc>
inline void list<T, Alloc>::reverse()
{
	if (node->next == node || node->next->next == node)
		return;
	/*
	for (int i = 0; i < size(); i++)
	{
		iterator first = begin();
		iterator last = end();
		--last;
		transfer(begin(), last, end());
	}*/
	iterator first = begin();
	++first;
	while (first != end()) {
		iterator old = first;
		++first;
		transfer(begin(), old, first);
	}
}


}