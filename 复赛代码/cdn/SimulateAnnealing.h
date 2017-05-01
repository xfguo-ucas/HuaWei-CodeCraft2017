//
//  SimulateAnnealing.h
//  
//
//  Created by Mordekaiser on 17/4/21.
//
//

#ifndef ____SimulateAnnealing__
#define ____SimulateAnnealing__

#include <stdio.h>
#include "Graph.h"
#include <vector>

#include <string>
#include <set>
using std::vector;
class SimulateAnnealing {
private:
    double initialTemper;
    int step;   // 降温步长，每step步就降温
    double decreaseRate; // 降温比率
    struct service{
        std::set<int> Position;
        int cost;
    };
    
    int MAX_ITER_TIME = 85;
    int minTemper;
    std::unordered_map<std::string, int> _ServiceToCost; // 存储每个个体的DNA对应的cost，每次计算都存下来，下次需要直接取
    vector<int> _servicePostion;
    
    vector<int > Normalize;//解空间一，度大于一
    vector<double> NormalizeDu;//解空间1，对应的轮盘梯度
    
    vector<int > consuNodeneed;//解空间一，度大于一
    vector<double> consuNodeneedDu;//解空间1，对应的轮盘梯度
    vector<int> NodePrice;
    vector<double> NodePriceDu;
    
    
public:
    service _BestService;
    void nextPath_MAXDU(service& curService ,service& nextService, Graph& graph);
    void climbMountain(Graph& graph);
    int LunPanSelect(vector<double>& Lunpan, vector<int>& value);
    void SA(Graph& graph);
    int getCost(service& nextService, Graph& graph);
    void nextPath_Consume(service& curService, service& nextService, Graph& graph);
    void nextPath_Price(service& curService, service& nextService, Graph& graph);
};
#endif /* defined(____SimulateAnnealing__) */
