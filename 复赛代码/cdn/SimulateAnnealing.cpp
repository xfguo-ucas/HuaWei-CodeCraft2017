//
//  SimulateAnnealing.cpp
//  
//
//  Created by Mordekaiser on 17/4/21.
//
//

#include "SimulateAnnealing.h"
#include <vector>
#include <random>
#include "Graph.h"
#include "MiniCostFlow.h"
#include "deploy.h"
#include "solveAlgorithm.h"
#include <algorithm>
extern MiniCostFlow MCF;
extern MiniCostFlow MCF2;
extern vector<vector<int>> graph_param;
solveAlgorithm sol;
using namespace std;

void SimulateAnnealing::SA(Graph& graph){
    

    time_t startTime, endTime;
    
    solveAlgorithm tempsol;
    vector<int> servicePositionInConsume,randService;
    
    servicePositionInConsume =tempsol. BFS1( graph,3);
    sort(servicePositionInConsume.begin(), servicePositionInConsume.end());
    servicePositionInConsume.erase(unique(servicePositionInConsume.begin(), servicePositionInConsume.end()), servicePositionInConsume.end());
    for (int i:servicePositionInConsume) {
        int flag = 0,flag1=0;
        for (int j = 0; j < graph.numVertexs; j++) {
            if (graph.contains[i][j] != MYINFINITY) {
                flag1++;
                flag+=graph.contains[i][j];
                if (flag >= 50&&flag1>=3) {
                    randService.push_back(i);
                    break;
                }
            }
        }
    }
    
    
    

    
    
    startTime = time(nullptr);
    endTime = time(nullptr);
    int numService;
    //int MAX_FLOW;
    //int CONSUME_MAX_FLOW;
    if (graph.numVertexs > 800) {
        initialTemper = 0;
        step = 400;
        decreaseRate = 0.97;
        minTemper = 1;
        numService = 800;
        //VarationAfterIters = 100000;  // 1000次之后只有变异操作
        //MAX_FLOW = 200;
        //CONSUME_MAX_FLOW = 50;
    }
    else {
        initialTemper = 0;
        step = 500;
        decreaseRate = 0.99;
        minTemper = 1;
        numService = 300;
        //VarationAfterIters = 6000000;  // 1000次之后只有变异操作
        //MAX_FLOW = 50;
        //CONSUME_MAX_FLOW = 20;
    }
    // --- 计算解空间---
    double AllDu = 0;
    for (int i =0; i < graph.numVertexs; i++) {
        if (graph.VertexDu[i] > 1){
            Normalize.push_back(i);
            AllDu += graph.VertexDu[i];
        }
    }
    
    double addDu = 0;
    for (int i = 0;i < Normalize.size(); i++) {
        NormalizeDu.push_back(pow(graph.VertexDu[Normalize[i]]/AllDu,1));
        addDu += NormalizeDu[i];
    }
    
    NormalizeDu[0] = NormalizeDu[0] / addDu;
    for(int i = 1; i < NormalizeDu.size(); i++){
        NormalizeDu[i] = NormalizeDu[i] / addDu;
        NormalizeDu[i] += NormalizeDu[i - 1];
    }
    
    double allNeed = 0;
    for (int i =0; i < graph.numConsumes; i++) {
        
        consuNodeneed.push_back(graph.consume_position[i].first);
        allNeed += graph.consume_position[i].second;
        
    }
    
    double addConsuNeedu = 0;
    
    for (int i = 0;i < consuNodeneed.size(); i++) {
        consuNodeneedDu.push_back(pow(graph.consume_position[i].second/allNeed,1));
        addConsuNeedu += consuNodeneedDu[i];
    }
    
    consuNodeneedDu[0] = consuNodeneedDu[0] / addConsuNeedu;
    for(int i = 1; i < consuNodeneedDu.size(); i++){
        consuNodeneedDu[i] = consuNodeneedDu[i] / addConsuNeedu;
        consuNodeneedDu[i] += consuNodeneedDu[i - 1];
        
    }
    
    //---便宜的点
    int temp_size = graph_param.size();
    
    double allPrice = 0;
    for(int i = 0; i < graph.numVertexs; i++){
        NodePrice.push_back(i);
    }
    vector<double> NodePrice_t;
    for(int i = 0; i < graph.numVertexs; i++){
        NodePrice_t.push_back(1/(double)(graph_param[temp_size-1][i]));
        allPrice += NodePrice_t[i];
    }
    double addPricedu = 0;
    for(int i = 0;i < NodePrice.size(); i++){
        NodePriceDu.push_back(pow(NodePrice_t[i]/allPrice, 1));
        addPricedu += NodePriceDu[i];
    }
    NodePriceDu[0] = NodePriceDu[0] / addPricedu;
    for(int i = 1; i < NodePriceDu.size(); i++){
        NodePriceDu[i] = NodePriceDu[i] / addPricedu;
        NodePriceDu[i] += NodePriceDu[i -1];
    }
    
    double temperature = initialTemper;
    service curService, nextService, BestService;
    //vector<int> randService = sol.rand_m(numService, graph.numVertexs);
    auto InitialService = MCF.getInitialPopulation_SA(graph, graph_param, randService);
    for (auto i : InitialService) {
        curService.Position.insert(i);
    }

    int Cost = getCost(curService, graph);
    curService.cost = Cost;
    BestService = curService;
    
    int delta;
    int iter = 0;
    int FLAG = 0;
    while (1) {
        curService = BestService;
        for (int i = 0; i < step; i++) {
        
            if (endTime - startTime > MAX_ITER_TIME) {
                graph.service_position.clear();
                _BestService = BestService;
                for (auto i : _BestService.Position)
                    graph.service_position.push_back(i);
                return;
            }
            int randNUM = rand() % 1000;
            if (randNUM < 450)
                nextPath_MAXDU(curService, nextService, graph); //
            else if (randNUM < 800)
                nextPath_Consume(curService, nextService, graph);
            else
                nextPath_Price(curService, nextService, graph);
            
            int Cost = getCost(nextService, graph);
            nextService.cost = Cost;
            
            delta = Cost - BestService.cost;
            
            if (FLAG > 500) {
                temperature = 10;
                FLAG = 0;
            }
            if (delta < 0) {
                curService = nextService;
                BestService = nextService;
                FLAG = 0;

            }
            else {
                double randNum = (rand() % 1000) / 1000.0;
                double PG = exp(-(delta)/(10 * temperature));
                if (PG > randNum) {
                    curService = nextService;
                }
                FLAG++;
            }
            temperature *= decreaseRate;
            endTime = time(nullptr);
            iter++;
           // cout << curService.cost << " " << iter << " " << temperature << " " << endTime - startTime <<  " " << curService.Position.size() << endl;
        }
        
        
//        if (temperature < 0.001) {
//            temperature = minTemper;
//        }
    }
    
    
}

