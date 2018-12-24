#pragma once
#include"zss_iterator.h"
#include"zss_uninitialized.h"

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
	typedef __deque_iterator<T, const T&, const T*, Bufsiz> const_iterator;
	static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

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
	reference operator*() const { return *cur; }
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
	self& operator+(difference_type n) const {
		self tmp = *this;                //����Ҫ�½���������Ϊ+=����������+�����Ϊ��ֵ����Ķ��󲻿���������
		return tmp += n;                 //����+=
	}

	self& operator-=(difference_type n) const {
		return *this += -n;
	}
	self& operator-(difference_type n) const {
		self tmp = *this;
		return tmp -= n;
	}

	reference operator[] (difference_type n) const {
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
	
	void fill_initialize(size_type n, const reference value);
	void create_map_and_nodes(size_type num_elements);
	

public:
	deque(int n, const reference value&) :start(), finish(), map(0), map_size(0){
		fill_initialize(n, value);
	}

	iterator begin() { return start; }
	iterator end() { return finish; }

	reference operator[](size_type n) {
		return start[difference_type(n)];   //����__deque_iterator<>::operator[]
	}

	reference front() { return *start; }    //����__deque_iterator<>::operator[],������ֵָ
	reference back() {
		iterator *(finish - 1);             //����__deque_iterator<>::operator-
	}

	size_type size() const { return finish - start; }
	size_type max_size() const { return size_type(-1); }          //?????????
	bool empty() const { return start == finish; }
};

template<typename T,typename Alloc,size_t BufSize>
inline void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const reference value) {
	create_map_and_nodes(n);                //��deque�Ľṹ�����������ź�
	map_pointer cur;
	for (cur = start.node; cur < finish.node; ++cur) {
		uninitialized_fill(*cur, *cur + buffer_size(), value);
	}
}

template<typename T, typename Alloc, size_t BufSize>
inline void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
	//��Ҫ�����=(Ԫ�ظ�����ÿ�������������ɵ�Ԫ�ظ���)+1
	//���պ������������һ����㣨�����ã���Ϊ�е��Ѿ������ˣ�
	size_type num_nodes = num_elements / buffer_size() + 1;

	//һ��map����Ҫ�������ڵ㡣����8���������"����ڵ���+2"��ǰ���Ԥ��һ��������ʱ���ã�
	map_size = std::max(initial_map_size(), num_nodes + 2);
	map = map_allocator::allocate(map_size);
	//�������ó�һ��������map_size���ڵ㡱��map

	//������nstart��nfinishָ��map��ӵ�е�ȫ���ڵ������������
	//(ȷ��������ǰҲ����������䣬����ͷβ��������һ����)
	map_pointer nstart = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = nstart + num_node - 1;

	map_pointer cur;
	//Ϊmap�ڵ�ÿ�����ý�����û����������л���������������deque�Ŀ��ÿռ�(���һ����������������һЩʣ��)
	for (cur = nstart; cur <= nfinish; ++cur)
		*cur = allocate_note();
	start.set_node(nstart);
	finish.set_node(nfinish);
	start.cur = start.first;
	finish.cur = finish.first + num_elements % buffer_size();
	//���պ������������һ�����
	//��ʱ����curָ��������һ������Ӧ�Ļ���������ʼ��
}

}