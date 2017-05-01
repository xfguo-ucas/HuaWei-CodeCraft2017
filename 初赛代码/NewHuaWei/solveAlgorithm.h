//
//  solveAlgorith.h
//  
//
//  Created by Mordekaiser on 17/3/7.
//
//  求解算法类的声明，所有求解函数将在这里声明

#ifndef ____solveAlgorithm__
#define ____solveAlgorithm__

#include <stdio.h>
#include "Graph.h"
#include "partGraph.h"
#include <sstream>


class solveAlgorithm {
public:
    //friend class Genome; // 使其成为Genome的友元类，方便Genome调用solveAlgorithm的getServicePositon函数以及solver函数
    solveAlgorithm();   // 遗传算法所需的成员变量在此初始化
    ~solveAlgorithm();
    
    inline bool isANumber(char ch) {
        return ( ch >= '0' && ch <= '9' );
    }
    
    vector<vector<int>> parseFile(char* topo[MAX_EDGE_NUM], int line_num); // 解析文件，并存储成二维数组
    vector<bool> getServicePosition(Graph& graph); // 获取服务器位置函数
    vector<bool> getServicePosition_GA(Graph& graph); // 遗传算法调用的获取服务器位置函数
    
    partGraph BFS(const Graph& graph, vector<bool> servicePosition, int hiearcy, int comsume_position);  // 广度优先搜索，返回小图
    
    vector<vector<float>> getRelevanceMatrix(partGraph& graph); // 根据所给的图求其关联矩阵，获取关联矩阵之后就可以得到相应的约束了
    vector<vector<float>> getRelevanceMatrix(Graph& graph);
    
    // bool searchPath(Graph& graph, const partGraph& smallGraph);
    
    
    // 计算代价
    int returnCost(const vector<float>& flowOfEdges);
    void BFS_cut(Graph& graph);
    
    void parseResultOfILP(vector<float>& flowOfEdges, Graph& graph);
    
    void result(const char *filename,  Graph& graph);
    vector<vector<float>> getRelevanceMatrixSuper(Graph& graph);
    vector<int> BFS1( Graph& graph,  int hiearcy);
    
    void return_text(Graph& graph,vector<float> &flowOfEdges);
    std::vector<int> rand_m(int m,int n);
    
    
    int solver(Graph& graph);
    
};


#endif /* defined(____solveAlgorith__) */
