
#include <iostream>
#include "deploy.h"
#include "Graph.h"
#include <stdio.h>
#include "solveAlgorithm.h"
#include "Genome.h"
#include <vector>
#include "MiniCostFlow.h"

#include <algorithm>
using namespace std;
MiniCostFlow MCF;
MiniCostFlow MCF2;
#include "SimulateAnnealing.h"

//MinCF MCF;
vector<vector<int>> graph_param;
//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    
    // -------------- 注意：：初始化和solvewithDM那块卡死了只用5档以下服务器，可能要调整

    // ------ 最后每个阈值都抖一下！！！

    //srand(1019);
    srand(static_cast<unsigned int>(time(nullptr)));
    solveAlgorithm sol;
    // 解析文件，并构建图
    
    graph_param = sol.parseFile(topo, line_num); // 解析文件，并存储到graph_param里面
    Graph graph(graph_param);   // 构建图

    // ---SA
    MCF2.initial(graph, graph_param);
    MCF.initial_greedy(graph, graph_param);
    SimulateAnnealing SA;
    SA.SA(graph);
    auto flowOfEdges = MCF2.getResult2(graph, graph_param, SA._BestService.cost);
    if(flowOfEdges[flowOfEdges.size()-1] > SA._BestService.cost)
    {
        vector<int> a;
        MCF.solve_greedy( graph, graph_param,a);
        flowOfEdges = MCF2.getResult2_greedy(graph, graph_param,a);
    }

    
    // --- GA-------
//    MCF2.initial(graph, graph_param);
//    MCF.initial_greedy(graph, graph_param);
//    Genome GenenticAlgorithm;
//    GenenticAlgorithm.GA(graph);
//    sort(graph.service_position.begin(), graph.service_position.end());
//    auto flowOfEdges = MCF2.getResult2(graph, graph_param, GenenticAlgorithm._BestIndividual._fittness);
//    if(flowOfEdges[flowOfEdges.size()-1]>GenenticAlgorithm._BestIndividual._fittness)
//    { vector<int> a;
//        
//        MCF.solve_greedy( graph, graph_param,a);
//        flowOfEdges = MCF2.getResult2_greedy(graph, graph_param,a);
//    }
//    
//    
    

   
    sol.return_text(graph, flowOfEdges);
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



