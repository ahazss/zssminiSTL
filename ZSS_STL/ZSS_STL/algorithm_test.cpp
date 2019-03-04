#include<iostream>
#include"zss_deque.h"
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

	fill(iv1.begin(), iv1.end(), 9);       //������ȫ����9
	for_each(iv1.begin(), iv1.end(), display<int>());

	fill_n(iv1.begin(), 3, 7);             //�ӵ�������ָλ�ÿ�ʼ����3��7
	for_each(iv1.begin(), iv1.end(), display<int>());
	std::cout << std::endl;

	vector<int>::iterator ite1 = iv1.begin();         //ָ��7
	vector<int>::iterator ite2 = ite1;
	advance(ite2, 3);                                 //ָ��9
	iter_swap(ite1, ite2);
	std::cout << *ite1 << ' ' << *ite2 << std::endl;
	for_each(iv1.begin(), iv1.end(), display<int>());
	std::cout << std::endl;
	//����ȡ����֮����
	std::cout << max(*ite1, *ite2) << std::endl;
	//����ȡ����֮С��
	std::cout << min(*ite1, *ite2) << std::endl;

	swap(*iv1.begin(), *iv2.begin());   //������ֵ�Ե�
	for_each(iv1.begin(), iv1.end(), display<int>());
	std::cout << std::endl;
	for_each(iv2.begin(), iv2.end(), display<int>());


	int ia[] = { 0,1,2,3,4,5,6,7,8 };

	//�������������յ������������ص������û������
	copy(ia + 2, ia + 7, ia);
	for_each(ia, ia + 9, display<int>());       // 2 3 4 5 6 5 6 7 8
	std::cout << std::endl;





}