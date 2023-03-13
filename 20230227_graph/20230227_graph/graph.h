#pragma once
#include "unionset.h"

#include <vector>
#include <map>
#include <cassert>
#include <iostream>
#include <string>
#include <queue>
#include <set>
using namespace std;

namespace ns_graph
{
	template<class V, class W, W MAX_W = INT_MAX, bool Direction = false>
	class Graph
	{
	private:
		std::vector<V> _vertex;         //�涥��
		std::map<V, size_t> _vmap;         //�涥���Ӧ���±�
		std::vector<std::vector<W>> _matrix; //��ߵľ���
	public:
		typedef Graph<V, W, MAX_W, false> Self;
		Graph() = default;
		Graph(const V* v, size_t n)
		{
			_vertex.reserve(n);
			for (int i = 0; i < n; i++)
			{
				_vertex.push_back(v[i]);
				_vmap[v[i]] = i;
			}
			//����ҲҪ���ռ�
			//_matrix.reserve(n);
			_matrix.resize(n);
			for (auto& e : _matrix)
			{
				e.resize(n, MAX_W);
			}
		}
		int GetVertexIndex(const V& vertex)
		{
			auto it = _vmap.find(vertex);
			if (it != _vmap.end())
			{
				//return _vmap[vertex];
				return it->second;
			}
			else
			{
				//assert(false);
				return -1;
			}
		}
		bool _AddEdge(int x, int y, const W& w)
		{
			_matrix[x][y] = w;
			if (Direction == false)
			{
				_matrix[y][x] = w;
			}
			return true;
		}
		bool AddEdge(const V& src, const V& des, const W& w)
		{
			int x = GetVertexIndex(src);
			int y = GetVertexIndex(des);
			if (x == -1 || y == -1)
			{
				return false;
			}
			return _AddEdge(x, y, w);
		}

		//��Ⱥ���ȱ����㷨
		void BFS(const V& vertex)
		{
			std::queue<V> q;
			q.push(vertex);
			std::vector<bool> is_search;//��¼�������ޱ���
			is_search.resize(_vertex.size(), false);
			int degree = 0;
			while (!q.empty())
			{
				int qsize = q.size();
				std::cout << "��" << degree << "������: ";
				while (qsize--)
				{
					V tmp = q.front();
					q.pop();
					std::cout << tmp << " ";
					int vindex = GetVertexIndex(tmp);
					is_search[vindex] = true;
					for (int i = 0; i < _matrix[vindex].size(); i++)
					{
						if (_matrix[vindex][i] != MAX_W && is_search[i] == false)
						{
							q.push(_vertex[i]);
						}
					}
				}
				std::cout << std::endl;
				degree++;
			}
			//�жϹ�������
			for (int i = 0; i < is_search.size(); i++)
			{
				if (is_search[i] == false)
				{
					std::cout << _vertex[i] << " ";
				}
			}
		}
		void _DFS(int index, std::vector<bool>& visted)
		{
			/*std::cout << _vertex[index] << " ";*/
			visted[index] = true;
			for (int i = 0; i < _vertex.size(); i++)//����ÿһ��
			{
				if (_matrix[index][i] != MAX_W && visted[i] == false)
				{
					//�ӷ��������Ķ��㿪ʼ ���ֳ�������ݹ�
					_DFS(i, visted);
				}
			}
			std::cout << _vertex[index] << " ";
		}
		void DFS(const V& vertex)
		{
			int index = GetVertexIndex(vertex);
			std::vector<bool> visted;
			visted.resize(_vertex.size(), false);
			_DFS(index, visted);
		}

		//��С������
		struct Edge
		{
			size_t _rsc;
			size_t _des;
			W _weight;
			Edge(size_t rsc, size_t des, W weight)
				:_rsc(rsc), _des(des), _weight(weight)
			{}
			Edge() {}

