#pragma once
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



}