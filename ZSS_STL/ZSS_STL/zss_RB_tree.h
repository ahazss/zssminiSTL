#pragma once
#include"zss_iterator.h"
#include"zss_alloc.h"
#include"zss_construct.h"

namespace ZSS {

	typedef bool __rb_tree_color_type;
	const __rb_tree_color_type __rb_tree_red = false;         //��ɫΪ0
	const __rb_tree_color_type __rb_tree_black = true;        //��ɫΪ1

	struct __rb_tree_node_base
	{
		typedef __rb_tree_color_type color_type;
		typedef __rb_tree_node_base* base_ptr;

		color_type color;           //�����ɫ���Ǻ켴��
		base_ptr parent;            //RBtree������������֪������㣬ָ�򸸽���ָ��
		base_ptr left;              //ָ������ָ��
		base_ptr right;             //ָ���ҽ��ָ��

		static base_ptr minimum(base_ptr x)
		{
			while (x->left != nullptr) x = x->left;         //һֱ�����ߣ��ͻ��ҵ���Сֵ
			return x;
		}

		static base_ptr maximum(base_ptr x)
		{
			while (x->right != 0) x = x->right;       //һֱ�����ߣ��ͻ��ҵ����ֵ
			return x;
		}
	};

	template <typename Value>                         //RB_tree_node_base���������޹ض�node���������йأ�˫��
	struct __rb_tree_node :public __rb_tree_node_base
	{
		typedef __rb_tree_node<Value>* link_type;
		Value value_field;                            //���ֵ
	};

	//ȫ�ֺ���
	//�½ڵ�Ϊ�죬�����봦�ĸ����ҲΪ�죬���������ת
	inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
	{
		//xΪ��ת��
		__rb_tree_node_base* y = x->right;   //��yΪ��ת������ӽڵ�
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;             //�ǵ�����parent
		y->parent = x->parent;
		//��y��ȫ����x��λ��
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}

	inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
	{
		//xΪ��ת��
		__rb_tree_node_base* y = x->left;   //��yΪ��ת������ӽڵ�
		x->left = y->right;
		if (y->right != nullptr)
			y->right->parent = x;             //�ǵ�����parent
		y->parent = x->parent;
		//��y��ȫ����x��λ��
		if (x == root)
			root = y;
		else if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}


