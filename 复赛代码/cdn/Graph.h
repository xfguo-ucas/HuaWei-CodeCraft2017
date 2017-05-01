//
//  Graph.h
//  图的邻接矩阵实现
//
//  Created by Mordekaiser on 17/3/5.
//
//

#ifndef ____Graph__
#define ____Graph__

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <map>
#include "lib_io.h"

#define MYINFINITY 147483647 // 表示无解，或者没有这条边

using std::vector;


class Graph {
public:
    int numVertexs;
    int numConsumes;
    int cost_per_service;
    int edges;
    int serviceLevels;
    vector<vector<int>> level2flow;
    // INFINITY表示没有边
    vector<vector<int>> contains;   // numVertexs x numVertexs的二维矩阵，存储容量
    vector<vector<int>> single_price;   // numVertexs x numVertexs的二维矩阵，存储带宽限制
    
    // 存储消费节点的位置，key是消费节点，对应的value是一个元组, 元组第一个元素是消费节点连接的网络节点，第二个值是带宽
    std::map<int, std::pair<int, int>> consume_position;
    
    std::vector<int> service_position;   // 将遗传算法选取的服务器位置存下来
    
    //对每一条弧进行编号，方便送入线性规划求解之后还原求解结果
    std::map<std::pair<int, int>, int> edgesToIndex;
    
    // 第一个表示边的编号，边的两个顶点
    std::map<int, std::pair<int, int> > indexToEdges;
    vector<vector<int>> result; // 输出结果
    
    // 存储消费点对应的带宽需求，consume_position这个成员变量不好处理多个消费点对应一个网络节点的场景
    //  key网络点(消费点对应的网络点)，value是带宽需求
    std::map<int, int> NetNodeNeed;
    vector<int> servicePositionInConsume;
    
    
    // Floyd算法求最短路径
    vector<vector<int>> PathMatrix;
    vector<vector<int>> ShortPathTable;
    vector<int> ServicePositionByFolyd;
    vector<int> VertexDu;
    
    vector<int> VertexFlow;
    vector<int> flagComsume;
public:
    void ShortestPath_Floyd();

    Graph(vector<vector<int>>& graph_param); // 利用解析后的结果构造图
    //explicit Graph(Graph const &graph);   传const应用作为拷贝构造函数会导致BFS函数报错，不知道怎么解决？
    
};
#endif /* defined(____Graph__) */
