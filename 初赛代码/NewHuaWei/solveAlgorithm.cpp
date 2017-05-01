//
//  solveAlgorith.cpp
//
//
//  Created by Mordekaiser on 17/3/7.
//
//  求解算法类的函数实现
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <random>
#include <cstring>
#include "solveAlgorithm.h"
#include "MiniCostFlow.h"
using namespace std;

extern MiniCostFlow MCF;
extern vector<vector<int>> graph_param;

solveAlgorithm::solveAlgorithm() {} // 构造函数什么都不做

solveAlgorithm::~solveAlgorithm() {} // 析构函数什么都不做

vector<vector<int>> solveAlgorithm::parseFile(char* topo[MAX_EDGE_NUM], int line_num) {
    // @郭晓锋

    vector<vector<int>> result;
    vector<int> line;
    
    
    // 解析出顶点数，网络链路数，消费节点数，并存于result[0]中
    line.push_back(atoi(strtok(topo[0], " ")));
    line.push_back(atoi(strtok(nullptr, " ")));
    line.push_back(atoi(strtok(nullptr, " ")));
    result.push_back(line);
    
    // 解析出视频内容服务器成本，并存于result[1]
    line.clear();
    line.push_back(atoi(strtok(topo[2], " ")));
    result.push_back(line);
    
    // 解析出网络链路之间的链接关系，以及带宽大小，网络租用费，并存入result[2]...result[边数目 + 1]中
    // 如果result某个元素result[num][0] == 65535，那么边数 == num - 2
    // 这也说明边的连接关系描述到此结束，接下去的文件内容是描述消费节点和网络节点之间的关系
    // 从第四行开始读,每一行的结束符是'\0'
    // 但是整段的结束符是0x0d
    int index = 0;
    int numEdges = result[0][1];
    while (index < numEdges ) {
        line.clear();
        line.push_back(atoi(strtok(topo[index + 4], " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
                       
        result.push_back(line);
        index++;
    }

    index = 0;
    int numConsumes = result[0][2];
    while (index < numConsumes) {
        line.clear();
        line.push_back(atoi(strtok(topo[numEdges + 5 + index], " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        result.push_back(line);
        index++;
    }
    
    
    
    return result;
}


vector<bool> solveAlgorithm::getServicePosition(Graph& graph) {
    // 获取服务器位置函数
    // 注意：要进行均值方差归一化，还有度和带宽的权重设置
    // 注意，返回的result的大小应等于顶点数，如果这个顶点有服务器，那我就为true
    // @黄渲淋负责
    vector<bool> result(graph.numVertexs, false);
    vector<vector<int>> uniformization(graph.numVertexs, vector<int>(graph.numVertexs, MYINFINITY));
    uniformization = graph.contains;
    //归一化带宽
    double sum = 0.0;
    double sum_single = 0.0;
    int  dun_single = 0;
    int  dun_sum = 0;
    vector<float> consum;
    vector<float> dun;
    for (int i = 0; i < graph.numVertexs; i++)
    {
        dun_single = 0;
        sum_single = 0;
        for (int j = 0; j < graph.numVertexs; j++)
        {
            //cout << graph.numVertexs << endl;
            if (uniformization[i][j] > 0 && uniformization[i][j] != MYINFINITY) {
                dun_single++;
                sum += uniformization[i][j];
                sum_single += uniformization[i][j];
                dun_sum++;
                
            }
        }
        dun.push_back(dun_single);
        consum.push_back(sum_single);
    }
    //cout << graph.numVertexs << endl;
    for (int i = 0; i < graph.numVertexs ; i++)
    {
        dun[i] = dun[i] / dun_sum;
        consum[i] = consum[i] / sum;
        consum[i] = 0.5*dun[i] + 0.5*consum[i];
    }
    
    vector<float> consum_copy = consum;
    
    std::sort(consum.begin(), consum.end());
    int i = 0;
    
    while (i < graph.numConsumes / 3) {
        result[ std::find(consum_copy.begin(), consum_copy.end(), consum[graph.numVertexs - i - 1]) - consum_copy.begin() ] = true;
        i++;
    }
    /*
     for (auto i : result)
     cout << i << endl;
     */
    for (int i = 0; i < result.size(); i++) {
        if (result[i] == true)
            graph.service_position.push_back(i);
    }
    
    
    return result;
}

// 这个获取服务器节点函数是专为遗传算法准备的，
vector<bool> solveAlgorithm::getServicePosition_GA(Graph& graph) {
    // 获取服务器位置函数
    // 注意：要进行均值方差归一化，还有度和带宽的权重设置
    // 注意，返回的result的大小应等于顶点数，如果这个顶点有服务器，那我就为true
    // @黄渲淋负责
    //-------------------------------------
    
    vector<bool> result(graph.numVertexs, true);
    
    /*
     vector<vector<int>> uniformization(graph.numVertexs, vector<int>(graph.numVertexs, INFINITY));
     uniformization = graph.contains;
     //归一化带宽
     double sum = 0.0;
     double sum_single = 0.0;
     int  dun_single = 0;
     int  dun_sum = 0;
     vector<float> consum;
     vector<float> dun;
     for (int i = 0; i < graph.numVertexs; i++)
     {
     dun_single = 0;
     sum_single = 0;
     for (int j = 0; j < graph.numVertexs; j++)
     {
     //cout << graph.numVertexs << endl;
     if (uniformization[i][j] > 0 && uniformization[i][j] != INFINITY) {
     dun_single++;
     sum += uniformization[i][j];
     sum_single += uniformization[i][j];
     dun_sum++;
     
     }
     }
     dun.push_back(dun_single);
     consum.push_back(sum_single);
     }
     //cout << graph.numVertexs << endl;
     for (int i = 0; i < graph.numVertexs ; i++)
     {
     dun[i] = dun[i] / dun_sum;
     consum[i] = consum[i] / sum;
     consum[i] = 0.5*dun[i] + 0.5*consum[i];
     }
     
     vector<float> consum_copy = consum;
     
     std::sort(consum.begin(), consum.end());
     int i = 0;
     
     while (i < 3 * graph.numConsumes / 3) {
     result[ std::find(consum_copy.begin(), consum_copy.end(), consum[graph.numVertexs - i - 1]) - consum_copy.begin() ] = true;
     i++;
     }
     /*
     for (auto i : result)
     cout << i << endl;
     */
    /*
     for (int i = 0; i < result.size(); i++) {
     if (result[i] == true)
     graph.service_position.push_back(i);
     }
     */
    //---------------------------
    
    return result;
}

partGraph solveAlgorithm::BFS(const Graph& graph, vector<bool> servicePosition, int hiearcy, int comsume_position)   {
    
    // @胡钦涛负责
    // 广度优先搜索，返回小图
    // partGraph由Graph派生而来。
    // 这里graph是按值传递，后期还有很大优化空间！！！
    
    // comsume_position = 2;
    
    int hiearcy_count=0;//层级计数
    vector<int> service_position;//返回服务器位置
    
    
    partGraph smallGraph(graph, comsume_position, service_position); // 这里第三个参数服务器的位置暂时不初始化
    
    vector<bool> visit(smallGraph.numVertexs, false);//记录访问的节点
    std::queue<int> Que_hi;
    auto comsume_net = smallGraph.consume_position[comsume_position].first;//通过消费节点查找网络节点；
    int count = comsume_net;//记录访问的节点
    while ( hiearcy_count < hiearcy ) {//BFS
        //std::cout << hiearcy_count << " < " << hiearcy << std::endl;
        if( !visit[count] ) {
            visit[count] = 1;//访问过节点保留，否则删除
            if ( servicePosition[count] == 1 ) { //该点是否为服务器节点
                hiearcy_count++;
                // cout << "count = " << count << endl;
                smallGraph.service_position.push_back(count);//记录下服务器位置
            }
            if ( hiearcy_count < hiearcy)
                Que_hi.push(count);
            while ( !Que_hi.empty() ){
                //cout << "fff" << endl;
                auto i =Que_hi.front();
                Que_hi.pop();
                
                for(int m=0;m < smallGraph.numVertexs;m++)
                {
                    if( smallGraph.contains[i][m] == MYINFINITY )
                        continue;
                    
                    //std::cout << m << std::endl;
                    if( !visit[m] ){
                        visit[m] = 1; //访问过节点保留，否则删除
                        if( servicePosition[m] == 1 )   // 如果这点是服务器节点，就存起来
                        {
                            // cout << "m = " << m << endl;
                            if( hiearcy_count < hiearcy)
                                smallGraph.service_position.push_back(m);
                            hiearcy_count++;
                            
                        }
                        if(hiearcy_count < hiearcy)
                            Que_hi.push(m);
                    }
                }
            }
        }
        
        
    }
    
    // 把没有访问过的点置为INFINTY，即删除这条边
    int hasArcs = 0;   // 用于记录被删除的弧数，被删除的边数等于这个的一半
    for(int n = 0;n < smallGraph.numVertexs;n++)
        for(int j = 0;j < smallGraph.numVertexs;j++)
        {
            if( n != j && ( visit[n] != true || visit[j] != true) ) {
                smallGraph.contains[n][j] = MYINFINITY;
                smallGraph.single_price[n][j] = MYINFINITY;
            }
            
            if (smallGraph.contains[n][j] != MYINFINITY)
                hasArcs++;
        }
    
    //  更新顶点数
    for (auto i : visit) {
        if (i != true)
            smallGraph.numVertexs--;
    }
    // 更新小图的其他参数，小图的参数是从基类派生来，所以需要进行修改。
    smallGraph.numConsumes = 1;     // 更新消费点数目
    smallGraph.edges = hasArcs / 2; // 更新边的数目
    auto temp = smallGraph.consume_position[comsume_position];      // 存储消费节点对于的value，以此更新小图
    smallGraph.consume_position.erase( smallGraph.consume_position.begin(), smallGraph.consume_position.end() );
    smallGraph.consume_position.insert(std::make_pair(comsume_position, temp)); // 更新消费节点与网络节点的连接关系
    
    
    /*
     cout << "comsume_position: " << comsume_position << endl;
     // 输出服务器位置
     
     for (auto i : smallGraph.service_position)  // smallGraph的service_position成员不用更新，这个成员会屏蔽基类成员
     cout << "service position: " << i << endl;
     */
    
    //partGraph smallGraph(graph, comsume_position, service_position);
    
    return smallGraph;
}



//--------------------------- 接受大图，求关联矩阵----------------------
vector<vector<float>> solveAlgorithm::getRelevanceMatrix(Graph& graph) {
    
    vector<vector<float>> CoefficientMatrix(graph.numVertexs+graph.edges*2+1,vector<float>(graph.edges*2+2,0));
    // map的key表示边对应的两个顶点，value表示这条边的编号，单价
    std::map<std::pair<int, int>, int> edges;
    std::map<std::pair<int, int>, std::pair<int,int>> edges_for_contains;
    std::map<int, std::pair<int,int>> index2Edges;
    
    int count=0;
    for (int i = 0; i < graph.contains.size(); i++) {
        for (int j = 0; j < graph.contains.size(); j++)
            if ( MYINFINITY != graph.contains[i][j] ){ // 如果i，j之间存在边
                //cout << "edge : " << i << ", " << j << " : " << count << endl;
                edges.insert( make_pair( make_pair(i, j), count ) );
                edges_for_contains.insert( make_pair( make_pair(i, j), make_pair(count,graph.contains[i][j]) ) );
                index2Edges.insert(make_pair(count, make_pair(i, j)));
                count++;
            }
    }
    
    
    graph.edgesToIndex = edges;
    graph.indexToEdges = index2Edges;
    
    //给约束矩阵赋值
    
    //目标函数的系数写入到矩阵的第一行
    int single_price = 0;
    for(auto temp = edges.begin();temp != edges.end();temp++){
        single_price = graph.single_price[temp->first.first][temp->first.second];
        CoefficientMatrix[0][temp->second] = single_price;
        
    }
    
    //每条弧对应的流量上限写入相应的约束条件中去
    for(auto temp = edges_for_contains.begin();temp != edges_for_contains.end();temp++)
        CoefficientMatrix[temp->second.first+1+graph.numVertexs][2*graph.edges+1] = temp->second.second;
    
    for(int j = 1;j <= graph.numVertexs; j++){
        for (int l = 0;l < graph.numVertexs; l++) {
            if (edges.find(make_pair(j-1 , l)) != edges.end()) {
                CoefficientMatrix[j][edges[make_pair(j-1, l)]] = 1;
                CoefficientMatrix[j][edges[make_pair(l, j-1)]] = -1;
            }//流进为负, 流出为正
        }
        CoefficientMatrix[j][graph.edges*2] = 1 ;//等式约束系数
    }//点弧连接矩阵的生成。
    
    //写入各个消费点需要的流量
    int tempindex;
    for (auto i = graph.NetNodeNeed.begin(); i != graph.NetNodeNeed.end(); i++) {
        tempindex = i->first;
        
        //-------------------为了线性规划能求解-----------
        // 线性规划不能接受右边参数小于0的情况，需要将其负号反转
        for (int j = 0; j < CoefficientMatrix[tempindex+1].size() - 2; j++)
            CoefficientMatrix[tempindex+1][j] = - CoefficientMatrix[tempindex+1][j];
        
        CoefficientMatrix[tempindex+1][2*graph.edges+1] = graph.NetNodeNeed[tempindex];
    }
    
    for (int i = graph.numVertexs+1; i <= graph.numVertexs+graph.edges*2; i++) {
        for(int j = 0; j < graph.edges*2; j++){
            if (i == (j + graph.numVertexs+1)) {
                CoefficientMatrix[i][j] = 1;
                break;
            }
        }
        CoefficientMatrix[i][2*graph.edges] = 0;// 容量约束的符号位，0表示小于等于
    }
    
    
    
    
    
    // 如果对于服务器点，这个点的约束应该删除，不过这样会造成一定的冗余。
    for (auto i : graph.service_position)
        for (int j = 0; j < CoefficientMatrix[i].size(); j++)
            CoefficientMatrix[i+1][j] = 0;
    
    
    return CoefficientMatrix;
}

/*
void solveAlgorithm::parseResultOfILP(vector<float>& flowOfEdges,
                                      Graph& graph){
    if (flowOfEdges[0] == 1) {
        vector<int > nodeLink;//记录每一条链路
        vector<int > nodeFlow;//记录需要改变的边号的索引
        int size_seriv = graph.service_position.size();
        int size_edgs = flowOfEdges.size()-2;   // 解里面未知数的个数
        int i = 0;
        int flag = 1;
        int consum;
        //vector<bool > visit(size_edgs, false);



        
        int sum_per_seriv = 0;
        for(size_seriv; size_seriv > 0; size_seriv--, i++){
            //int end = graph.search_consume_position;
            int start = graph.service_position[i];
            for(int k = 1; k <= size_edgs; k++){
                if (graph.indexToEdges[k-1].first == start) {
                    sum_per_seriv += flowOfEdges[k];
                }
            }
            while(sum_per_seriv != 0){//只要服务器节点还有流量流出，就继续找链路
                int min_flow =MYINFINITY;
                sum_per_seriv = 0;
                start = graph.service_position[i];
                nodeLink.push_back(start);
                int j = start;
                
                flag = 1;
                while (flag){//直到找到消费节点连接的网络节点
                    flag = 1;
                    for(int k = 1; k <= size_edgs; k++){
                        if(flowOfEdges[k] != 0){
                            if (graph.indexToEdges[k-1].first == j) {
                                if(min_flow >= flowOfEdges[k] ) {
                                    min_flow = flowOfEdges[k];
                                }
                                j = graph.indexToEdges[k-1].second;
                                for(int ii = 0; ii < graph.numConsumes; ii++ ){
                                    if((j == graph.consume_position[ii].first) &&
                                       (find(graph.service_position.begin(), graph.service_position.end(), j) == graph.service_position.end()) )
                                    {
                                        flag = 0;
                                        consum = ii;
                                        //min_flow = flowOfEdges[k];
                                    }
                                }
                                nodeLink.push_back(j);
                                nodeFlow.push_back(k);
                                break;
                            }
                        }
                    }
                }
                nodeLink.push_back(consum);
                nodeLink.push_back(min_flow);
                graph.result.push_back(nodeLink);
                
                for(int s = 0; s < nodeFlow.size(); s++)
                    flowOfEdges[nodeFlow[s]]-= min_flow;//找到一条链路后修改相应的解，直到解都为0
                
                nodeLink.clear();
                nodeFlow.clear();
                for(int k = 1; k <= size_edgs; k++){
                    if (graph.indexToEdges[k-1].first == start) {
                        
                        sum_per_seriv += flowOfEdges[k];
                    }
                }
            }
        }

        for(int i = 0; i < graph.numConsumes; i++){
            for (auto j:graph.service_position){
                if (graph.consume_position[i].first == j)
                    graph.result.push_back(vector<int>{j, i, graph.consume_position[i].second});
            }
        }
    }
    else {
        for(int i = 0; i < graph.numConsumes; i++){
            for (auto j:graph.service_position){
                if (graph.consume_position[i].first == j)
                    graph.result.push_back(vector<int>{j, i, graph.consume_position[i].second});
            }
        }
    }
    
}
*/

void solveAlgorithm::result(const char *filename,  Graph& graph){
    string s;
    //char a = ;
    char *topo_file= new char[1000000];
    char c[10];
    for(int i = 0;i < graph.result.size();i++){
        for(int j = 0; j< graph.result[i].size()-1; j++){
            sprintf(c, "%d ", graph.result[i][j]);
            s += c;
        }
        sprintf(c, "%d", graph.result[i][graph.result[i].size()-1]);
        s += c;
        if(i <graph.result.size()-1){
        sprintf(c, "\n");
        s += c;
        }
    }
    sprintf(c, "%d\n\n", graph.result.size());
    s = c+s;

    
    memcpy(topo_file,s.c_str(),s.length());
    
    write_result(topo_file, filename);
    
    delete [] topo_file;
}

int solveAlgorithm::returnCost(const vector<float>& flowOfEdges) {
    // 如果求解成功，则打印这个消费节点，及其对应的服务器节点，以及流量，按比赛要求的格式打印
    // 注意：若求解成功，要更新图里面相应弧的容量！！！！！！
    // 如果不成功，则输出遍历层数，无解，且记下这个消费节点，待会增加BFS的深度，再求一次
    if(flowOfEdges[0] == 1){
        int size = flowOfEdges.size();
        int cost;
        cost = flowOfEdges[size-1];
        
        return cost;
    }
    else
        return MYINFINITY;
}

void solveAlgorithm::BFS_cut(Graph& graph){
    vector<int> servicePositionInConsume2 = BFS1(graph, 2);
    sort(servicePositionInConsume2.begin(), servicePositionInConsume2.end());
    servicePositionInConsume2.erase(unique(servicePositionInConsume2.begin(), servicePositionInConsume2.end()), servicePositionInConsume2.end());
    graph.servicePositionInConsume = servicePositionInConsume2;
    vector<bool> servicePositionToBool(graph.numVertexs, false);
    for(int i=0; i < servicePositionInConsume2.size(); i++){
        servicePositionToBool[servicePositionInConsume2[i]] = 1;
    }
    int hasArcs = 0;
    for(int n = 0;n < graph.numVertexs;n++)
        for(int j = 0;j < graph.numVertexs;j++)
        {
            if( n != j && ( servicePositionToBool[n] != true || servicePositionToBool[j] != true) ) {
                graph.contains[n][j] = MYINFINITY;
                graph.single_price[n][j] = MYINFINITY;
            }
            
            if (graph.contains[n][j] != MYINFINITY)
                hasArcs++;
        }
    graph.numVertexs =  servicePositionInConsume2.size();
    //    for (auto i : visit_node) {
    //        if (i == true)
    //            graph.numVertexs++;
    //    }
    // 更新小图的其他参数，小图的参数是从基类派生来，所以需要进行修改。
    //        graph.numConsumes = 0;     // 更新消费点数目
    //        graph.consume_position.erase( graph.consume_position.begin(), graph.consume_position.end() );
    //        for (int i=0 ; i < visit_consume.size(); i++) {
    //            if (visit_consume[i] == true){
    //                smallGraph.numConsumes++;
    
    //                auto temp = graph.consume_position[i];      // 存储消费节点对于的value，以此更新小图
    
    //                smallGraph.consume_position.insert(std::make_pair(i, temp)); // 更新消费节点与网络节点的连接关系
    //            }
    //    }
    
    graph.edges = hasArcs / 2; // 更新边的数目
}



int solveAlgorithm::solver(Graph& graph) {
    
    int costAllConsumes = 0;
    
    //vector<vector<float>> CoefficientMatrix = getRelevanceMatrix(graph);  // 获取关联矩阵
    
    // flowOfEdges的第0位是标志位，表示求解成功与否
    //vector<float> flowOfEdges = linear_programming(CoefficientMatrix);
    //parseResultOfILP(flowOfEdges, graph);
    // 解析线性规划求解的结果并打印，同时更新图里面的容量矩阵
    // 如果无解，也输出相应提示
    // 返回值为代价，如果无解，则返回INFINTY
    //costAllConsumes = ZKW::zkw(graph, graph_param);
    costAllConsumes = MCF.solve( graph, graph_param);
    //costAllConsumes = returnCost(flowOfEdges);
    
//    if (costAllConsumes == MYINFINITY)
//        return costAllConsumes;
//    else
//        costAllConsumes = costAllConsumes + graph.service_position.size() * graph.cost_per_service;
    
    return costAllConsumes;
}

vector<int> solveAlgorithm::BFS1( Graph& graph,  int hiearcy)   {
    
    // @胡钦涛负责
    // 广度优先搜索，返回消费点出发，两层以内的网络节点，以此作为服务器的初始化位置

    
    
    int hiearcy_count=0;//层级计数
    vector<int> service_position;//返回服务器位置
    
    
    //partGraph smallGraph(graph, comsume_position, service_position); // 这里第三个参数服务器的位置暂时不初始化
    
    // vector<bool> visit(graph.numVertexs, false);//记录访问的节点
    std::queue<int> Que_hi;
    for(int f=0;f<graph.numConsumes;f++){
        vector<bool> visit(graph.numVertexs, false);
        hiearcy_count=0;
        auto comsume_net = graph.consume_position[f].first;//通过消费节点查找网络节点；
        int count = comsume_net;//记录访问的节点
        while ( hiearcy_count < hiearcy ) {//BFS
            //std::cout << hiearcy_count << " < " << hiearcy << std::endl;
            if( !visit[count] ) {
                visit[count] = 1;//访问过节点保留，否则删除
                service_position.push_back(count);
                // if ( servicePosition[count] == 1 ) { //该点是否为服务器节点
                hiearcy_count++;
                // cout << "count = " << count << endl;
                //smallGraph.service_position.push_back(count);//记录下服务器位置
                //}
                if ( hiearcy_count < hiearcy)
                    Que_hi.push(count);
                while ( !Que_hi.empty() ){
                    //cout << "fff" << endl;
                    auto i =Que_hi.front();
                    Que_hi.pop();
                    
                    for(int m=0;m < graph.numVertexs;m++)
                    {
                        if( graph.contains[i][m] == MYINFINITY )
                            continue;
                        
                        //std::cout << m << std::endl;
                        if( !visit[m] ){
                            service_position.push_back(m);
                            visit[m] = 1; //访问过节点保留，否则删除
                            //if( servicePosition[m] == 1 )   // 如果这点是服务器节点，就存起来
                            //{
                            // cout << "m = " << m << endl;
                            //if( hiearcy_count < hiearcy)
                            // smallGraph.service_position.push_back(m);
                            
                            //}
                            if(hiearcy_count < hiearcy)
                                Que_hi.push(m);
                            else
                                break;//tianjia!!!!
                        }
                    }
                    hiearcy_count++;
                    
                }
            }
            // break;
            
            
        }
    }
    
    
    return service_position;
}



//修改：returnco传参，减去graph，getRelevanceMatrix，右边可以为负，注释线规初始化A矩阵，线性规划符号有问题；

bool isService(int node,Graph& graph){
    for(int i:graph.service_position)
        if(node==i)
            return true;
    return false;
}
void return_i(int i,vector<int> stack_r,Graph& graph,vector<float> &flowOfEdges){
    stack_r.push_back(i);
    if(isService(i,graph)){
        vector<int> tmp=stack_r;
        for(int i1=tmp.size()-1;i1>2;i1--){
            int t=graph.edgesToIndex[make_pair(tmp[i1],tmp[i1-1])]+1;
            if (flowOfEdges[t] <= 0)
                return;
        }
        if(tmp[0]>graph.consume_position[tmp[1]].second)
            tmp[0]=graph.consume_position[tmp[1]].second;
        for(int i1=tmp.size()-1;i1>2;i1--){
            int t=graph.edgesToIndex[make_pair(tmp[i1],tmp[i1-1])]+1;
            if (tmp[0]>flowOfEdges[t])
                tmp[0]=flowOfEdges[t];
        }
        for(int i1=tmp.size()-1;i1>2;i1--){
            int t=graph.edgesToIndex[make_pair(tmp[i1],tmp[i1-1])]+1;
            //cout<<tmp[i1]<<" ";
            flowOfEdges[t]-=tmp[0];
        }
        graph.consume_position[tmp[1]].second-=tmp[0];
//        cout<<tmp[2]<<" ";
//        cout<<tmp[1]<<" ";
//        cout<<tmp[0]<<" ";
//        cout<<endl;
        vector<int> tmp1;
        for(int i1=tmp.size()-1;i1>=0;i1--){
            tmp1.push_back(tmp[i1]);
        }
        graph.result.push_back(tmp1);
        return;
    }
    
    int k,j=0;
    
    for(int m=0;m<graph.numVertexs;m++){
        //for(auto f=t.begin();f!=t.end();f++){
        if(graph.edgesToIndex.find(make_pair(m,i))!=graph.edgesToIndex.end()){
            if(flowOfEdges[graph.edgesToIndex[make_pair(m,i)]+1]>0){
                if(graph.consume_position[stack_r[1]].second>0){
                    vector<int> stack_r1=stack_r;
                    //                    if(stack_r1[0]>flowOfEdges[graph.edgesToIndex[make_pair(m,i)]+1])
                    //                        stack_r1[0]=flowOfEdges[graph.edgesToIndex[make_pair(m,i)]+1];
                    
                    return_i(m,stack_r1,graph,flowOfEdges);
                }
                
            }
        }
    }
    
    
    
}

void solveAlgorithm::return_text(Graph& graph,vector<float> &flowOfEdges){
    int size_seriv = graph.service_position.size();
    int size_edgs = flowOfEdges.size()-2;
    int size_con=graph.consume_position.size();
    vector<int> stack_re;
    int m,need;
    
    
    for(int i=0;i<size_con;i++){//找出所有消费节点
        stack_re.clear();
        stack_re.push_back(graph.consume_position[i].second);
        m=graph.consume_position[i].first;
        stack_re.push_back(i);
        // stack_re.push_back(m);
        return_i(m,stack_re,graph,flowOfEdges);
    }
    //cout<<endl;
    //  }
}



std::vector<int> solveAlgorithm::rand_m(int m,int n) {
    // assume m is 5 and n is 25.
    //int m
    //int n
    
    // initialize numbers.
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);
    
    // do random shuffle.
    
    // std::random_device rd;
    srand(int(time(NULL)));
    
    std::mt19937 g(rand()%98456862);
    // std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    
    // show first m numbers.
    std::vector<int> j(m);
    std::copy_n(v.begin(), m, j.begin());
    //std::cout << std::endl;
    
    return j;
}

void mergeService(Graph& graph) {
    for (int i = 0; i < graph.service_position.size(); i++) {
        int mindist = MYINFINITY;
        int mergedService1;
        int mergedService2;
        for (int j = 1; j < graph.service_position.size(); j++)
            if (mindist > graph.ShortPathTable[i][j]) {
                mindist = graph.ShortPathTable[i][j];
            }
    }
}

