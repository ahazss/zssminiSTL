#include"zss_list.h"
#include"zss_vector.h"
using namespace ZSS;

int main()
{
	//û�����������rbegin��rend�������������rend++��ζ�Ŷ����ڲ�����������--(����)
	/*
	//��һ�֣�ͨ�����캯��
    int myints[] = { 75,23,65,42,13 };
	list<int> mylist1(myints, myints + 5);
	list<int> mylist2(2, 100);         // 2��ֵΪ100��Ԫ��
	//�ڶ��֣���push_back����push_front
	for (int i = 1; i <= 5; ++i) mylist1.push_back(i);
	mylist2.push_front(200);
	mylist2.push_front(300);
	//������,��assign
	list<int> first;
	list<int> second;
	first.assign(7, 100);                       // ��first���7��ֵΪ100��Ԫ��
	second.assign(first.begin(), first.end());  // ����first��second
	first.assign(myints, myints + 3);           // ������myints��������Ӹ�first
    for (list<int>::iterator it = mylist1.begin(); it != mylist1.end(); ++it)
		   std::cout << ' ' << *it;
    mylist1.push_front(200);
	mylist1.pop_front();
    mylist1.push_back(1);
    mylist1.pop_back();
	mylist1.insert(it, 10);                      // 1 10 2 3 4 5  "it" ��Ȼָ������2
	mylist1.insert(it, 2, 20);                   // 1 10 20 20 2 3 4 5
	vector<int> myvector(2, 30);                //����vector����������ʼ��Ϊ����2��ֵΪ30��Ԫ��                
	mylist1.insert(it, myvector.begin(), myvector.end());//��vector������ֵ����list��

	*/

	list<int> mylist;
	list<int>::iterator it1, it2;

	// set some values:
	for (int i = 1; i < 10; ++i) mylist.push_back(i * 10);

	// 10 20 30 40 50 60 70 80 90
	it1 = it2 = mylist.begin(); 
	                           // 10 20 30 40 50 60 70 80 90
	advance(it2, 6);           // ^                 ^
	++it1;                     //    ^              ^

	it1 = mylist.erase(it1);   // 10 30 40 50 60 70 80 90
							   //    ^           ^

	it2 = mylist.erase(it2);   // 10 30 40 50 60 80 90
							   //    ^           ^

	++it1;                      //       ^       ^
	--it2;                      //       ^    ^
								//û�б��������䷵��ֵ
	mylist.erase(it1, it2);     // 10 30 60 80 90
								//       ^
	std::cout << "*it1 : " << *it1 << std::endl;
	std::cout << "mylist contains:";
	for (it1 = mylist.begin(); it1 != mylist.end(); ++it1)
		std::cout << ' ' << *it1;

	
}