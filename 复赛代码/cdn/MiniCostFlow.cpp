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
    Max_servie_level = graph.serviceLevels/2;
}

int MiniCostFlow::solve(Graph& graph,vector<vector<int>> &graph_param)
{
    vector<int> bflow;
    memcpy(begint,bT,NodeNum*sizeof(int));  // 把bT（边的连接关系）赋给begint，
    memcpy(c,cT,Count*sizeof(int)); // 把最初的残余图赋给c
    allFlow=0;
    for(int i:graph.service_position)
        add_Node(NodeNum,i,graph_param[graph_param.size() - 2][1],0);
    int cost=getCost();
    
    
    //cost+=graph.service_position.size()*graph.cost_per_service;
    if(needFlow>allFlow)
        cost=MYINFINITY;
    else{
        //bflow.clear();
        for(int j=1;j<=graph.service_position.size();j++) {
            //cout << c[j*2+EdgeNum*4+GustNum*2] << endl;
            bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        }
        
        vector<int> serviceprice;
        int tempsize = graph_param.size();
        for (int i = 0; i < graph.service_position.size(); i++) {
            int tempLevels = graph.serviceLevels;
            
            while (tempLevels) {
                if (bflow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                    serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                    break;
                }
                tempLevels--;
            }
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            //cout << serviceprice[i] << endl;
            //cout << graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i] << endl;
            cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            
        }
        
    }
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return cost;
}

