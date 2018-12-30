#include<iostream>
#include"zss_deque.h"
using namespace ZSS;

int main()
{
	deque<int> que;
	que.push_back(1);
	que.push_back(2);
	que.push_back(3);
	std::cout << "基本操作：" << std::endl;
	std::cout << "队列遍历：" << std::endl;
	deque<int>::iterator ator;
	for ( ator= que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	int a;
	std::cout << "队首插入元素：";
	std::cin >> a;
	que.push_front(a);
	std::cout << "队列遍历：" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "队尾插入元素：";
	std::cin >> a;
	que.push_back(a);
	std::cout << "队列遍历：" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "中间插入元素：";
	std::cin >> a;
	que.insert(que.begin() + 1, 1, a);    //第一个位置为迭代器位置，该程序插入的为在首位置之后、第一个元素之前插入。元素标号从第0个开始。 
	std::cout << "队列遍历：" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "删除队首元素：" << std::endl;
	que.pop_front();
	std::cout << "队列遍历：" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "删除队尾元素：" << std::endl;
	que.pop_back();
	std::cout << "队列遍历：" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "队列元素个数：" << que.size() << std::endl;
	std::cout << "队列清0:" << std::endl;
	que.clear();
	std::cout << "队列元素个数：" << que.size() << std::endl;
	return 0;
}