			bool operator>(const Edge& e) const
			{
				return _weight > e._weight;
			}
		};
		struct Greater
		{
			bool operator()(Edge e1, Edge e2)
			{
				return e1._weight > e2._weight;
			}
		};
		W Kruskal(Self& mintree)
		{
			//��ʼ��ͼ
			mintree._vertex = _vertex;
			mintree._vmap = _vmap;
			int n = _vertex.size();
			mintree._matrix.resize(n);
			for (auto& e : mintree._matrix)
			{
				e.resize(n, MAX_W);
			}

			std::priority_queue<Edge, std::vector<Edge>, Greater> pq;
			ns_unionset::UnionFindSet union_(_vertex.size());
			for (int x = 0; x < _matrix.size(); x++)
			{
				for (int y = 0; y < _matrix[x].size(); y++)
				{
					//�����б��Ƚ���
					if (_matrix[x][y] != MAX_W)
					{
						Edge newedge(x, y, _matrix[x][y]);
						pq.push(newedge);
					}
				}
			}
			W weight = 0;
			while (!pq.empty())
			{
				Edge e = pq.top();
				pq.pop();
				if (union_.Union(e._rsc, e._des))//���Ϊ��˵�������ɻ�·������ӱ�
				{
					std::cout << "rsc:" << e._rsc << "  des:" << e._des << std::endl;
					mintree.AddEdge(_vertex[e._rsc], _vertex[e._des], e._weight);
					weight += e._weight;
				}
			}
			return weight;
		}
		W Prim(Self& mintree, const V& src)
		{
			//��ʼ��ͼ
			mintree._vertex = _vertex;
			mintree._vmap = _vmap;
			int n = _vertex.size();
			mintree._matrix.resize(n);
			for (auto& e : mintree._matrix)
			{
				e.resize(n, MAX_W);
			}

			std::vector<bool> visted(_vertex.size(), false);//һ�����鼴��ʵ��������ͬ�ļ���
			int src_index = GetVertexIndex(src);
			visted[src_index] = true;//�׸�����
			W weight = 0;
			ns_unionset::UnionFindSet u(_vertex.size());
		m:
			{
				//�����{}����ʡ�� Ŀ����Ϊ��ʹ��pq�������󣬽����������ڣ�����pq
				std::priority_queue<Edge, std::vector<Edge>, Greater> pq;
				for (int i = 0; i < visted.size(); i++)
				{
					if (visted[i] == true)
					{
						for (int j = 0; j < visted.size(); j++)
						{
							if (visted[j] == false && _matrix[i][j] != MAX_W)
							{
								Edge newedge(i, j, _matrix[i][j]);
								pq.push(newedge);
							}
						}
					}
				}
				while (!pq.empty())
				{
					Edge edge_tmp = pq.top();
					//�����ɱߵ��������㣬����ͬһ������
					visted[edge_tmp._rsc] = true;
					visted[edge_tmp._des] = true;
					if (u.Union(edge_tmp._rsc, edge_tmp._des))
					{
						mintree.AddEdge(_vertex[edge_tmp._rsc], _vertex[edge_tmp._des], edge_tmp._weight);
						weight += edge_tmp._weight;
						break;//�����кܶ�� ��ֻ��Ҫһ��
					}
					else
						pq.pop();//˵���б߹��ɱջ���ȥ��
				}
			}
			for (int k = 0; k < visted.size(); k++)
			{
				if (visted[k] == false)
				{
					goto m;
				}
			}
			return weight;

		}