list<Genome::Individual> MiniCostFlow::getInitialPopulation(Graph& graph,vector<vector<int>> &graph_param, vector<int>& inputservice)
{
    vector<int> a,b,abest, abestservice;
    abest=inputservice;
    int flag = 0;
    int addnum=0,costbest=MYINFINITY;
    //    for(int m=0;m<graph.consume_position.size();m++)
    //        abest.push_back(graph.consume_position[m].first);
    
    list<Genome::Individual> population;
    for(int i=0;i<2;i++){
        flag = 0;
        // cout<<endl<<i<<endl;
        a=abest;
        addnum=0;
        
        for(int m=graph.consume_position.size();m>0;m--){
            memcpy(begint,bT,NodeNum*sizeof(int));
            memcpy(c,cT,Count*sizeof(int));
            allFlow=0;
            for(int i:a)
                add_Node(NodeNum,i,graph.level2flow[Max_servie_level][0],0);
            //cout << a.size() << endl;
            int cost=getCost(graph);
            // cost+=a.size()*graph.cost_per_service;
            if(needFlow>allFlow)
            {
                for(int i:a)
                    ClearAll_butc(NodeNum,i,MYINFINITY,0);
                
                cost=MYINFINITY;
            }
            else{
                
                b.clear();
                for(int j=1;j<=a.size();j++)
                    b.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
                
                // --------bflow可能为0，即有冗余的服务器------
                vector<int> serviceprice;
                int tempsize = graph_param.size();
                for (int i = 0; i < a.size(); i++) {
                    int tempLevels = graph.serviceLevels;
                    while (tempLevels) {
                        if (b[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                            serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                            break;
                        }
                        tempLevels--;
                    }
                }
                for (int i = 0; i < a.size(); i++) {
                    
                    cost+=graph_param[tempsize - 1][a[i]] + serviceprice[i];
                    
                }
                
                // }
                
                //cout<<cost<<endl;
                if(costbest>cost)
                {
                    costbest=cost;
                    abest=a;
                    flag = 0;
                }
                else
                    flag++;
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
                
                if(b.size()-addnum>0){
                    //do{
                    //  tt1=aservice[distance(bflow.begin(),std::min_element(bflow.begin(),bflow.end()-addnum))];
                    a.erase(a.begin()+distance(b.begin(),std::min_element(b.begin(),b.end()-addnum)));
                    b.erase(b.begin()+distance(b.begin(),std::min_element(b.begin(),b.end()-addnum)));
                }//while(b[distance(b.begin(),std::min_element(b.begin(),b.end()-addnum))]==0);
            }
            if( cost==MYINFINITY)
                for(int i=0;i<graph.consume_position.size();i++)
                    
                    if(c[i*2+EdgeNum*4+2]!=graph.consume_position[i].second)
                    {
                        a.push_back(graph.consume_position[i].first);
                        addnum++;
                        break;
                    }
            if (flag >= 20)
                break;
            
        }
        
    }
    cout << costbest << endl;
    return population;
}




list<Genome::Individual> MiniCostFlow::getInitialPopulation_Improve(Graph& graph,vector<vector<int>> &graph_param,const vector<int>& inputservice)
{
    vector<int> aservice,bflow, a1service,b1flow,abestservice;
    vector<int> priceofservice = graph_param[graph_param.size() - 1];
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
                {deal*=deal;
                    add_Node(NodeNum,i,graph_param[graph_param.size() - 2][1],0);
                }
            }
            else
                for(int i:aservice)
                    add_Node(NodeNum,i,graph_param[graph_param.size() - 2][1],0);
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
                add_Node(NodeNum,i,graph_param[graph_param.size() - 2][1],0);
            cost=getCost();
            for (int i : aservice)
                cost+=priceofservice[i] + graph_param[graph_param.size() -2][2];
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
            //            if (oldcost < cost) {
            //                addnum++;
            //                aservice.push_back(tt1);
            //                bflow.push_back(MYINFINITY);
            //            }
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
    cout<<"costbest"<<costbest<<endl;
    graph.service_position = abestservice;
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

// 钦涛的DM输出结果
vector<float> MiniCostFlow::getResult(Graph& graph,vector<vector<int>> &graph_param,int costinput)
{
    
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    
    allFlow=0;
    for(int i:graph.service_position)
        add_Node(NodeNum,i,graph_param[graph_param.size() - 2][1],0);   // 构造超级源点，费用应该设置为0？
    int cost=getCost();
    memcpy(c2,c,(Count + 100)*sizeof(int));
    vector<int> bflow,b2flow;
    for(int j=1;j<=graph.service_position.size();j++) {
        bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        // serviceSend.insert(make_pair(graph.service_position[j-1], bflow[j-1]));
    }
    int tempsize = graph_param.size();
    vector<int> serviceprice;
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;
        while (tempLevels) {
            
            if (bflow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                break;
            }
            tempLevels--;
        }
    }
    
    for (int i = 0; i < graph.service_position.size(); i++) {
        
        cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
        //  cout<<cost<<endl;
        
        
    }
    
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    int j=0;
    
    int diff;
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;
        
        while (tempLevels) {
            diff =  graph_param[tempsize - tempLevels][1] - graph_param[tempsize - 1 - tempLevels][1];
            if (bflow[i] < graph_param[tempsize - 1 - tempLevels][1]+ diff/2) {
                add_Node(NodeNum,graph.service_position[i],graph_param[tempsize - tempLevels - 1][1],0);
                // serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                break;
            }
            tempLevels--;
        }
    }
    cost=getCost();
    if(needFlow>allFlow)
        cost=MYINFINITY;
    else{
        b2flow.clear();
        for(int j=1;j<=graph.service_position.size();j++)
            b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        
        // --------bflow可能为0，即有冗余的服务器------
        vector<int> serviceprice;
        for (int i = 0; i < graph.service_position.size(); i++) {
            int tempLevels = graph.serviceLevels;
            while (tempLevels) {
                if (b2flow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                    serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                    break;
                }
                tempLevels--;
            }
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            //cout << serviceprice[i] << endl;
            //cout << graph_param[tempsize - 1][aservice[i]] + serviceprice[i] << endl;
            cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            //  cout<<cost<<endl;
            
            
        }
        
    }
    if(cost>costinput)
        memcpy(c,c2,(Count + 100)*sizeof(int));
    bflow.clear();
    for(int j=1;j<=graph.service_position.size();j++) {
        bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        serviceSend.insert(make_pair(graph.service_position[j-1], bflow[j-1]));
    }
    // --------bflow可能为0，即有冗余的服务器------
    //int tempsize = graph_param.size();
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;
        while (tempLevels) {
            if (bflow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                servicelevel.insert( make_pair( graph.service_position[i] ,graph_param[tempsize - tempLevels - 1][0] ));    // key表示网络点，value表示服务器档次
                break;
            }
            tempLevels--;
        }
    }
    
    
    
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
    
    for (int i = 0; i < graph.service_position.size(); i++) {
        graph.edgesToIndex.insert(make_pair(make_pair(S, graph.service_position[i]), index));
        index++;
        flowOfEdges.push_back(bflow[i]);
    }
    
    flowOfEdges.push_back(cost);
    //cout<<"bestcost"<<cost<<endl;
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return flowOfEdges;
}


