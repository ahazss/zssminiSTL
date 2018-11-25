#include"zss_vector.h"

namespace ZSS {
	template<typename T,typename Alloc>
	void vector<T,Alloc>::insert_aux(iterator position, const T& x) {
		//����ΪʲôҪ�ж�finish��out��ϵ��������push_back()���Ѿ��жϹ��˲ŵ��õ��𣿣�������
		if (finish != end_of_storage) {          //���б��ÿռ�
			//�ڱ��ÿռ���ʼ������һ������������vector���һ��Ԫ��ֵΪ���ֵ
			construct(finish, *(finish - 1));
			++finish;
			T x_copy = x;
			std::copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {                                   //���ޱ��ÿռ�
			//���ԭ��СΪ0��������1
			//���ԭ��С��Ϊ0��������ԭ��С��������ǰ�����������ԭ���ݣ���������������Ԫ��
			const size_type old_size = size();
			const size)type len = old_size != 0 ? 2 * old_size : 1;

			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = uninitialized_copy(start, position, new_start);
				construct(new_finish, x);
				++new_finish;
				//��ԭvector�ı��ÿռ������Ҳ�������������position��finish�����Ѿ��غ����𣬲�����;
				//new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);                      //��һ�������⣬����Ѿ�����õ�ȫ���ͷ�
				data_allocator::deallocate(new_start, len);
				throw;
			}

			//�������ͷ�ԭvector
			destroy(begin(), end());
			deallocate();

			//������������ָ���µ�vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
			
		}
	}
}