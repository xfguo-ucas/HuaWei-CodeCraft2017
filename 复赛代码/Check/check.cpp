
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include<fstream>
#include <unordered_map>
#include <stack>
using namespace std;
#define INFINITY 65535
int **Flow,**Price,**minFlow,**minPrice;
int GustsNode[500];
int NodeNum ;
int EdgeNum ;
int GustNum ;
int SerPrice;
int GustFlow;
int allFlow;
int GustNeed[500];
typedef int Path[1500];
typedef int MinPrice[1500];
typedef int USEFLOW[1500][1500];
vector<vector<int>> ServiceLevels;
std::unordered_map<int, int> service_send;
std::unordered_map<int, int> output_service_level;
std::unordered_map<int, int> service_deploy_cost;
int RIGHT = 1; // 校验成功
int COST = 0;
int	check()
{
    FILE *fp = fopen("/Volumes/Macintosh HD/Users/mordekaiser/Documents/cpp_execise/NewMCF/case_example/2/result0.txt","r");
    if (fp == NULL)
    {
        cout << "No File!" <<endl;
        return 0;
    }
    char line[60];
    fgets(line, sizeof(line),fp);
    fgets(line, sizeof(line),fp);
    stack<int> bian;
    Path p;
    memset(p,0,sizeof(Path));
    int tempflag = 0;
    while(fgets(line, sizeof(line),fp))
    {
        if(line[0]=='\n') break;
        char *str;
        bian.push(atoi(strtok(line, " ")));
        
        int temp = bian.top();
        if (service_send.find(temp) == service_send.end())
            service_send.insert(make_pair(temp, 0));
        
        while(str=strtok(NULL, " "))
            bian.push(atoi(str));
        int servicelevel = bian.top();
        bian.pop();
        service_send[temp] += bian.top();
        
        if (temp == 620) {
            tempflag++;
            //cout << bian.top() << endl;
            //cout << service_send[temp] << endl;
        }
        output_service_level.insert(make_pair(temp, servicelevel));

        
        int start,end;
        int allFlow,flow=bian.top();
        //	cout<<flow<<endl;
        bian.pop();
        end=bian.top();//消费点
        
        p[end]+=flow;
        bian.pop();
        allFlow=0;
        end=bian.top(); //网络点

        bian.pop();
        while(!bian.empty())
        {
            start=bian.top();
            Flow[start][end]-=flow;
            COST += flow * Price[start][end];
            if(Flow[start][end]<0)
            {
                // 不一定是正好超出flow这么多，只是减去flow之后，流量上限小于0而已了
                cout<<start<<"->"<<end<<"流量超限"<<flow<<endl;
                RIGHT  = 0;
            }
            bian.pop();
            end=start;
        }
    }
    for(int i=0;i<GustNum;++i)
    {
        if (p[i]<GustNeed[i]) {
            cout<<"消费节点:"<<i<<"流量不满足"<<endl;
            RIGHT  = 0;
        }
    }
    for (auto ii : service_send) {
        int diff = ii.second - ServiceLevels[ output_service_level[ii.first] ][1];
        COST = COST + ServiceLevels[ output_service_level[ii.first] ][2] + service_deploy_cost[ii.first];
        if (diff > 0) {
            cout << "服务器节点:" << ii.first << "流量超出" << diff << endl;
            RIGHT  = 0;
        }
    }
    return RIGHT;
}

int main()
{
    srand(unsigned(time(NULL)));
    
    int i=0,j=0;
    Flow = new int*[2500];
    for(i=0;i<2500;++i)
        Flow[i] = new int[2500];
    Price = new int*[2500];
    for(i=0;i<2500;++i)
        Price[i] = new int[2500];
    for (i=0;i<2500;++i)
        for(j=0;j<2500;++j)
        {
            Flow[i][j]=0;
            Price[i][j]=INFINITY;
        }
    FILE *fp = fopen("/Volumes/Macintosh HD/Users/mordekaiser/Documents/cpp_execise/NewMCF/case_example/1/case0.txt","r");
    if (fp == NULL)
    {
        return 0;
    }
    
    char line[60];
    fgets(line, sizeof(line),fp);
    NodeNum = atoi(strtok(line, " "));
    EdgeNum = atoi(strtok(NULL, " "));
    GustNum = atoi(strtok(NULL," "));

    //-----
    fgets(line, sizeof(line),fp);
    int k;
    while(fgets(line, sizeof(line),fp))
    {
        if(line[0]=='\r') break;
        i = atoi(strtok(line, " "));
        j = atoi(strtok(NULL, " "));
        k = atoi(strtok(NULL, " "));
        ServiceLevels.push_back({i, j, k});
    }
    
    //------
    while(fgets(line, sizeof(line),fp))
    {
        
        if(line[0]=='\r') break;
        i = atoi(strtok(line, " "));
        j = atoi(strtok(nullptr, " "));
        service_deploy_cost.insert(make_pair(i, j));
    }
    
    
    
    while(fgets(line, sizeof(line),fp))
    {
        if(line[0]=='\r') break;
        i = atoi(strtok(line, " "));
        j = atoi(strtok(NULL, " "));
        Flow[i][j] = atoi(strtok(NULL, " "));
        Flow[j][i] = Flow[i][j];
        Price[i][j] = atoi(strtok(NULL, " "));
        Price[j][i]=Price[i][j];
        
    }	
    
    GustFlow=0;
    while(fgets(line, sizeof(line),fp))
    {
        i = atoi(strtok(line, " "));

        j = atoi(strtok(NULL, " "));
        k = atoi(strtok(nullptr, " "));
        GustsNode[i] = j;
        //auto a = strtok(nullptr, " ");
        Flow[NodeNum+i][j] = k;
        Flow[j][NodeNum+i] = Flow[NodeNum+i][j];
        //Flow[j][NodeNum+1]=Flow[j][NodeNum+1];
        GustFlow +=	Flow[NodeNum+i][j];
        GustNeed[i]=Flow[NodeNum+i][j];
        Price[j][NodeNum+i]=0;
        Price[NodeNum+i][j]=0;
    }
    fclose(fp);
    if (check()) {
        cout << "No Problem! Cost is " << COST << endl;
    }
    return 0;
}