int MiniCostFlow::solveWithDM(Graph& graph,vector<vector<int>> &graph_param)
{
    vector<int> bflow,b2flow;
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    for(int i:graph.service_position)
        add_Node(NodeNum,i,graph_param[graph_param.size() - 2][1],0);
    int cost=getCost();
    int tempsize = graph_param.size();
    //cost+=graph.service_position.size()*graph.cost_per_service;
    if(needFlow>allFlow){
        
        cost=MYINFINITY;
        for(int i:graph.service_position)
            ClearAll(NodeNum,i,MYINFINITY,0);
        return cost;
    }
    else{
        bflow.clear();
        for(int j=1;j<=graph.service_position.size();j++) {
            //cout << c[j*2+EdgeNum*4+GustNum*2] << endl;
            bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        }
        
        // --------bflow可能为0，即有冗余的服务器------
        vector<int> serviceprice;
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            int tempLevels = graph.serviceLevels;
            while (tempLevels) {
                
                if (bflow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                    serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                    break;
                }
                tempLevels--;
            }
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            //cout << serviceprice[i] << endl;
            //cout << graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i] << endl;
            cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            
        }
        
        
        for(int i:graph.service_position)
            ClearAll(NodeNum,i,MYINFINITY,0);
        
        memcpy(begint,bT,NodeNum*sizeof(int));
        memcpy(c,cT,Count*sizeof(int));
        allFlow=0;
        int j=0;
        
        int diff;
        for (int i = 0; i < graph.service_position.size(); i++) {
            int tempLevels = graph.serviceLevels;
            while (tempLevels) {
                diff =  graph_param[tempsize - tempLevels][1] - graph_param[tempsize - 1 - tempLevels][1];
                if (bflow[i] < graph_param[tempsize - 1 - tempLevels][1] + diff / 2) {
                    
                    add_Node(NodeNum,graph.service_position[i],graph_param[tempsize - tempLevels - 1][1],0);
                    // serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                    break;
                }
                tempLevels--;
            }
        }
        int  cost1=getCost();
        
        if(needFlow>allFlow)
            cost1=MYINFINITY;
        else{
            b2flow.clear();
            for(int j=1;j<=graph.service_position.size();j++)
                b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
            
            // --------bflow可能为0，即有冗余的服务器------
            vector<int> serviceprice;
            for (int i = 0; i < graph.service_position.size(); i++) {
                int tempLevels = graph.serviceLevels;
                while (tempLevels) {
                    if (b2flow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                        serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                        break;
                    }
                    tempLevels--;
                }
            }
            
            for (int i = 0; i < graph.service_position.size(); i++) {
                //cout << serviceprice[i] << endl;
                //cout << graph_param[tempsize - 1][aservice[i]] + serviceprice[i] << endl;
                cost1+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
                //  cout<<cost<<endl;
                
                
            }
            
        }
        
        
        
        for(int i:graph.service_position)
            ClearAll(NodeNum,i,MYINFINITY,0);
        if(cost>cost1)
            cost=cost1;
    }
    return cost;
    
}

int MiniCostFlow::solveWithDM2(Graph& graph,vector<vector<int>> &graph_param)
{
    vector<int> bflow,b2flow;
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    int tempLevels = graph.serviceLevels;
    allFlow=0;
    for(int i:graph.service_position){

        
        for(int j = 0; j < tempLevels; j++)
        {
            add_Node(NodeNum,i,graph.level2flow[j][3],graph.level2flow[j][2]);
        }
    }
    
    int cost=getCost();
    
    
    //vector<int> bflow,b2flow;
    
    int tempsize = graph_param.size();
    
    if(needFlow>allFlow){
        
        cost=MYINFINITY;
        for(int i:graph.service_position)
            for(int k=1;k<=tempLevels;k++)
                ClearAll(NodeNum,i,MYINFINITY,0);
        return cost;
    }
    else{
        bflow.clear();
        
        for(int j=1;j<=graph.service_position.size();j++) {
            int sum=0;
            for(int m=1;m<=tempLevels;m++)
                if(c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2]>0){
                    sum += c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                    cost -= graph.level2flow[m-1][2] * c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];

                }
            
            bflow.push_back(sum);
        }
        // --------计算服务器档次的价格
        vector<int> serviceprice;
        

        getServiclevel(graph, bflow);//记录各个服务器对应的档次
        for(auto i:graph.service_position){
            serviceprice.push_back(graph.level2flow[servicelevel[i]][1]);
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            
            cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            
        }
        
        int tempLevels = graph.serviceLevels;
        for(int i:graph.service_position)
            for(int k=1;k<=tempLevels;k++)
                ClearAll(NodeNum,i,MYINFINITY,0);
        
        memcpy(begint,bT,NodeNum*sizeof(int));
        memcpy(c,cT,Count*sizeof(int));
        allFlow=0;
        int j=0;
        int diff;

        //------------重新抖动加边计算
        map<int,int> flow_dou;//记录每个服务器能抖的流量
        for(auto i: graph.service_position){
            if(servicelevel[i] > 0)
            {
                if((serviceSend[i]-graph.level2flow[servicelevel[i]-1][0])==25)
                    flow_dou.insert(make_pair(i,0));
                else
                    flow_dou.insert(make_pair(i,(serviceSend[i]-graph.level2flow[servicelevel[i]-1][0])));
            }
            else
                flow_dou.insert(make_pair(i,serviceSend[i]));
        }
        diff=25;

        while(diff){
            int containFlow = 0;//能够吸纳的流量
            int provideFlow = 0;//需要抖掉的流量
            for(auto i:graph.service_position){
                if(flow_dou[i] > diff){
                    containFlow += (25 - flow_dou[i]);
                }
                else{
                    provideFlow += flow_dou[i];
                }
            }
            if((containFlow - provideFlow) > 10)
                break;

            diff--;
        }


        for(auto i: graph.service_position){
            //diff = graph.level2flow[servicelevel[i]][3];
            if(servicelevel[i] > 0)
                if(serviceSend[i] < graph.level2flow[servicelevel[i]-1][0] + diff){
                    servicelevel[i]--;
                }
        }
        for (auto i: graph.service_position){
            add_Node(NodeNum, i, graph.level2flow[servicelevel[i]][0], 0);
        }

        int  cost1=getCost();
        
        if(needFlow>allFlow)
            cost1=MYINFINITY;
        else{
            b2flow.clear();
            for(int j=1;j<=graph.service_position.size();j++){
                b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
                // flow12[graph.service_position[j-1]]=((c[j*2+EdgeNum*4+GustNum*2]+50)>graph_param[graph_param.size() - 2][1])?graph_param[graph_param.size() - 2][1]:(c[j*2+EdgeNum*4+GustNum*2]+50);
            }
            
            // --------bflow可能为0，即有冗余的服务器------
            vector<int> serviceprice;
            getServiclevel(graph, b2flow);
            for(auto i:graph.service_position){
                serviceprice.push_back(graph.level2flow[servicelevel[i]][1]);
            }
            
            for (int i = 0; i < graph.service_position.size(); i++) {
                //cout << serviceprice[i] << endl;
                //cout << graph_param[tempsize - 1][aservice[i]] + serviceprice[i] << endl;
                cost1+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
                //  cout<<cost<<endl;
                
                
            }
            
        }
        
        
        
        for(int i:graph.service_position)
            ClearAll(NodeNum,i,MYINFINITY,0);
        if(cost>cost1)
            cost=cost1;
        return cost;
        
    }
}

