//
//  partGraph.cpp
//  
//
//  Created by Mordekaiser on 17/3/7.
//
//
#include "Graph.h"
#include <vector>

#include "partGraph.h"


partGraph::partGraph(const Graph& graph, int& _search_consume_position,
                     vector<int>& _service_position) : Graph(graph){
    // 派生类的实现，调用基类构造函数, 并初始化相应成员
    search_consume_position = _search_consume_position;// 搜索这个消费节点就可以得到一个小图
    service_position = _service_position;   // 得到的小图包含服务器的位置
    
    
}

