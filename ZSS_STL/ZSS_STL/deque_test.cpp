#include<iostream>
#include"zss_deque.h"
using namespace ZSS;

int main()
{
	deque<int> que;
	que.push_back(1);
	que.push_back(2);
	que.push_back(3);
	std::cout << "����������" << std::endl;
	std::cout << "���б�����" << std::endl;
	deque<int>::iterator ator;
	for ( ator= que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	int a;
	std::cout << "���ײ���Ԫ�أ�";
	std::cin >> a;
	que.push_front(a);
	std::cout << "���б�����" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "��β����Ԫ�أ�";
	std::cin >> a;
	que.push_back(a);
	std::cout << "���б�����" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "�м����Ԫ�أ�";
	std::cin >> a;
	que.insert(que.begin() + 1, 1, a);    //��һ��λ��Ϊ������λ�ã��ó�������Ϊ����λ��֮�󡢵�һ��Ԫ��֮ǰ���롣Ԫ�ر�Ŵӵ�0����ʼ�� 
	std::cout << "���б�����" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "ɾ������Ԫ�أ�" << std::endl;
	que.pop_front();
	std::cout << "���б�����" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "ɾ����βԪ�أ�" << std::endl;
	que.pop_back();
	std::cout << "���б�����" << std::endl;
	for (ator = que.begin(); ator != que.end(); ator++)
		std::cout << *ator << std::endl;
	std::cout << "����Ԫ�ظ�����" << que.size() << std::endl;
	std::cout << "������0:" << std::endl;
	que.clear();
	std::cout << "����Ԫ�ظ�����" << que.size() << std::endl;
	return 0;
}