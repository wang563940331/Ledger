/*
 * @Author: yu.wang
 * @Date: 2026-02-01 15:05:31
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-28 22:08:28
 * @Description: 
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ledgermanager.h"
#include "src/curveGraph/curveGraph.h"

#include <QMessageBox>
#include <QDir>
// Include QtCharts headers
#include <QtCharts/QChartView>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ledgerManager(new LedgerManager(this))
    , curveGraph(new CurveGraph(this))
{
    ui->setupUi(this);
    // 设置窗口标题
    this->setWindowTitle("记账软件1.1");

    // 初始化账本功能
    initLedger();
    
    // 设置默认日期：上一个记录日期的下一个月的29号（如果没有则28号）
    QDate defaultDate;
    if (ledgerManager->isFirstRecord()) {
        // 第一次填写，使用当前日期
        defaultDate = QDate::currentDate();
    } else {
        // 获取上一个记录的日期
        QDate previousDate = ledgerManager->getPreviousDate();
        if (previousDate.isValid()) {
            // 计算下一个月的日期
            int nextMonthYear = previousDate.year();
            int nextMonthMonth = previousDate.month() + 1;
            if (nextMonthMonth > 12) {
                nextMonthMonth = 1;
                nextMonthYear++;
            }
            
            // 尝试使用29号，如果没有则使用28号
            if (QDate::isValid(nextMonthYear, nextMonthMonth, 29)) {
                defaultDate = QDate(nextMonthYear, nextMonthMonth, 29);
            } else {
                defaultDate = QDate(nextMonthYear, nextMonthMonth, 28);
            }
        } else {
            // 如果无法获取上一个日期，使用当前日期
            defaultDate = QDate::currentDate();
        }
    }
    ui->dateEdit->setDate(defaultDate);
    
    // 连接信号槽
    connect(ui->totalDepositSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    connect(ui->salarySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    connect(ui->fixedDepositSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    connect(ui->expenseSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    
    // 设置窗口打开时自动全屏显示
    this->showMaximized();
    
    // 初始化计算
    calculateAmounts();
    
    // 初始化图表视图
    curveGraph->initChartView(ui->chartView);
    
    // 初始加载数据
    curveGraph->updateData(ledgerManager->getModel());
    
    // 连接标签页切换信号
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    
    // 设置默认显示记账界面（索引0）
    ui->tabWidget->setCurrentIndex(0);
}

/**
 * @brief 析构函数
 */
MainWindow::~MainWindow()
{
    // 先删除curveGraph，确保它能正确处理chartView的引用
    delete curveGraph;
    
    // 再删除ledgerManager
    delete ledgerManager;
    
    // 最后删除ui，此时chartView已被安全处理
    delete ui;
}

/**
 * @brief 初始化账本功能
 */
void MainWindow::initLedger()
{
    // 设置文件路径
    //excelFilePath = QDir::toNativeSeparators("H:/My_project/QT/Ledger/ledger.csv");
    excelFilePath = QDir::currentPath() + "/ledger.csv";
    
    // 加载数据
    ledgerManager->loadData(excelFilePath);
    
    // 使用LedgerManager初始化表格视图
    ledgerManager->initTableView(ui->tableView);
    
    // 设置定期余额默认值为上一次记录的值（如果有）
    double previousFixedDeposit = ledgerManager->getPreviousFixedDeposit();
    if (previousFixedDeposit > 0) {
        ui->fixedDepositSpinBox->setValue(previousFixedDeposit);
    }
    
    // 使用LedgerManager配置UI控件
    ledgerManager->configureUI(ui->monthlyDepositSpinBox, 
                             ui->disposableAmountSpinBox, 
                             ui->expenseSpinBox);
}

/**
 * @brief 计算金额
 */
void MainWindow::calculateAmounts()
{
    double totalDeposit = ui->totalDepositSpinBox->value();
    double salary = ui->salarySpinBox->value();
    double fixedDeposit = ui->fixedDepositSpinBox->value();
    double expenseSpinBox = ui->expenseSpinBox->value();
    double monthlyDeposit = 0.0;
    
    // 使用LedgerManager计算可支配额度
    double disposableAmount = ledgerManager->calculateDisposableAmount(totalDeposit, fixedDeposit);
    ui->disposableAmountSpinBox->setValue(disposableAmount);
    
    // 检查是否有上一次记录
    if (ledgerManager->isFirstRecord()) {
        // 第一次填写，手动计算当月存款
        monthlyDeposit = salary - expenseSpinBox;
        ui->monthlyDepositSpinBox->setValue(monthlyDeposit);
    } else {
        // 使用LedgerManager自动计算金额
        ledgerManager->calculateAmounts(totalDeposit, salary, expenseSpinBox, monthlyDeposit, true);
        
        // 更新控件值
        ui->expenseSpinBox->setValue(expenseSpinBox);
        ui->monthlyDepositSpinBox->setValue(monthlyDeposit);
    }
}

/**
 * @brief 保存按钮点击事件处理函数
 */
void MainWindow::on_saveButton_clicked()
{
    // 获取当前数据
    QDate date = ui->dateEdit->date();
    double totalDeposit = ui->totalDepositSpinBox->value();
    double salary = ui->salarySpinBox->value();
    double fixedDeposit = ui->fixedDepositSpinBox->value();
    double expense = ui->expenseSpinBox->value();
    double monthlyDeposit = ui->monthlyDepositSpinBox->value();
    QString note = ui->noteLineEdit->text();
    
    // 添加记录到账本
    if (ledgerManager->addRecord(date, totalDeposit, salary, fixedDeposit, expense, monthlyDeposit, note)) {
        // 保存数据
        ledgerManager->saveData(excelFilePath);
        
        // 调整列宽
        ui->tableView->resizeColumnsToContents();
        
        // 重新设置列宽调整模式为拉伸，确保表头不会左缩进
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        
        // 清空输入框，但保留定期余额为当前值
        ui->totalDepositSpinBox->setValue(0);
        ui->salarySpinBox->setValue(0);
        ui->expenseSpinBox->setValue(0);
        ui->noteLineEdit->clear();
        
        // 重新计算，确保状态正确
        calculateAmounts();
    }
}

/**
 * @brief 标签页切换事件处理函数
 * @param index 当前标签页索引
 */
void MainWindow::onTabChanged(int index)
{
    if (index == 1) { // 图表标签页
        // 更新图表数据
        curveGraph->updateData(ledgerManager->getModel());
    }
}