vector<float> MiniCostFlow::getResult2(Graph& graph,vector<vector<int>> &graph_param,int costinput)
{
    
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    int bestcost;
    int tempLevels = graph.serviceLevels;
    allFlow=0;
    for(int i:graph.service_position){
        
        for(int j = 0; j < tempLevels; j++)
        {
            add_Node(NodeNum,i,graph.level2flow[j][3],graph.level2flow[j][2]);
        }
    }
    
    int cost=getCost();
    
    vector<int> bflow,b2flow;
    
    
    for(int j=1;j<=graph.service_position.size();j++) {
        int sum=0;
        for(int m=1;m<=tempLevels;m++)
            if(c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2]>0){
                sum += c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                cost -= graph.level2flow[m-1][2] * c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                
            }
        
        bflow.push_back(sum);
    }
    
    int tempsize = graph_param.size();
    
    for(int i:graph.service_position)
        for(int k=1;k<=tempLevels;k++)
            ClearAll(NodeNum,i,MYINFINITY,0);
    int diff1=1;
    while(diff1<25){
        memcpy(begint,bT,NodeNum*sizeof(int));
        memcpy(c,cT,Count*sizeof(int));
        allFlow=0;
        int j=0;
        getServiclevel(graph, bflow);
        
        for(auto i: graph.service_position){
            //diff = graph.level2flow[servicelevel[i]][3];
            if(servicelevel[i] > 0)
                if(serviceSend[i] < graph.level2flow[servicelevel[i]-1][0] + diff1){
                    servicelevel[i]--;
                }
        }
        for (auto i: graph.service_position){
            add_Node(NodeNum, i, graph.level2flow[servicelevel[i]][0], 0);
        }
        cost=getCost();
        
        if(needFlow>allFlow){
            cost=MYINFINITY;
            break;
        }
        else{
            memcpy(c2,c,(Count+100)*sizeof(int));
            b2flow.clear();
            for(int j=1;j<=graph.service_position.size();j++)
                b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
            
            
            // --------计算服务器档次的价格
            vector<int> serviceprice;
            
            
            getServiclevel(graph, b2flow);//记录各个服务器对应的档次
            for(auto i:graph.service_position){
                serviceprice.push_back(graph.level2flow[servicelevel[i]][1]);
            }
            
            for (int i = 0; i < graph.service_position.size(); i++) {
                
                cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
                
            }
            
            for(int i:graph.service_position)
                ClearAll_butc(NodeNum,i,MYINFINITY,0);
            cout<<diff1<<"bestcost"<<cost<<endl;
            bestcost=cost;
            
        }
        diff1++;
    }
    vector<float> flowOfEdges;
    if(bestcost>costinput)
    {
        for(int i:graph.service_position)
            ClearAll(NodeNum,i,MYINFINITY,0);
        flowOfEdges.push_back(bestcost);
        return flowOfEdges;
        
    }
    memcpy(c,c2,(Count+100)*sizeof(int));
    bflow.clear();
    for(int j=1;j<=graph.service_position.size();j++) {
        bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        //serviceSend.insert(make_pair(graph.service_position[j-1], bflow[j-1]));
    }
    
    getServiclevel(graph, bflow);
    
    
    
    flowOfEdges.push_back(1);
    int index = 0;
    for (int i = 2; i <= graph.edges*4; i += 2) {
        if (c[i] > 0) {
            graph.edgesToIndex.insert( make_pair(make_pair(endt[i], endt[i-1]), index) );
            index++;
            flowOfEdges.push_back(c[i]);
        }
    }
    
    for (int i = 0; i < graph.service_position.size(); i++) {
        graph.edgesToIndex.insert(make_pair(make_pair(S, graph.service_position[i]), index));
        index++;
        flowOfEdges.push_back(bflow[i]);
    }
    
    
    flowOfEdges.push_back(bestcost);
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return flowOfEdges;
}

