#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>

#include <vector>
#include <map>

using namespace std;

//对使用的数据结构进行宏定义
//对映射的遍历进行宏定义简化
#define structure map<vector<int>, double>
#define FOR_MAP(ii,T) for(structure::iterator (ii)=(T).begin();(ii)!=(T).end();(ii)++)
#define FOR_next_MAP(jj,ii,T) for(structure::iterator (jj)=(ii);(jj)!=(T).end();(jj)++)
#define VI vector<int>



namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();           //求解按钮的槽
    void on_pushButton_2_clicked();         //选择文件的槽

private:
    Ui::Widget *ui;
    double MIN_SUP ;                        //最小支持度和置信度设置
    double MIN_CONF;
    int numItems=0;                         //文件中项集的数目
    QString path;                           //数据集路径名

    structure C;
    structure L;

    structure Frequent_itemsets;            //收集所有的频繁项集
    structure Support_data;                 //记录出现的项集的支持度

    structure H;                            //求解关联规则时后件集合
    void readData();                        //读取文件数据并显示

    void C1();                              //生成单个物品的候选集
    void L1();                              //通过C1进行最小支持度的筛选生成L1
    void generate_C();                      //这里只是上一层L进行了组合，没有计算支持度
    void generate_L();                      //C中符合最小支持度的项集挑选出来
    void output(structure );                //将项集和频率对应的字典格式化输出
    void scan_D();                          //扫描数据集设置C的频率
    void prune();                           //从L生成的候选集C进行修剪
    bool check_compatibility(VI ,VI );      //检测两个vector內的除最后元素的其他元素是否一一对应相等
    void set_count(VI );
    void generateRules();
    VI minus_v(VI ,VI );
    void merge_H();
    void apriori();

};

#endif // WIDGET_H
