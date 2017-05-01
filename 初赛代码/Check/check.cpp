
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include<fstream>
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
void	check()
{
    FILE *fp = fopen("/Volumes/Macintosh HD/Users/mordekaiser/Documents/cpp_execise/NewHuaWei/case_example/第一批练习用例/result0.txt","r");
    if (fp == NULL)
    {
        return ;
    }
    char line[60];
    fgets(line, sizeof(line),fp);
    int num=atoi(line);
    
    fgets(line, sizeof(line),fp);
    stack<int> bian;
    Path p;
    memset(p,0,sizeof(Path));
    while(fgets(line, sizeof(line),fp))
    {
        if(line[0]=='\n') break;
        int temp;
        char *str;
        bian.push(atoi(strtok(line, " ")));
        while(str=strtok(NULL, " "))
            bian.push(atoi(str));
        int start,end;
        int allFlow,flow=bian.top();
        //	cout<<flow<<endl;
        bian.pop();
        end=bian.top();
        p[end]+=flow;
        bian.pop();
        allFlow=0;
        end=bian.top();
        bian.pop();
        while(!bian.empty())
        {
            start=bian.top();
            Flow[start][end]-=flow;
            if(Flow[start][end]<0)
            {
                cout<<start<<"->"<<end<<"流量超限"<<flow<<endl;
            }
            bian.pop();
            end=start;
        }
    }
    for(int i=0;i<GustNum;++i)
    {
        if (p[i]<GustNeed[i])
            cout<<"消费节点:"<<i<<"流量不满足"<<endl;
    }
}

int main()
{
    srand(unsigned(time(NULL)));
    int i=0,j=0;
    Flow = new int*[1500];
    for(i=0;i<1500;++i)
        Flow[i] = new int[1500];
    Price = new int*[1500];
    for(i=0;i<1500;++i)
        Price[i] = new int[1500];
    for (i=0;i<1500;++i)
        for(j=0;j<1500;++j)
        {
            Flow[i][j]=0;
            Price[i][j]=INFINITY;
        }
    FILE *fp = fopen("/Volumes/Macintosh HD/Users/mordekaiser/Documents/cpp_execise/NewHuaWei/case_example/第二批练习用例/2/case1.txt","r");
    if (fp == NULL)
    {
        return 0;
    }
    
    char line[60];
    fgets(line, sizeof(line),fp);
    NodeNum = atoi(strtok(line, " "));
    EdgeNum = atoi(strtok(NULL, " "));
    GustNum = atoi(strtok(NULL," "));
    
    
    fgets(line, sizeof(line),fp);
    fgets(line, sizeof(line),fp);
    SerPrice = atoi(strtok(line," "));
    fgets(line, sizeof(line),fp);
    
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
        GustsNode[i] = j;
        Flow[NodeNum+i][j] = atoi(strtok(NULL, " "));
        Flow[j][NodeNum+i] = Flow[NodeNum+i][j];
        //Flow[j][NodeNum+1]=Flow[j][NodeNum+1];
        GustFlow +=	Flow[NodeNum+i][j];
        GustNeed[i]=Flow[NodeNum+i][j];
        Price[j][NodeNum+i]=0;
        Price[NodeNum+i][j]=0;
    }
    fclose(fp);
    check();
    return 0;
}