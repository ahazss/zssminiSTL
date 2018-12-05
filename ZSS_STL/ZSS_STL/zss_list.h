#pragma once
#include"zss_iterator.h"
#include"zss_alloc.h"
#include"zss_allocator.h"

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
	pointer operator->() const { return &(operator* ()); }           //返回的是结点数据的地址 ？？？？？？

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
	typedef __list_iterator<T,T&,T*>  iterator;                          //所有用iterator的位置再注意一下！！！！！！！              

	list() { empty_initialize(); }                             //产生一个空链表


	iterator begin() { return (link_type)(node->next); }     //??????? 那个->重载??????
	iterator end()   { return node; }
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
	iterator insert(iterator position, const T& x)             //插入结点在position之前
	{            
		//在0 1 2 3 4 中3的位置插入99 结果为 0 1 2 99 3 4
		//因为是在position插元素前，不用担心更新尾指针
		link_type newnode = create_node(x);
		newnode->next = position.node;
		newnode->prev = position.node->prev;
		position.node->prev->next = newnode;
		position.node->prev = newnode;
		return newnode;
	}   
	iterator erase(iterator position)                          //移除迭代器position所指结点
	{
		link_type next_node = link_type(position.node->next);
		link_type prev_node = link_type(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);                            //只释放结点，迭代器不需要专门释放？？？？？？？
		return iterator(next_node);
	}
	void pop_front() { erase(begin()); }                        //移除头结点
	void pop_back() {                                           //移除尾结点 尾结点自减然后上面的元素被释放
		iterator tmp = end();
		erase(--tmp);                                           
	}
	void remove(const T& x);                                    //将值为x的所有元素删除




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
	while (first != last)
	{
		iterator next = first;
		++next;
		if (*first == x) earse(first);            //earse只释放了node，而iterator ？？？？？再理一下！！！ 
		first = next;
	}
}
}