void MiniCostFlow::initial_greedy(Graph& graph,vector<vector<int>> &graph_param){
    
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
        add_Node(i,j,graph_param[k+2][2],10 );
        add_Node(j,i,graph_param[k+2][2],10 );
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
    Max_servie_level = graph.serviceLevels/2;
    memcpy(cT,c,Count*sizeof(int));
    memcpy(bT,begint,NodeNum*sizeof(int));
}

vector<float> MiniCostFlow::getResult2_greedy(Graph& graph,vector<vector<int>> &graph_param)
{
    
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    int cost1;
    int tempLevels = graph.serviceLevels;
    allFlow=0;
    for(int i:graph.service_position){

        for(int j = 0; j < tempLevels; j++)
        {
            add_Node(NodeNum,i,graph.level2flow[j][3],graph.level2flow[j][2]);
        }
    }
    
    int cost=getCost();
    //memcpy(c2,c,Count*sizeof(int));
    vector<int> bflow,b2flow;
    
    //    for(int j=1;j<=graph.service_position.size();j++) {
    //        int sum=0;
    //        for(int m=1;m<=3;m++)
    //            if(c[m*2+EdgeNum*4+GustNum*2+(j-1)*3*2]>0)
    //                sum+=c[m*2+EdgeNum*4+GustNum*2+(j-1)*3*2];
    
    
    //        bflow.push_back(sum);
    
    //        //bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
    //        // serviceSend.insert(make_pair(graph.service_position[j-1], bflow[j-1]));
    //    }
    
    //int tempLevels = graph.serviceLevels;
    for(int j=1;j<=graph.service_position.size();j++) {
        int sum=0;
        for(int m=1;m<=tempLevels;m++)
            if(c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2]>0){
                sum += c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                 cost -= graph.level2flow[m-1][2] * c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                
            }
        
        bflow.push_back(sum);
    }
    
    
    vector<int> serviceprice;
    int tempsize = graph_param.size();
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;
        while (tempLevels) {
            if (bflow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                break;
            }
            tempLevels--;
        }
    }
    
    for (int i = 0; i < graph.service_position.size(); i++) {
        
        cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
        
    }
    
    

    
    for(int i:graph.service_position)
        for(int k=1;k<=tempLevels;k++)
            ClearAll(NodeNum,i,MYINFINITY,0);
    
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    int j=0;
    
    int diff;
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;
        while (tempLevels) {
            diff =  graph_param[tempsize - tempLevels][1] - graph_param[tempsize - 1 - tempLevels][1];
            if (bflow[i] < graph_param[tempsize - 1 - tempLevels][1]+diff/2) {
                add_Node(NodeNum,graph.service_position[i],graph_param[tempsize - tempLevels - 1][1],0);
                // serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                break;
            }
            tempLevels--;
        }
    }
    cost1=getCost();
    if(needFlow>allFlow)
        cost1=MYINFINITY;
    else{
        b2flow.clear();
        for(int j=1;j<=graph.service_position.size();j++)
            b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        
        // --------bflow可能为0，即有冗余的服务器------
        vector<int> serviceprice;
        for (int i = 0; i < graph.service_position.size(); i++) {
            int tempLevels = graph.serviceLevels;
            while (tempLevels) {
                if (b2flow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                    serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                    break;
                }
                tempLevels--;
            }
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            
            cost1+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            
        }
        cout<<"bestcost"<<cost1<<endl;
        
    }
    cout << ((cost1 > cost) ? cost : cost1) << endl;
    if(cost1>cost)
    {
        for(int i:graph.service_position)
            ClearAll(NodeNum,i,MYINFINITY,0);
        int j1=0;
        
        memcpy(begint,bT,NodeNum*sizeof(int));
        memcpy(c,cT,Count*sizeof(int));
        allFlow=0;
        for(int i:graph.service_position){
            add_Node(NodeNum,i,bflow[j1],0);
            j1++;
        }
        
        cost=getCost();
        
    }
    // memcpy(c,c2,Count*sizeof(int));
    bflow.clear();
    for(int j=1;j<=graph.service_position.size();j++) {
        bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        serviceSend.insert(make_pair(graph.service_position[j-1], bflow[j-1]));
    }
    
    // --------bflow可能为0，即有冗余的服务器------
    //int tempsize = graph_param.size();
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;
        while (tempLevels) {
            if (bflow[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                servicelevel.insert( make_pair( graph.service_position[i] ,graph_param[tempsize - tempLevels - 1][0] ));    // key表示网络点，value表示服务器档次
                break;
            }
            tempLevels--;
        }
    }
    
    
    
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
    
    for (int i = 0; i < graph.service_position.size(); i++) {
        graph.edgesToIndex.insert(make_pair(make_pair(S, graph.service_position[i]), index));
        index++;
        flowOfEdges.push_back(bflow[i]);
    }
    
    //    for (int i = 0; i < graph.consume_position.size(); i++) {
    //        graph.edgesToIndex.insert(make_pair(make_pair(graph.consume_position[i].first, T), index));
    //        index++;
    //        flowOfEdges.push_back(graph.consume_position[i].second);
    //    }
    //
    flowOfEdges.push_back(cost);
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return flowOfEdges;
}

