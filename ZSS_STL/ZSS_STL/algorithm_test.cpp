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
	//mismatch�ж���������ĵ�һ����ƥ��㣬����һ����������������ɵ�pair
	//��һ��������ָ���һ����Ĳ�ƥ��㣬�ڶ���������ָ��ڶ�����Ĳ�ƥ���
	//����д����Σ�գ�Ӧ�����ж��Ƿ����������end(),�ٽ��н�����

	//������������[first,last)��������ȣ�equal()����true
	//std::cout << equal(iv1.begin(), iv1.end(), iv2.begin());
	//std::cout << equal(iv1.begin(), iv1.end(), &ia[3]);
	//std::cout << equal(iv1.begin(), iv1.end(), &ia[3], std::less<int>());




}