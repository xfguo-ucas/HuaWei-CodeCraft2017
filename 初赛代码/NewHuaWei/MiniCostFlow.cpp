//
//  MiniCostFlow.cpp
//  
//
//  Created by Mordekaiser on 17/4/3.
//
//

#include "MiniCostFlow.h"
#include "Genome.h"
#include <algorithm>

using namespace std;

void MiniCostFlow::initial(Graph& graph,vector<vector<int>> &graph_param){
    
    int i=0,j=0;

    NodeNum = graph.numVertexs;
    
    EdgeNum = graph.edges;
    GustNum = graph.consume_position.size();
    // 初始化图，针对普通网络节点
    for(int k=0;k<EdgeNum;k++)
    {
        i = graph_param[k+2][0];
        j = graph_param[k+2][1];
       // Current[i][j] = graph_param[k+2][2];
//Current[j][i] = Current[i][j];
       // Single_Price[i][j] = graph_param[k+2][3];
       // Single_Price[j][i] = Single_Price[i][j];
        add_Node(i,j,graph_param[k+2][2],graph_param[k+2][3]);
        add_Node(j,i,graph_param[k+2][2],graph_param[k+2][3]);
    }
    needFlow=0;
    
    // 初始化图，针对每一个消费节点（将他们合成超级汇点），针对服务器节点的初始化放到zkw里面了
    for(int k=0;k<graph.consume_position.size();k++)
    {
        i = k;
        j = graph_param[k+EdgeNum+2][1];    // 消费点相连的网络节点
        NumConsumeNode[i] = j;   // GustsNode存储消费点相连的网络点
       // Current[NodeNum+i][j] = graph_param[i+EdgeNum+2][2];   // Flow里面还存储了消费点需要的带宽
        add_Node(j,NodeNum+1,graph_param[i+EdgeNum+2][2],0);//将消费点相连的网络节点连接到超级汇点上，费用应为0
        //Current[j][NodeNum+i] = Current[NodeNum+i][j];
        //Current[NodeNum+i][NodeNum+GustNum+1] = Current[NodeNum+i][j];
        //Single_Price[NodeNum+i][NodeNum+GustNum+1]=0;
        needFlow+=	graph_param[i+EdgeNum+2][2];
        //Single_Price[NodeNum+i][j]=0;  // 消费点相连的网络点这条链路的单价
        //Single_Price[j][NodeNum+i]=0;
    }
    N=NodeNum+2;    // N表示节点数
    S=NodeNum,T=NodeNum+1;  // S表示超级源点的编号，T表示超级汇点的编号
    memcpy(cT,c,Count*sizeof(int));
    memcpy(bT,begint,NodeNum*sizeof(int));
}

int MiniCostFlow::solve(Graph& graph,vector<vector<int>> &graph_param)
{
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    for(int i:graph.service_position)
        add_Node(NodeNum,i,MYINFINITY,0);
    int cost=getCost();
    int sum=0;
    for(int i:tmp1)
        sum+=i;
    
    cost+=graph.service_position.size()*graph.cost_per_service;
    if(needFlow>allFlow)
        cost=MYINFINITY;
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return cost;
}

vector<float> MiniCostFlow::getResult(Graph& graph,vector<vector<int>> &graph_param)
{
    
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    for(int i:graph.service_position)
        add_Node(NodeNum,i,MYINFINITY,0);   // 构造超级源点，费用应该设置为0？
    int cost=getCost();
    
    
    vector<float> flowOfEdges;
    flowOfEdges.push_back(1);
    int index = 0;
    for (int i = 2; i <= graph.edges*4; i += 2) {
        if (c[i] > 0) {
            graph.edgesToIndex.insert( make_pair(make_pair(endt[i], endt[i-1]), index) );
            index++;
            flowOfEdges.push_back(c[i]);
        }
    }
    flowOfEdges.push_back(cost);
    
    return flowOfEdges;
}