		//ʱ�临�Ӷ� O(n^2)
		void Dijkstra(const V& index, std::vector<W>& dist, std::vector<int>& ppath)
		{
			//��ʼ��dist ppth
			dist.resize(_vertex.size(), MAX_W);
			int src= GetVertexIndex(index);
			dist[src] = 0;
			ppath.resize(_vertex.size(), -1);
			std::vector<bool> gone(_vertex.size(), false);//�������Ѿ������Ĺ��ĵ�
			
			//˼·�ǣ�һ��������ݱߵ�ָ�����������Ŀ�ĵغ��¼���߹��ľ��룻
			//�����Ŀ�ĵ��кܶ������һ��ѡ�ĸ����������أ�ѡ��¼�µľ�������̵��Ǹ��ط�����Ϊ�����㣬��������
			//�Դ����ƣ�֪��ÿ�����㶼��Ϊ��������Ϊֹ
			for (int j = 0; j < _vertex.size(); j++)
			{
				//priority��pairĬ�����ȸ���.first����,����Ĭ����less
				std::priority_queue<pair<W, int>, vector<pair<W, int>>, greater<pair<W, int>>> pq;
				for (int i = 0; i < dist.size(); i++)
				{
					if (dist[i] != MAX_W && gone[i] == false)
						pq.push({ dist[i] , i });
				}
				src = pq.top().second;
				if (dist[src] != MAX_W)//δ����������ֵ ����С
				{
					gone[src] = true;
					for (int des = 0; des < _matrix[src].size(); des++)
					{
						if (_matrix[src][des] != MAX_W)
						{
							//ֻ�б�ԭ����Ȩ��С����Ҫ��
							int new_weight = _matrix[src][des] + dist[src];
							if (new_weight < dist[des])
							{
								dist[des] = new_weight;
								ppath[des] = src;
							}
						}
					}
				}
			}
		}
		void PathPrint(const V& src, std::vector<W>& dist, std::vector<int>& ppath)
		{
			int index = GetVertexIndex(src);
			for (int i = 0; i < ppath.size(); i++)
			{
				std::vector<int> path;
				int sci = i;
				while (sci != index)
				{
					path.push_back(sci);
					sci = ppath[sci];
				}
				path.push_back(index);
				std::reverse(path.begin(), path.end());
				for (auto& e : path)
				{
					std::cout << _vertex[e] << " ";
				}
				std::cout << "---weight:" << dist[i] << std::endl;
			}

		}

		bool BellmanFord(const V& index, std::vector<W>& dist, std::vector<int>& ppath)
		{
			//��ʼ��dist ppth
			dist.resize(_vertex.size(), MAX_W);
			int src = GetVertexIndex(index);
			dist[src] = 0;
			ppath.resize(_vertex.size(), -1);

			int num = 0;
			for (int k = 0; k < _vertex.size(); k++)
			{
				bool updata = false;//��¼�Ƿ���Ҫ���� 
				src %= _vertex.size();//Ϊ��ÿһ�����㶼����Ϊ������

				std::vector<bool> gone(_vertex.size(), false);//�������Ѿ������Ĺ��ĵ�
				for (int j = 0; j < _vertex.size(); j++)
				{
					//priority��pairĬ�����ȸ���.first����,����Ĭ����less
					std::priority_queue<pair<W, int>, vector<pair<W, int>>, greater<pair<W, int>>> pq;
					for (int i = 0; i < dist.size(); i++)
					{
						if (dist[i] != MAX_W && gone[i] == false)
							pq.push({ dist[i] , i });
					}
					src = pq.top().second;
					if (dist[src] != MAX_W)//δ����������ֵ ����С
					{
						gone[src] = true;
						for (int des = 0; des < _matrix[src].size(); des++)
						{
							if (_matrix[src][des] != MAX_W)
							{
								//ֻ�б�ԭ����Ȩ��С����Ҫ��
								int new_weight = _matrix[src][des] + dist[src];
								if (new_weight < dist[des])
								{
									updata = true;
									dist[des] = new_weight;
									ppath[des] = src;
								}
							}
						}
					}
				}
				if (updata == false)
				{
					break;
				}
				num++;
				src++;
			}
			
			std::cout << "������" << num << "��\n";
			//��⸺�ػ�
			for (int i = 0; i < _matrix.size(); i++)
			{
				for (int j = 0; j < _matrix[i].size(); j++)
				{
					//dist[i] + _matrix[i][j] < dist[j]  �����˵�����ܼ������� �������˻�
					// index��j �����·���Ѿ����� dist[j]���� ��dist[i] + _matrix[i][j]��dist[j]��ҪС ˵�����ܼ�������
					if (_matrix[i][j] != MAX_W && dist[i] + _matrix[i][j] < dist[j])
					{
						return false;
					}
				}
			}
			return true;
		}

