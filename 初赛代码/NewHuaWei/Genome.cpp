//
//  Genome.cpp
//  
//
//  Created by Mordekaiser on 17/3/14.
//
//

#include <algorithm>
#include "Genome.h"
#include "MiniCostFlow.h"
#include <random>
#include "solveAlgorithm.h"
#include <memory>


using namespace std;
extern MiniCostFlow MCF;
extern vector<vector<int>> graph_param;
// 构造函数这里会修改graph，添加服务器位置----------
Genome::Genome() {
    
    _generations = 0;
    
}

void Genome::GA(Graph& graph) {
    // 注意，执行任何一个操作都不能导致个体DNA的大小为0！！！！
    time_t time_start, time_stop;
    time_start = time(nullptr);
    
    max_service_size = graph.NetNodeNeed.size();
    
    int iters = 0;
    int BestCost = MYINFINITY;
    int flag = 0;
    while (iters < MAX_ITERATERS) {
        //cout << "----------------------------" << endl;
        cout << "iters: " << iters << endl;
        //cout << "----------------------------" << endl;
        iters++;

        // 选择出两个最优的个体，然后进行，交叉，变异，遗传
        // 选择出来的两个最优个体直接修改Genome的成员，不返回值
        VARIATION_PROB += 0.002;
        select(graph);  // 选择前50%个体,存储到_son_populations成员中
        
        time_stop = time(nullptr);
        long costtime = time_stop - time_start;
        cout << "cost time : " << costtime << endl;
        cout << "The Best Individual size is " << _BestIndividual._DNA.size() << " : "<< _BestIndividual._fittness << endl << endl;
        
        
        if (_BestIndividual._fittness < BestCost) {
            BestCost = _BestIndividual._fittness;
            flag = 0;
        }
        if (_BestIndividual._fittness == BestCost) {
            flag++;

            if (flag >= 200) {
                graph.service_position.clear();
                for (auto i : _BestIndividual._DNA)
                    graph.service_position.push_back(i);
                cout << "Time is : " << time_stop - time_start << endl;
                break;
            }
        }
        
        
        if (time_stop - time_start > MAX_ITER_TIME || iters >= MAX_ITERATERS) {
            graph.service_position.clear();
            for (auto i : _BestIndividual._DNA)
                graph.service_position.push_back(i);
            cout << "Time is : " << time_stop - time_start << endl;
            break;
        }
        intersect(_populations); // 前50%个体交叉, 交叉后个体的DNA片段将会改变

        variation(_populations);
        
        
    }
}

