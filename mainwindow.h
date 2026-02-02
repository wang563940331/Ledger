#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ledgermanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

private:
    Ui::MainWindow *ui;                 //!< UI对象指针
    LedgerManager *ledgerManager;       //!< 账本管理器指针
    QString excelFilePath;              //!< Excel文件路径
    
    /**
     * @brief 初始化账本
     */
    void initLedger();
};
#endif // MAINWINDOW_H