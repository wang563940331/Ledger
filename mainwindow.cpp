/*
 * @Author: yu.wang
 * @Date: 2026-02-01 15:05:31
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-02 13:52:56
 * @Description: 
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ledgermanager.h"

#include <QMessageBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ledgerManager(new LedgerManager(this))
{
    ui->setupUi(this);
    // 设置窗口标题
    this->setWindowTitle("记账软件1.0");

    // 设置默认日期为当前日期
    ui->dateEdit->setDate(QDate::currentDate());
    
    // 初始化账本功能
    initLedger();
    
    // 连接信号槽
    connect(ui->totalDepositSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    connect(ui->salarySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    connect(ui->fixedDepositSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    connect(ui->expenseSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::calculateAmounts);
    
    // 设置窗口打开时自动全屏显示
    this->showMaximized();
    
    // 初始化计算
    calculateAmounts();
}

MainWindow::~MainWindow()
{
    delete ledgerManager;
    delete ui;
}

void MainWindow::initLedger()
{
    // 设置文件路径
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