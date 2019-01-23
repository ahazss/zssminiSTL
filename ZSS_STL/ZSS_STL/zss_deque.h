#pragma once
#include"zss_alloc.h"
#include"zss_iterator.h"
#include"zss_uninitialized.h"
#include<algorithm>

namespace ZSS {

inline size_t __deque_buf_size(size_t n, size_t sz)
{
	//n�Ǹ�������n��Ϊ0�����û��Լ����壬��Ϊ0����bufferʹ��Ĭ�ϴ�С512bytes
	//���sz(Ԫ�ش�С)С��512������512/sz�����sz��С��512������1
	//buffer�ǻ�������С��n��ÿ�����������ԷŶ��ٸ�Ԫ��
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template<typename T,typename Ref,typename Ptr,size_t BufSiz>
struct __deque_iterator {
	typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
	static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }    //buffer_size����ÿ���������Ŷ���Ԫ��

	typedef random_access_iterator_tag iterator_category;    // random����������+n,-n�Ĺ���
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T** map_pointer;
	
	typedef __deque_iterator self;

	//����������������
	T* cur;                 //�˵�������ָ�Ļ�������currentԪ��
	T* first;               //�˵�������ָ�Ļ�������ͷ
	T* last;                //�˵�������ָ�Ļ�������β
	map_pointer node;       //ָ��ܿ�����

	//ʹ��set_node��һ��������
	void set_node(map_pointer new_node) {
		node = new_node;                //node��ָ����set_node���Ǹ���������node��һ��(���ǰ��)
		first = *new_node;              //���µ�ǰ��������first��lastָ��
		last = first + difference_type(buffer_size());
	}
	
	//����������
	value_type& operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); }        //ָ�����ݵ�ָ��
	difference_type operator- (const self& x) const {            //��this-x�ľ���
		return difference_type(buffer_size())*(node - x.node - 1) + (cur - first) + (x.last - x.cur);
	}
	
	self& operator++() {             //������������ۼӣ����Է������������
		++cur;
		if (cur == last) {           //����ѵ����˻�������β��
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	self operator++(int) {           //����һ���µĶ���
		self tmp = *this;
		++*this;                     //this��ָ������������ָ��
		return tmp;
	}
	self& operator--() {
		if (cur == first) {          //���ѵ����ڻ�������ͷ��
			set_node(node - 1);      //���л���ǰһ��Ԫ��(������)�����һ��Ԫ��
			cur = last;
		}
		--cur;                       //�л���ǰһ��Ԫ��
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}

	//����ʵ�������ȡ����������ֱ����Ծn������
	self& operator+=(difference_type n) {
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size()))
			//Ŀ����ͬһ��������
			cur += n;
		else {
			//Ŀ��λ�ò���ͬһ��������
			difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) :
				-difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			//�л�����ȷ��Ԫ��
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}
	self operator+(difference_type n) const  {
		self tmp = *this;                //����Ҫ�½���������Ϊ+=����������+�����Ϊ��ֵ����Ķ��󲻿���������
		return tmp += n;                 //����+=
	}

	self& operator-=(difference_type n) {
		return *this += -n;
	}
	self operator-(difference_type n) const {
		self tmp = *this;
		return tmp -= n;
	}

	value_type& operator[] (difference_type n) const {
		return *(*this + n);     //������*Ϊ���ص�*������*cur(����ǰָ���ֵ)
	}
	bool operator==(const self& x) const { return cur == x.cur; }    //����cur��ָ��ĵ�ַ�Ƿ����
	bool operator!=(const self& x) const { return !(*this == x); }   //�������ص�==
	bool operator<(const self& x) const {
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
};