		void FloydWarshall(std::vector<std::vector<W>>& vvDist, std::vector<std::vector<int>>& vvParentPath)
		{
			//vvDist��src��des�����·��Ȩ�� vvParentPath���������·�������������ֱ��·����
			//�ȳ�ʼ��������ά����
			int n = _vertex.size();
			vvDist.resize(n);
			vvParentPath.resize(n);
			for (int i = 0; i < n; i++)
			{
				vvDist[i].resize(n, MAX_W);
				vvParentPath[i].resize(n, -1);
			}

			for (int src = 0; src < _matrix.size(); src++)
			{
				for (int des = 0; des < _matrix[src].size(); des++)
				{
					if (_matrix[src][des] != MAX_W)
					{
						vvDist[src][des] = _matrix[src][des];
						vvParentPath[src][des] = src;
					}
					if (src == des)
					{
						//�Լ����Լ�Ȩ�ظ�Ϊ0
						vvDist[src][des] = 0;
					}
				}
			}

			int k;
			//ÿһ���㶼�п�����Ϊ�м䶥��
			for (int k = 0; k < n; k++)
			{
				for (int src = 0; src < vvDist.size(); src++)
				{
					for (int des = 0; des < vvDist[src].size(); des++)
					{
						if (vvDist[src][k] != MAX_W && vvDist[k][des] != MAX_W
							&& vvDist[src][k] + vvDist[k][des] < vvDist[src][des])
						{
							vvDist[src][des] = vvDist[src][k] + vvDist[k][des];
							//�ؼ�vvParentPath[k][des]��һ����k 
							//��ΪvvParentPath���������֮������·����������ֱ��·��
							//�� src->k->des ��ʱ�� vvParentPath[k][des] == k
							//�� src->k->x->y->des ��ʱ�� vvParentPath[k][des] == y �� !=k
							//vvParentPath[src][des] = k; //д������
							vvParentPath[src][des] = vvParentPath[k][des];
						}
					}
				}
			}
		}
		void print()
			{
				for (int x = 0; x < _matrix.size(); x++)
				{
					for (int y = 0; y < _matrix[x].size(); y++)
					{
						if (_matrix[x][y] == INT_MAX)
						{
							std::cout << "*" << " ";
						}
						else
						{
							std::cout << _matrix[x][y] << " ";
						}
					}
					std::cout << std::endl;
				}
			}
	};
		void test1()
		{
			//ͼ�Ļ�������
			Graph<char, int, INT_MAX, false> g("0123", 4);
			g.AddEdge('0', '1', 1);
			g.AddEdge('0', '3', 4);
			g.AddEdge('1', '3', 2);
			g.AddEdge('1', '2', 9);
			g.AddEdge('2', '3', 8);
			g.AddEdge('2', '1', 5);
			g.AddEdge('2', '0', 3);
			g.AddEdge('3', '2', 6);
			g.print();
		}
		void test2()
		{
			//��Ȳ��� & ��Ȳ���
			std::string a[] = { "����", "����", "����", "����", "����" };
			Graph<std::string, int> g1(a, sizeof(a) / sizeof(std::string));
			g1.AddEdge("����", "����", 100);
			g1.AddEdge("����", "����", 200);
			g1.AddEdge("����", "����", 30);
			g1.AddEdge("����", "����", 30);
			//g1.BFS("����");
			g1.DFS("����");
		}
		void test3()
		{
			//��С����������
			//kruskal�㷨			
			const char* str = "abcdefghi";
			Graph<char, int> g(str, strlen(str));
			g.AddEdge('a', 'b', 4);
			g.AddEdge('a', 'h', 8);
			//g.AddEdge('a', 'h', 9);
			g.AddEdge('b', 'c', 8);
			g.AddEdge('b', 'h', 11);
			g.AddEdge('c', 'i', 2);
			g.AddEdge('c', 'f', 4);
			g.AddEdge('c', 'd', 7);
			g.AddEdge('d', 'f', 14);
			g.AddEdge('d', 'e', 9);
			g.AddEdge('e', 'f', 10);
			g.AddEdge('f', 'g', 2);
			g.AddEdge('g', 'h', 1);
			g.AddEdge('g', 'i', 6);
			g.AddEdge('h', 'i', 7);
			//Graph<char, int> kminTree;
			//std::cout << "Kruskal:" << g.Kruskal(kminTree) << std::endl;
			//kminTree.BFS('b');
			//kminTree.DFS('b');
			//kminTree.print();

			//Graph<char, int> pminTree;
			//std::cout << "Prim:" << g.Prim(pminTree, 'c') << std::endl;
			//pminTree.BFS('c');
			for (int i = 0; i < strlen(str); i++)
			{
				std::cout << "����ǣ�" << str[i] << std::endl;
				Graph<char, int> pminTree;
				std::cout << g.Prim(pminTree, str[i]) << std::endl;
				pminTree.BFS(str[i]);
				std::cout << "--------------------------------------------------\n";
			}
			//pminTree.print();

		}
		void test4()
		{
			using namespace std;
			const char* str = "syztx";
			Graph<char, int, INT_MAX, true> g(str, strlen(str));
			g.AddEdge('s', 't', 10);
			g.AddEdge('s', 'y', 5);
			g.AddEdge('y', 't', 3);
			g.AddEdge('y', 'x', 9);
			g.AddEdge('y', 'z', 2);
			g.AddEdge('z', 's', 7);
			g.AddEdge('z', 'x', 6);
			g.AddEdge('t', 'y', 2);
			g.AddEdge('t', 'x', 1);
			g.AddEdge('x', 'z', 4);
			//vector<int> dist;
			//vector<int> parentPath;
			//g.Dijkstra('z', dist, parentPath);
			//g.PathPrint('z', dist, parentPath);

			for (int i = 0; i < strlen(str); i++)
			{
				vector<int> dist;
				vector<int> parentPath;
				g.Dijkstra(str[i], dist, parentPath);
				g.PathPrint(str[i], dist, parentPath);
				std::cout << "-----------------------\n";
			}
		}
		void test5()
		{
			//Dijkstra���Դ���Ȩ·��
			using namespace std;
			const char* str = "syztx";
			Graph<char, int, INT_MAX, true> g(str, strlen(str));
			g.AddEdge('s', 't', 6);
			g.AddEdge('s', 'y', 7);
			g.AddEdge('y', 'z', 9);
			g.AddEdge('y', 'x', -3);
			g.AddEdge('z', 's', 2);
			g.AddEdge('z', 'x', 7);
			g.AddEdge('t', 'x', 5);
			g.AddEdge('x', 't', -2);
			g.AddEdge('t', 'y', 8);
			g.AddEdge('t', 'z', -4);
			vector<int> dist;
			vector<int> parentPath;
			g.Dijkstra('s', dist, parentPath);
			g.PathPrint('s', dist, parentPath);
			cout << "-----------------------" << endl;
		}
		void test6()
		{
			using namespace std;
			const char* str = "syztx";
			Graph<char, int, INT_MAX, true> g(str, strlen(str));

			//-------------------------------------------------------------------------------------
			g.AddEdge('s', 't', 6);
			g.AddEdge('s', 'y', 7);
			g.AddEdge('y', 'z', 9);
			g.AddEdge('y', 'x', -3);
			g.AddEdge('z', 's', 2);
			g.AddEdge('z', 'x', 7);
			g.AddEdge('t', 'x', 5);
			g.AddEdge('x', 't', -2);
			g.AddEdge('t', 'y', 8);
			g.AddEdge('t', 'z', -4);

			// ΢��ͼ�ṹ�����и�Ȩ��·�Ĳ���
			//g.AddEdge('s', 't', 6);
			//g.AddEdge('s', 'y', 7);
			//g.AddEdge('y', 'x', -3);
			//g.AddEdge('y', 'z', 9);
			//g.AddEdge('y', 'x', -3);
			//g.AddEdge('y', 's', 1); // ����
			//g.AddEdge('z', 's', 2);
			//g.AddEdge('z', 'x', 7);
			//g.AddEdge('t', 'x', 5);
			//g.AddEdge('t', 'y', -8); // ����
			//g.AddEdge('t', 'z', -4);
			//g.AddEdge('x', 't', -2);

			//const char* str = "syz";
			//Graph<char, int, INT_MAX, true> g(str, strlen(str));
			//g.AddEdge('s', 'z', 2);
			//g.AddEdge('z', 'y', -2);
			//g.AddEdge('y', 's', -1);
			//-------------------------------------------------------------------------------------
			
			//vector<int> dist;
			//vector<int> parentPath;

			//if(g.BellmanFord('x', dist, parentPath))
			//	g.PathPrint('x', dist, parentPath);
			//else
			//	cout << "�и���\n";

			//for (int i = 0; i < strlen(str); i++)
			//{
			//	vector<int> dist;
			//	vector<int> parentPath;
			//	if (g.BellmanFord(str[i], dist, parentPath))
			//		g.PathPrint_(str[i], dist, parentPath);
			//	else
			//	{
			//		cout << "�и���\n";
			//		for (auto& e : parentPath)
			//		{
			//			cout << e << " ";
			//		}
			//		cout << endl;
			//	}
			//	std::cout << "-----------------------\n";
			//}
			
			//***************************************************************************************
			vector<vector<int>> vvDist;
			vector<vector<int>> vvParentPath;
			g.FloydWarshall(vvDist, vvParentPath);

			// ��ӡ��������֮������·��
			for (size_t i = 0; i < strlen(str); ++i)
			{
				g.PathPrint(str[i], vvDist[i], vvParentPath[i]);
				cout << endl;
			}

		}
		void test()
		{
			const char* str = "12345";
			Graph<char, int, INT_MAX, true> g(str, strlen(str));
			g.AddEdge('1', '2', 3);
			g.AddEdge('1', '3', 8);
			g.AddEdge('1', '5', -4);
			g.AddEdge('2', '4', 1);
			g.AddEdge('2', '5', 7);
			g.AddEdge('3', '2', 4);
			g.AddEdge('4', '1', 2);
			g.AddEdge('4', '3', -5);
			g.AddEdge('5', '4', 6);
			vector<vector<int>> vvDist;
			vector<vector<int>> vvParentPath;
			g.FloydWarshall(vvDist, vvParentPath);

			// ��ӡ��������֮������·��
			for (size_t i = 0; i < strlen(str); ++i)
			{
				g.PathPrint(str[i], vvDist[i], vvParentPath[i]);
				std::cout << "-----------------------\n";
			}

			//for (int i = 0; i < strlen(str); i++)
			//{
			//	vector<int> dist;
			//	vector<int> parentPath;
			//	if (g.BellmanFord(str[i], dist, parentPath))
			//		g.PathPrint(str[i], dist, parentPath);
			//	else
			//	{
			//		cout << "�и���\n";
			//		for (auto& e : parentPath)
			//		{
			//			cout << e << " ";
			//		}
			//		cout << endl;
			//	}
			//	std::cout << "-----------------------\n";
			//}

		}
}