	//ȫ�ֺ���
	//����������ƽ��(�ı���ɫ����ת����)
	//����1Ϊ�����ڵ㣬����2Ϊroot
	inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
	{
		x->color = __rb_tree_red;     //�½ڵ��Ϊ��
		while (x != root && x->parent->color == __rb_tree_red)    //x��Ϊ���ڵ���x�ĸ��ڵ�Ϊ��ɫ
		{
			if (x->parent == x->parent->parent->left)     //���ڵ�Ϊ�游�ڵ�����ӽڵ�
			{
				__rb_tree_node_base* y = x->parent->parent->right;      //yΪ���ڵ���ֵܽڵ�
				if (y&&y->color == __rb_tree_red) {            //y������yΪ��ɫ
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {                                    //�޲����ڵ㣬�򲮸��ڵ�Ϊ��
					if (x == x->parent->right) {          //����½ڵ�Ϊ���ڵ�����ӽڵ�
						x = x->parent;
						__rb_tree_rotate_left(x, root);   //��һ����Ϊ������
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else {      //�����Ϊ�游�ڵ�����ӽڵ�
				__rb_tree_node_base* y = x->parent->parent->left;
				if (y&&y->color == __rb_tree_red) {            //y������yΪ��ɫ
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {                                    //�޲����ڵ㣬�򲮸��ڵ�Ϊ��
					if (x == x->parent->left) {          //����½ڵ�Ϊ���ڵ�����ӽڵ�
						x = x->parent;
						__rb_tree_rotate_right(x, root);   //��һ����Ϊ������
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}
		root->color = __rb_tree_black;
	}

	struct __rb_tree_base_iterator
	{
		typedef __rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;        //�������ͱ�Ϊ˫�������
		typedef ptrdiff_t difference_type;
		base_ptr node;                                //�����������������ӹ�ϵ
		void increment() {
			//��operator++()����(����)
			if (node->right != nullptr) {             //�����ҽڵ�,��Ϊ�������µĵ�һ�����
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			else {                                    //û���ҽڵ�
				base_ptr y = node->parent;            //�ҳ����ڵ�
				while (node == y->right) {            //�����н�㱾��Ϊ���ӽڵ�
					node = y;
					y = y->parent;                    //һֱ���ݣ�ֱ����Ϊ���ӽڵ�Ϊֹ
				}
				if (node->right != y)                 //��ʱ�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ�
					node = y;                         //��Ӧ����������ڵ������ӽڵ�
			}
		}
		void decrement() {
			//��operator--()����
			if (node->color == __rb_tree_red && node->parent->parent == node)                    //���ڵ�ĸ��������Լ���������������
				node = node->right;
			//�������������nodeΪheaderʱ(�༴nodeΪend()ʱ)
			else if (node->left != nullptr) {          //�������ӽڵ�ʱ
				base_ptr y = node->left;
				while (y->right != nullptr)
					y = y->right;
				node = y;
			}
			else {                                     //�ȷǸ��ڵ㣬�������ӽڵ�
				base_ptr y = node->parent;
				while (y->left == node) {              //�����н��Ϊ���ӽڵ�
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}
	};

	//RB-tree�����������
	template<typename Value, typename Ref, typename Ptr>
	struct __rb_tree_iterator :public __rb_tree_base_iterator
	{
		typedef Value value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef __rb_tree_iterator<Value, Value&, Value*>  iterator;
		typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
		typedef __rb_tree_iterator<Value, Ref, Ptr> self;
		typedef __rb_tree_node<Value>* link_type;
        

		__rb_tree_iterator() {}
		__rb_tree_iterator(link_type x) { node = x; }               //node�Ǵ�base_iterator�м̳�����
		__rb_tree_iterator(const iterator& it) { node = it.node; }
		
		reference operator*() const { 
			return ((link_type)(node))->value_field; }
		pointer operator->() const { return &(operator*()); }
		bool operator==(const self& a) const {  return this->node == a.node; }
		bool operator!=(const self& a) const {  return !(*this == a); }

		self& operator++() { increment(); return *this; }
		self operator++(int) {
			self tmp = *this;
			increment();
			return tmp;
		}
		self& operator--() { decrement(); return *this; }
		self& operator--(int) {
			self tmp = *this;
			decrement();
			return tmp;
		}
	};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc = alloc>
	class rb_tree {
	protected:
		typedef void* void_pointer;
		typedef __rb_tree_node_base* base_ptr;
		typedef __rb_tree_node<Value> rb_tree_node;
		typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
		typedef __rb_tree_color_type color_type;
	public:
		typedef Key key_value;
		typedef Value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef rb_tree_node* link_type;
		typedef size_t size_type;
		typedef ptrdiff_t differencr_type;
	protected:
		//�ڵ��ڴ����
		link_type get_node() { return rb_tree_node_allocator::allocate(); }
		void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

		link_type create_node(const value_type& x) {
			link_type tmp = get_node();               //���ÿռ�
			construct(&tmp->value_field, x);          //��������
			return tmp;
		}
		link_type clone_node(link_type x) {           //����һ���ڵ�
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = nullptr;
			tmp->right = nullptr;
			return tmp;
		}
		void destroy_node(link_type p) {
			destroy(&p->value_field);                 //��������
			put_node(p);                              //�ͷ��ڴ�
		}

	protected:
		size_type node_count;                         //׷�ټ�¼���Ĵ�С(�ڵ�����)
		link_type header;                             //header��Ϊ�˼򻯶Ը��ڵ�Ĵ���header��root��Ϊ�Է��ĸ����
		Compare key_compare;                          //�ڵ��ļ�ֵ��С�Ƚ�׼��function object

		//����������������ȡ��header�ĳ�Ա
		link_type& root() const { return (link_type&)header->parent; }
		link_type& leftmost() const { return (link_type&)header->left; }
		link_type& rightmost() const { return (link_type&)header->right; }

		//����6����������ȡ�ýڵ�x�ĳ�Ա
		static link_type& left(link_type x)
		{
			return (link_type&)(x->left);
		}
		static link_type& right(link_type x)
		{
			return (link_type&)(x->right);
		}
		static link_type& parent(link_type x)
		{
			return (link_type&)(x->parent);
		}
		static reference value(link_type x)
		{
			return x->value_field;
		}
		static const Key& key(link_type x)
		{
			return KeyOfValue()(value(x));
		}
		static color_type& color(link_type x)
		{
			return (color_type&)(x->color);
		}

		//��ȡ����ֵ��Сֵ
		static link_type minimum(link_type x) {
			return (link_type)__rb_tree_node_base::minimum(x);
		}
		static link_type maximum(link_type x) {
			return (link_type)__rb_tree_node_base::maximum(x);
		}

	public:
		typedef __rb_tree_iterator<value_type, reference, pointer> iterator;

	private:
		iterator __insert(base_ptr x, base_ptr y, const  value_type& v);
		link_type __copy(link_type x, link_type p);
		void __erase(link_type x);
		iterator find(const Key& k);
		//��ʼ��header
		void init() {
			header = get_node();
			color(header) = __rb_tree_red;          //headerΪ��ɫ������header��root
			root() = nullptr;
			leftmost() = header;
			rightmost() = header;
		}

	public:
		rb_tree(const Compare& comp = Compare())
			:node_count(0), key_compare(comp) {
			init();
		}
		rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x)
			:node_count(0), key_compare(x.key_compare) //�������캯�� 
		{
			if (x.root() == 0)
				init();
			else {
				color(header) = __rb_tree_red;
				root() = __copy(x.root(), header);
				leftmost() = minimum(root());
				rightmost() = maximum(root());
			}
			node_count = x.node_count;
		}

		~rb_tree() {
			clear();
			put_node(header);                  //value_fieldΪ�գ�����Ҫ��������
		}

		//rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&  
			//operator=(const m& x);

		Compare key_comp() const { return key_compare; }
		iterator begin() { return leftmost(); }                 //rb����ʼΪ����(��С)�ڵ㴦
		iterator end() { return header; }                     //rb���յ�Ϊheader��ָ��
		bool empty() const { return node_count == 0; }
		void clear() {
			iterator it = end();
			while (node_count != 0) {
				--it;
				destroy_node((link_type)it.node);
				node_count--;
			}
		}
		size_type size() const { return node_count; }
		size_type max_size() const { return size_type(-1); }

		//��x���뵽RB-tree��(���ֽڵ�ֵ��һ�޶�)
		std::pair<iterator, bool> insert_unique(const value_type& x);
		//��x���뵽RB-tree��(����ڵ�ֵ�ظ�)
		iterator insert_equal(const value_type& x);

		void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
			link_type temp = header;
			header = x.header;
			x.header = temp;
		}
	};

	//������ֵ����ֵ�����ظ�������ֵΪһ��rb-tree��������ָ�������ڵ�
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v) {
		link_type y = header;
		link_type x = root();          //�Ӹ��ڵ㿪ʼ
		while (x != nullptr) {
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
			//���ڵ�ֵ���������㣬����Ϊ�ҽڵ�
		}
		return __insert(x, y, v);
		//xΪ�²����㣬yΪ�����ĸ���㣬vΪ��ֵ
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v) {
		link_type y = header;
		link_type x = root();          //�Ӹ��ڵ㿪ʼ
		bool comp = true;
		while (x != nullptr) {
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));     //v��ֵС��Ŀǰ�ڵ�ļ�ֵ
			x = comp ? left(x) : right(x);                   //������������"С�ڻ����"������
		}
		//��ʱy��ָΪ�����ĸ����(��ΪҶ�ڵ�)
		iterator j = iterator(y);                            //�������jָ������ĸ����y
		if (comp) {                                          //�뿪whileѭ��ʱcompΪ�棬���󽫲������
			if (j == begin())                                //��������ĸ��ڵ�Ϊ������(����������Ƚڵ�)
				return std::pair<iterator, bool>(__insert(x, y, v), true);
			else                                             //�����֮����㲻Ϊ����ڵ�,����j����ͷ׼������
				--j;
		}
		if (key_compare(key(link_type(j.node)), KeyOfValue()(v)))       //С����ֵ���������Ҳ�(����������Ƚڵ�)
			return std::pair<iterator, bool>(__insert(x, y, v), true);

		//���������ˣ���ʾ��ֵһ�������м�ֵ�ظ����򲻸ý��в���
		return std::pair<iterator, bool>(j, false);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const Value& v) {
		//x_Ϊ��ֵ����㣬����y_Ϊ�����֮����㣬����vΪ��ֵ
		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;

		//key_compare �Ǽ�ֵ��С�Ƚ�׼���Ǹ�function object
		if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;               //��y����ڵ���Ϊx,��ʹ�õ�yΪheaderʱ��leftmost()=z
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())     //���yΪ����ڵ�
				leftmost() = z;           //ά��leftmost��ʹ����Զָ������ڵ�
		}
		else {
			z = create_node(v);
			right(y) = z;             //���²���ڵ�Ϊ�����ĸ��ڵ�y�����ҽڵ�
			if (y == rightmost())
				rightmost() == z;       //ά�����ҽڵ�
		}
		parent(z) = y;
		left(z) = nullptr;
		right(z) = nullptr;
		//�½ڵ����ɫ����__rb_tree_rebalance()�趨������
		__rb_tree_rebalance(z, header->parent);   //����1Ϊ�����ڵ㣬����2Ϊroot
		++node_count;
		return iterator(z);           //����һ��ָ�������ڵ�ĵ�����
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k)
	{
		link_type y = header;
		link_type x = root();
		while (x != 0)
			if (!key_compare(key(x), k)) {    //x��ֵ����kʱ��������
				y = x; x = left(x);
			}
			else 
				x = right(x);
		iterator j = iterator(y);
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}

	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type     // __copy(x.root(), header); ����Ϊ(�ӽڵ㣬�����)
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) 
	{      
		//xΪroot  pΪheader(�Լ���)											
		link_type top = clone_node(x);
		top->parent = p;

		if (x->right)
			top->right = __copy(right(x), top);        //�ݹ��㷨
		p = top;
		x = left(x);

		while (x != 0) {
			link_type y = clone_node(x);
			p->left = y;              //ĿǰΪֹ p���������� �� ���ڵ㶼�Ѿ��������
			y->parent = p;          //�������ĸ��ڵ㸳ֵ
			if (x->right)           //���������ҽڵ㲻Ϊ��
				y->right = __copy(right(x), y);     //�ݹ��Ϊ���������ҽڵ㸳ֵ
			p = y;             // ����һֱѭ��  xΪ��ǰ�ڵ�  pΪ���ڵ�
			x = left(x);
		}
		return top;
	}
}