//size_tָ��������С��Ĭ��ֵ0��ʾ��ʹ��512bytes������
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
	//ר���ռ�����������СΪһ��Ԫ�ش�С
	typedef simple_alloc<value_type, Alloc> data_allocator;
	//ר���ռ�����������СΪһ��ָ���С
	typedef simple_alloc<pointer, Alloc> map_allocator;
	//Ԫ��ָ���ָ��
	typedef pointer* map_pointer;    //T**

	iterator start;
	iterator finish;

	map_pointer map;                 //ָ��map��map��һ�������ռ䣬���е�ÿ��Ԫ��
	                                 //����һ��ָ��(��Ϊ�ڵ�)ָ��һ�黺����
	size_type map_size;              //map�ڿ����ɶ���ָ��
	
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
			//���mapβ�˵Ľڵ㱸�ÿռ䲻�㣬��Ҫ�任һ��map
			//���ø���ģ�����ԭ���ģ��ͷ�ԭ����
			reallocate_map(nodes_to_add, false);
	}
	void reserve_map_at_front(size_type nodes_to_add = 1) {
		if (nodes_to_add > (start.node - map)) 
			//���mapǰ�˵Ľڵ㱸�ÿռ䲻�㣬��Ҫ����һ��map
			//���ø���ģ�����ԭ���ģ��ͷ�ԭ����
			reallocate_map(nodes_to_add, true);
	}
	void reallocate_map(size_type nodes_to_add, bool add_at_front);
    iterator insert_aux(iterator position, const value_type& x);

public:
	deque(int n, const value_type& value) :start(), finish(), map(0), map_size(0){
		fill_initialize(n, value);          //nΪ����Ԫ�ظ���
	}
	deque():start(),finish(),map(0),map_size(0) {
		fill_initialize(0, T());
	}

	iterator begin() { return start; }
	iterator end() { return finish; }

	value_type& operator[](size_type n) {
		return start[difference_type(n)];   //����__deque_iterator<>::operator[]
	}

	value_type& front() { return *start; }    //����__deque_iterator<>::operator*,������ֵָ
	value_type& back() {
		return *(finish - 1);             //����__deque_iterator<>::operator-
	}

	size_type size() const { return finish - start; }
	size_type max_size() const { return size_type(-1); }          //?????????
	bool empty() const { return start == finish; }
	void clear();

	void push_back(const value_type& t) {                         //����дvalue_type&�ᱨ���Բ�֪��ԭ�򣿣�������������
		if (finish.cur != finish.last - 1) {
			//��󻺳�������һ�����ϵı��ÿռ�
			construct(finish.cur, t);             //ֱ���ڱ��ÿռ��Ϲ��캯��
			++finish.cur;
		}
		else                                      //��󻺳���ֻʣһ��Ԫ�ر��ÿռ�
			push_back_aux(t);
	}                 
	void push_front(const value_type& t) {
		if (start.cur != start.first) {
			//��һ���������б��ÿռ�
			construct(start.cur - 1, t);
			--start.cur;
		}
		else //��һ���������ޱ��ÿռ�
			push_front_aux(t);
	}

	void pop_back() {
		if (finish.cur != finish.first) {
			//��󻺳�����һ��������Ԫ��
			--finish.cur;                 //����ָ���൱���ų�������Ԫ��
			destroy(finish.cur);          //������Ԫ������
		}
		else
			pop_back_aux();               //ָ�����һ��Ԫ�ص���һ����finish.curָ��û��Ԫ�أ����л��������ͷŹ���
	}
	void pop_front() {
		if (start.cur != start.last-1) {
			//��һ��������һ��������Ԫ��
			destroy(start.cur);          //����һԪ������
			++start.cur;                 //����ָ���൱���ų�������Ԫ��
			
		}
		else
			pop_front_aux();               //ָ�����һ��Ԫ�ص���һ����finish.curָ��û��Ԫ�أ����л��������ͷŹ���
	}

	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);

	iterator insert(iterator position, const value_type& x);
	iterator insert(iterator position, size_type n, const value_type& x);
};

