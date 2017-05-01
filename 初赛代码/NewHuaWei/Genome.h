//
//  Genome.h
//  
//
//  Created by Mordekaiser on 17/3/14.
//
//
// 注意：变异概率可以大一些，但是每代择优的比例一定要少

#ifndef ____Genome__
#define ____Genome__

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include "Graph.h"

// 以下这些参数不要轻易修改，已调整到较合适的值
const int MAX_INDIVIDUALS_PER_POPULATION = 80;   // 一个种群里面最多包含的个体数目
const int MAX_ITERATERS = 2500;   // 最大迭代次数

const int MAX_ITER_TIME = 80;

using std::vector;


class Genome {

public:
    struct Individual {
        
        int _fittness;
        std::list<int> _DNA;
    };

    // 单个个体，_DNA成员代表：服务器位置,个数， _fittness成员代表适应度    
    std::list<Individual> _populations;    // 父代种群
    //vector<Individual> _son_populations; // 子代种群, 注意，用的是list，方便插入删除
    
    
    int _generations;
    
    vector<int> _servicePosition;   // 候选服务器位置，初始化时用调用solveAlgorithm里面的getServicePosition_GA()函数
    
    int max_service_size;
    
    std::unordered_map<std::string, int> ServiceToCost; // 存储每个个体的DNA对应的cost，每次计算都存下来，下次需要直接取
    int _numIndividualsFromFather;
    
public:
    float CROSS_PORB = 0.8;     // 交叉概率
    float VARIATION_PROB = 0.4;     // 变异概率
    float GET_PERCENTAGE_INDIVIDUAL = 0.5; // 每次获取前15%的优秀个体

    Genome();
    Individual _BestIndividual; // 当代最优个体

    
    void GA(Graph& graph); // 遗传算法求解的主函数
    
    void select(Graph& graph); // 选择过程, 选择出来的个体直接覆盖类成员，不返回！
    void variation(std::list<Individual>& populations ); // 变异过程
    void intersect(std::list<Individual>& populations); // 交叉过程
    void nextPath(vector<int>& BestDNA ,vector<int>& NextDNA, Graph& graph);
    void climbMountain(Graph& graph);
    
    
    // 计算适应度函数，应该调用solveAlgorithm类里面的solver函数
    // 注意，必须值传递，不能修改graph
    void getFittness(Individual& individual, Graph& graph);
    bool isSameIndividual(Individual& individual1, Individual& individual2); // 比较两个个体是否相同
    
    std::pair<int, int> getRandomSection(const Individual& individual, int max_intersect_length); // 获取交叉区间，参数为可接受的最长的交叉区间
    
    // 拷贝srcDNA片段中的部分DNA到destDNASec
    void copy_delete_DNASec(std::list<int>& srcDNA, int minPos, int maxPos, std::list<int>& destDNASec);
    
    // 消除DNA片段的冲突
    vector<int> removeDepuInDNA(const vector<int>& firstDNA, const vector<int>& secDNA);
    static bool compare(Individual& individual1, Individual& individual2) {
        return individual1._fittness < individual2._fittness;
        
    }
    
};


#endif /* defined(____Genome__) */
