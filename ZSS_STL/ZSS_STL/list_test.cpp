#include"zss_list.h"
#include"zss_vector.h"
using namespace ZSS;

int main()
{
	//没有逆向迭代器rbegin和rend，逆向迭代器的rend++意味着对其内部迭代器进行--(重载)
	//sort没有加比较函数compare的方式

	/*
	//第一种，通过构造函数
    int myints[] = { 75,23,65,42,13 };
	list<int> mylist1(myints, myints + 5);
	list<int> mylist2(2, 100);         // 2个值为100的元素
	//第二种，用push_back，或push_front
	for (int i = 1; i <= 5; ++i) mylist1.push_back(i);
	mylist2.push_front(200);
	mylist2.push_front(300);
	//第三种,用assign
	list<int> first;
	list<int> second;
	first.assign(7, 100);                       // 给first添加7个值为100的元素
	second.assign(first.begin(), first.end());  // 复制first给second
	first.assign(myints, myints + 3);           // 将数组myints的内容添加给first
    for (list<int>::iterator it = mylist1.begin(); it != mylist1.end(); ++it)
		   std::std::std::std::std::cout << ' ' << *it;
    mylist1.push_front(200);
	mylist1.pop_front();
    mylist1.push_back(1);
    mylist1.pop_back();
	mylist1.insert(it, 10);                      // 1 10 2 3 4 5  "it" 仍然指向数字2
	mylist1.insert(it, 2, 20);                   // 1 10 20 20 2 3 4 5
	vector<int> myvector(2, 30);                //创建vector容器，并初始化为含有2个值为30的元素                
	mylist1.insert(it, myvector.begin(), myvector.end());//将vector容器的值插入list中
    it1 = mylist.erase(it1);
    mylist1.erase(it1, it2);
	first.swap(second);                         //交换元素
    mylist.resize(5);
	mylist.resize(8, 100);
	mylist.resize(12);
	mylist.clear();                             //清空list中的所有元素
	mylist1.splice(it, mylist2);                // mylist1: 1 10 20 30 2 3 4   (mylist1:1 2 3 4 mylist2: 10 20 30)
								                // mylist2 (empty) "it" 仍然指向数字2
    mylist2.splice(mylist2.begin(), it);        // mylist1: 1 10 20 30 3 4
	                                            // mylist2: 2
    mylist.remove(x);                           //除去list中所有为x的元素
	mylist.unique();                            //删除list中连续相同的元素
    mylist.sort();                              //对list进行排序，不可调用STL algorithm sort
    mylist.reverse();                           //倒序
	*/

}