void  MiniCostFlow::getServiclevel(Graph& graph, vector<int>& bflow){
    servicelevel.clear();
    serviceSend.clear();
    for (int i = 0; i < graph.service_position.size(); i++) {
        int tempLevels = graph.serviceLevels;


        serviceSend.insert(make_pair(graph.service_position[i], bflow[i]));

        for(int j = 0; j < tempLevels; j++){
            if(bflow[i] <= graph.level2flow[j][0]){
                servicelevel.insert( make_pair( graph.service_position[i] ,j ));    // key表示网络点，value表示服务器档次
                break;
            }
        }
    }

}

vector<int> MiniCostFlow::getInitialPopulation_SA(Graph& graph,vector<vector<int>> &graph_param, vector<int>& inputservice)
{
    vector<int> a,b,abest, abestservice;
    abest=inputservice;
    int flag = 0;
    int addnum=0,costbest=MYINFINITY;
    //    for(int m=0;m<graph.consume_position.size();m++)
    //        abest.push_back(graph.consume_position[m].first);
    
    for(int i=0;i<2;i++){
        flag = 0;
        // cout<<endl<<i<<endl;
        a=abest;
        addnum=0;
        
        for(int m=graph.consume_position.size();m>0;m--){
            memcpy(begint,bT,NodeNum*sizeof(int));
            memcpy(c,cT,Count*sizeof(int));
            allFlow=0;
            for(int i:a)
                add_Node(NodeNum,i, graph.level2flow[Max_servie_level][0],0);
            //cout << a.size() << endl;
            int cost=getCost(graph);
            // cost+=a.size()*graph.cost_per_service;
            if(needFlow>allFlow)
            {
                for(int i:a)
                    ClearAll_butc(NodeNum,i,MYINFINITY,0);
                
                cost=MYINFINITY;
            }
            else{
                b.clear();
                for(int j=1;j<=a.size();j++)
                    b.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
                
                // --------bflow可能为0，即有冗余的服务器------
                vector<int> serviceprice;
                int tempsize = graph_param.size();
                for (int i = 0; i < a.size(); i++) {
                    int tempLevels = graph.serviceLevels;
                    while (tempLevels) {
                        if (b[i] <= graph_param[tempsize - 1 - tempLevels][1]) {
                            serviceprice.push_back(graph_param[tempsize - tempLevels - 1][2]);
                            break;
                        }
                        tempLevels--;
                    }
                }
                for (int i = 0; i < a.size(); i++) {
                    
                    cost+=graph_param[tempsize - 1][a[i]] + serviceprice[i];
                    
                }
                
                // }
              
                if(costbest>cost)
                {
                    costbest=cost;
                    abest=a;
                    flag = 0;
                }
                else
                    flag++;
//                if (cost != MYINFINITY) {
//                    Genome::Individual Ind1;
//                    Ind1._fittness=cost;
//                    for (auto temp : a)
//                        Ind1._DNA.push_back(temp);
//                    // Ind1._DNA=a;
//                    population.push_back(Ind1);
//                }
                
                
                for(int i:a)
                    ClearAll_butc(NodeNum,i,MYINFINITY,0);
                
                //        b.clear();
                //        for(int j=1;j<=a.size();j++)
                //            b.push_back(c[j*2+EdgeNum*4+GustNum*2]);
                
                if(b.size()-addnum>0){
                    //do{
                    //  tt1=aservice[distance(bflow.begin(),std::min_element(bflow.begin(),bflow.end()-addnum))];
                    a.erase(a.begin()+distance(b.begin(),std::min_element(b.begin(),b.end()-addnum)));
                    b.erase(b.begin()+distance(b.begin(),std::min_element(b.begin(),b.end()-addnum)));
                }//while(b[distance(b.begin(),std::min_element(b.begin(),b.end()-addnum))]==0);
            }
            if( cost==MYINFINITY)
                for(int i=0;i<graph.consume_position.size();i++)
                    
                    if(c[i*2+EdgeNum*4+2]!=graph.consume_position[i].second)
                    {
                        a.push_back(graph.consume_position[i].first);
                        addnum++;
                        break;
                    }
            if (flag >= 20)
                break;
            
        }
        
    }
    cout << costbest << endl;
    return abest ;
}

