#pragma once
#include<iostream>
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



	//accumulate �����汾
	//������������ʱΪ�ۼ�,BinaryOperationΪ��Ԫ������
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
			init = binary_op(init, *first);   //��ÿһ��Ԫ��ִ�ж�Ԫ����
		return init;
	}

	//adjacent������������ʱ����������[first,last)������Ԫ�صĲ�ֵ
	template <typename InputIterator, typename OutputIterator>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result)
	{
		if (first == last) return result;
		*result = *first;                   //�ȼ�¼��һ��Ԫ��
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //������������
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
		*result = *first;                   //�ȼ�¼��һ��Ԫ��
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //������������
			typename iterator_traits<InputIterator>::value_type tmp = *first;
			*++result = binary_op(tmp,value);
			value = tmp;
		}
		return ++result;
	}

	template <typename InputIterator1, typename InputIterator2, typename T>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init)
	{
		//�Ե�һ����֮Ԫ�ظ���Ϊ���ݣ����������ж���һ��
		for (; first1 != last1; ++first1, ++first2)
			init = init + (*first1*(*first2));         //ִ���������е�һ���ڻ�
		return init;
	}

	template <typename InputIterator1, typename InputIterator2, typename T,
	          typename BinaryOperation1, typename BinaryOperation2>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
	                BinaryOperation1 binary_op1,BinaryOperation2 binary_op2)
	{
		//�Ե�һ����֮Ԫ�ظ���Ϊ���ݣ����������ж���һ��
		for (; first1 != last1; ++first1, ++first2)
			init = binary_op1(init, binary_op2(*first1, *first2));         
		//������ṩ�ķº�����ȡ����һ�汾�е�operator*��operator+
		return init;
	}


	//partial_sum���ڼ���ֲ��ܺ�
	template <typename InputIterator, typename OutputIterator>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result)
	{
		if (first == last) return result;
		*result = *first;
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //������������
			value = value + *first;         //ǰn��Ԫ�ص��ܺ�
			*++result = value;              //ָ����Ŀ�Ķ�
		}
		return ++result;                    //�������������β��λ��
	}

	template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result,BinaryOperation binary_op)
	{
		if (first == last) return result;
		*result = *first;
		typename iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {           //������������
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

	//�汾2���ݴη�����ָ��Ϊ�˷����㣬��n>=0ʱ����1x^n
	template<typename T,typename Integer,typename MonoidOperation>
	T power(T x, Integer n, MonoidOperation op) {
		if (n == 0)
			return 1;               //ȡ��֤ͬԪ��   ��������
		else {
			while ((n & 1) == 0) {    //nΪż��
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

	//mismatch��ƽ�бȽ��������У�ָ�����ߵĵ�һ����ƥ��㣬����һ�Ե��������ֱ�ָ���������еĲ�ƥ���
	template <typename InputIterator1, typename InputIterator2>
	std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
		//���£��������1���꣬�ͽ������������һ�����ж��Ķ�ӦԪ����ȣ��ͽ���
		//����1��Ԫ�ر������2��Ԫ�أ��������޿�Ԥ��
		while (first1 != last1 && *first1 == *first2) {
			++first1;
			++first2;
		}
		return std::pair<InputIterator1, InputIterator2>(first1, first2);
	}

	template <typename InputIterator1, typename InputIterator2,typename BinaryPredicate>
	std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,BinaryPredicate binary_pred) {
		//�����û��Զ���Ƚϲ���
		while (first1 != last1 && binary_pred(*first1, *first2)) {
			++first1;
			++first2;
		}
		return std::pair<InputIterator1, InputIterator2>(first1, first2);
	}

	//equal ������������[first,last)��������ȣ�����true
	//���ڶ����е�Ԫ�رȽ϶࣬�������Ԫ�ز��迼�ǣ����Ե���equalǰ�����ж���������Ԫ�ظ����Ƿ����
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
		for (; first != last; ++first) {       //�����߹���������
			*first = value;                    //�趨��ֵ
		}
	}

	template <typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
		for (; n > 0; --n, ++first)            //�����߹���������
			*first = value;                    //�趨��ֵ
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
		//����֪�����ͣ������޷�����tmp����
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

	//copy ���ĵ���[result,result+last-first)�е�������ָ�Ķ��󣬶��Ǹ��ĵ���������
	//��Ϊ��������Ԫ�ظ�����ֵ�����ɸı����������
	//��ȫ�����汾
	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
		//__copy_dispatch��һ����ȫ�����汾������ƫ�ػ��汾
	}

	//���غ��������ԭ��ָ��(����Ϊһ������ĵ�����)const char* ��const wchar_t*�������ڴ�ֱ�ӿ�������
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
			//��ȫ��������ݵ���������Ĳ�ͬ�������˲�ͬ��__copy(),Ϊ���ǲ�ͬ����ĵ�������ʹ�õ�ѭ��������ͬ���п���֮��
		}
	};

	template<typename T>
	struct __copy_dispatch<T*, T*>
	{
		T* operator()(T* first, T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;    //���Ƿ���trivial_assignment_operator�����ò�ͬ����
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

	//__copy()(���ݵ��������಻ͬ���з���)
	//InputIterator�汾,Input_iterator_tag���ڽ������ص�����Ӧ����
	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//�Ե������Ƿ���ͬ������ѭ���Ƿ�������ٶ���
		for (; first != last; ++result, ++first)
			*result = *first;
		return result;
	}

	//RandomAccessIterator �汾
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
		//��trivial
		memmove(result, first, sizeof(T)*(last - first));
		return result + (last - first);
	}

	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
	{
		//��trivial,ԭ��ָ����һ��RandomAccessIterator ���Խ���__copy_d()���
		return _copy_d(first, last, result, (ptrdiff_t*)0);
	}
}