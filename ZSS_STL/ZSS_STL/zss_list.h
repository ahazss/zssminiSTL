#pragma once
#include"zss_alloc.h"
#include"zss_construct.h"

namespace ZSS {

template<typename T>
struct __list_node {
	//˫������
	typedef __list_node<T>* node_pointer;
	node_pointer prev;
	node_pointer next;
	T data;
};

template<typename T, typename Ref, typename Ptr>
struct __list_iterator {
	typedef __list_iterator<T, T&, T*>  iterator;            //__list_iterator �����ԭ��ָ���ƫ�ػ��汾
	typedef __list_iterator<T, Ref, Ptr>  self;              //�������𣿣���

	//iterator �����ͱ�
	typedef bidirectional_iterator_tag iterator_category;    //˫��iterator��û��+n,-n�Ĺ���
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef __list_node<T>*  link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	link_type node;                                          //�õ�������Ӧ���ָ��

	//constructor
	__list_iterator() {}
	__list_iterator(link_type x) :node(x) {}                 //ָ�����ָ��         //????? �ֱ���ʲô�����ʹ��
	__list_iterator(const iterator& x) :node(x.node) {}      //������

	bool operator==(const self& x) const { return node == x.node; }
	bool operator!=(const self& x) const { return node != x.node; }
	//�Ե�����ȡֵ
	reference operator* () const { return (*node).data; }
	
	//�������ĳ�Ա��ȡ(member access)������
	pointer operator->() const { return &(operator* ()); }           //������a a->b�ȼ��� *(a).b  �������Ƿ������ݵĵ�ַ ?

	self& operator++ () {                            //ǰ��++
		node = (link_type)((*node).next);
		return *this;
	}

	self& operator++(int) {                          //����++
		self tmp = *this;
		++*this;
		return tmp;
	}

	self& operator--() {                             //ǰ��--
		node = (link_type)((*node).prev);
		return *this;
	}

	self& operator--(int) {                          //����--
		self tmp = *this;
		--*this;
		return tmp;
	}
};

template <typename T, typename Alloc=alloc>
class list {
	//˫��ѭ������
protected:
	typedef __list_node<T> list_node;                       //����ֱ�Ӷ�����public�У���Ϊ__list_node<T>��Ҫ���صģ���
	typedef simple_alloc<list_node, Alloc>  list_node_allocator;   //ר���ռ���������ÿ������һ������С
	                                                               //list_node_allocator(n) ��ʾ����n�����ռ�

public:
	typedef T                         value_type;
	typedef value_type*               pointer;
	typedef value_type&               reference;
	typedef size_t                    size_type;                         //��32λϵͳ��size_tΪ4�ֽڣ�64λϵͳ��Ϊ8�ֽ�
	typedef ptrdiff_t                 difference_type;
	typedef list_node*                link_type;
	typedef __list_iterator<T,T&,T*>  iterator;                          //iterator���ͣ�������++��--ǰ�ú��ã�'������'(����data)��              

	list() { empty_initialize(); }                             //����һ��������
	template<typename iter>
	list(iter first, iter last)                          //����[first,last)�����ݸ�list
	{                       
		empty_initialize();
		insert(node, first, last);
	}
	list(size_type n, const T& x)                              //n��ֵΪx������
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

	iterator begin() const { return (link_type)(node->next); }    //�����->�ǽ�����еĲ��ǵ������е�
	iterator end() const  { return node; }                        //��������Ϊlink_node*,����õ������Ĺ��캯��,endΪβ��Ԫ��
	bool empty() const { return node->next == node; }
	size_type size() const {
		size_type result = 0;
		result=distance(begin(), end());                       //������zss_iterator�У����ݵ��������ͽ��в�ͬ����
		return result;
	}
	void clear();                                              //������нڵ�(��������)
	//ȡͷ��������(Ԫ��ֵ)
	reference front() { return *begin(); }
	reference back() { return *(--end()); }

	void push_front(const T& x) { insert(begin(), x); }        //����һ���ڵ���Ϊͷ���
	void push_back(const T& x) { insert(end(), x); }           //��push_back����Ԫ��ʱ���˺����ڲ�����insert()
	void pop_front() { erase(begin()); }                        //�Ƴ�ͷ���
	void pop_back() {                                           //�Ƴ�β��� β����Լ�Ȼ�������Ԫ�ر��ͷ�
		iterator tmp = end();
		erase(--tmp);                                           
	}

