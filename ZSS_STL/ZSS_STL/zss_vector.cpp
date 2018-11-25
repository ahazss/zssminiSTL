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
			const size)type len = old_size != 0 ? 2 * old_size : 1;

			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = uninitialized_copy(start, position, new_start);
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
}