int MiniCostFlow::solve_greedy(Graph& graph,vector<vector<int>> &graph_param,vector<int> &bflow)
{
    vector<int> b2flow;
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    int tempLevels = graph.serviceLevels;
    allFlow=0;
    for(int i:graph.service_position){
        
        
        for(int j = 0; j < tempLevels; j++)
        {
            add_Node(NodeNum,i,graph.level2flow[j][3],graph.level2flow[j][2]);
        }
    }
    
    int cost=getCost(graph);
    
    
    //vector<int> bflow,b2flow;
    
    int tempsize = graph_param.size();
    
    if(needFlow>allFlow){
        
        cost=MYINFINITY;
        for(int i:graph.service_position)
            for(int k=1;k<=tempLevels;k++)
                ClearAll(NodeNum,i,MYINFINITY,0);
        return cost;
    }
    else{
        bflow.clear();
        
        for(int j=1;j<=graph.service_position.size();j++) {
            int sum=0;
            for(int m=1;m<=tempLevels;m++)
                if(c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2]>0){
                    sum += c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                    // cost -= graph.level2flow[m-1][2] * c[m*2+EdgeNum*4+GustNum*2+(j-1)*tempLevels*2];
                    
                }
            
            bflow.push_back(sum);
        }
        // --------计算服务器档次的价格
        vector<int> serviceprice;
        
        
        getServiclevel(graph, bflow);//记录各个服务器对应的档次
        for(auto i:graph.service_position){
            serviceprice.push_back(graph.level2flow[servicelevel[i]][1]);
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            
            cost+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            
        }
        
        int tempLevels = graph.serviceLevels;
        
        for(int i:graph.service_position)
            for(int k=1;k<=tempLevels;k++)
                ClearAll(NodeNum,i,MYINFINITY,0);
        return cost;
    }
}
int MiniCostFlow::solve_DM_greedy(Graph& graph,vector<vector<int>> &graph_param,vector<int> &bflow)
{
    vector<int> b2flow;
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    int j=0;
    int diff;
    getServiclevel(graph, bflow);
    int tempsize = graph_param.size();
    //------------重新抖动加边计算
//    map<int,int> flow_dou;//记录每个服务器能抖的流量
//    for(auto i: graph.service_position){
//        if(servicelevel[i] > 0)
//        {
//            if((serviceSend[i]-graph.level2flow[servicelevel[i]-1][0])==25)
//                flow_dou.insert(make_pair(i,0));
//            else
//                flow_dou.insert(make_pair(i,(serviceSend[i]-graph.level2flow[servicelevel[i]-1][0])));
//        }
//        else
//            flow_dou.insert(make_pair(i,serviceSend[i]));
//    }
//    diff=25;
//    int containFlow = 0;//能够吸纳的流量
//    int provideFlow = 0;//需要抖掉的流量
//    while(diff){
//         containFlow = 0;//能够吸纳的流量
//         provideFlow = 0;//需要抖掉的流量
//        for(auto i:graph.service_position){
//            if(flow_dou[i] > diff){
//                containFlow += (25 - flow_dou[i]);
//            }
//            else{
//                provideFlow += flow_dou[i];
//            }
//        }
//        
//        if((containFlow - provideFlow) > 80)
//            break;
//        
//        diff--;
//    }
    //cout << diff << endl;
    for(auto i: graph.service_position){
        diff = graph.level2flow[servicelevel[i]][3];
        if(servicelevel[i] > 0)
            if(serviceSend[i] < graph.level2flow[servicelevel[i]-1][0] + diff/3){
                servicelevel[i]--;
            }
    }
    for (auto i: graph.service_position){
        add_Node(NodeNum, i, graph.level2flow[servicelevel[i]][0], 0);
    }
    
    int  cost1=getCost();
    
    if(needFlow>allFlow)
        cost1=MYINFINITY;
    else{
        b2flow.clear();
        for(int j=1;j<=graph.service_position.size();j++){
            b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
            // flow12[graph.service_position[j-1]]=((c[j*2+EdgeNum*4+GustNum*2]+50)>graph_param[graph_param.size() - 2][1])?graph_param[graph_param.size() - 2][1]:(c[j*2+EdgeNum*4+GustNum*2]+50);
        }
        
        // --------bflow可能为0，即有冗余的服务器------
        vector<int> serviceprice;
        getServiclevel(graph, b2flow);
        for(auto i:graph.service_position){
            serviceprice.push_back(graph.level2flow[servicelevel[i]][1]);
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            //cout << serviceprice[i] << endl;
            //cout << graph_param[tempsize - 1][aservice[i]] + serviceprice[i] << endl;
            cost1+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            //  cout<<cost<<endl;
            
            
        }
        
    }
    
    
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return cost1;
    
    
    
}

