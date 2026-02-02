#ifndef CURVEGRAPH_H
#define CURVEGRAPH_H

#include <QObject>
#include <QStandardItemModel>
#include <QDateTime>

// Forward declarations for QtCharts classes
class QChart;
class QLineSeries;
class QDateTimeAxis;
class QValueAxis;
class QChartView;

class CurveGraph : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit CurveGraph(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~CurveGraph();

    /**
     * @brief 更新图表数据
     * @param model 数据模型指针
     */
    void updateData(QStandardItemModel *model);
    
    /**
     * @brief 初始化图表视图
     * @param chartView 图表视图指针
     */
    void initChartView(QChartView *chartView);

private:
    QChart *chart;              //!< 图表对象
    QLineSeries *series;        //!< 曲线系列
    QDateTimeAxis *axisX;       //!< X轴（日期轴）
    QValueAxis *axisY;          //!< Y轴（金额轴）
    QChartView *chartView;      //!< 图表视图
    
    /**
     * @brief 初始化图表
     */
    void initChart();
    
    /**
     * @brief 格式化日期
     * @param dateStr 日期字符串
     * @return 返回QDateTime对象
     */
    QDateTime formatDate(const QString &dateStr);
};

#endif // CURVEGRAPH_H