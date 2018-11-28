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
			const size_type len = old_size != 0 ? 2 * old_size : 1;

			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = uninitialized_copy(start, position, new_start);      //���ص��Ǹ���������λ�õ���һλ
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

    template<typename T,typename Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
    {
		if (n != 0) {
			if (size_type(end_of_storage - finish) >= n) {
				//���ÿռ���ڵ���"����Ԫ��"
				T x_copy = x;
				//���¼�������֮�������Ԫ�ظ���
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n) {
					//�����֮�������Ԫ�ظ�����������Ԫ�ظ���
					//����ֱ���ƶ����� 0 1 2 3_ _,Ҫ�Ѵ�0��ʼ���������λ���������ƻḲ�ǵ�2��ֵ������2û�а취����
					uninitialized_copy(finish - n, finish, finish);       //�Ƚ���n��Ԫ�غ���nλ
					finish += n;                                          //��vectorβ�˱�Ǻ���nλ
					copy_backward(position, old_finish, finish);          //copy��copy_backward�Ĺ��ܼ���һ����copy_backward(first,last,result)��
																		  //Ԫ��[first,last-1)������[result-(last-first),result),�����һ��Ԫ�ؿ�ʼ����
																		  //��ʱʣ�µ�elems_after-nλ��λ
					std::fill(position, position + n, x_copy);
				}
				else {
					//�����������Ԫ�ظ���С�ڵ�������Ԫ�ظ���
					uninitialized_fill_n(finish, n - elelms_after, x_copy);   //�ڶ��������ĺ���Ϊsize
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					std::fill(position, old_finish, x_copy);
					//�����������ǳ���Ч�ʵĿ��ǣ���finish���µ�finish+n-elems_after�е�Ԫ����ֻ�������ڴ�ģ����Դ�ʱ��un..��Ч�ʸ���(���ǲ�ͬ�ͱ�)
					//��un..���������Ѿ���Ԫ�ص���䣬���Խ��Ѿ���Ԫ�صĺ�ûӴԪ�صķֿ�
				}
			}
			else {
				//���ÿռ�С������Ԫ�ظ���(Ҫ���������ڴ�)
				const size_type old_size = size();
				const size_type len = old_size > n ? 2 * old_size : old_size + n;
				//���������µ�vector�ռ�
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try {
					//���Ƚ���vector�Ĳ����֮ǰ��Ԫ�ظ��Ƶ��¿ռ�
					new_finish = uninitialized_copy(start, position, new_start);
					//�ٽ�����Ԫ�أ���ֵ��Ϊn�������¿ռ�
					new_finish = uninitialized_copy(new_finish, n, x);
					//�ٽ���vector�Ĳ����֮���Ԫ�ظ��Ƶ��¿ռ�
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...) {
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				//������ͷžɵ�vector
				destroy(start, finish);
				deallocate();
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
    }
}

