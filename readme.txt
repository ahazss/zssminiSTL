基于侯捷《STL源码剖析》
Part 1 完成了基础的allocator，但未进行应用及检验
          其中包括stl_alloc，stl_construct,stl_uninitialized(基于SGI较为复杂的内存          分配方式)
Part 2 完成了traits特性与基础iterator设计，但未进行应用于检验
          其中包括stl_iterator,type_traits
Part 3 序列容器
          完成了vector的基础操作（仍需考虑 insert的算法原因及std算法后面修正）