#include"zss_set.h"
using namespace ZSS;
/*
int main()
{
	int i;
	int ia[5] = { 0,1,2,3,4 };
	set<int> iset(ia,ia+5);
	
	std::cout << iset.size() << std::endl;
	iset.insert(3);
	std::cout << iset.size() << std::endl;
	iset.insert(5);
	std::cout << iset.size() << std::endl;

	//erase
	set<int>::iterator ite1 = iset.begin();
	set<int>::iterator ite2 = iset.end();
	for(;ite1!=ite2;ite1++)
		std::cout << *ite1 << " ";
	std::cout << std::endl;

	//使用STL算法find()找寻元素
	ite1 = std::find(iset.begin(), iset.end(), 3);
	if(ite1!=iset.end())
		std::cout << "3 found" << std::endl;

	//面对关联式容器使用其所提供的find来找元素更为高效
	ite1 = iset.find(3);
	if(ite1!=iset.end())
		std::cout << "3 found" << std::endl;

	//企图通过迭代器来改变set元素是不被允许的
	//*ite1=9;
}*/