list<Genome::Individual> MiniCostFlow::getInitialPopulation(Graph& graph,vector<vector<int>> &graph_param)
{
    vector<int> a,b,abest;
    int addnum=0,costbest=MYINFINITY;
    for(int m=0;m<graph.consume_position.size();m++)
        abest.push_back(graph.consume_position[m].first);
    // for(int i=0;i<3;i++){
    // cout<<endl<<i<<endl;
    a=abest;
    addnum=0;
    list<Genome::Individual> population;
    for(int m=graph.consume_position.size();m>0;m--){
        memcpy(begint,bT,NodeNum*sizeof(int));
        memcpy(c,cT,Count*sizeof(int));
        allFlow=0;
        for(int i:a)
            add_Node(NodeNum,i,MYINFINITY,0);
        //cout << a.size() << endl;
        int cost=getCost();
        cost+=a.size()*graph.cost_per_service;
        if(needFlow>allFlow)
            cost=MYINFINITY;
        
        
        //cout<<cost<<endl;
        if(costbest>cost)
        {
            costbest=cost;
            abest=a;
        }
        
        if (cost != MYINFINITY) {
            Genome::Individual Ind1;
            Ind1._fittness=cost;
            for (auto temp : a)
                Ind1._DNA.push_back(temp);
            // Ind1._DNA=a;
            population.push_back(Ind1);
        }
        
        
        for(int i:a)
            ClearAll_butc(NodeNum,i,MYINFINITY,0);
        
        b.clear();
        for(int j=1;j<=a.size();j++)
            b.push_back(c[j*2+EdgeNum*4+GustNum*2]);
        
        if(b.size()-addnum>0)
            a.erase(a.begin()+distance(b.begin(),std::min_element(b.begin(),b.end()-addnum)));
        
        if( cost==MYINFINITY)
            for(int i=0;i<graph.consume_position.size();i++)
                
                if(c[i*2+EdgeNum*4+2]!=graph.consume_position[i].second)
                {
                    a.push_back(graph.consume_position[i].first);
                    addnum++;
                    break;
                }
        
    }
    //  }
    
    return population;
}



list<Genome::Individual> MiniCostFlow::getInitialPopulation_Improve(Graph& graph,vector<vector<int>> &graph_param,const vector<int>& inputservice)
{
    vector<int> aservice,bflow, a1service,b1flow,abestservice;
    int oldcost = MYINFINITY;
    int tt1;
    list<Genome::Individual> population;
    // vector<flowandnode> flowandroad;
    int addnum=0,costbest=MYINFINITY,flag=0;
    float deal=1.02;
    abestservice=inputservice;
    //for(int m=0;m<graph.consume_position.size();m++)
    // abestservice.push_back(graph.consume_position[m].first);
    for(int i=0;i<2;i++){
        deal+=0.005;
        flag=0;
        //   cout<<endl<<i<<endl;
        aservice=abestservice;
        addnum=0;
        for(int m=graph.consume_position.size();m>30;m--){
            
            
            int j=0;
            
            memcpy(begint,bT,NodeNum*sizeof(int));
            memcpy(c,cT,Count*sizeof(int));
            allFlow=0;
            float price=0.256;
            j=aservice.size();
            a1service=aservice;
            b1flow=bflow;
            // if(b.size()==3)
            //    cout<<1<<endl;
            if(m!=graph.consume_position.size())
            {
                aservice.clear();
                for(;a1service.size()!=0;)//按照流量大小设置价格
                {
                    
                    
                    int t1=a1service[distance(b1flow.begin(),std::max_element(b1flow.begin(),b1flow.end()))];
                    a1service.erase(a1service.begin()+distance(b1flow.begin(),std::max_element(b1flow.begin(),b1flow.end())));
                    b1flow.erase(b1flow.begin()+distance(b1flow.begin(),std::max_element(b1flow.begin(),b1flow.end())));
                    aservice.push_back(t1);
                    
                    
                    
                    //add_Node(NodeNum,t1,MYINFINITY,int(price));
                    
                }
                for(int i:aservice)
                {price*=deal;
                    add_Node(NodeNum,i,MYINFINITY,price);
                }
            }
            else
                for(int i:aservice)
                    add_Node(NodeNum,i,MYINFINITY,0);
            int cost=getCost();
            // cost+=a.size()*graph.cost_per_service;
            if(needFlow>allFlow)
                cost=MYINFINITY;
            
            
            // cout<<cost<<endl;
            
            
            
            for(int i:aservice)
                ClearAll_butc(NodeNum,i,0,0);
            
            bflow.clear();
            for(int j=1;j<=aservice.size();j++)
                bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
            
            if(bflow.size()-addnum>0){
                tt1 = aservice[ distance(bflow.begin(),std::min_element(bflow.begin(),bflow.end()-addnum)) ];
                aservice.erase(aservice.begin()+distance(bflow.begin(),std::min_element(bflow.begin(),bflow.end()-addnum)));
                bflow.erase(bflow.begin()+distance(bflow.begin(),std::min_element(bflow.begin(),bflow.end()-addnum)));
                //cout<<b.size()<<endl;
            }
            if(bflow.size()-addnum<=0)
                addnum=0;
            
            if( cost==MYINFINITY)
                for(int i=0;i<graph.consume_position.size();i++)
                    
                    if(c[i*2+EdgeNum*4+2]!=graph.consume_position[i].second)//如果某个消费点没满足，直接放一个服务器
                    {
                        aservice.push_back(graph.consume_position[i].first);
                        bflow.push_back(graph.consume_position[i].second);
                        addnum++;
                        break;
                    }
            
            allFlow=0;
            
            memcpy(begint,bT,NodeNum*sizeof(int));
            memcpy(c,cT,Count*sizeof(int));
            
            for(int i:aservice)
                add_Node(NodeNum,i,MYINFINITY,0);
            cost=getCost();
            cost+=aservice.size()*graph.cost_per_service;
            if(needFlow>allFlow)
                cost=MYINFINITY;
            for(int i:aservice)
                ClearAll(NodeNum,i,MYINFINITY,0);
            //cout<< cost<<endl;
            // 初始化种群
            Genome::Individual ind1;
            if (cost != MYINFINITY) {
                ind1._fittness = cost;
                for (auto temp : aservice)
                    ind1._DNA.push_back(temp);
                population.push_back(ind1);
            }
            if (oldcost < cost) {
                addnum++;
                aservice.push_back(tt1);
                bflow.push_back(MYINFINITY);
            }
            oldcost = cost;
            
            if(costbest>cost)
            {
                costbest=cost;
                abestservice=aservice;
                flag=0;
            }
            else
                flag++;
            if(flag>=20)
                break;//判断是否结束
            
        }
        
        
        
    }
    //cout<<"costbest"<<costbest<<endl;
    
    return population;
}


