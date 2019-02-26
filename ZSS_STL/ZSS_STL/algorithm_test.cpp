#include<iostream>
#include"zss_algorithm.h"
#include"zss_vector.h"
using namespace ZSS;

template<typename T>
struct display {
	void operator()(const T& x) const {
		std::cout << x << ' ';
	}
};

int main()
{
	int ia[9] = { 0,1,2,3,4,5,6,7,8 };
	vector<int> iv1(ia, ia + 5);
	vector<int> iv2(ia, ia + 9);

	//std::cout << accumulate(iv.begin(), iv.end(), 0) << std::endl;
	//std::cout << accumulate(iv.begin(), iv.end(), 0,std::minus<int>()) << std::endl;	
	
	//std::cout << *(mismatch(iv1.begin(), iv1.end(), iv2.begin()).first);        
	//std::cout << *(mismatch(iv1.begin(), iv1.end(), iv2.begin()).second);
	//mismatch判断两个区间的第一个不匹配点，返回一个由两个迭代器组成的pair
	//第一个迭代器指向第一区间的不匹配点，第二个迭代器指向第二区间的不匹配点
	//以上写法很危险，应该先判断是否等于容器的end(),再进行解引用

	//若两个序列在[first,last)区间内相等，equal()返回true
	//std::cout << equal(iv1.begin(), iv1.end(), iv2.begin());
	//std::cout << equal(iv1.begin(), iv1.end(), &ia[3]);
	//std::cout << equal(iv1.begin(), iv1.end(), &ia[3], std::less<int>());




}