namespace ns_link_graph
{

	template<class V, class W, bool Direction = false>
	class Graph
	{
	public:
		struct edge
		{
			V _v;
			W _weight;
			edge* _next;
			edge(W weight, V v)
				:_v(v)
				, _weight(weight)
				, _next(nullptr)
			{
			}
			edge() {}
		};

		Graph(const V* v, size_t n)
		{
			_vertex.reserve(n);
			for (int i = 0; i < n; i++)
			{
				_vertex.push_back(v[i]);
				_vmap[v[i]] = i;
			}
			_linkv.resize(n, nullptr);
			//_linkv.reserve(n);
			for (int i = 0; i < n; i++)
			{
				_linkv[i] = new edge(0, _vertex[i]);
				//��ͷ�ڵ�
				//_linkv.push_back(new edge(0, _vertex[i]));
			}
		}
		int GetVertexIndex(const V& vertex)
		{
			auto it = _vmap.find(vertex);
			if (it != _vmap.end())
			{
				//return _vmap[vertex];
				return it->second;
			}
			else
			{
				//assert(false);
				return -1;
			}
		}
		void _AddEdge(int rsc, int des, const W& w)
		{
			edge* e = new edge(w,_vertex[des]);
			//ͷ��
			edge* tmp=_linkv[rsc]->_next;
			_linkv[rsc]->_next = e;
			e->_next = tmp;
			if (Direction == false)
			{
				std::swap(rsc, des);
				edge* e = new edge(w, _vertex[des]);
				edge* tmp = _linkv[rsc]->_next;
				_linkv[rsc]->_next = e;
				e->_next = tmp;
			}

		}
		void AddEdge(const V& src, const V& des, const W& w)
		{
			int x = GetVertexIndex(src);
			int y = GetVertexIndex(des);
			if (x == -1 || y == -1)
			{
				return ;
			}
			 _AddEdge(x, y, w);
		}

