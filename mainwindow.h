#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "src/ledgermanager/ledgerManager.h"
#include "src/curveGraph/curveGraph.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Include QtCharts namespace
namespace QtCharts {
    class QChartView;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow();

private slots:
    /**
     * @brief 保存按钮点击事件处理
     */
    void on_saveButton_clicked();
    
    /**
     * @brief 计算金额
     */
    void calculateAmounts();
    
    /**
     * @brief 标签页切换事件处理
     * @param index 当前标签页索引
     */
    void onTabChanged(int index);

private:
    Ui::MainWindow *ui;                 //!< UI对象指针
    LedgerManager *ledgerManager;       //!< 账本管理器指针
    CurveGraph *curveGraph;             //!< 图表管理器指针
    QString excelFilePath;              //!< Excel文件路径
    
    /**
     * @brief 初始化账本
     */
    void initLedger();
};
#endif // MAINWINDOW_H