int SimulateAnnealing::getCost(service& nextService, Graph& graph) {
    graph.service_position.clear();
    for (auto i = nextService.Position.begin(); i != nextService.Position.end(); i++)
        graph.service_position.push_back(*i);

    
    std::ostringstream buf;
    for (auto i : graph.service_position)
        buf << i << "-1";//每个服务器位置用-1隔开，这样就避免了重复问题
    
    if (_ServiceToCost.find(buf.str()) != _ServiceToCost.end()) {
        int Fittness = _ServiceToCost[buf.str()];
        return Fittness;
    }
    
    int Fittness =  sol.solver(graph);
    _ServiceToCost.insert(make_pair(buf.str(), Fittness));
    
    return Fittness;
}

void SimulateAnnealing::nextPath_Consume(service& curService, service& nextService, Graph& graph){
    nextService = curService;
    nextService.cost = MYINFINITY;
    int flag;

    flag = rand() % 3;
    
    if (flag == 0){  // 表示删除
        int tempdelete;
        set<int>::iterator tempiter;
        while (1) {
            // 删除点应该是度大的点或者流量小的。这样才有可能从别处补偿,如果实在容易死循环，就重新随机一个flag，避免此问题
            tempdelete = LunPanSelect(consuNodeneedDu, consuNodeneed);
            tempiter = nextService.Position.find(tempdelete);
            if ( tempiter != nextService.Position.end())
                break;
        }
        nextService.Position.erase(tempiter);
    }
    
    if (flag == 1){ // 增加服务器
        while (1) {
            int tempincrease = LunPanSelect(consuNodeneedDu, consuNodeneed);
            if (nextService.Position.insert(tempincrease).second)
                break;
        }
    }
    
    if (flag == 2){  // 变异服务器
        while (1) {
            int tempvarPos = rand() % nextService.Position.size();
            auto tempiter = nextService.Position.begin();
            while (tempvarPos) {
                tempiter++;
                tempvarPos--;
            }
            int tempvar = LunPanSelect(consuNodeneedDu, consuNodeneed);
            if (graph.contains[*tempiter][tempvar] != MYINFINITY) {
                if (nextService.Position.insert(tempvar).second){  //新生成的不和以前的冲突
                    nextService.Position.erase(*tempiter);
                    break;
                }
            }
        }
    }
}