int MiniCostFlow::getAugmentChain(int u,int f)
{
    if (u == T)
    {
        return f;}
    haslabel[u] = true;
    for (int now = cur[u]; now; now = nextt[now])
        if (c[now] && !haslabel[endt[now]]&& d[u] == d[endt[now]]+cost[now])
            if (int tmp = getAugmentChain(endt[now],MIN(f,c[now]))){
                
                c[now] -= tmp;
                c[OPPOSITE(now)] += tmp;
                
                cur[u] = now;
                return tmp;
            }
    
    return 0;
}

// 找增广链
//d[]数组表示单价
// 调整标号
bool MiniCostFlow::Changelabel()
{
    // modlabel返回为false表示未找到满足条件的流
    int tmp = MYINFINITY;
    for (int i = 0; i<=N; i++)  // 遍历每一个网络节点，包括超级源点
        if (haslabel[i])   // 如果该点被标记过
            for (int now = begint[i]; now; now = nextt[now])    // 遍历以now为起点的正向弧
                if (c[now]&&!haslabel[endt[now]])
                    tmp = MIN(tmp,d[endt[now]]+cost[now]-d[i]);
    if (tmp == MYINFINITY)
        return true;
    for ( int i = 0; i<=N; i++)
        if (haslabel[i])
            haslabel[i] = false,d[i] += tmp;
    return false;
}


// 构造残余网络
void MiniCostFlow::add_Node(int a,int b,int flow, int v)
{
    Count++;nextt[Count] = begint[a]; begint[a] = Count; endt[Count] = b; c[Count] = flow; cost[Count] = v;
    
    Count++;nextt[Count] = begint[b]; begint[b] = Count; endt[Count] = a; c[Count] = 0; cost[Count] = -v;
    
}

// 每次求解最小费用流后清空
void MiniCostFlow::ClearAll(int a,int b,int flow, int v)
{
    nextt[Count] = 0; begint[a] = 0; endt[Count] = 0; c[Count] = 0; cost[Count] = 0; Count--;
    nextt[Count] = 0; begint[b] = 0; endt[Count] = 0; c[Count] = 0; cost[Count] = 0; Count--;
}

// 清空所有，除了c，这个用于getInitialPopulation_Improve()函数
void MiniCostFlow::ClearAll_butc(int a,int b,int flow, int v)
{
    nextt[Count] = 0; begint[a] = 0; endt[Count] = 0;  cost[Count] = 0; Count--;
    nextt[Count] = 0; begint[b] = 0; endt[Count] = 0;  cost[Count] = 0; Count--;
}

int MiniCostFlow::getCost()
{
    
    memset(haslabel,0,sizeof(haslabel));
    memset(d,0,sizeof(d));
    int costflow = 0,tmp;
    do
    {
        memcpy(cur,begint,N*sizeof(int));
        while (tmp = getAugmentChain(S,needFlow))
        {
            costflow += tmp*d[S];//d[S]表示增广链的费用？
            memset(haslabel,0,sizeof(haslabel));
            allFlow+=tmp;
            //tmp1.push_back(tmp);
            
        }
    }
    while(!Changelabel());
    return costflow;
}


