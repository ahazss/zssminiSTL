#include"zss_list.h"
#include"zss_vector.h"
using namespace ZSS;

int main()
{
	//û�����������rbegin��rend�������������rend++��ζ�Ŷ����ڲ�����������--(����)
	//sortû�мӱȽϺ���compare�ķ�ʽ

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
		   std::std::std::std::std::cout << ' ' << *it;
    mylist1.push_front(200);
	mylist1.pop_front();
    mylist1.push_back(1);
    mylist1.pop_back();
	mylist1.insert(it, 10);                      // 1 10 2 3 4 5  "it" ��Ȼָ������2
	mylist1.insert(it, 2, 20);                   // 1 10 20 20 2 3 4 5
	vector<int> myvector(2, 30);                //����vector����������ʼ��Ϊ����2��ֵΪ30��Ԫ��                
	mylist1.insert(it, myvector.begin(), myvector.end());//��vector������ֵ����list��
    it1 = mylist.erase(it1);
    mylist1.erase(it1, it2);
	first.swap(second);                         //����Ԫ��
    mylist.resize(5);
	mylist.resize(8, 100);
	mylist.resize(12);
	mylist.clear();                             //���list�е�����Ԫ��
	mylist1.splice(it, mylist2);                // mylist1: 1 10 20 30 2 3 4   (mylist1:1 2 3 4 mylist2: 10 20 30)
								                // mylist2 (empty) "it" ��Ȼָ������2
    mylist2.splice(mylist2.begin(), it);        // mylist1: 1 10 20 30 3 4
	                                            // mylist2: 2
    mylist.remove(x);                           //��ȥlist������Ϊx��Ԫ��
	mylist.unique();                            //ɾ��list��������ͬ��Ԫ��
    mylist.sort();                              //��list�������򣬲��ɵ���STL algorithm sort
    mylist.reverse();                           //����
	*/

}