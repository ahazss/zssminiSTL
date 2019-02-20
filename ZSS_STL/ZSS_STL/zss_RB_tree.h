#pragma once
#include"zss_iterator.h"
#include"zss_alloc.h"
#include"zss_construct.h"

namespace ZSS {

	typedef bool __rb_tree_color_type;
	const __rb_tree_color_type __rb_tree_red = false;         //红色为0
	const __rb_tree_color_type __rb_tree_black = true;        //黑色为1

	struct __rb_tree_node_base
	{
		typedef __rb_tree_color_type color_type;
		typedef __rb_tree_node_base* base_ptr;

		color_type color;           //结点颜色，非红即黑
		base_ptr parent;            //RBtree的许多操作必须知道父结点，指向父结点的指针
		base_ptr left;              //指向左结点指针
		base_ptr right;             //指向右结点指针

		static base_ptr minimum(base_ptr x)
		{
			while (x->left != nullptr) x = x->left;         //一直向左走，就会找到最小值
			return x;
		}

		static base_ptr maximum(base_ptr x)
		{
			while (x->right != 0) x = x->right;       //一直向右走，就会找到最大值
			return x;
		}
	};

	template <typename Value>                         //RB_tree_node_base与结点类型无关而node与结点类型有关，双层
	struct __rb_tree_node :public __rb_tree_node_base
	{
		typedef __rb_tree_node<Value>* link_type;
		Value value_field;                            //结点值
	};

	//全局函数
	//新节点为红，若插入处的父结点也为红，则需进行旋转
	inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
	{
		//x为旋转点
		__rb_tree_node_base* y = x->right;   //令y为旋转点的右子节点
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;             //记得设置parent
		y->parent = x->parent;
		//令y完全顶替x的位置
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
		//x为旋转点
		__rb_tree_node_base* y = x->left;   //令y为旋转点的左子节点
		x->left = y->right;
		if (y->right != nullptr)
			y->right->parent = x;             //记得设置parent
		y->parent = x->parent;
		//令y完全顶替x的位置
		if (x == root)
			root = y;
		else if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}


