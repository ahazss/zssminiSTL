#pragma once
#include<iostream>
#include"zss_iterator.h"



namespace ZSS {


	//heap algorithms
	//STL中实现的是最大堆

	//push_heap实现
	template <typename RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		//此函数调用时，新元素已经被插入至容器底端
		__push_heap_aux(first, last, distance_type(first), value_type(first));
	}

	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
	{
		__push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
	}

	template <typename RandomAccessIterator, typename Distance, typename T>
	void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value)
	{
		//不可指定大小比较标准
		Distance parent = (holeIndex - 1) / 2;              //找出父结点
		while (holeIndex > topIndex && *(first + parent) < value)
		{
			//当尚未到达顶端，且父结点小于新值
			*(first + holeIndex) = *(first + parent);       //令洞的值为父结点值
			holeIndex = parent;                             //调整洞号，向上提升至父结点
			parent = (holeIndex - 1) / 2;                   //新洞的父结点
		}//持续至顶端，或满足heap的次序特性为止
		*(first + holeIndex) = value;                       //令洞值为新值，完成插入操作
	}

	//pop_heap实现
	template<typename RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		__pop_heap_aux(first, last, value_type(first));
	}

	template <typename RandomAccessIterator, typename T>
	inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*)
	{
		__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
		//pop操作的应该是容器的第一个元素，首先设定欲调整值为尾值，然后将首值调整至尾结点
		//重整[first,last-1]使之成为一个合格的heap
	}

	template <typename RandomAccessIterator, typename T, typename Distance>
	inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator result, T value, Distance*)
	{
		*result = *first;                //设定尾指为首值，则尾值即为欲求结果
										 //由客户端以底层容器的pop_back()取出尾值
		__adjust_heap(first, Distance(0), Distance(last - first), value);
		//以上欲调整heap，洞号为0(亦即树根处)，欲调整值为value(原尾值)
	}

	template <typename RandomAccessIterator, typename Distance, typename T>
	void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value)
	{
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2;                  //右子节点
		while (secondChild < len)
		{
			if (*(first + secondChild) < *(first + secondChild - 1))
				secondChild--;                                     //令较大子值为洞值
			//洞号下移
			if (value < *(first + secondChild))
			{
				*(first + holeIndex) = *(first + secondChild);
				holeIndex = secondChild;
				//找出新洞结点的右子节点
				secondChild = 2 * (secondChild + 1);
			}
			else
				break;
		}
		if (secondChild == len)
		{
			//没有右子节点，只有左子节点
			//令左子节点为洞值，再令洞号下移至左子节点处
			if (value < *(first + secondChild - 1))
			{
				*(first + holeIndex) = *(first + secondChild - 1);
				holeIndex = secondChild - 1;
			}
		}
		//将欲调整值填入当前的洞号内
		*(first + holeIndex) = value;
	}

	//sort_heap实现(堆排序)
	template <typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		//以下每执行一次pop_heap()，极大值都被放在尾端
		//扣除刚刚放置好的元素再执行一次pop_heap(),次极值又被放在新尾端。一直进行下去，最后即得排序结果
		while (last - first > 1)
			pop_heap(first, last--);             //每执行pop_heap一次，操作范围即缩一格
	}

	//make_heap实现
	//将[first,last)排列为一个heap
	template <typename RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		__make_heap(first, last, value_type(first), distance_type(first));
	}

	template <typename RandomAccessIterator, typename T, typename Distance>
	void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*)
	{
		if (last - first < 2) return;             //长度为0或1则不用重新排列
		Distance len = last - first;
		//找出第一个需要重排的子树头部，以parent标示出
		Distance parent = (len - 2) / 2;

		while (true)
		{
			//重排以parent为首的子树，len是为了让__adjust_heap()判断操作范围
			__adjust_heap(first, parent, len, T(*(first + parent)));
			if (parent == 0) return;              //走完根节点即结束
			parent--;                             //(即将重排子树的)头部向前一个结点
		}
	}



	//accumulate 两个版本
	//不带第三参数时为累加,BinaryOperation为二元操作符
	template <typename InputIterator, typename T>
	T accumulate(InputIterator first, InputIterator last, T init)
	{
		for (; first != last; ++first)
			init = init + *first;
		return init;
	}

	template <typename InputIterator, typename T, typename BinaryOperation>
	T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
	{
		for (; first != last; ++first)
			init = binary_op(init, *first);   //对每一个元素执行二元操作
		return init;
	}

	//adjacent不带第三参数时，用来计算[first,last)中相邻元素的差值
	template <typename InputIterator, typename OutputIterator>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result)
	{
		if (first == last) return result;
		*result = *first;                   //先记录第一个元素
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //遍历整个区间
			typename iterator_traits<InputIterator>::value_type tmp = *first;
			*++result = tmp - value;
			value = tmp;
		}
		return ++result;
	}

	template<typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op)
	{
		if (first == last) return result;
		*result = *first;                   //先记录第一个元素
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //遍历整个区间
			typename iterator_traits<InputIterator>::value_type tmp = *first;
			*++result = binary_op(tmp,value);
			value = tmp;
		}
		return ++result;
	}

	template <typename InputIterator1, typename InputIterator2, typename T>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init)
	{
		//以第一序列之元素个数为依据，将两个序列都走一遍
		for (; first1 != last1; ++first1, ++first2)
			init = init + (*first1*(*first2));         //执行两个序列的一般内积
		return init;
	}

	template <typename InputIterator1, typename InputIterator2, typename T,
	          typename BinaryOperation1, typename BinaryOperation2>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
	                BinaryOperation1 binary_op1,BinaryOperation2 binary_op2)
	{
		//以第一序列之元素个数为依据，将两个序列都走一遍
		for (; first1 != last1; ++first1, ++first2)
			init = binary_op1(init, binary_op2(*first1, *first2));         
		//以外界提供的仿函数来取代第一版本中的operator*和operator+
		return init;
	}


	//partial_sum用于计算局部总和
	template <typename InputIterator, typename OutputIterator>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result)
	{
		if (first == last) return result;
		*result = *first;
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //遍历整个区间
			value = value + *first;         //前n个元素的总和
			*++result = value;              //指定给目的端
		}
		return ++result;                    //返回输出区间最尾端位置
	}

	template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result,BinaryOperation binary_op)
	{
		if (first == last) return result;
		*result = *first;
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //遍历整个区间
			value = binary_op(value,*first);
			*++result = value;
		}
		return ++result;
	}

	/*
	template<typename T,typename Integer>
	inline T power(T x, Integer n) {
		return power(x, n, (std::multiplies<T>()));
	}

	//版本2，幂次方，若指定为乘法运算，则当n>=0时返回1x^n
	template<typename T,typename Integer,typename MonoidOperation>
	T power(T x, Integer n, MonoidOperation op) {
		if (n == 0)
			return 1;               //取出证同元素   ？？？？
		else {
			while ((n & 1) == 0) {    //n为偶数
				n >> 1;
				x = op(x, x);
			}
			T result = x;
			n >>= 1;
			while (n != 0) {
				x = op(x, x);
				if ((n & 1) != 0)
					result = op(result, x);
				n >>= 1;
			}
			return result;
		}
	}*/

	//mismatch，平行比较两个序列，指出两者的第一个不匹配点，返回一对迭代器，分别指向两序列中的不匹配点
	template <typename InputIterator1, typename InputIterator2>
	std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
		//以下，如果序列1走完，就结束，如果序列一和序列二的对应元素相等，就结束
		//序列1的元素必须多于2的元素，否则结果无可预期
		while (first1 != last1 && *first1 == *first2) {
			++first1;
			++first2;
		}
		return std::pair<InputIterator1, InputIterator2>(first1, first2);
	}

	template <typename InputIterator1, typename InputIterator2,typename BinaryPredicate>
	std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,BinaryPredicate binary_pred) {
		//允许用户自定义比较操作
		while (first1 != last1 && binary_pred(*first1, *first2)) {
			++first1;
			++first2;
		}
		return std::pair<InputIterator1, InputIterator2>(first1, first2);
	}

	//equal 若两个序列在[first,last)区间内相等，返回true
	//若第二序列的元素比较多，多出来的元素不予考虑，所以调用equal前，先判断两个序列元素个数是否相等
	template<typename InputIterator1, typename InputIterator2>
	inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) 
	{
		for (; first1 != last1; ++first1, ++first2)
			if (*first1 != *first2)
				return false;
		return true;
	}

	template<typename InputIterator1, typename InputIterator2,typename BinaryPredicate>
	inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,BinaryPredicate binary_pred)
	{
		for (; first1 != last1; ++first1, first2)
			if (!binary_pred(*first1, *first2))
				return false;
		return true;
	}

	template <typename ForwardIterator,typename T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value) {
		for (; first != last; ++first) {       //迭代走过整个区间
			*first = value;                    //设定新值
		}
	}

	template <typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
		for (; n > 0; --n, ++first)            //迭代走过整个区间
			*first = value;                    //设定新值
		return first;
	}

	template<typename OutputIterator,typename Comp>
	void for_each(OutputIterator first, OutputIterator last, Comp comp) {
		for (; first != last; ++first) {
			comp(*first);
		}
	}

	template<typename ForwardIterator1,typename ForwardIterator2>
	inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
		//必须知道类型，否则无法创建tmp变量
		typename iterator_traits<ForwardIterator1>::value_type tmp = *a;
		*a = *b;
		*b = tmp;
	}

	template<typename T>
	inline const T& max(const T& a, const T& b) {
		return a < b ? b : a;
	}

	template<typename T,typename Compare>
	inline const T& max(const T& a, const T& b,Compare comp) {
		return comp(a, b) ? b : a;
	}

	template<typename T>
	inline const T& min(const T& a, const T& b) {
		return a > b ? b : a;
	}

	template<typename T, typename Compare>
	inline const T& min(const T& a, const T& b, Compare comp) {
		return comp(b, a) ? b : a;
	}

	template<typename T>
	inline void swap(T& a, T& b) {
		T tmp = a;
		a = b;
		b = tmp;
	}

	//copy 更改的是[result,result+last-first)中迭代器所指的对象，而非更改迭代器本身
	//会为输出区间的元素赋予新值，不可改变迭代器本身
	//完全泛化版本
	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
		//__copy_dispatch有一个完全泛化版本和两个偏特化版本
	}

	//多载函数，针对原生指针(可视为一种特殊的迭代器)const char* 和const wchar_t*，进行内存直接拷贝操作
	inline char* copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, sizeof(wchar_t)*(last - first));
		return result + (last - first);
	}

	template<typename InputIterator, typename OutputIterator>
	struct __copy_dispatch
	{
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return __copy(first, last, result, iterator_category(first));
			//完全泛化版根据迭代器种类的不同，调用了不同的__copy(),为的是不同种类的迭代器所使用的循环条件不同，有快慢之分
		}
	};

	template<typename T>
	struct __copy_dispatch<T*, T*>
	{
		T* operator()(T* first, T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;    //据是否有trivial_assignment_operator来调用不同函数
			return __copy_t(first, last, result, t());
		}
	};

	template<typename T>
	struct __copy_dispatch<const T*, T*>
	{
		T* operator()(const T* first,const T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};

	//__copy()(根据迭代器种类不同进行分类)
	//InputIterator版本,Input_iterator_tag用于进行重载调用相应函数
	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//以迭代器是否相同，决定循环是否继续，速度慢
		for (; first != last; ++result, ++first)
			*result = *first;
		return result;
	}

	//RandomAccessIterator 版本
	template<typename RandomAccessIterator, typename OutputIterator>
	inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
	{
		return __copy_d(first, last, result, distance_type(first));
	}

	template<typename RandomAccessIterator, typename OutputIterator,typename Distance>
	inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; --n, ++result, ++first)
			*result = *first;
		return result;
	}

	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __true_type)
	{
		//有trivial
		memmove(result, first, sizeof(T)*(last - first));
		return result + (last - first);
	}

	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
	{
		//有trivial,原生指针是一种RandomAccessIterator 所以交给__copy_d()完成
		return _copy_d(first, last, result, (ptrdiff_t*)0);
	}
}