	iterator insert(iterator position, const T& x)             //��������position֮ǰ
	{           
		//��0 1 2 3 4 ��3��λ�ò���99 ���Ϊ 0 1 2 99 3 4
		//��Ϊ����position��Ԫ��ǰ�����õ��ĸ���βָ��
		link_type newnode = create_node(x);
		newnode->next = position.node;
		newnode->prev = position.node->prev;
		position.node->prev->next = newnode;
		position.node->prev = newnode;
		return newnode;                                        //�����iteratorӦ���ǻ���õ������Ĺ��캯��
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

	iterator erase(iterator position)                          //�Ƴ�������position��ָ���
	{
		link_type next_node = link_type(position.node->next);
		link_type prev_node = link_type(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);                            //ֻ�ͷŽ�㣬����������Ҫר���ͷ�(û�п��ٶ���ռ�)�Լ��ͷ�
		return iterator(next_node);                             //����iterator���캯��������һ���µ�������ַ
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
			//����
			size_type diff = n - size();
			while (diff != 0) {
                insert(node, x);
				diff--;
			}
		}
		else if (n < size()) {
			//����ǰn��Ԫ�غ�node
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

	void swap(list<T>& right){                                  //������ͷ���
		link_type node1 = node;
		node = right.node;
		right.node = node1;
	}
	void remove(const T& x);                                    //��ֵΪx������Ԫ��ɾ��
	void unique();                                              //�Ƴ���ֵ��ͬ������Ԫ����ֻʣһ��
	//�����splice����&&����
	void splice(iterator position, list<T>& right);             //�ӺͲ�������right�ӵ�position֮ǰ
	void splice(iterator position, iterator i);                 //�ӺͲ�������i��ָԪ�ؽӵ�position֮ǰ
	void splice(iterator position, iterator first, iterator last);  //�ӺͲ�������first��lastԪ�ؽӵ�position֮ǰ

	void sort();                                                    //������ΪlistΪbidirectional_iterator����STL��sort�㷨ֻ����RandomAccessIterator
	                                                                //����list�������Լ���sort����
	void merge(list<T>& x);                                         //merge��x�ϲ���*this�ϣ�����list���ݶ������Ⱦ�����������
	void reverse();                                                 //��*this��������������


protected:
	link_type node;                      //һ��ָ�뼴��(��״˫������)
	                                     //��ָ��nodeָ���������β�˵�һ���հ׽�㣬node��ɳ�Ϊlast������
	//����һ����㲢����
	link_type get_node() { return list_node_allocator::allocate(); }
	//�ͷ�һ�����                                    
	void put_node(link_type p) { list_node_allocator::deallocate(p); }
	//����(���ò�����)һ����㣬����Ԫ��ֵ
	link_type create_node(const T& x) {
		link_type p = get_node();
		construct(&p->data, x);                  //ȫ�ֺ���,��Ϊ�ǰ�x��ֵ������data�����Դ���ȥ�ĵ�ַΪdata�ĵ�ַ
		                                         //->���ȼ�����& ������->�ĵ��÷�ʽ��Ϊ����������������
		return p;
	}
	//����(�������ͷ�)һ����� 
	void destroy_node(link_type p) {
		destroy(&p->data);                       //����������Խ���е�data����
		put_node(p);                             //�ͷ�һ�����
	}

	void empty_initialize() {
		node = get_node();               //����һ�����ռ䣬��nodeָ����
		node->next = node;               //ͷβ��ָ���Լ�������Ԫ��ֵ
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
		cur = cur->next;                //����ֻҪ������������ɾ�����У���Ϊ���������Ԫ��
		destroy_node(del);              //����Ҫά��˫�����ӣ�������ʹЧ�ʱ��
	}
	//�ָ�node��ʼ״̬
	node->next = node;
	node->prev = node;
}

template<typename T, typename Alloc>
inline void list<T, Alloc>::remove(const T& x)
{
	//����ֵ����x��Ԫ�ض�ɾ��
	iterator first = begin();
	iterator last = end();
	while (first != last)                         //�����!=�����ع��ģ��Ƚ�node��ַ
	{
		iterator next = first;
		++next;
		if (*first == x) erase(first);            //earse�ͷ���node������iterator��iterator�ı䣬����ʱfirst����ĵ�ַΪ��Ч��ַ 
		first = next;                             //��next��first��ֵ�ĺ���Ϊʹfisrtָ��ͬһ����ַ��&first��&next�������������ǲ���ı��	                                    
	}
}

template<typename T, typename Alloc>
inline void list<T, Alloc>::unique() {
	iterator first = begin();
	iterator last = end();
	if (first == last) return;                    //������
	iterator next = first;
	while ((++next) != last)
	{
		if (*first == *next)  erase(next);
		else first = next;
		next = first;                             //�ָ�next
	}
}

template<typename T,typename Alloc>
inline void list<T, Alloc>::transfer(iterator position, iterator first, iterator last)
{
	//��[first,last)�ڵ�����Ԫ���ƶ���position֮ǰ
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
	//��list�嵽position֮ǰ
	if(!right.empty())
	     transfer(position, right.begin(), right.end());
}
template<typename T, typename Alloc>
inline void list<T, Alloc>::splice(iterator position, iterator i)
{
	//��һ��Ԫ�ز嵽position֮ǰ
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
	//����quick sort���п�������
	//����ǿ���������һ��Ԫ�أ��Ͳ������κβ�����ʹ��size()==0/1�жϽ���
	if (node->next == node || node->next->next == node)
		return;

	//һЩ�µ�lists��Ϊ�н����ݴ����
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
			carry.swap(counter[i++]);          //����i���ٽ���++
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
	//������list����С˳��ϲ���this����
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();

	//ǰ��������list���Ѿ�����������
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
	//�����2�ȴ����꣬1�ľ����ţ�1�ȴ����꣬��2��Ҫ����1��ȷ������������
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