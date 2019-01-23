#pragma once
#include"zss_alloc.h"
#include"zss_construct.h"
#include"zss_uninitialized.h"

namespace ZSS {

template<typename T, typename Alloc=alloc>                     //allocΪ��һ����ڶ���������
class vector {
public:
    //vector��Ƕ���ͱ���
	typedef T               value_type;
	typedef value_type*     pointer;
	typedef value_type*     iterator;
	typedef value_type&     reference;
	typedef const T*        const_reference;
	typedef size_t          size_type;                         //��32λϵͳ��size_tΪ4�ֽڣ�64λϵͳ��Ϊ8�ֽ�
	typedef ptrdiff_t       difference_type;

protected:
	//simple_alloc��SGI STL�Ŀռ�������,��zss_alloc.h
	typedef simple_alloc<value_type, Alloc>  data_allocator;
	iterator start;                           //��ʾĿǰʹ�ÿռ��ͷ��ʵ��Ϊָ��
	iterator finish;                          //��ʾĿǰʹ�ÿռ��β
	iterator end_of_storage;                  //��ʾĿǰ���ÿռ��β

	void insert_aux(iterator position, const T& x);

	void deallocate() {
		if (start)
			data_allocator::deallocate(start, end_of_storage - start);      //�˴�ҲΪָ��ļ������˴�����Ӧ����Ԫ���Ѿ�����֮��
	}

