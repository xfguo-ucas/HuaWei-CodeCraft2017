//
//  Graph.cpp
//
//
//  Created by Mordekaiser on 17/3/5.
//
//

#include "Graph.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

Graph::Graph(vector<vector<int>>& graph_param) :  numVertexs( graph_param[0][0] ), edges(graph_param[0][1]),
numConsumes( graph_param[0][2]),    cost_per_service( graph_param[1][0] ),
contains(numVertexs, vector<int>(numVertexs, MYINFINITY)),
single_price(numVertexs, vector<int>(numVertexs, MYINFINITY)),
VertexDu(numVertexs, 0),
//VertexFlow(numVertexs, 0), flagComsume(numVertexs, 0),
//PathMatrix(numVertexs, vector<int>(numVertexs, MYINFINITY)),    //存储路径
//ShortPathTable(numVertexs, vector<int>(numVertexs, MYINFINITY)), //最短距离矩阵（存储两两点之间的最短距离）
service_position(0)
{
    // 初始化邻接矩阵
    // graph_param[0][1]表示网络链路数量
    for (int i = 2 ; i < graph_param[0][1] + 2; i++) {
        // graph_param[i][2]表示容量
        // graph_param[i][3]表示单价
        contains[graph_param[i][0]][graph_param[i][1]] = graph_param[i][2];
        contains[graph_param[i][1]][graph_param[i][0]] = graph_param[i][2];
        
      //  VertexFlow[graph_param[i][0]] += graph_param[i][2];
      //  VertexFlow[graph_param[i][1]] += graph_param[i][2];
        
        
        single_price[graph_param[i][0]][graph_param[i][1] ] = graph_param[i][3];
        single_price[graph_param[i][1]][graph_param[i][0] ] = graph_param[i][3];
        VertexDu[graph_param[i][0]]++;
        VertexDu[graph_param[i][1]]++;
        // ShortPathTable[graph_param[i][0]][graph_param[i][1] ] = graph_param[i][3];
        // ShortPathTable[graph_param[i][1]][graph_param[i][0] ] = graph_param[i][3];
    }
    
    serviceLevels = graph_param.size() - 1 - 1 -
    graph_param[0][1] - graph_param[0][2] - 1;
    
    int tempsize = graph_param.size();
    int tempLevels = serviceLevels;
    vector<int> temp;
    int cost_performance;
    int i = 0;
    while (tempLevels) {
        // 每行表示:流量-价格-性价比-其他
        temp.push_back(graph_param[tempsize - 1 - tempLevels][1]);
        temp.push_back(graph_param[tempsize - 1 - tempLevels][2]);
        
        if(tempLevels == serviceLevels){
            temp.push_back(temp[1]/temp[0]);
            temp.push_back(graph_param[tempsize - 1 - tempLevels][1]);
        }
        
        if(tempLevels < serviceLevels){
            cost_performance = (temp[1]-level2flow[i-1][1])/(temp[0]-level2flow[i-1][0]);
            if(cost_performance > level2flow[i-1][2]){
                temp.push_back(cost_performance);
                
            }
            else
                temp.push_back(level2flow[i-1][2] + 1);
            
            temp.push_back(temp[0]-level2flow[i-1][0]);
        }
        level2flow.push_back(temp);
        temp.clear();
        tempLevels--;
        i++;
        
    }
    
    
    
    int line_number = graph_param[0][1] + 2;
    for (int i = 0; i < numConsumes; i++) {
        consume_position.insert(std::make_pair( graph_param[line_number][0], std::make_pair(graph_param[line_number][1], graph_param[line_number][2] )));
       // flagComsume[graph_param[line_number][1]]++;
       // VertexFlow[graph_param[line_number][1]] += graph_param[line_number][2];
        // std::cout << graph_param[line_number][1] << std::endl;
        line_number++;
    }
    
    for (auto i = consume_position.begin(); i != consume_position.end(); i++) {  // 遍历每一个消费点
        if (NetNodeNeed.find(i->second.first) != NetNodeNeed.end())    // 也就是说出现了多个消费点连接一个网络点
            NetNodeNeed[i->second.first] += i->second.second;
        else
            NetNodeNeed[i->second.first] = i->second.second;
    }
    
}

void Graph::ShortestPath_Floyd() {
    // 初始化PathMatrix
    for (int i = 0; i < numVertexs; i++)
        for (int j = 0; j < numVertexs; j++)
            PathMatrix[i][j] = j;
    
    for (int k = 0; k < numVertexs; k++)
        for (int v = 0; v < numVertexs; v++)
            for (int w = 0; w < numVertexs; ++w)
                if (ShortPathTable[v][w] >  (ShortPathTable[v][k] + ShortPathTable[k][w]) ) {
                    ShortPathTable[v][w] = ShortPathTable[v][k] + ShortPathTable[k][w];
                    PathMatrix[v][w] = PathMatrix[v][k];
                }
    /*
     int k;
     
     for (auto temp = NetNodeNeed.begin(); temp != NetNodeNeed.end(); temp++) {
     int mindistance = MYINFINITY;
     int w;
     for (auto each = NetNodeNeed.begin(); each != NetNodeNeed.end(); each++) {
     if (temp->first == each->first)
     continue;
     else {
     if (mindistance > ShortPathTable[temp->first][each->first]) {
     w = each->first;
     mindistance = ShortPathTable[temp->first][each->first];
     //cout << w << endl;
     }
     }
     }
     
     //cout << temp->first << " To " << w << endl;
     // 不要消费点？还是要？暂时不要把
     ServicePositionByFolyd.push_back(temp->first);
     k = PathMatrix[temp->first][w];
     //cout << temp ->first << " "<< k;
     while (k != w) {
     ServicePositionByFolyd.push_back(k);
     k = PathMatrix[k][w];
     //cout << " " << k;
     }
     //cout << " " << w << endl;
     ServicePositionByFolyd.push_back(w);
     
     }
     
     std::sort(ServicePositionByFolyd.begin(), ServicePositionByFolyd.end());
     ServicePositionByFolyd.erase(std::unique(ServicePositionByFolyd.begin(), ServicePositionByFolyd.end()),
     ServicePositionByFolyd.end());
     */
}
