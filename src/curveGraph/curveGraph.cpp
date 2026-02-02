#include "curveGraph.h"
#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>

/**
 * @brief 构造函数
 * @param parent 父对象指针
 */
CurveGraph::CurveGraph(QObject *parent) : QObject(parent)
{
    initChart();
}

/**
 * @brief 析构函数
 */
CurveGraph::~CurveGraph()
{
    // 如果chartView存在，先停止它引用chart
    if (chartView) {
        chartView->setChart(nullptr);
    }
    
    // 只需要删除chart，因为chart会自动删除添加到它的series和axis
    delete chart;
}

/**
 * @brief 初始化图表
 */
void CurveGraph::initChart()
{
    // 创建图表
    chart = new QChart();
    chart->setTitle("总存款金额变化趋势");
    chart->setAnimationOptions(QChart::AllAnimations);
    
    // 创建曲线系列
    series = new QLineSeries();
    series->setName("当前总存款金额");
    series->setMarkerSize(8);
    series->setPen(QPen(Qt::blue, 2));
    
    // 创建X轴（日期轴）
    axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("记账日期");
    axisX->setTickCount(8);
    
    // 创建Y轴（金额轴）
    axisY = new QValueAxis();
    axisY->setTitleText("当前总存款金额");
    axisY->setLabelFormat("¥%.0f"); // 只显示整数值
    axisY->setTickCount(10);
    axisY->setTickInterval(1000); // 默认刻度间隔为1000，后续会根据数据范围调整
    axisY->setMinorTickCount(2);
    
    // 将系列和坐标轴添加到图表
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
}
/**
 * @brief 更新图表数据
 * @param model 数据模型指针
 */
