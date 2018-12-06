#pragma once
#include"zss_iterator.h"
#include"zss_alloc.h"
#include"zss_allocator.h"

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
	pointer operator->() const { return &(operator* ()); }           //������a a->b�ȼ��� *(a).b  �������Ƿ������ݵĵ�ַ

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


	iterator begin() { return (link_type)(node->next); }     //�����->�ǽ�����еĲ��ǵ������е�
	iterator end()   { return node; }                        //��������Ϊlink_node* Ӧ�û���õ������Ĺ��캯��   �������ٲ���һ��
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
	iterator erase(iterator position)                          //�Ƴ�������position��ָ���
	{
		link_type next_node = link_type(position.node->next);
		link_type prev_node = link_type(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);                            //ֻ�ͷŽ�㣬����������Ҫר���ͷ�(û�п��ٶ���ռ�)�Լ��ͷ�
		return iterator(next_node);                             //����iterator���캯��������һ���µ�������ַ
	}
	void pop_front() { erase(begin()); }                        //�Ƴ�ͷ���
	void pop_back() {                                           //�Ƴ�β��� β����Լ�Ȼ�������Ԫ�ر��ͷ�
		iterator tmp = end();
		erase(--tmp);                                           
	}
	void remove(const T& x);                                    //��ֵΪx������Ԫ��ɾ��
	void unique();                                              //�Ƴ���ֵ��ͬ������Ԫ����ֻʣһ��



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
		if (*first == x) earse(first);            //earse�ͷ���node������iterator��iterator�ı䣬����ʱfirst����ĵ�ַΪ��Ч��ַ 
		first = next;                             //��next��first��ֵ�ĺ���Ϊʹfisrtָ��ͬһ����ַ��&first��&next�������������ǲ���ı��	                                    
	}
}

template<typename T, typename Alloc>
inline void list<T, Alloc>::unique() {
	iterator first = begin();
	iterator last = end();
	if (first == last) return;                    //������
	iterator next = first;
	while ((++next) != next)
	{
		if (*first == *next)  erase(next);
		else first = next;
		next = first;
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
}