		void _DFS(int index,std::vector<bool>& visted)
		{
			if (visted[index] == false)
			{
				edge* cur = _linkv[index];
				visted[index] = true;
				while (cur)
				{
					int tmpindex = GetVertexIndex(cur->_v);
					_DFS(tmpindex, visted);
					cur = cur->_next;
				}
				std::cout << _vertex[index] << " ";
			}
		}
		void DFS(const V& vertex)
		{
			int index = GetVertexIndex(vertex);
			std::vector<bool> visted(_vertex.size(),false);
			_DFS(index,visted);
		}
		void print()
		{
			for (int i = 0; i < _linkv.size(); i++)
			{
				edge* cur = _linkv[i];
				while (cur != nullptr)
				{
					std::cout << cur->_v << ":"<<cur->_weight<<" ";
					cur = cur->_next;
				}
				std::cout << std::endl;
			}
		}
	private:
		std::vector<V> _vertex;         //�涥��
		std::map<V, size_t> _vmap;         //�涥���Ӧ���±�
		std::vector<edge*> _linkv;   //���
	};

	void test1()
	{
		std::string a[] = { "����", "����", "����", "����" };
		Graph<std::string, int> g1(a, 4);
		g1.AddEdge("����", "����", 100);
		g1.AddEdge("����", "����", 200);
		g1.AddEdge("����", "����", 30);
		g1.print();
		/*Graph<char, int> g("0123", 4);
		g.AddEdge('0', '1', 1);
		g.AddEdge('0', '3', 4);
		g.AddEdge('1', '3', 2);
		g.AddEdge('1', '2', 9);
		g.AddEdge('2', '3', 8);
		g.AddEdge('2', '1', 5);
		g.AddEdge('2', '0', 3);
		g.AddEdge('3', '2', 6);
		g.print();*/

	}
	void test()
	{
		//��Ȳ��� & ��Ȳ���
		std::string a[] = { "����", "����", "����", "����", "����" };
		Graph<std::string, int> g1(a, sizeof(a) / sizeof(std::string));
		g1.AddEdge("����", "����", 100);
		g1.AddEdge("����", "����", 200);
		g1.AddEdge("����", "����", 30);
		g1.AddEdge("����", "����", 30);
		//g1.BFS("����");
		g1.DFS("����");
	}
}