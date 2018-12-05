#include<iostream>
#include<list>
#include<algorithm>
using namespace std;

int main()
{
	list<int> a;
	a.push_back(0);
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);
	a.erase(a.begin());
	cout << *(a.begin()) << endl;

}