void SimulateAnnealing::nextPath_MAXDU(service& curService ,service& nextService, Graph& graph){
    // 思路：保留99.6%的元素，然后随机产生元素，进行替换剩余元素或者直接丢弃剩余元素，或者丢弃后
    // 在后面加上新元素
    nextService = curService;
    nextService.cost = MYINFINITY;
    int flag;

    flag = rand() % 3;
    
    if (flag == 0){  // 表示删除
        int tempdelete;
        set<int>::iterator tempiter;
        while (1) {
            // 删除点应该是度大的点或者流量小的。这样才有可能从别处补偿,如果实在容易死循环，就重新随机一个flag，避免此问题
            tempdelete = LunPanSelect(NormalizeDu, Normalize);
            tempiter = nextService.Position.find(tempdelete);
            if ( tempiter != nextService.Position.end())
                break;
        }
        nextService.Position.erase(tempiter);
    }
    
    if (flag == 1){ // 增加服务器
        while (1) {
            int tempincrease = LunPanSelect(NormalizeDu, Normalize);
            if (nextService.Position.insert(tempincrease).second)
                break;
        }
    }
    
    if (flag == 2){  // 变异服务器
        while (1) {
            int tempvarPos = rand() % nextService.Position.size();
            auto tempiter = nextService.Position.begin();
            while (tempvarPos) {
                tempiter++;
                tempvarPos--;
            }
            int tempvar = LunPanSelect(NormalizeDu, Normalize);
            if (graph.contains[*tempiter][tempvar] != MYINFINITY) {
                if (nextService.Position.insert(tempvar).second){  //新生成的不和以前的冲突
                    nextService.Position.erase(*tempiter);
                    break;
                }
            }
        }
    }
   
}

void SimulateAnnealing::nextPath_Price(service& curService, service& nextService, Graph& graph){
    nextService = curService;
    nextService.cost = MYINFINITY;
    int flag;
    
    flag = rand() % 3;
    
    if (flag == 0){  // 表示删除
        int tempdelete;
        set<int>::iterator tempiter;
        while (1) {
            // 删除点应该是度大的点或者流量小的。这样才有可能从别处补偿,如果实在容易死循环，就重新随机一个flag，避免此问题
            tempdelete = LunPanSelect(NodePriceDu, NodePrice);
            tempiter = nextService.Position.find(tempdelete);
            if ( tempiter != nextService.Position.end())
                break;
        }
        nextService.Position.erase(tempiter);
    }
    
    if (flag == 1){ // 增加服务器
        while (1) {
            int tempincrease = LunPanSelect(NodePriceDu, NodePrice);
            if (nextService.Position.insert(tempincrease).second)
                break;
        }
    }
    
    if (flag == 2){  // 变异服务器
        while (1) {
            int tempvarPos = rand() % nextService.Position.size();
            auto tempiter = nextService.Position.begin();
            while (tempvarPos) {
                tempiter++;
                tempvarPos--;
            }
            int tempvar = LunPanSelect(NodePriceDu, NodePrice);
            if (graph.contains[*tempiter][tempvar] != MYINFINITY) {
                if (nextService.Position.insert(tempvar).second){  //新生成的不和以前的冲突
                    nextService.Position.erase(*tempiter);
                    break;
                }
            }
        }
    }
}



int SimulateAnnealing::LunPanSelect(vector<double>& LunPan, vector<int>& value){
    double randNum1 = (rand() % 1000)/1000.0;
    double sumPs = 0;
    int i = 0;
    while (LunPan[i] < randNum1) {
        i++;
    }
    return value[i];
}