	//全局函数
	//重新令树形平衡(改变颜色及旋转树形)
	//参数1为新增节点，参数2为root
	inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
	{
		x->color = __rb_tree_red;     //新节点必为红
		while (x != root && x->parent->color == __rb_tree_red)    //x不为根节点且x的父节点为红色
		{
			if (x->parent == x->parent->parent->left)     //父节点为祖父节点的左子节点
			{
				__rb_tree_node_base* y = x->parent->parent->right;      //y为父节点的兄弟节点
				if (y&&y->color == __rb_tree_red) {            //y存在且y为红色
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {                                    //无伯父节点，或伯父节点为黑
					if (x == x->parent->right) {          //如果新节点为父节点的右子节点
						x = x->parent;
						__rb_tree_rotate_left(x, root);   //第一参数为左旋点
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else {      //父结点为祖父节点的右子节点
				__rb_tree_node_base* y = x->parent->parent->left;
				if (y&&y->color == __rb_tree_red) {            //y存在且y为红色
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {                                    //无伯父节点，或伯父节点为黑
					if (x == x->parent->left) {          //如果新节点为父节点的右子节点
						x = x->parent;
						__rb_tree_rotate_right(x, root);   //第一参数为左旋点
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
		typedef bidirectional_iterator_tag iterator_category;        //迭代器型别为双向迭代器
		typedef ptrdiff_t difference_type;
		base_ptr node;                                //用于与容器产生连接关系
		void increment() {
			//被operator++()调用(中序)
			if (node->right != nullptr) {             //若有右节点,则为右子树下的第一个结点
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			else {                                    //没有右节点
				base_ptr y = node->parent;            //找出父节点
				while (node == y->right) {            //若现行结点本身为右子节点
					node = y;
					y = y->parent;                    //一直上溯，直到不为右子节点为止
				}
				if (node->right != y)                 //此时的右子节点不等于此时的父节点
					node = y;                         //对应特殊情况根节点无右子节点
			}
		}
		void decrement() {
			//被operator--()调用
			if (node->color == __rb_tree_red && node->parent->parent == node)                    //父节点的父结点等于自己？？？？？？？
				node = node->right;
			//此种情况发生于node为header时(亦即node为end()时)
			else if (node->left != nullptr) {          //若有左子节点时
				base_ptr y = node->left;
				while (y->right != nullptr)
					y = y->right;
				node = y;
			}
			else {                                     //既非根节点，亦无左子节点
				base_ptr y = node->parent;
				while (y->left == node) {              //当现行结点为左子节点
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}
	};

	//RB-tree的正规迭代器
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
		__rb_tree_iterator(link_type x) { node = x; }               //node是从base_iterator中继承来的
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
		//节点内存管理
		link_type get_node() { return rb_tree_node_allocator::allocate(); }
		void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

		link_type create_node(const value_type& x) {
			link_type tmp = get_node();               //配置空间
			construct(&tmp->value_field, x);          //构造内容
			return tmp;
		}
		link_type clone_node(link_type x) {           //复制一个节点
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = nullptr;
			tmp->right = nullptr;
			return tmp;
		}
		void destroy_node(link_type p) {
			destroy(&p->value_field);                 //析构内容
			put_node(p);                              //释放内存
		}

	protected:
		size_type node_count;                         //追踪记录树的大小(节点数量)
		link_type header;                             //header是为了简化对根节点的处理，header和root互为对方的父结点
		Compare key_compare;                          //节点间的键值大小比较准则，function object

		//以下三个函数用于取得header的成员
		link_type& root() const { return (link_type&)header->parent; }
		link_type& leftmost() const { return (link_type&)header->left; }
		link_type& rightmost() const { return (link_type&)header->right; }

		//以下6个函数用于取得节点x的成员
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

		//求取极大值极小值
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
		//初始化header
		void init() {
			header = get_node();
			color(header) = __rb_tree_red;          //header为红色以区分header和root
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
			:node_count(0), key_compare(x.key_compare) //拷贝构造函数 
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
			put_node(header);                  //value_field为空，不需要析构内容
		}

		//rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&  
			//operator=(const m& x);

		Compare key_comp() const { return key_compare; }
		iterator begin() { return leftmost(); }                 //rb树起始为最左(最小)节点处
		iterator end() { return header; }                     //rb树终点为header所指处
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

		//将x插入到RB-tree中(保持节点值独一无二)
		std::pair<iterator, bool> insert_unique(const value_type& x);
		//将x插入到RB-tree中(允许节点值重复)
		iterator insert_equal(const value_type& x);

		void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
			link_type temp = header;
			header = x.header;
			x.header = temp;
		}
	};

	//插入新值，键值允许重复，返回值为一个rb-tree迭代器，指向新增节点
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v) {
		link_type y = header;
		link_type x = root();          //从根节点开始
		while (x != nullptr) {
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
			//若节点值大则向左结点，否则为右节点
		}
		return __insert(x, y, v);
		//x为新插入结点，y为插入点的父结点，v为新值
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v) {
		link_type y = header;
		link_type x = root();          //从根节点开始
		bool comp = true;
		while (x != nullptr) {
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));     //v键值小于目前节点的键值
			x = comp ? left(x) : right(x);                   //遇大则向左，遇"小于或等于"则向右
		}
		//此时y所指为插入点的父结点(必为叶节点)
		iterator j = iterator(y);                            //令迭代器j指向插入点的父结点y
		if (comp) {                                          //离开while循环时comp为真，遇大将插入左侧
			if (j == begin())                                //如果插入点的父节点为最左结点(不可能有相等节点)
				return std::pair<iterator, bool>(__insert(x, y, v), true);
			else                                             //插入点之父结点不为最左节点,调整j，回头准备测试
				--j;
		}
		if (key_compare(key(link_type(j.node)), KeyOfValue()(v)))       //小于新值，将插入右侧(不可能有相等节点)
			return std::pair<iterator, bool>(__insert(x, y, v), true);

		//若进行至此，表示新值一定与树中键值重复，则不该进行插入
		return std::pair<iterator, bool>(j, false);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const Value& v) {
		//x_为新值插入点，参数y_为插入点之父结点，参数v为新值
		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;

		//key_compare 是键值大小比较准则，是个function object
		if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;               //把y的左节点设为x,这使得当y为header时，leftmost()=z
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())     //如果y为最左节点
				leftmost() = z;           //维护leftmost，使他永远指向最左节点
		}
		else {
			z = create_node(v);
			right(y) = z;             //令新插入节点为插入点的父节点y的最右节点
			if (y == rightmost())
				rightmost() == z;       //维护最右节点
		}
		parent(z) = y;
		left(z) = nullptr;
		right(z) = nullptr;
		//新节点的颜色将在__rb_tree_rebalance()设定并调整
		__rb_tree_rebalance(z, header->parent);   //参数1为新增节点，参数2为root
		++node_count;
		return iterator(z);           //返回一个指向新增节点的迭代器
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k)
	{
		link_type y = header;
		link_type x = root();
		while (x != 0)
			if (!key_compare(key(x), k)) {    //x键值大于k时，向左走
				y = x; x = left(x);
			}
			else 
				x = right(x);
		iterator j = iterator(y);
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}

	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type     // __copy(x.root(), header); 广义为(子节点，父结点)
		inline rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) 
	{      
		//x为root  p为header(自己的)											
		link_type top = clone_node(x);
		top->parent = p;

		if (x->right)
			top->right = __copy(right(x), top);        //递归算法
		p = top;
		x = left(x);

		while (x != 0) {
			link_type y = clone_node(x);
			p->left = y;              //目前为止 p的左右子树 和 父节点都已经复制完毕
			y->parent = p;          //左子树的父节点赋值
			if (x->right)           //左子树的右节点不为空
				y->right = __copy(right(x), y);     //递归的为左子树的右节点赋值
			p = y;             // 保持一直循环  x为当前节点  p为父节点
			x = left(x);
		}
		return top;
	}
}