template<typename T,typename Alloc,size_t BufSize>
inline void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) {
	create_map_and_nodes(n);                //��deque�Ľṹ�����������źã�nΪ����Ԫ�ؽ�����
	map_pointer cur;
	for (cur = start.node; cur < finish.node; ++cur) {
		uninitialized_fill(*cur, *cur + __deque_iterator<T, T&, T*, BufSize>::buffer_size(), value);
	}
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
	//num_elementsΪҪ���õ�Ԫ�ؽ�����
	//��Ҫ�ѱ����Ľ����=(Ԫ�ظ���/ÿ�������������ɵ�Ԫ�ظ���)+1
	//���պ������������һ����㣨�����ã���Ϊ�е��Ѿ������ˣ�
	size_type num_nodes = num_elements / __deque_iterator<T, T&, T*, BufSize>::buffer_size() + 1;

	//һ��map����Ҫ�������ڵ㡣����8���������"����ڵ���+2"��ǰ���Ԥ��һ��������ʱ���ã�
	map_size = std::max(size_type(8), num_nodes + 2);
	map = map_allocator::allocate(map_size);
	//�������ó�һ��������map_size���ڵ㡱��map,map_allocator�����ÿ��Ԫ�ش�СΪһ��T*��С

	//������nstart��nfinishָ��map��ӵ�е�ȫ���ڵ������������
	//(ȷ��������ǰҲ����������䣬����ͷβ��������һ����)
	map_pointer nstart = map + (map_size - num_nodes) / 2;    //num_nodes��Ҫ����
	map_pointer nfinish = nstart + num_nodes - 1;             //allocate��ʵ�ʽ������ȵ�ǰfinish��ָ��Ҫ��
	                                                          //��1��Ϊ�˱�֤�ÿռ���Ԫ�أ�����������໺����

	map_pointer cur;
	//Ϊmap�ڵ�ÿ�����ý��(�������num_elements��)���û�����
	//���л���������������deque�Ŀ��ÿռ�(���һ����������������һЩʣ��)
	for (cur = nstart; cur <= nfinish; ++cur)
		*cur = allocate_node();
	start.set_node(nstart);             //set_node���ź��˵�������Ӧ��node,first,last
	finish.set_node(nfinish);
	start.cur = start.first;
	finish.cur = finish.first + num_elements % __deque_iterator<T, T&, T*, BufSize>::buffer_size();
	//���պ������������һ�����
	//��ʱ����curָ��������һ������Ӧ�Ļ���������ʼ��������ָ�����һ���ڵ���һ��
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
	//��������ֻʣһ������Ԫ�ؿռ�ʱ�Żᱻ����
	value_type t_copy = t;
	reserve_map_at_back();                           //������ĳ�������������»�һ��map
	*(finish.node + 1) = allocate_node();            //����һ���½ڵ�(������),����ָ����ָ�������
	construct(finish.cur, t_copy);                   //��ʣһ������Ԫ�ؿռ�
	finish.set_node(finish.node + 1);
	finish.cur = finish.first;
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t)
{
	//����һ��������û���κα��ÿռ�ʱ�Żᱻ����
	value_type t_copy = t;
	reserve_map_at_front();                          //������ĳһ�������������»�һ��map(start.node�Ѿ�����map��ͷ)
	*(start.node - 1) = allocate_node();             //����һ���½ڵ�(������)������ָ����ָ�������
	start.set_node(start.node - 1);
	start.cur = start.last - 1;
	construct(start.cur, t_copy);
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::pop_back_aux()
{
	//��finish.cur==finish.first�Żᱻ����
	deallocate_node(finish.first);     //�ͷŻ�����
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	destroy(finish.cur);               //����deallocate��㣬���������������ͷ�
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::pop_front_aux()
{
	destroy(start.cur);               //����һ�������ĵ�һ��Ԫ������
	deallocate_node(start.first);     //�ͷŵ�һ������
	start.set_node(start.node + 1);
	start.cur = start.first;          //ָ����һ���������ĵ�һ��Ԫ��
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front)
{
	size_type old_num_nodes = finish.node - start.node + 1;
	size_type new_num_nodes = old_num_nodes + nodes_to_add;

	map_pointer new_nstart;
	if (map_size > 2 * new_num_nodes) {
		new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		//nstart = map + (map_size - num_nodes) / 2; new_num_nodes������
		if (new_nstart < start.node)
			std::copy(start.node, finish.node + 1, new_nstart);                             //��ǰ�ƣ�������������
		else
			std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);    //�����ƣ���ǰ��������
	}
	else {
		size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
		map_pointer new_map = map_allocator::allocate(new_map_size);
		new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		//��ԭmap���ݿ�������
		std::copy(start.node, finish.node + 1, new_nstart);
		//�ͷ�ԭmap
		map_allocator::deallocate(map, map_size);
		map = new_map;
		map_size = new_map_size;
	}
	//���¶��������start��finish
	start.set_node(new_nstart);
	finish.set_node(new_nstart + old_num_nodes - 1);
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::clear()
{
	//deque�ĳ�ʼ״̬������һ����������clear()֮��ҲӦ�ñ���һ��������
	//�������ͷ��β�����ÿһ��������(��Ϊ����)
	for (map_pointer node = start.node + 1; node < finish.node; ++node) {
		//��������������Ԫ������
		destroy(*node, *node + __deque_iterator<T, T&, T*, BufSize>::buffer_size());                  //Ϊnodeָ��Ļ��������ݵ�ֵ
		data_allocator::deallocate(*node, __deque_iterator<T, T&, T*, BufSize>::buffer_size());       //���ٵĿռ�
	}

	if (start.node != finish.node) {                            //������ͷβ����������
		destroy(start.cur, start.last);                         //��ͷ������������Ԫ������
		destroy(finish.first, finish.cur);                      //��β������������Ԫ������
		data_allocator::deallocate(finish.first, __deque_iterator<T, T&, T*, BufSize>::buffer_size());       //�ͷ�β����������ͷ����������
	}
	else                                                        //ֻ��һ��������
		destroy(start.cur, finish.cur);                         //����Ԫ�ز�����������
	finish = start;                                             //����״̬
}

template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T,Alloc,BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator pos)
{
	//���pos��ָ��Ԫ��
	iterator next = pos;
	++next;
	difference_type index = pos - start;         //�����֮ǰ��Ԫ�ظ���
	if (index < (size() >> 1)) {                 //��������֮ǰ��Ԫ���٣���ǰ,>>1��λ����һλ������2
		std::copy_backward(start, pos, next);
		pop_front();
	}
	else {                                       //������Ԫ�ظ��࣬����
		copy(next, finish, pos);
		pop_back();
	}
	return start + index;                        //���ص�+ pos���һ��Ԫ��
}

template<typename T, typename Alloc, size_t BufSize>
inline typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
{
	if (first == start && last == finish) {
		clear();
		return finish;
	}
	else {
		difference_type n = last - first;       //�������ĳ���
		difference_type elems_before = first - start;    //�������ǰ��Ԫ�ظ���
		if (elems_before < (size() - n) / 2) {           //ǰ����Ԫ�ؽ���
			std::copy_backward(start, first, last);      //����ƶ�ǰ��Ԫ�أ������������,start����ƶ�n
			iterator new_start = start + n;              //���deque�����
			destroy(start, new_start);                   //������Ԫ���ͷ�
			//������Ļ������ͷ�
			for (map_pointer cur = start.node; cur < new_start.node; ++cur)
				data_allocator::deallocate(*cur, __deque_iterator<T, T&, T*, BufSize>::buffer_size());
			start = new_start;
		}
		else {                                            //�󷽵�Ԫ�ؽ���
			std::copy(last, finish, first);
			iterator new_finish = finish - n;             //���deque��β��
			destroy(new_finish, finish);                  //������Ԫ���ͷ�
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
	if (position.cur == start.cur) {         //����������deque��ǰ��
		push_front(x);
		return start;
	}
	else if (position.cur == finish.cur) {       //����������deque��β��
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
	if (index < size() / 2) {                 //�����ǰԪ�ؽ���(��ǰ��)
		push_front(front());                  //������start��ֵ��ͬ�Ľ������ǰ��
		iterator front1 = start;
		//Ԫ���ƶ�
		++front1;
		iterator front2 = front1;
		++front2;
		position = start + index;                  //Ҫ�����λ��
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
		std::copy_backward(position, back2, back1);    //Ԫ���ƶ�
	}
	*position = x_copy;            //�ڲ������������ֵ
	return position;
}


}