int MiniCostFlow::getCost(Graph& graph)
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
    int  cost=0;
    
    for (int i = 2; i <= graph.edges*4; i += 2) {
        if (c[i] > 0) {
            cost+=  c[i] * graph.single_price[endt[i]][endt[i-1]];
            
        }
    }
    
    return cost;
}


vector<float> MiniCostFlow::getResult2_greedy(Graph& graph,vector<vector<int>> &graph_param,vector<int> &bflow)
{
    
    vector<int> b2flow;
    memcpy(begint,bT,NodeNum*sizeof(int));
    memcpy(c,cT,Count*sizeof(int));
    allFlow=0;
    int j=0;
    int diff;
    getServiclevel(graph, bflow);
    int tempsize = graph_param.size();
    //------------重新抖动加边计算
//    map<int,int> flow_dou;//记录每个服务器能抖的流量
//    for(auto i: graph.service_position){
//        if(servicelevel[i] > 0)
//        {
//            if((serviceSend[i]-graph.level2flow[servicelevel[i]-1][0])==25)
//                flow_dou.insert(make_pair(i,0));
//            else
//                flow_dou.insert(make_pair(i,(serviceSend[i]-graph.level2flow[servicelevel[i]-1][0])));
//        }
//        else
//            flow_dou.insert(make_pair(i,serviceSend[i]));
//    }
//    diff=25;
//    
//    while(diff){
//        int containFlow = 0;//能够吸纳的流量
//        int provideFlow = 0;//需要抖掉的流量
//        for(auto i:graph.service_position){
//            if(flow_dou[i] > diff){
//                containFlow += (25 - flow_dou[i]);
//            }
//            else{
//                provideFlow += flow_dou[i];
//            }
//        }
//        if(containFlow > provideFlow)
//            break;
//        
//        diff--;
//    }
    
    
    for(auto i: graph.service_position){
        diff = graph.level2flow[servicelevel[i]][3];
        if(servicelevel[i] > 0)
            if(serviceSend[i] < graph.level2flow[servicelevel[i]-1][0] + diff/3){
                servicelevel[i]--;
            }
    }
    for (auto i: graph.service_position){
        add_Node(NodeNum, i, graph.level2flow[servicelevel[i]][0], 0);
    }
    
    int  cost1=getCost();
    
    if(needFlow>allFlow)
        cost1=MYINFINITY;
    else{
        b2flow.clear();
        for(int j=1;j<=graph.service_position.size();j++){
            b2flow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
            // flow12[graph.service_position[j-1]]=((c[j*2+EdgeNum*4+GustNum*2]+50)>graph_param[graph_param.size() - 2][1])?graph_param[graph_param.size() - 2][1]:(c[j*2+EdgeNum*4+GustNum*2]+50);
        }
        
        // --------bflow可能为0，即有冗余的服务器------
        vector<int> serviceprice;
        getServiclevel(graph, b2flow);
        for(auto i:graph.service_position){
            serviceprice.push_back(graph.level2flow[servicelevel[i]][1]);
        }
        
        for (int i = 0; i < graph.service_position.size(); i++) {
            //cout << serviceprice[i] << endl;
            //cout << graph_param[tempsize - 1][aservice[i]] + serviceprice[i] << endl;
            cost1+=graph_param[tempsize - 1][graph.service_position[i]] + serviceprice[i];
            //  cout<<cost<<endl;
            
            
        }
        
    }
    cout<<cost1<<endl;
    // memcpy(c,c2,Count*sizeof(int));
    vector<float> flowOfEdges;
    bflow.clear();
    for(int j=1;j<=graph.service_position.size();j++) {
        bflow.push_back(c[j*2+EdgeNum*4+GustNum*2]);//取出对应服务器走的流量
        // serviceSend.insert(make_pair(graph.service_position[j-1], bflow[j-1]));
    }
    
    getServiclevel(graph, bflow);
    
    
    
    flowOfEdges.push_back(1);
    int index = 0;
    for (int i = 2; i <= graph.edges*4; i += 2) {
        if (c[i] > 0) {
            graph.edgesToIndex.insert( make_pair(make_pair(endt[i], endt[i-1]), index) );
            index++;
            flowOfEdges.push_back(c[i]);
        }
    }
    
    for (int i = 0; i < graph.service_position.size(); i++) {
        graph.edgesToIndex.insert(make_pair(make_pair(S, graph.service_position[i]), index));
        index++;
        flowOfEdges.push_back(bflow[i]);
    }
    
    //    for (int i = 0; i < graph.consume_position.size(); i++) {
    //        graph.edgesToIndex.insert(make_pair(make_pair(graph.consume_position[i].first, T), index));
    //        index++;
    //        flowOfEdges.push_back(graph.consume_position[i].second);
    //    }
    //
    flowOfEdges.push_back(cost1);
    
    for(int i:graph.service_position)
        ClearAll(NodeNum,i,MYINFINITY,0);
    
    return flowOfEdges;
}



