基于侯捷《STL源码剖析》
Part 1 完成了基础的allocator，但未进行应用及检验
          其中包括stl_alloc，stl_construct,stl_uninitialized(基于SGI较为复杂的内存分配方式)
Part 2 完成了traits特性与基础iterator设计，但未进行应用于检验
          其中包括stl_iterator,type_traits
Part 3 序列式容器
          完成了vector的基础操作（仍需考虑 insert的算法原因及std算法后面修正）
          完成了list的基础操作
          完成了deque的基础操作
          (以上部分均只考虑了正向迭代器，未实现reverse等)
          完成了stack的基础操作(基于deque)
          完成了queue的基础操作(基于deque)
          完成了heap(zss_algorithm)和priority_queue的基础操作(不可改变比较方式）
          (序列容器在const方面仍存在一些问题，还有一些std算法需要完成）
Part 4 关联式容器
          完成基本RB-tree(原理及细节部分仍需进一步注意)
          完成基于RB-tree的set和map
          (未完成hashtable和multi部分，及其中一些功能仍需具体完善)
Part 5 基础算法
          完成数值算法
          完成基本算法(如fill，fill_n，swap等)
         
待完成部分：
       算法    仿函数    配接器    关联式容器完善    问题查找（如const，反向迭代器等）  最终完善（文件管理等）
       
