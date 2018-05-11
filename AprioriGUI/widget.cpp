#include "widget.h"
#include "ui_widget.h"

#include <iostream>
#include <fstream>




Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("Apriori");

    MIN_SUP=ui->lineEdit->text().toDouble();
    MIN_CONF=ui->lineEdit_2->text().toDouble();

    path=QString("../Apriori_GUI/input.txt");
}

Widget::~Widget()
{
    delete ui;
}


void Widget::readData()
{
    //读入
    ui->textBrowser->append(QString("读取训练集数据："));
    ifstream file2;
    file2.open(path.toStdString(),ios::in);
    string str;
    while(getline(file2,str,'\n'))
    {
         if(str.size()>0)
         ui->textBrowser->append(QString::fromStdString(str));
    }
}

void Widget::on_pushButton_clicked()
{
    C.clear();
    L.clear();
    Support_data.clear();
    Frequent_itemsets.clear();
    H.clear();
    numItems=0;
    MIN_SUP=ui->lineEdit->text().toDouble();
    MIN_CONF=ui->lineEdit_2->text().toDouble();

    //apriori算法，发现频繁项集
    ui->textBrowser->append("\nApriori算法求解频繁项集和关联规则\n最小支持度:"
              +ui->lineEdit->text()+";最小置信度:"+ui->lineEdit_2->text());
    apriori();

    ui->textBrowser->append("\nSupport data:");
    output(Support_data);
    ui->textBrowser->append("\nFrequent_itemsets:");
    output(Frequent_itemsets);
    ui->textBrowser->append("\nAssociation rules:");

    //从频繁项集中挖掘关联规则
    generateRules();

}

void Widget::on_pushButton_2_clicked()
{
    QString p = QFileDialog::getOpenFileName(this, tr("选择数据集"), ".", tr("txt Files(*.txt)"));
    if(!p.isNull()) path=p;
    //下面两步可以获得不含路径的文件名
    //int first=path.lastIndexOf("/");
    //path=path.right(path.length()-first-1);
    std::cout<<path.toStdString()<<std::endl;
    readData();
}


void Widget::apriori()
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
            //cout<<"C1\n";
            ui->textBrowser->append("C1");
            output(C);

            //******************
            FOR_MAP(ii,C)
            {
                VI v=ii->first;
                Support_data[v]=ii->second;
            }
            //******************

            L1();
            //cout<<"L1\n";
            ui->textBrowser->append("L1");
            output(L);

            mv=!mv;
        }
        else
        {
            generate_C();
            if(C.size()==0)
                break;
            //cout<<"\nC"<<index<<"\n";
            ui->textBrowser->append("\nC"+QString::number(index,10));
            output(C);
            prune();
            if (C.size()==0)
            {
                break;
            }
            //cout<<"\nC"<<index<<" after prune \n";
            ui->textBrowser->append("\nC"+QString::number(index,10)+" after prune ");
            output(C);
            scan_D();
            //******************
            FOR_MAP(ii,C)
            {
                VI v=ii->first;
                Support_data[v]=ii->second;
            }
            //******************
            //cout<<"\nC"<<index<<" after scaning dataset \n";
            ui->textBrowser->append("\nC"+QString::number(index,10)+" after scaning dataset ");
            output(C);
            generate_L();
            if (L.size()==0)
            {
                break;
            }
            //cout<<"\nL"<<index<<"\n";
            ui->textBrowser->append("\nL"+QString::number(index,10));
            output(L);
            index++;
        }
    }
}
void Widget::C1()
{
    ifstream fin;
    fin.open(path.toStdString());
    if(!fin)
    {
        cerr<<"Input file opening error\n";
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

void Widget::output(structure T)
{
    //cout<<"**********************"<<endl;
    ui->textBrowser->append("**********************");
    VI v;
    FOR_MAP(ii,T)
    {
        QString s;
        v.clear();
        v=ii->first;
        /*cout<<'{';*/s.append("{");
        for (int i = 0; i < v.size(); ++i)
        {

//            cout<<v[i];
            s.append(QString::number(v[i],10));
            if(v[i]!=v.back()) { /*cout<<", ";*/s.append(", "); }

        }
        /*cout<<'}';*/s.append("}");
        //cout<<" :---(frequency)----->> "<<ii->second;
        //cout<<"  Support: "<<ii->second;
        //cout<<"\n";
        s.append("  Support: "+QString::number(ii->second));
        ui->textBrowser->append(s);

    }
}

void Widget::L1()
{
    //cout<<endl;
    ui->textBrowser->append(" ");
    FOR_MAP(ii,C)
    {
        if (ii->second >= MIN_SUP)
        {
            L[ii->first]=ii->second;
            Frequent_itemsets[ii->first]=ii->second;
        }
    }

}

void Widget::generate_C()
{
    //clean(C);
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

bool Widget::check_compatibility(VI a,VI b)
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

void Widget::prune()
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

void Widget::scan_D()
{
    ifstream fin;
    fin.open(path.toStdString());
    if(!fin)
        {
            cerr<<"Input file opening error\n";
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

void Widget::set_count(VI a)
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

void Widget::generate_L()
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

void Widget::generateRules()
{
    ofstream fout;
    fout.open("output.txt");
    if(!fout)
    {
        cerr<<"Output file opening error\n";
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
                QString str;
                VI p=minus_v(freqSet,ii->first);
                double conf=supp/Support_data[p];
                if(conf>=MIN_CONF){
                    /*cout<<'{';*/fout<<'{';str.append("{");
                    for(int x : p)
                    {
                        if(x!=p.back()) {
                            /*cout<<x<<", ";*/fout<<x<<", "; str.append(QString::number(x,10)+", ");
                        }
                        else {/*cout<<x<<"} ";*/fout<<x<<"} ";str.append(QString::number(x,10)+"} ");}
                    }

                    //cout<<"----> {";
                    fout<<"----> {";
                    str.append("----> {");
                    for(int x : ii->first){
                        if(x!=ii->first.back())
                        {   /*cout<<x<<", ";*/fout<<x<<", ";str.append(QString::number(x,10)+", "); }
                        else {  /*cout<<x<<'}';*/   fout<<x<<'}';str.append(QString::number(x,10)+"}");}
                    }
                    //cout<<" conf: "<<conf<<endl;
                    fout<<" conf: "<<conf<<endl;
                    str.append(" conf: "+QString::number(conf));
                }
                ui->textBrowser->append(str);
            }
            //对H进行可能的合并，与generate_C类似


        merge_H();
        m++;
        }

    }
}

VI Widget::minus_v(VI va,VI vb)
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

void Widget::merge_H()
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
