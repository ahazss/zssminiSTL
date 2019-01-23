#pragma once
#include"zss_iterator.h"


namespace ZSS {



	//heap algorithms
	//STL��ʵ�ֵ�������

	//push_heapʵ��
	template <typename RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		//�˺�������ʱ����Ԫ���Ѿ��������������׶�
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
		//����ָ����С�Ƚϱ�׼
		Distance parent = (holeIndex - 1) / 2;              //�ҳ������
		while (holeIndex > topIndex && *(first + parent) < value)
		{
			//����δ���ﶥ�ˣ��Ҹ����С����ֵ
			*(first + holeIndex) = *(first + parent);       //���ֵΪ�����ֵ
			holeIndex = parent;                             //�������ţ����������������
			parent = (holeIndex - 1) / 2;                   //�¶��ĸ����
		}//���������ˣ�������heap�Ĵ�������Ϊֹ
		*(first + holeIndex) = value;                       //�ֵΪ��ֵ����ɲ������
	}

	//pop_heapʵ��
	template<typename RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		__pop_heap_aux(first, last, value_type(first));
	}

	template <typename RandomAccessIterator, typename T>
	inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*)
	{
		__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
		//pop������Ӧ���������ĵ�һ��Ԫ�أ������趨������ֵΪβֵ��Ȼ����ֵ������β���
		//����[first,last-1]ʹ֮��Ϊһ���ϸ��heap
	}

	template <typename RandomAccessIterator, typename T, typename Distance>
	inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator result, T value, Distance*)
	{
		*result = *first;                //�趨βָΪ��ֵ����βֵ��Ϊ������
										 //�ɿͻ����Եײ�������pop_back()ȡ��βֵ
		__adjust_heap(first, Distance(0), Distance(last - first), value);
		//����������heap������Ϊ0(�༴������)��������ֵΪvalue(ԭβֵ)
	}

	template <typename RandomAccessIterator, typename Distance, typename T>
	void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value)
	{
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2;                  //���ӽڵ�
		while (secondChild < len)
		{
			if (*(first + secondChild) < *(first + secondChild - 1))
				secondChild--;                                     //��ϴ���ֵΪ��ֵ
			//��������
			if (value < *(first + secondChild))
			{
				*(first + holeIndex) = *(first + secondChild);
				holeIndex = secondChild;
				//�ҳ��¶��������ӽڵ�
				secondChild = 2 * (secondChild + 1);
			}
			else
				break;
		}
		if (secondChild == len)
		{
			//û�����ӽڵ㣬ֻ�����ӽڵ�
			//�����ӽڵ�Ϊ��ֵ����������������ӽڵ㴦
			if (value < *(first + secondChild - 1))
			{
				*(first + holeIndex) = *(first + secondChild - 1);
				holeIndex = secondChild - 1;
			}
		}
		//��������ֵ���뵱ǰ�Ķ�����
		*(first + holeIndex) = value;
	}

	//sort_heapʵ��(������)
	template <typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		//����ÿִ��һ��pop_heap()������ֵ��������β��
		//�۳��ոշ��úõ�Ԫ����ִ��һ��pop_heap(),�μ�ֵ�ֱ�������β�ˡ�һֱ������ȥ����󼴵�������
		while (last - first > 1)
			pop_heap(first, last--);             //ÿִ��pop_heapһ�Σ�������Χ����һ��
	}

	//make_heapʵ��
	//��[first,last)����Ϊһ��heap
	template <typename RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		__make_heap(first, last, value_type(first), distance_type(first));
	}

	template <typename RandomAccessIterator, typename T, typename Distance>
	void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*)
	{
		if (last - first < 2) return;             //����Ϊ0��1������������
		Distance len = last - first;
		//�ҳ���һ����Ҫ���ŵ�����ͷ������parent��ʾ��
		Distance parent = (len - 2) / 2;

		while (true)
		{
			//������parentΪ�׵�������len��Ϊ����__adjust_heap()�жϲ�����Χ
			__adjust_heap(first, parent, len, T(*(first + parent)));
			if (parent == 0) return;              //������ڵ㼴����
			parent--;                             //(��������������)ͷ����ǰһ�����
		}
	}



}