void Genome::select(Graph& graph) {
    //srand(0);
    if (_generations == 0) {
        _generations++;
        std::list<int> service_connect_consume;
        for (auto ii : graph.NetNodeNeed)
            service_connect_consume.push_back(ii.first);
        
        //random_shuffle(service_connect_consume.begin(), service_connect_consume.end());
        
        // 获取服务器位置，个数，并将bool型数组换成int，方便后面计算
        solveAlgorithm tempsol;
//        vector<bool> servicePositionTemp = tempsol.getServicePosition_GA(graph);
//        
//        for (int j = 0; j < servicePositionTemp.size(); j++)
//            if (servicePositionTemp[j] == true)
//                _servicePosition.push_back(j);
        
        
        // 初始化在消费点周围！
        
        vector<int> servicePositionInConsume = tempsol.BFS1(graph, 1);  //有一半初始化在消费点周围的两层，另一半随机初始化
        sort(servicePositionInConsume.begin(), servicePositionInConsume.end());
        servicePositionInConsume.erase(unique(servicePositionInConsume.begin(), servicePositionInConsume.end()), servicePositionInConsume.end());
        
        
        vector<int> servicePositionInConsume2 = tempsol.BFS1(graph, 2); // 解空间为广度优先两层以内的点
        sort(servicePositionInConsume2.begin(), servicePositionInConsume2.end());
        servicePositionInConsume2.erase(unique(servicePositionInConsume2.begin(), servicePositionInConsume2.end()), servicePositionInConsume2.end());
        
        //graph.servicePositionInConsume = _servicePosition;
        //graph.servicePositionInConsume = servicePositionInConsume; // 初始化解空间
        if (graph.numVertexs < 200)
            _servicePosition = servicePositionInConsume2;   // 解空间
        else
            _servicePosition = servicePositionInConsume;
        
        
        _populations = MCF.getInitialPopulation_Improve(graph, graph_param, servicePositionInConsume);  // 获取初始种群
        
        int temp = 10;
        _populations.sort(compare);
        auto tempiter1 = _populations.begin();
        while (temp) {
            tempiter1++;
            temp--;
        }
        _populations.erase( tempiter1, _populations.end());
        
        int numService;
        if (graph.numVertexs < 200) {
            numService = 120;
            VARIATION_PROB += 0.3;
        }
        else if (graph.numVertexs < 400) {
            numService = 200;
            VARIATION_PROB += 0.2;
        }
        else
            numService = 400;
        
        
        vector<int> randService = tempsol.rand_m(numService , graph.numVertexs);
        auto tempPopulation2 = MCF.getInitialPopulation_Improve(graph, graph_param, randService);
        temp = 10;
        tempPopulation2.sort(compare);
        auto tempiter2 = tempPopulation2.begin();
        while (temp) {
            tempiter2++;
            temp--;
        }
        tempPopulation2.erase(tempiter2, tempPopulation2.end());
//
//        vector<int> randService3 = tempsol.rand_m(numService , graph.numVertexs);
//        auto tempPopulation3 = MCF.getInitialPopulation_Improve(graph, graph_param, randService3);
//        temp = 5;
//        tempPopulation3.sort(compare);
//        auto tempiter3 = tempPopulation3.begin();
//        while (temp) {
//            tempiter3++;
//            temp--;
//        }
//        tempPopulation3.erase(tempiter3, tempPopulation3.end());
//        
//        _populations = tempPopulation1;
        _populations.merge(tempPopulation2, compare);
//        _populations.merge(tempPopulation3, compare);
        _BestIndividual = *(_populations.begin());
        _numIndividualsFromFather = 20;
        return;
// 以前的初始化方法，直接一层初始化
//        zkw_start(graph, graph_param);
//        _populations.sort(compare);
//        _generations++;
//        _numIndividualsFromFather = int(MAX_INDIVIDUALS_PER_POPULATION * GET_PERCENTAGE_INDIVIDUAL);
//        
//        int temp = _numIndividualsFromFather;
//        auto tempiter = _populations.begin();
//        while (temp) {
//            tempiter++;
//            temp--;
//        }
//        _populations.erase(tempiter, _populations.end());
//        _BestIndividual = *(_populations.begin());
//        return;
    }   // 如果是初代，则进行初始化种群
    
    
    // 否则，之间运行下面代码
    // 计算种群中各个个体的适应度
    for (auto  temp = _populations.begin(); temp != _populations.end(); temp++) {
        //cout << "Individual " << tempcount++  << " : " << temp->_DNA.size() << endl;
        getFittness(*temp, graph);
        //cout << "costAllConsumes: " << temp->_fittness << endl << endl;
        //sumOfFittness += _populations.at(i)._fittness;
    }
    
    // 寻找代价前20%的个体（代价越小，越前）
    _populations.sort(compare);
    
    _BestIndividual = *(_populations.begin());

}// 选择过程



void Genome::intersect(list<Individual>& populations) {
    
    auto fatherBegin = populations.begin();
    auto fatherEnd = populations.end();
    fatherEnd--;

    int numOfIntersect = MAX_INDIVIDUALS_PER_POPULATION/2 - 1;
    while (numOfIntersect) {
        numOfIntersect--;
        int changePos1 = rand() % (_numIndividualsFromFather);
        int changePos2 = rand() % (_numIndividualsFromFather);
        auto firstIter = populations.begin();   // 指向要交换的第一个个体

        while (changePos1) {
            firstIter++;
            changePos1--;
        }
        auto secondIter = populations.begin();   // 指向要交换的第二个个体
        while (changePos2) {
            secondIter++;
            changePos2--;
        }
        
        Individual individual1 = *firstIter;
    
        Individual individual2 = *secondIter;


        if ( (rand() % 1000)/1000.0 > CROSS_PORB ) {   // 交叉概率为20%
            
            _populations.push_back(individual1);
            _populations.push_back(individual2);
            continue;
        }
        // 最大交叉大小为DNA长度的1/3, 最小交叉大小为0，即不交叉
        pair<int, int> intersec1 = getRandomSection(individual1, individual1._DNA.size() * 1 / 3 );
        pair<int, int> intersec2 = getRandomSection(individual2, individual2._DNA.size() * 1 / 3 );
        
        // 选取DNA片段,注意，这里会删除divididual1的部分元素，这样为了方便后面交叉操作
        list<int> firstSec;
        copy_delete_DNASec(individual1._DNA, intersec1.first, intersec1.second, firstSec);
        list<int> secondSec;
        copy_delete_DNASec(individual2._DNA, intersec2.first, intersec2.second, secondSec);
        
        for (auto i = firstSec.begin(); i != firstSec.end(); ) {
            if (find(secondIter->_DNA.begin(), secondIter->_DNA.end(), *i) != secondIter->_DNA.end() ) {
                // 如果firstSec里面的第i个元素在*secondIter里面，则说明是冲突片段，应该删除
                firstSec.erase(i++);
            }
            else
                i++;
        }
        
        for (auto i = secondSec.begin(); i != secondSec.end(); ) {
            if (find(firstIter->_DNA.begin(), firstIter->_DNA.end(), *i) != firstIter->_DNA.end() ) {
                // 如果secondSec里面的第i个元素在*firstIter里面，则说明是冲突片段，应该删除
                secondSec.erase(i++);
            }
            else
                i++;
        }


        
        // DNA交叉，先删除原有片段
        for (auto i : firstSec) {
            if (individual2._DNA.size() >= max_service_size)
                break;
            individual1._DNA.erase( find(individual1._DNA.begin(), individual1._DNA.end(), i) );
            individual2._DNA.push_back(i);
        }
        for (auto i : secondSec) {// 在individual1中找不到secondSec的元素，就push_back
            if (individual1._DNA.size() >= max_service_size)
                break;
            individual2._DNA.erase( find(individual2._DNA.begin(), individual2._DNA.end(), i) );
            individual1._DNA.push_back(i);
        }
        
        populations.push_back(individual1);
        populations.push_back(individual2);
    }
    
    
    populations.push_back(*fatherBegin);
    fatherBegin++;
    populations.push_back(*fatherBegin);
    // 删除前面的父代的元素
    fatherBegin--;
    populations.erase(fatherBegin, ++fatherEnd);
    
    return;
    
}   // 交叉算法

