#pragma once
#include<cstddef>

//五种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag :public input_iterator_tag {};    //继承有利于型别判断,消除单纯传递调用函数
struct bidirectional_iterator_tag :public forward_iterator_tag {};
struct random_access_iterator_tag :public bidirectional_iterator_tag {};

//为避免写代码时遗漏型别，迭代器最好继承std::iterator
template <typename Category, typename T,
	      typename Distance=ptrdiff_t, typename Pointer=T*, typename Reference =T&>
struct iterator {
	typedef Category  iterator_category;
	typedef T         value_type;
	typedef Distance  difference_type;
	typedef Pointer   pointer;
	typedef Reference reference;
};

//traits
template <typename Iterator>
struct iterator_traits {
	//内嵌型别 typename告诉编译器后面的为型别
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type        value_type;
	typedef typename Iterator::difference_type   difference_type;
	typedef typename Iterator::pointer           pointer;
	typedef typename Iterator::reference         reference;
};

//针对原生指针的traits偏特化版
template <typename T>
struct iterator_traits<T*> {
	//因为原生指针不能继承iterator基类，所以需要对后三种类型也具体声明
	typedef typename random_access_iterator_tag  iterator_category;
	typedef typename T                           value_type;
	typedef typename ptrdiff_t                   difference_type;
	typedef typename T*                          pointer;
	typedef typename T&                          reference;
};

//针对pointer-to-const的traits偏特化版
template <typename T>
struct iterator_traits<const T*> {
	//因为pointer-to-const不能继承iterator基类，所以需要对后三种类型也具体声明
	typedef typename random_access_iterator_tag  iterator_category;
	typedef typename T                           value_type;
	typedef typename ptrdiff_t                   difference_type;
	typedef typename const T*                    pointer;
	typedef typename const T&                    reference;
};

//决定某迭代器的类型
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category           //返回类型
iterator_category(const Iterator&) {
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();                                                 //产生category的临时对象，并进行返回
}

//决定某个迭代器的distance type
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*          //返回类型
distance_type(const Iterator&) {
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);  //将0转换为对应类型并返回
}

//决定某个迭代器的value type
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*               //返回类型
value_type(const Iterator&) {
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);       //将0转换为对应类型并返回
}

//以下是整组distance函数
template <typename InputIterator>                                                 //除random以外的均使用这个(继承)
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last) {
		++first; ++n;
	}
	return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
	return last-first;
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
	typedef typename iterator_traits<InputIterator>::iterator_category category;
	return __distance(first, last, category());
}

//以下是整组advance函数
template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
	while (n--) ++i;
}

template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
	if (n >= 0)
		while (n--) ++i;
	else
		while (n++)  --i;
}

template <typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
	i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n) {
	__advance(i, n, iterator_category(i));
}