//
//  PSO.cpp
//  
//
//  Created by Mordekaiser on 17/4/5.
//
//


/*
 * 使用C语言实现粒子群算法(PSO)
 * 参考自《MATLAB智能算法30个案例分析》
 * update: 16/12/3
 * 本例的寻优非线性函数为
 * f(x,y) = sin(sqrt(x^2+y^2))/(sqrt(x^2+y^2)) + exp((cos(2*PI*x)+cos(2*PI*y))/2) - 2.71289
 * 该函数有很多局部极大值点，而极限位置为(0,0),在(0,0)附近取得极大值
 */
#include "PSO.h"
#include "MiniCostFlow.h"
#include<iostream>


#define c1 2 //加速度因子一般是根据大量实验所得
#define c2 2
#define dim 10000
#define maxgen 300  // 迭代次数
#define sizepop 100 // 种群规模
int ParticleDim,num=0;  // 粒子的维数
int popmax;  // 个体最大取值
int popmin;  // 个体最小取值
float Vmax;  // 速度最大值
float Vmin;  //速度最小值
float Vmax1;  // 速度最大值
float Vmin1;  //速度最小值
int pop[sizepop][dim]; // 定义种群数组
float V[sizepop][dim]; // 定义种群速度数组
int fitness[sizepop]; // 定义种群的适应度数组
int result[maxgen];  //定义存放每次迭代种群最优值的数组
int pbest[sizepop][dim];  // 个体极值的位置
int gbest[dim]; //群体极值的位置
int fitnesspbest[sizepop]; //个体极值适应度的值
int fitnessgbest; // 群体极值适应度值
int genbest[maxgen][dim]; //每一代最优值取值粒子


using namespace std;



extern MiniCostFlow MCF;
extern vector<vector<int>> graph_param;
//适应度函数
int getFittness(int x[],Graph& graph)
{
    //x是服务器的位置
    graph.service_position.clear();
    for (int i = 0; i < ParticleDim; i++)
        if (x[i] == 1)
            graph.service_position.push_back(i);
    
    return -MCF.solve(graph, graph_param);
    
    
}

// 种群初始化
void pop_init(Graph& graph)
{
    for(int i=0;i<sizepop;i++)
    {
        int service_size = (rand() % graph.NetNodeNeed.size()) + 1;
        int randService;
        while (service_size) {
            randService = rand() % ParticleDim;
            if (pop[i][randService] == 1)
                continue;
            pop[i][randService] = 1;
            V[i][randService] =(((double)rand())/RAND_MAX-0.5); //-0.5到0.5之间
            service_size--;
        }
        
        fitness[i] = getFittness(pop[i],graph); //计算适应度函数值
    }
    
    int * best_fit_index; // 用于存放群体极值和其位置(序号)
    best_fit_index = max(fitness,sizepop); //求群体极值
    int index = *best_fit_index;
    // 群体极值位置
    for(int i=0;i<ParticleDim;i++)
    {
        gbest[i] = pop[index][i];
    }
    // 个体极值位置
    for(int i=0;i<sizepop;i++)
    {
        for(int j=0;j<ParticleDim;j++)
        {
            pbest[i][j] = pop[i][j];
        }
    }
    // 个体极值适应度值
    for(int i=0;i<sizepop;i++)
    {
        fitnesspbest[i] = fitness[i];
    }
    //群体极值适应度值
    int bestfitness = *(best_fit_index+1);
    fitnessgbest = bestfitness;
    
}

// max()函数定义
int* max(int * fit,int size)
{
    int index = 0; // 初始化序号
    int max = *fit; // 初始化最大值为数组第一个元素
    static int best_fit_index[2];
    for(int i=1;i<size;i++)
    {
        if(*(fit+i) > max) {
            max = *(fit+i);
            index = i;
        }
    }
    best_fit_index[0] = index;
    best_fit_index[1] = max;
    return best_fit_index;
    
}

// 迭代寻优
void PSO_func(Graph& graph)
{
    pop_init(graph);

    //迭代寻优
    double temp;
    for(int i=0;i<maxgen;i++)
    {
        for(int j=0;j<sizepop;j++)
        {
            //速度更新及粒子更新
            for(int k=0;k<ParticleDim;k++)
            {
                // 速度更新
                float rand1 = (float)rand()/RAND_MAX; //0到1之间的随机数
                float rand2 = (float)rand()/RAND_MAX;
                V[j][k] = 0.729 * V[j][k] +( c1*rand1*(pbest[j][k]-pop[j][k])) + (c2*rand2*(gbest[k]-pop[j][k]));
                
                // 每个维度的更新几率限制在0.268~0.731之间
                if (V[j][k] > 1)
                    V[j][k] = 1;
                if (V[j][k] < -1)
                    V[j][k] = -1;
                
                temp =  1.0/(1 + exp(-V[j][k]));
                float randtemp = (float)rand()/RAND_MAX;
                if (randtemp < temp)
                    pop[j][k] = 1;
                else
                    pop[j][k] = 0;
   
                // 粒子更新
                /*
                pop[j][k] = pop[j][k] + V[j][k];
                if(pop[j][k] > popmax)
                    pop[j][k] = popmax;
                if(pop[j][k] < popmin)
                    pop[j][k] = popmin;
                 */
            }
            
            fitness[j] = getFittness(pop[j],graph); //新粒子的适应度值
            // std::cout<<std::endl<<"I:"<<num++<<std::endl;
        }
        for(int j=0;j<sizepop;j++)
        {
            // 个体极值更新
            if(fitness[j] > fitnesspbest[j])
            {
                for(int k=0;k<ParticleDim;k++)
                {
                    pbest[j][k] = pop[j][k];
                }
                fitnesspbest[j] = fitness[j];
            }
            // 群体极值更新
            if(fitness[j] > fitnessgbest)
            {
                for(int k=0;k<ParticleDim;k++)
                    gbest[k] = pop[j][k];
                fitnessgbest = fitness[j];
            }
        }
        for(int k=0;k<ParticleDim;k++)
        {
            genbest[i][k] = gbest[k]; // 每一代最优值取值粒子位置记录
        }
        result[i] = fitnessgbest; // 每代的最优值记录到数组
        std::cout << "iters " << i << " :  " << -fitnessgbest << std::endl;
    }
}

// 主函数
void optimize(Graph& graph)

{
    
    ParticleDim = graph.numVertexs;  // 粒子的维数
    popmax=1;  // 个体最大取值
    popmin=0;  // 个体最小取值
    Vmax=1;  // 速度最大值
    Vmin=-1;  //速度最小值
    //     Vmax1=2;  // 速度最大值
    //     Vmin1=-2;  //速度最小值
    // clock_t start,finish; //程序开始和结束时间
    //start = clock(); //开始计时
    //srand((unsigned)time(nullptr)); // 初始化随机数种子
    PSO_func(graph);
    int * best_arr;
    best_arr = max(result,maxgen);
    int best_gen_number = *best_arr; // 最优值所处的代数
    int best = *(best_arr+1); //最优值
    printf("%d    %d     :%d\n",maxgen,best_gen_number+1,-best);
    //    printf("取到最优值的位置为(%lf,%lf).\n",genbest[best_gen_number][0],genbest[best_gen_number][1]);
    // finish = clock(); //结束时间
    // int duration = (int)(finish - start)/CLOCKS_PER_SEC; // 程序运行时间
    //  printf("程序运行耗时:%lf\n",duration);
    //return 0;
}