// 变异算法
void Genome::variation(list<Individual>& populations) {
    
    /*
    for(auto i = populations.begin(); i != populations.end(); i++)
    {
        double ran=rand()%1000/1000.0;
        if(ran>= VARIATION_PROB)
            continue;
         
        int newDNA;
        while(1)
        {
            newDNA=_servicePosition[ rand()%_servicePosition.size() ];
            if( find(i->_DNA.begin(), i->_DNA.end(), newDNA) == i->_DNA.end())
                break;  // 找不到就说明不是冲突基因
            // 直接把变异操作改成重新随机初始化或者会更好？
        }
        
        // 找出变异元素位置
        int varPos = rand() % i->_DNA.size();
        auto deletedPos = i->_DNA.begin();
        while (varPos) {
            deletedPos++;
            varPos--;
        }
        *deletedPos = newDNA;  // 替换变异元素
    }
    */
    
    int numOfVaritions = MAX_INDIVIDUALS_PER_POPULATION - 2;//最后两个最优秀的不变
    auto individual = populations.begin();
    while (numOfVaritions) {
        
        numOfVaritions--;
        if ( (rand() % 1000)/1000.0 > VARIATION_PROB)  {   //变异概率为0.6
            continue;
        }
    
        int flag = rand() % 3;
        if (flag == 2) { // flag为1表示变异操作为随机增加服务器
            // 增加的服务器数量后面除以10 防止增加太多
            int increased_service = 1;
            int NumDNA = 0;
            while (NumDNA < increased_service) {
                int newDNASegment = _servicePosition[ rand() % _servicePosition.size() ];
                if (find(individual->_DNA.begin(), individual->_DNA.end(), newDNASegment) == individual->_DNA.end()) {
                    individual->_DNA.push_back(newDNASegment);
                    //cout << "increase: " << newDNASegment << endl;
                    NumDNA++;
                }
            }
        }
        
        else if (flag == 0) {  // flag为0，则变异操作为随机减少服务器
            int decreased_service;
        
                // 减少
            decreased_service  = 1;
            
            
            int NumDNA = 0;
            
            while (NumDNA < decreased_service) {
                int decreasedPos = rand() % individual->_DNA.size();
                auto decreasedIter = individual->_DNA.begin();
                while(decreasedPos) {
                    decreasedIter++;
                    decreasedPos--;
                }
                individual->_DNA.erase(decreasedIter);
                NumDNA++;
            }
            
        }
        
        else {   // flag为2,则进行突变，即随机选取部分服务器位置进行随机替换。可以理解为适当的加入扰动
            int numOfChange = 1;// 改变的服务器不超过现在的1 / 4
            int NumDNA = 0;
            int changePosition;
            int changeSerive;
            while (NumDNA < numOfChange) {
                changePosition = rand() % individual->_DNA.size();
                
                changeSerive = _servicePosition [rand() % _servicePosition.size()];
                auto changeIter = individual->_DNA.begin();
                while(changePosition) {
                    changeIter++;
                    changePosition--;
                }
                *changeIter = changeSerive;
                NumDNA++;
            }
        }
        individual++;
    }
    
    return;
}

