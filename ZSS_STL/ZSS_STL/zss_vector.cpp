#include"zss_vector.h"

namespace ZSS {
	template<typename T,typename Alloc>
	void vector<T,Alloc>::insert_aux(iterator position, const T& x) {
		//不懂为什么要判断finish和out关系，不是在push_back()中已经判断过了才调用的吗？？？？？
		if (finish != end_of_storage) {          //还有备用空间
			//在备用空间起始处构造一个函数，并以vector最后一个元素值为其初值
			construct(finish, *(finish - 1));
			++finish;
			T x_copy = x;
			std::copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {                                   //已无备用空间
			//如果原大小为0，则配置1
			//如果原大小不为0，则配置原大小的两倍，前半段用来放置原数据，后半段用来放置新元素
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;

			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = uninitialized_copy(start, position, new_start);      //返回的是复制完后结束位置的下一位
				construct(new_finish, x);
				++new_finish;
				//将原vector的备用空间的内容也拷贝过来，这个position和finish不是已经重合了吗，不懂用途
				//new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);                      //若一旦出问题，则把已经构造好的全部释放
				data_allocator::deallocate(new_start, len);
				throw;
			}

			//析构并释放原vector
			destroy(begin(), end());
			deallocate();

			//调整迭代器，指向新的vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
			
		}
	}

    template<typename T,typename Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
    {
		if (n != 0) {
			if (size_type(end_of_storage - finish) >= n) {
				//备用空间大于等于"新增元素"
				T x_copy = x;
				//以下计算插入点之后的现有元素个数
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n) {
					//插入点之后的现有元素个数大于新增元素个数
					//不可直接移动，如 0 1 2 3_ _,要把从0开始的向后移两位，按次序复制会覆盖掉2的值，则导致2没有办法复制
					uninitialized_copy(finish - n, finish, finish);       //先将后n个元素后移n位
					finish += n;                                          //将vector尾端标记后移n位
					copy_backward(position, old_finish, finish);          //copy与copy_backward的功能几乎一样，copy_backward(first,last,result)将
																		  //元素[first,last-1)拷贝到[result-(last-first),result),从最后一个元素开始复制
																		  //此时剩下的elems_after-n位到位
					std::fill(position, position + n, x_copy);
				}
				else {
					//插入点后的现有元素个数小于等于新增元素个数
					uninitialized_fill_n(finish, n - elelms_after, x_copy);   //第二个参数的含义为size
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					std::fill(position, old_finish, x_copy);
					//这样做可能是出于效率的考虑，在finish到新的finish+n-elems_after中的元素是只开辟了内存的，所以此时用un..的效率更高(考虑不同型别)
					//而un..不适用于已经有元素的填充，所以将已经有元素的和没哟元素的分开
				}
			}
			else {
				//备用空间小于新增元素个数(要分配额外的内存)
				const size_type old_size = size();
				const size_type len = old_size > n ? 2 * old_size : old_size + n;
				//以下配置新的vector空间
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try {
					//首先将旧vector的插入点之前的元素复制到新空间
					new_finish = uninitialized_copy(start, position, new_start);
					//再将新增元素（初值均为n）填入新空间
					new_finish = uninitialized_copy(new_finish, n, x);
					//再将旧vector的插入点之后的元素复制到新空间
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...) {
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				//清除并释放旧的vector
				destroy(start, finish);
				deallocate();
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
    }
}

