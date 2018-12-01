#include"zss_vector.h"
#include<vector>
using namespace ZSS;

int main()
{
	int a[8] = { 1,2,3,4,5,6,7,8 };
	vector<int> a1(10);                           //定义了10个整型元素的向量，但没有给出初值，其值是不确定的。
	vector<int> a2(10, 1);                        //定义了10个整型元素的向量,且给出每个元素的初值为1
	vector<int> a3(a2.begin(), a2.begin() + 3);   //定义了a值为b中第0个到第2个（共3个）元素
	vector<int> a4(a, a + 7);                     //从数组中获得初值
	vector<int> a5(a4);
    
	/*
	a4.back();                                    //返回a的最后一个元素
    a4.front();                                   //返回a的第一个元素
    a1.empty();                                   //判断a是否为空，空则返回ture,不空则返回false
    a1.clear();                                   //清空a中的元素
	a4[3];                                        //返回a的第i个元素，当且仅当a[i]存在
    a4.size();                                    //返回a中元素的个数
	a4.capacity();                                //返回a在内存中总共可以容纳的元素个数
	a4.erase(a4.begin());                         //删除指定元素
	a4.erase(a4.begin() + 1, a4.begin() + 3);     //删除[a4,begin()+1,a.begin()+3)元素
    a4.pop_back();                                //删除a向量的最后一个元素
	a4.push_back(5);                              //在a的最后一个向量后插入一个元素
	a5 == a4;                                     //b为向量，向量的比较操作还有!=,>=,<=,>,<
	*/
	
	a1.insert(a4.begin() + 1, 5,1); //在a的第1个元素（从第0个算起）的位置插入数值5，如a为1,2,3,4，插入元素后为1,5,2,3,4
	
	//a1.insert(a.begin() + 1, 3, 5); //在a的第1个元素（从第0个算起）的位置插入3个数，其值都为5
	//a1.insert(a.begin() + 1, b + 3, b + 6); //b为数组，在a的第1个元素（从第0个算起）的位置插入b的第3个元素到第5个元素（不包括b+6），如b为1,2,3,4,5,9,8         ，插入元素后为1,4,5,9,2,3,4,5,9,8
	for (int i = 0; i < a4.size(); i++)
	{
		std::cout << a4[i] << std::endl;
	}
	a4.push_back(8);
	std::cout << a4.size() << " " << a4.capacity() << std::endl;
	//待解决，没有显示size和capacity
	/*
	a1.assign(b.begin(), b.begin() + 3); //b为向量，将b的0~2个元素构成的向量赋给a
	a2.assign(4, 2); //是a只含4个元素，且每个元素为2
	
	
	
	
	
	
	
	

	a1.resize(10); //将a的现有元素个数调至10个，多则删，少则补，其值随机
	a1.resize(10, 2); //将a的现有元素个数调至10个，多则删，少则补，其值为2
	a1.reserve(100); //将a的容量（capacity）扩充至100，也就是说现在测试a.capacity();的时候返回值是100.这种操作只有在需要给a添加大量数据的时候才         显得有意义，因为这将避免内存多次容量扩充操作（当a的容量不足时电脑会自动扩容，当然这必然降低性能） 
	a1.swap(b); //b为向量，将a中的元素和b中的元素进行整体性交换
	
	*/
}