void Genome::climbMountain(Graph& graph) {
    time_t time_start, time_stop;
    time_start = time(nullptr);
    
    int BestCost;
    
    int iters = 0;
    graph.service_position.clear();
    for (auto temp : graph.NetNodeNeed)
        graph.service_position.push_back(temp.first);
    vector<int> BestDNA;
    BestDNA = graph.service_position;
    BestCost = graph.service_position.size() * graph.cost_per_service;
    
    while (iters < MAX_ITERATERS) {
        cout << "iters: " << iters << endl;
        iters++;
        
        vector<int> nextDNA;
        
        nextPath(BestDNA, nextDNA, graph);
        graph.service_position = nextDNA;
        
        int cost = MCF.solve(graph, graph_param);
        if (cost < BestCost) {
            BestCost = cost;
            BestDNA = nextDNA;
        }
        cout << "Best cost is " << BestCost << endl << endl;
    }
    
    time_stop = time(nullptr);
    int costtime = time_stop - time_start;
    cout << "cost time : " << costtime << endl;
    
    graph.service_position = BestDNA;
    
}

void Genome::nextPath(vector<int>& BestDNA ,vector<int>& NextDNA, Graph& graph){
    // 思路：保留99.6%的元素，然后随机产生元素，进行替换剩余元素或者直接丢弃剩余元素，或者丢弃后
    // 在后面加上新元素
    int k=0,i,j;
    for(i=0;i < BestDNA.size();++i)    // 遍历DNA里面的每一个元素
    {
        double ran = rand()%1000/1000.0;;
        if(ran<0.996)
        {
            NextDNA.push_back(BestDNA[i]) ;
            k++;
            continue;
        }
    }
    int numVarition=rand()%2;
    for(i=k;i<k+numVarition;++i)
    {
        bool hasSame = false;
        int randService = rand()% graph.numVertexs;
        for(j=0;j<i;++j)
        {
            if (randService == BestDNA[j]) {
                hasSame = true;
                break;
            }
        }
        
        if (hasSame)
            continue;
        NextDNA.push_back(randService);
    }
}
    


// graph必须值传递,因为每次求完一个个体的适应度后，graph必须还原
void Genome::getFittness(Individual& individual,Graph& graph) {
    graph.service_position.clear();
    for (auto i : individual._DNA)
        graph.service_position.push_back(i);
    solveAlgorithm tempsol;
    
    std::ostringstream buf;
    sort(graph.service_position.begin(), graph.service_position.end());
    for (auto i : graph.service_position)
        buf << i << "-1";//每个服务器位置用-1隔开，这样就避免了重复问题
    
    if (ServiceToCost.find(buf.str()) != ServiceToCost.end()) {
        int Fittness = ServiceToCost[buf.str()];
        individual._fittness = Fittness;
        return;
    }
    
    individual._fittness =  tempsol.solver(graph);
    ServiceToCost[buf.str()] = individual._fittness;
    //cout << "costAllConsumes: " << individual._fittness << endl << endl;
   
    return;
}


// 消除DNA片段的冲突
vector<int> removeDepuInDNA(const vector<int>& firstDNA, const vector<int>& secDNA) {
    vector<int> result;
    for (size_t i = 0; i < firstDNA.size(); i++)
        if (find(secDNA.begin(), secDNA.end(), firstDNA.at(i)) == secDNA.end() )
            // 如果firstDNA里面的第i个元素不在secDNA里面，则说明不是冲突片段，应该保存
            result.push_back(firstDNA.at(i));

    return result;
}


void Genome::copy_delete_DNASec(list<int>& srcDNA, int minPos, int maxPos, list<int>& destDNASec) {
    int length = maxPos - minPos;
    if (length == 0)
        return;
    auto tempiter = srcDNA.begin();
    while (minPos) {
        tempiter++;
        minPos--;
    }
    
    while (length ) {
        destDNASec.push_back(*tempiter);
        tempiter++;
        length--;
    }
}


std::pair<int, int> Genome::getRandomSection(const Individual& individual ,int max_intersect_length){
    // 交叉区间不能超过DNA的长度
    //srand(0);
    int minPos = rand() % individual._DNA.size();
    int maxPos = rand() % individual._DNA.size();
    if (maxPos < minPos)
        swap(maxPos, minPos);
    
    // 保证区间左端点小于右端点, 且长度不能超过最大交叉长度
    while ((maxPos - minPos) > max_intersect_length && (maxPos - minPos) < 0 ) {
        maxPos = rand() % individual._DNA.size();
        if (maxPos < minPos)
            swap(maxPos, minPos);
    }
    
    return make_pair(minPos, maxPos);
} // 获取交叉区间，参数为可接受的最长的交叉区间

bool Genome::isSameIndividual(Individual& individual1, Individual& individual2) {
    if (individual1._DNA.size() != individual2._DNA.size())
        return false;
    auto iter1 = individual1._DNA.begin();
    auto iter2 = individual2._DNA.begin();;
    for (; iter1 != individual1._DNA.end(); iter1++, iter2++)
        if (*iter1 != *iter2)
            return false;
    
    return true;
}
