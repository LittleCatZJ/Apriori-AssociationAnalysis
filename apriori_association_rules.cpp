/** Original author:https://github.com/ak94/Apriori  */


/**
 *Copyright:
 *This code is free software; you can redistribute it and/or modify
 *it under the terms of the GPL-3.0
 *
 * @author zhangjian
 * @date 2018.05
 *
 */

///////////////////////////////////////////////////////////////////////


#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

//对使用的数据结构进行宏定义
//对映射的遍历进行宏定义简化
#define structure map<vector<int>, double>
#define FOR_MAP(ii,T) for(structure::iterator (ii)=(T).begin();(ii)!=(T).end();(ii)++)
#define FOR_next_MAP(jj,ii,T) for(structure::iterator (jj)=(ii);(jj)!=(T).end();(jj)++)
#define VI vector<int>

const double MIN_SUP =0.5;
const double MIN_CONF=0.6;
int numItems=0;

structure C;                            //Apriori扫描时迭代使用
structure L;

structure Frequent_itemsets;            //收集所有的频繁项集
structure Support_data;                 //记录出现的项集的支持度

structure H;                            //求解关联规则时后件集合

void C1();                              //生成单个物品的候选集
void L1();                              //通过C1进行最小支持度的筛选生成L1
void generate_C();                      //这里只是上一层L进行了组合，没有计算支持度
void prune();                           //从L生成的候选集C进行修剪
void scan_D();                          //扫描数据集设置C中项集的支持度
void generate_L();                      //C中符合最小支持度的项集挑选出来
void apriori();
void generateRules();                   //生成关联规则
void merge_H();                         //对H进行可能的合并，与generate_C类似
//辅助函数
void set_count(VI );                    //给C中项集计数
bool check_compatibility(VI ,VI );      //检测两个vector內的除最后元素的其他元素是否一一对应相等
VI minus_v(VI ,VI );                    //从一个vector中删除与另一个相同的元素
void output(structure );                //将项集和频率对应的字典格式化输出

int main(int argc, char const *argv[])
{
    //apriori算法，发现频繁项集
	apriori();

    cout<<endl<<"Support data:"<<endl;
    output(Support_data);
    cout<<endl<<"Frequent_itemsets:"<<endl;
    output(Frequent_itemsets);
    cout<<endl<<"Association rules:\n";

    //从频繁项集中挖掘关联规则
    generateRules();

    system("PAUSE");
    return 0;
}

void apriori()
{
	C.clear();
	L.clear();

	bool mv=true;
	int index=2;
	while(true)
	{
	    //初始化生成C1和L1
		if (mv)
		{
			C1();
			cout<<"C1\n";
			output(C);

            //******************
            FOR_MAP(ii,C)
            {
                VI v=ii->first;
                Support_data[v]=ii->second;
            }
			//******************

			L1();
			cout<<"L1\n";
			output(L);

			mv=!mv;
		}
		else
		{
			generate_C();
			if(C.size()==0)
				break;
			cout<<"\nC"<<index<<"\n";
			output(C);
			prune();
			if (C.size()==0)
			{
				break;
			}
			cout<<"\nC"<<index<<" after prune \n";
			output(C);
			scan_D();
			//******************
            FOR_MAP(ii,C)
            {
                VI v=ii->first;
                Support_data[v]=ii->second;
            }
			//******************
			cout<<"\nC"<<index<<" after scaning dataset \n";
			output(C);
			generate_L();
			if (L.size()==0)
			{
				break;
			}
			cout<<"\nL"<<index<<"\n";
			output(L);
			index++;
		}
	}
}
void C1()
{
	ifstream fin;
	fin.open("input.txt");
	if(!fin)
    {
        cout<<"Input file opening error\n";
        exit(0);
    }

	int n;
	VI v;
	while(fin>>n)
	{
		v.clear();
		if (n==-1)
		{
		    numItems++;
			continue;
		}
		v.push_back(n);
		if(C.count(v)>0)
			C[v]++;
		else
			C[v]=1;
	}
	fin.close();

	FOR_MAP(ii,C)
	{
	    ii->second=ii->second/double(numItems);
	}
}

void output(structure T)
{
	cout<<"**********************"<<endl;
	VI v;
	FOR_MAP(ii,T)
	{
		v.clear();
		v=ii->first;
		cout<<'{';
		for (int i = 0; i < v.size(); ++i)
		{

            cout<<v[i];
            if(v[i]!=v.back()) cout<<", ";
		}
		cout<<'}';
		//cout<<" :---(frequency)----->> "<<ii->second;
		cout<<"  Support: "<<ii->second;
		cout<<"\n";

	}
}

void L1()
{
    cout<<endl;
	FOR_MAP(ii,C)
	{
		if (ii->second >= MIN_SUP)
		{
			L[ii->first]=ii->second;
			Frequent_itemsets[ii->first]=ii->second;
		}
	}

}

