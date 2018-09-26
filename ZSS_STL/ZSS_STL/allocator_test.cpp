#include "zss_allocator.h"
#include<vector>
#include<iostream>
using namespace std;

int main()
{
	int ia[5] = { 0,1,2,3,4 };
	unsigned int i;

	vector<int, ZSS::allocator<int>> iv(ia, ia + 5);                  //为什么会调用拷贝构造函数(U) 代理？？
	for (i = 0; i < iv.size(); i++)
	{
		cout << iv[i] << ' ';
	}
	cout << endl;
}