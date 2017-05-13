
#include <iostream>
#include "deploy.h"
#include "Graph.h"
#include <stdio.h>
#include "solveAlgorithm.h"
#include "Genome.h"
#include <vector>
#include "MiniCostFlow.h"
#include "PSO.h"
using namespace std;
MiniCostFlow MCF;

vector<vector<int>> graph_param;
//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    
    
    //rand(1019);
    srand(static_cast<unsigned int>(time(nullptr)));
    solveAlgorithm sol;
    // 解析文件，并构建图
    
    graph_param = sol.parseFile(topo, line_num); // 解析文件，并存储到graph_param里面
    Graph graph(graph_param);   // 构建图
    
    //graph.ShortestPath_Floyd();
    MCF.initial(graph, graph_param);
    //Genome GenenticAlgorithm;
    GenenticAlgorithm.GA(graph);
    //GenenticAlgorithm.climbMountain(graph);
    //PSO pso;
    //pso.optimize(graph);
    //optimize(graph);
    
    

    //    graph.service_position = vector<int>{107, 109, 102, 75, 44, 91 ,62, 7, 146, 126, 74, 85 ,90 ,26 ,23, 67, 35, 147 ,76 ,28 ,9 ,42, 78 ,63 ,149 ,135, 81 ,115, 8 ,15 ,25, 80, 82, 6 ,142 ,103 ,136 ,120 ,4,7 ,57 ,118, 10 ,153 ,29, 139, 154 ,17, 131, 119, 122 ,70, 155, 141, 106 };
    //auto flowOfEdges = ZKW::zkw_out(graph, graph_param);
    auto flowOfEdges = MCF.getResult(graph, graph_param);
    sol.return_text(graph, flowOfEdges);
    //sol.parseResultOfILP(flowOfEdges, graph);
    sol.result(filename, graph);
    
    
//    int costAllConsumes;
//
//    vector<vector<float>> CoefficientMatrix = sol.getRelevanceMatrix(graph);  // 获取关联矩阵
//    vector<float> flowOfEdges = linear_programming(CoefficientMatrix);
//    costAllConsumes = sol.returnCost(flowOfEdges) + graph.service_position.size() * graph.cost_per_service;
//    sol.parseResultOfILP(flowOfEdges, graph);
//    cout << "LP result: " << endl;
//    for (auto i : graph.service_position)
//        cout << i << " ";
//    cout << endl;
//    cout << "costAllConsumes: " << costAllConsumes << endl << endl;
//    sol.result(filename, graph);
    

    
        // 需要输出的内容
    /*
    char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";
    */
    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    //write_result(topo_file, filename);
    
}



