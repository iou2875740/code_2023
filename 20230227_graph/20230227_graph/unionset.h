#pragma once
#include <vector>
#include <iostream>

namespace ns_unionset
{
	class UnionFindSet
	{
	private:
		std::vector<int> _ufs;
	public:
		UnionFindSet(size_t n)
			:_ufs(n, -1)
		{}
		UnionFindSet() = default;
		int findroot(int index)
		{
			int pre = index;
			while (_ufs[index] >= 0)
			{
				index = _ufs[index];
			}
			int root = index;
			//压缩路径
			while (_ufs[pre] >= 0)
			{
				int tmp = _ufs[pre];
				_ufs[pre] = root;
				pre = tmp;
			}
			return root;
		}
		//将两个元素合并
		bool Union(int x, int y)
		{
			int root1 = findroot(x);
			int root2 = findroot(y);
			if (root1 == root2)
				return false;

			//少元素的集合并到多元素的集合
			if (abs(_ufs[root1]) < abs(_ufs[root2]))
			{
				std::swap(root1, root2);
			}
			_ufs[root1] += _ufs[root2];
			_ufs[root2] = root1;
			return true;
		}
		int count()
		{
			int size = 0;
			for (int i = 0; i < _ufs.size(); i++)
			{
				if (_ufs[i] < 0)
					size++;
			}
			return size;
		}
		void print1()
		{
			for (int i = 0; i < _ufs.size(); i++)
			{
				if (_ufs[i] < 0)
				{
					std::cout << i << "->";
					for (int j = 0; j < _ufs.size(); j++)
					{
						if (_ufs[j] == i)
						{
							std::cout << j ;
						}
					}
					std::cout << std::endl;
				}
			}
		}
		void print2()
		{
			for (int i = 0; i < _ufs.size(); i++)
			{
				std::cout << _ufs[i] << " ";
			}
		}
	};
	void test1()
	{
		UnionFindSet us(6);
		us.Union(0, 1);
		us.Union(1, 2);

		us.Union(3, 4);
		us.Union(4, 5);
		us.Union(0, 3);
		//us.print();
		us.findroot(5);
		us.print2(); 
	}
	void test2()
	{
		//测试压缩路径
		UnionFindSet us(6);
		us.Union(0, 1);
		us.Union(2, 0);
		us.Union(3, 2);
		us.Union(4, 3);
		us.Union(5, 4);
		us.findroot(0);
		us.print1();
		us.print2();
	}
	void test()
	{
		//测试少元素的集合并到多元素的集合
		UnionFindSet us(4);
		us.Union(0, 1);
		us.Union(0, 2);
		us.Union(3, 0);
		//us.Union(0, 3);
		us.print1();
		us.print2();
	}


}