void generate_C()
{
	C.clear();
	FOR_MAP(ii,L)
	{
		FOR_next_MAP(jj,ii,L)
		{
			if(jj==ii)
				continue;
			VI a,b;
			a.clear();
			b.clear();
			a=ii->first;
			b=jj->first;
			if(check_compatibility(a,b))
			{
				a.push_back(b.back());
				sort(a.begin(), a.end());
				C[a]=0;
			}
		}
	}
}

bool check_compatibility(VI a,VI b)
{
	bool compatible=true;
	for (int i = 0; i < a.size()-1; ++i)
	{
		if (a[i]!=b[i])
		{
			compatible=false;
			break;
		}
	}

	return compatible;
}

void prune()
{
	VI a,b;
	FOR_MAP(ii,C)
	{
		a.clear();
		b.clear();

		a=ii->first;
		for(int i = 0;i<a.size();i++)
		{
			b.clear();
			for (int j = 0; j < a.size(); ++j)
			{
				if(j==i)
					continue;
				b.push_back(a[j]);
			}
			if(L.find(b)==L.end())
            {
                ii->second=-1;
                break;
            }

		}

	}

	structure temp;
	temp.clear();
	FOR_MAP(ii,C)
	{
		if (ii->second != -1)
		{
			temp[ii->first]=ii->second;
		}
	}

	C.clear();
	C=temp;
	temp.clear();
}

void scan_D()
{
	ifstream fin;
	fin.open("input.txt");
	if(!fin)
		{
			cout<<"Input file opening error\n";
			exit(0);
		}

	int n;
	VI a;
	while(fin>>n)
	{
		if(n==-1 && a.size()>0)
		{
			set_count(a);
			a.clear();
		}else if(n!=-1)
		{
			a.push_back(n);
		}

	}
	fin.close();

    FOR_MAP(ii,C)
	{
	    ii->second=ii->second/double(numItems);
	}
}

void set_count(VI a)
{
	FOR_MAP(ii,C)
	{
		VI b;
		b.clear();
		b=ii->first;
		int true_count=0;
		if (b.size()<=a.size())
		{
			for (int i = 0; i < b.size(); ++i)
			{
				for (int j = 0; j < a.size(); ++j)
				{
					if(b[i]==a[j])
					{
						true_count++;
						break;
					}
				}
			}
		}

		if (true_count==b.size())
		{
			ii->second++;
		}
	}
}

void generate_L()
{
	L.clear();

	FOR_MAP(ii,C)
	{
		if(ii->second >= MIN_SUP)
		{
			L[ii->first]=ii->second;
			Frequent_itemsets[ii->first]=ii->second;
		}
	}
}

void generateRules()
{
    ofstream fout;
	fout.open("output.txt");
	if(!fout)
    {
        cout<<"Output file opening error\n";
        exit(0);
    }

    FOR_MAP(ii,Frequent_itemsets)
    {
        VI freqSet=ii->first;
        double supp=Support_data[freqSet];//这一步是否有问题
        H.clear();
        for(int i=0;i<freqSet.size();i++)
        {
            VI temp;
            temp.push_back(freqSet[i]);
            H[temp]=0;//无所谓的一步，借用数据结构和函数
        }

        int m=1;
        while(m<freqSet.size())
        {

            //对每个可能的后件进行遍历
            FOR_MAP(ii,H)
            {
                VI p=minus_v(freqSet,ii->first);
                double conf=supp/Support_data[p];
                if(conf>=MIN_CONF){
                    cout<<'{';fout<<'{';
                    for(int x : p)
                    {
                        if(x!=p.back()) {
                            cout<<x<<", ";fout<<x<<", ";
                        }
                        else {cout<<x<<"} ";fout<<x<<"} ";}
                    }

                    cout<<"----> {";
                    fout<<"----> {";
                    for(int x : ii->first){
                        if(x!=ii->first.back())
                        {   cout<<x<<", ";fout<<x<<", "; }
                        else {  cout<<x<<'}';   fout<<x<<'}';}
                    }
                    cout<<" conf: "<<conf<<endl;
                    fout<<" conf: "<<conf<<endl;
                }
            }
            //对H进行可能的合并，与generate_C类似


        merge_H();
        m++;
        }
    }
}

VI minus_v(VI va,VI vb)
{
    VI diff;
    vector<int>::iterator ia=va.begin();
    vector<int>::iterator ib=vb.begin();
    while(ib!=vb.end())
    {
        if(*ia==*ib){
            va.erase(ia);
            ib++;
        }
        else{
            ia++;
        }
    }
    return va;
}

void merge_H()
{
    structure temp;
	FOR_MAP(ii,H)
	{

		FOR_next_MAP(jj,ii,H)
		{
			if(jj==ii)
				continue;
			VI a,b;
			a.clear();
			b.clear();
			a=ii->first;
			b=jj->first;
			if(check_compatibility(a,b))
			{
				a.push_back(b.back());
				sort(a.begin(), a.end());
				temp[a]=0;
			}
		}
	}
	H=temp;
}
