#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING
#include<iostream>
#include"zss_RB_tree.h"

using namespace ZSS;
/*
int main()
{
	rb_tree<int, int, std::identity<int>, std::less<int>> itree,itree2;
	rb_tree<int, int, std::identity<int>, std::less<int>>::iterator ite1, ite2;
	
	std::cout << itree.size() << std::endl;
	itree.insert_unique(10);
	itree.insert_unique(7);
	itree.insert_unique(8);

	itree.insert_unique(15);
	itree.insert_unique(5);
	itree.insert_unique(6);
	itree.insert_unique(11);
	itree.insert_unique(13);
	itree.insert_unique(12);
	itree2.insert_unique(1);
	itree2.insert_unique(2);

	itree.swap(itree2);
    ite1 = itree.begin();
	ite2 = itree.end();

	std::cout << itree.size() << std::endl;
	for (; ite1 != ite2; ++ite1) {
		std::cout << *ite1 << std::endl;
	}
	std::cout << std::endl;

	ite1 = itree.begin();
	ite2 = itree.end();
	__rb_tree_base_iterator rbtite;
	for (; ite1 != ite2; ++ite1) {
		rbtite = __rb_tree_base_iterator(ite1);    //向上转型是没有问题的
		std::cout << *ite1 << '('<<rbtite.node->color<<") ";
	}
	std::cout << std::endl;

	return 0;
}*/