	void fill_initialize(size_type n, const T& value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

public:
	vector() :start(0), finish(0), end_of_storage(0) { }
	vector(size_type n, const T& value)              { fill_initialize(n, value); }
	vector(int n, const T& value)                    { fill_initialize(n, value); }
	vector(long n, const T& value)                   { fill_initialize(n, value); }   //ΪʲôҪ�ѿ�������Ĵ�С��ת��Ϊsize_t����long����װ�����𣿣���
	explicit vector(size_type n)                     { fill_initialize(n, T()); }

	vector(iterator first, iterator last) {
		start = data_allocator::allocate(last-first);
		uninitialized_copy(first, last, start);
		finish = start + (last - first);
		end_of_storage = finish;
	}

	vector(vector<T>& right)    //�������캯��
	{
		start = data_allocator::allocate(right.capacity());
		finish = uninitialized_copy(right.begin(), right.end(), start);
		end_of_storage = start + right.capacity();
		std::cout << "copy construct" << std::endl;
	}
	vector(vector<T>&& right)   //ת�ƿ������캯��
	{
		start = right.begin();
		finish = right.back();
		end_of_storage = (start + right.capacity());

		right.begin() = nullptr;
		right.back() = nullptr;
		right.end_of_storage() = nullptr;
		std::cout << "move construct" << std::endl;
	}
	vector<T>& operator= (const vector<T>& right)   //��ֵ���캯��
	{
		if (this != &right)
		{
			start = data_allocator::allocate(right.capacity());
			finish = uninitialized_copy(right.begin(), right.end(), start);
			end_of_storage = start + right.capacity();
		}
		std::cout << "copy assignment" << std::endl;
		return *this;
	}
	vector<T>& operator= (vector<T>&& right)        //ת�Ƹ�ֵ���캯��
	{
		if (this != &right)
		{
			start = right.begin();
			finish = right.back();
			end_of_storage = (start + right.capacity());

			right.begin() = nullptr;
			right.back() = nullptr;
			right.end_of_storage() = nullptr;
		}
		std::cout << "move assignment" << std::endl;
		return *this;
	}

	~vector() {
		destroy(start, finish);                                      //ȫ�ֺ�������zss_construct.h��(������)
		deallocate();                                                //vector��һ��member function(�ͷ��ڴ�)
	}
    
	iterator begin() const { return start; }                                       //һ���iterator����Ϊ vector<xxx>::iterator 
	iterator end() const  { return finish; }                                      //ʵ����ָ�Ļ���ָ�룬����ʱ��Ϊtypedef�����Լ�д����
	size_type size() const       { return size_type(end() - begin()); }
	size_type capacity() const   { return size_type(end_of_storage - start); }
	bool empty() const           { return begin() == end(); }
	reference front() { return *begin(); }            //��һ��Ԫ��
	reference back() { return *(end() - 1); }         //���һ��Ԫ��(ע��end()����ָ����β��ָ��)
	
	reference operator[](size_type n) {                                            //����������ָ������������мӼ�����
		if (n < size())
			return *(begin() + n);
		else {
			std::cout << "out of range" << std::endl;
			exit(1);
		}
	}
	bool operator==(const vector<T>& right) {
		if (size() != right.size())
			return false;
		else {
			for (int i = 0; i < size(); i++){
				if (*(begin() + i) != *(right.begin()+i))
					return false;
			}
			return true;
		}
	}
	bool operator!=(const vector<T>& right) {
		return (!operator==(right));
	}
	bool operator<=(const vector<T>& right){
		if (size() < right.size())
			return true;
		else if (size() > right.size())
			return false;
		else {
			for (int i = 0; i < size(); i++){
				if (*(begin() + i) >*(right.begin() + i))
					return false;
			}
			return true;
		}
	}
	bool operator>=(const vector<T>& right) {
		return !operator<=(right);
	}
	bool operator< (const vector<T>& right) {
		if (size() < right.size())
			return true;
		else if (size() > right.size())
			return false;
		else {
			for (int i = 0; i < size(); i++) {
				if (*(begin() + i) >= *(right.begin() + i))
					return false;
			}
			return true;
		}
	}
	bool operator> (const vector<T>& right) {
		return !operator<(right);
	}

	void push_back(const T& x) {                      //��Ԫ�ز�����β��
		if (finish != end_of_storage) {               //�����ڴ棬����Ҫ����
			construct(finish, x);                     //ȫ�ֺ�������zss_construct.h(�������ڴ��Ϲ���)
			++finish;                                 //��Ӧ���͵�ָ��ĵ���
		}
		else
			insert_aux(end(), x);                     //vector��member function(������capacity)
	}
	void pop_back() {                                 //��β��Ԫ��ȡ��
		--finish;                                     //finish������β��ָ��
		destroy(finish);                              //��zss_construct.h ������������δ�ͷ��ڴ�(����capacity����)
	}

	void assign(size_type n, const T& x) {
		uninitialized_fill_n(start, n, x);
	}
	void assign(iterator first, iterator last) {
		uninitialized_copy(first, last, start);
	}
	iterator erase(iterator position) {               //���ĳλ���ϵ�Ԫ��
		if (position + 1 != end())                    //position��ָ�Ĳ���βԪ��
			std::copy(position + 1, finish, position);     //����Ԫ����ǰ�ƶ���copy����--STL algorithm
		--finish;
		destroy(finish);                              //���������ͷ�
		return position;
	}
	iterator erase(iterator first, iterator last) {   //ɾ��[first,last)��Ԫ��
		if(last != end())
		    std::copy(last, finish, first); 
		for (int i = 0; i < last - first; i++) {
			--finish;
			destroy(finish);
		}
		return first;
	}
	void clear() { erase(begin(), end()); }

	void resize(size_type new_size) { resize(new_size, T()); }
	void resize(size_type new_size,const T& x);
	void reserve(size_type n);
	void insert(iterator first, const T& x);
	void insert(iterator first, size_type n, const T& x);
	void insert(iterator position, iterator first, iterator last);
	void swap(vector<T>& right) {
		iterator temp_start = start;
		iterator temp_finish = finish;
		start = right.start;
		finish = right.finish;
		right.start = temp_start;
		right.finish = temp_finish;
	}
	
protected:
	//���ÿռ䲢��������
	iterator allocate_and_fill(size_type n, const T& x) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
};

template<typename T, typename Alloc>
inline void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
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
inline void vector<T, Alloc>::insert(iterator position, const T& x) {
	insert(position, 1, x);
}

template<typename T, typename Alloc>
inline void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
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
				////������˼·Ϊ 0 1 2 3 _ _ -> 0 1 2 3 2 3 -> 0 1 0 1 2 3 -> x x 0 1 2 3 
				uninitialized_copy(finish - n, finish, finish);       //�Ƚ���n��Ԫ�غ���nλ(����Ч�ʸ��ߣ���)
				finish += n;                                          //��vectorβ�˱�Ǻ���nλ
				std::copy_backward(position, old_finish, finish);          //copy��copy_backward�Ĺ��ܼ���һ����copy_backward(first,last,result)��
																	  //Ԫ��[first,last-1]������[result-(last-first),result),�����һ��Ԫ�ؿ�ʼ����
																	  //��ʱʣ�µ�elems_after-nλ��λ
				std::fill(position, position + n, x_copy);
			}
			else {
				//�����������Ԫ�ظ���С�ڵ�������Ԫ�ظ���
				uninitialized_fill_n(finish,(n - elems_after), x_copy);   //�ڶ��������ĺ���Ϊsize
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
				new_finish = uninitialized_fill_n(new_finish, n, x);
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

template<typename T, typename Alloc>
inline void vector<T, Alloc>::insert(iterator position,iterator first,iterator last)
{
	size_type n = last - first;
	if (n != 0) {
		if (size_type(end_of_storage - finish) >= n) {
			//���ÿռ���ڵ���"����Ԫ��"
			//���¼�������֮�������Ԫ�ظ���
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				//�����֮�������Ԫ�ظ�����������Ԫ�ظ���
				//����ֱ���ƶ����� 0 1 2 3_ _,Ҫ�Ѵ�0��ʼ���������λ���������ƻḲ�ǵ�2��ֵ������2û�а취����
				uninitialized_copy(finish - n, finish, finish);       //�Ƚ���n��Ԫ�غ���nλ
				finish += n;                                          //��vectorβ�˱�Ǻ���nλ
				std::copy_backward(position, old_finish, finish);          //copy��copy_backward�Ĺ��ܼ���һ����copy_backward(first,last,result)��
																		   //Ԫ��[first,last-1)������[result-(last-first),result),�����һ��Ԫ�ؿ�ʼ����
																		   //��ʱʣ�µ�elems_after-nλ��λ
				std::copy(first, last, position);
			}
			else {
				//�����������Ԫ�ظ���С�ڵ�������Ԫ�ظ���
				std::copy(position, finish, (position + n));
				finish += n ;
				std::copy(first, last, position);
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
				new_finish = uninitialized_copy(first, last, new_finish);
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

template<typename T, typename Alloc>
inline void vector<T, Alloc>::resize(size_type new_size, const T& x) {
	//�ı��С(����capacity)�������������ֵ��Ϊx��������С(ɾ)
	size_type old_size = size();
	if (old_size > new_size) {
		//��С
		for (int i = new_size; i < old_size; i++) {
			destroy(begin() + i);
		}
		finish = begin() +new_size;
	}
	else {
		//����
		if (new_size < capacity()){
			//����Ҫ�������ڴ�
			finish=uninitialized_fill_n(finish, (new_size - old_size), x);
		}
		else {
			//��Ҫ�������ڴ�
			size_type len = 2 * old_size > new_size ? 2 * old_size : new_size;
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			//��ԭvectorԪ�ؿ���
			new_finish = uninitialized_copy(start, finish, new_start);
			new_finish = uninitialized_fill_n(new_finish, (new_size - old_size), x);
			//ɾ��ԭvector
			destroy(start, finish);
			deallocate();
			//����first��last,end_of_range
			start = new_start;
			finish = new_finish;
			end_of_storage = start + len;
		}
	}
}

template<typename T, typename Alloc>
inline void vector<T, Alloc>::reserve(size_type n)
{
	if (n < capacity())
		return;
	else {
		iterator new_start = data_allocator::allocate(n);
		iterator new_finish = new_start;
		new_finish = uninitialized_copy(start, finish, new_start);
		destroy(start, finish);
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = start + n;

	}

}
}