void CurveGraph::updateData(QStandardItemModel *model)
{
    // 清空现有数据
    series->clear();
    
    if (!model || model->rowCount() == 0) {
        return;
    }
    
    double minAmount = 0.0;
    double maxAmount = 0.0;
    bool firstPoint = true;
    
    // 遍历模型中的所有数据
    qDebug() << "Total rows:" << model->rowCount();
    for (int row = 0; row < model->rowCount(); ++row) {
        // 获取日期列（第0列）
        QStandardItem *dateItem = model->item(row, 0);
        if (!dateItem) {
            qDebug() << "Row" << row << ": dateItem is null";
            continue;
        }
        
        // 获取总存款金额列（第1列）
        QStandardItem *amountItem = model->item(row, 1);
        if (!amountItem) {
            qDebug() << "Row" << row << ": amountItem is null";
            continue;
        }
        
        // 格式化日期
        QString dateStr = dateItem->text();
        QDateTime date = formatDate(dateStr);
        if (!date.isValid()) {
            qDebug() << "Row" << row << ": invalid date" << dateStr;
            continue;
        }
        
        // 转换金额
        QString amountStr = amountItem->text();
        double amount = amountStr.toDouble();
        if (amount < 0) {
            qDebug() << "Row" << row << ": negative amount" << amount;
            continue;
        }
        
        // 更新最大最小金额
        if (firstPoint) {
            minAmount = amount;
            maxAmount = amount;
            firstPoint = false;
        } else {
            if (amount < minAmount) minAmount = amount;
            if (amount > maxAmount) maxAmount = amount;
        }
        
        // 添加数据点
        qDebug() << "Row" << row << ": adding point" << date << "-" << amount;
        series->append(date.toMSecsSinceEpoch(), amount);
    }
    
    qDebug() << "Total points added:" << series->count();
    
    // 设置Y轴范围
    if (!firstPoint) {
        double range = maxAmount - minAmount;
        double margin;
        
        // 如果只有一个数据点或范围很小，使用固定边距
        if (range < 1.0) {
            margin = 50.0; // 使用固定边距
        } else {
            margin = range * 0.1; // 正常情况下添加10%的边距
        }
        
        // 确保最小值不小于0
        minAmount = qMax(0.0, minAmount - margin);
        maxAmount = maxAmount + margin;
        
        // 设置合适的整数刻度间隔
        double tickInterval;
        if (maxAmount < 100) {
            tickInterval = 10; // 0-100之间，每10个单位一个刻度
        } else if (maxAmount < 500) {
            tickInterval = 50; // 100-500之间，每50个单位一个刻度
        } else if (maxAmount < 1000) {
            tickInterval = 100; // 500-1000之间，每100个单位一个刻度
        } else if (maxAmount < 5000) {
            tickInterval = 500; // 1000-5000之间，每500个单位一个刻度
        } else if (maxAmount < 10000) {
            tickInterval = 1000; // 5000-10000之间，每1000个单位一个刻度
        } else if (maxAmount < 50000) {
            tickInterval = 5000; // 10000-50000之间，每5000个单位一个刻度
        } else {
            tickInterval = 10000; // 50000以上，每10000个单位一个刻度
        }
        
        // 确保刻度值为整数，并且Y轴范围是tickInterval的整数倍
        minAmount = floor(minAmount / tickInterval) * tickInterval;
        maxAmount = ceil(maxAmount / tickInterval) * tickInterval;
        
        // 应用设置
        axisY->setRange(minAmount, maxAmount);
        axisY->setTickInterval(tickInterval);
        
        // 强制设置刻度数量，避免自动生成非整数刻度
        int tickCount = qRound((maxAmount - minAmount) / tickInterval) + 1;
        axisY->setTickCount(tickCount);
    }
    
    // 更新X轴范围
    if (series->count() > 0) {
        qDebug() << "Updating X-axis range...";
        QList<QPointF> points = series->points();
        qDebug() << "Points count:" << points.size();
        
        // 对于只有一个数据点的情况，不需要排序
        if (points.size() > 1) {
            std::sort(points.begin(), points.end(), [](const QPointF &a, const QPointF &b) {
                return a.x() < b.x();
            });
        }
        
        // 检查点是否有效
        if (points.isEmpty()) {
            qDebug() << "No valid points to set X-axis range";
            return;
        }
        
        // 使用第一个和最后一个点（对于单个点，它们是同一个）
        qDebug() << "First point x:" << points.first().x() << "y:" << points.first().y();
        qDebug() << "Last point x:" << points.last().x() << "y:" << points.last().y();
        
        // 确保时间戳有效
        if (points.first().x() <= 0 || points.last().x() <= 0) {
            qDebug() << "Invalid timestamp found";
            return;
        }
        
        QDateTime minDate = QDateTime::fromMSecsSinceEpoch(points.first().x());
        QDateTime maxDate = QDateTime::fromMSecsSinceEpoch(points.last().x());
        
        // 检查日期是否有效
        if (!minDate.isValid() || !maxDate.isValid()) {
            qDebug() << "Invalid date conversion";
            return;
        }
        
        qDebug() << "Min date before adjustment:" << minDate.toString();
        qDebug() << "Max date before adjustment:" << maxDate.toString();
        
        // 如果所有日期相同，添加一些边距
        if (minDate == maxDate) {
            qDebug() << "All dates are the same, adding margins...";
            // 使用addDays安全地添加边距
            minDate = minDate.addDays(-1);
            maxDate = maxDate.addDays(1);
            qDebug() << "Min date after adjustment:" << minDate.toString();
            qDebug() << "Max date after adjustment:" << maxDate.toString();
        }
        
        qDebug() << "Setting X-axis range...";
        axisX->setRange(minDate, maxDate);
        qDebug() << "X-axis range set successfully:" << minDate.toString() << "-" << maxDate.toString();
    }
}

/**
 * @brief 初始化图表视图
 * @param chartView 图表视图指针
 */
void CurveGraph::initChartView(QChartView *chartView)
{
    this->chartView = chartView;
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // 设置图表视图的样式
    chartView->setStyleSheet(
        "QChartView { background-color: #2a2a2a; color: white; }" 
        "QChart { background-color: #2a2a2a; color: white; }"
        "QChart::title { color: white; font-size: 16px; }"
        "QValueAxis { color: white; }"
        "QValueAxis::title { color: white; }"
        "QValueAxis::label { color: white; }"
        "QDateTimeAxis { color: white; }"
        "QDateTimeAxis::title { color: white; }"
        "QDateTimeAxis::label { color: white; }"
        "QLegend { color: white; }"
    );
}

/**
 * @brief 格式化日期
 * @param dateStr 日期字符串
 * @return 返回QDateTime对象
 */
QDateTime CurveGraph::formatDate(const QString &dateStr)
{
    // 支持多种日期格式
    QStringList formats = {
        "yyyy/MM/dd",
        "yyyy-MM-dd",
        "MM/dd/yyyy",
        "dd/MM/yyyy"
    };
    
    for (const QString &format : formats) {
        QDateTime date = QDateTime::fromString(dateStr, format);
        if (date.isValid()) {
            return date;
        }
    }
    
    qDebug() << "Invalid date format:" << dateStr;
    return QDateTime();
}