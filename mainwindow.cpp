/*
 * @Author: yu.wang
 * @Date: 2026-02-01 15:05:31
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-01 20:23:25
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
    
    // 设置表格视图
    ui->tableView->setModel(ledgerManager->getModel());
    
    // 加载数据
    ledgerManager->loadData(excelFilePath);
    
    // 在数据加载完成后调整列宽以适应内容
    ui->tableView->resizeColumnsToContents();
    
    // 为日期列设置最小宽度，确保完整显示
    ui->tableView->setColumnWidth(0, 100);
    
    // 设置定期余额默认值为上一次记录的值（如果有）
    double previousFixedDeposit = ledgerManager->getPreviousFixedDeposit();
    if (previousFixedDeposit > 0) {
        ui->fixedDepositSpinBox->setValue(previousFixedDeposit);
    }
    
    // 初始化控件状态
    // 设置当月存款控件为不可编辑和灰色状态
    ui->monthlyDepositSpinBox->setReadOnly(true);
    ui->monthlyDepositSpinBox->setStyleSheet("QDoubleSpinBox:read-only { background-color: lightgray; color: gray; }");
    //让所有列自动拉伸以占满整个表格宽度
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //为所有列设置最小宽度，确保内容不会被过度压缩
    ui->tableView->horizontalHeader()->setMinimumSectionSize(100);

    // 设置当月可支配额度控件为不可编辑和灰色状态，且不可选中
    ui->disposableAmountSpinBox->setReadOnly(true);
    ui->disposableAmountSpinBox->setStyleSheet("QDoubleSpinBox:read-only { background-color: lightgray; color: gray; }");
    
    // 设置当月开支控件初始状态
    if (ledgerManager->getRowCount() == 0) {
        // 第一次填写，当月开支可编辑
        ui->expenseSpinBox->setReadOnly(false);
        ui->expenseSpinBox->setStyleSheet("QDoubleSpinBox:read-only { background-color: white; color: black; }");
    } else {
        // 不是第一次填写，当月开支不可编辑且为灰色
        ui->expenseSpinBox->setReadOnly(true);
        ui->expenseSpinBox->setStyleSheet("QDoubleSpinBox:read-only { background-color: lightgray; color: gray; }");
    }
}

void MainWindow::calculateAmounts()
{
    double totalDeposit = ui->totalDepositSpinBox->value();//当前总存款金额：
    double salary = ui->salarySpinBox->value();//当月工资：
    double fixedDeposit = ui->fixedDepositSpinBox->value();//定期余额：
    double expenseSpinBox = ui->expenseSpinBox->value();//当月开支：
    double monthlyDeposit = 0.0;
    
    // 计算当月可支配额度 = 当前总存款金额 - 定期余额
    double disposableAmount = totalDeposit - fixedDeposit;
    ui->disposableAmountSpinBox->setValue(disposableAmount);

    
    // 检查是否有上一次记录
    int rowCount = ledgerManager->getRowCount();
    if (rowCount == 0) {
        // 第一次填写，没有上一次记录，提示用户手动填写当月开支
        ui->expenseSpinBox->setReadOnly(false);
        ui->expenseSpinBox->setStyleSheet("QDoubleSpinBox:read-only { background-color: white; color: black; }");
        // ui->expenseSpinBox->setValue(0.0);
        
        // 计算当月存款 = 当月工资 - 当月开支
        monthlyDeposit = salary - expenseSpinBox;
        ui->monthlyDepositSpinBox->setValue(monthlyDeposit);
    } else {
        // 有上一次记录，自动计算当月开支
        ui->expenseSpinBox->setReadOnly(true);
        ui->expenseSpinBox->setStyleSheet("QDoubleSpinBox:read-only { background-color: lightgray; color: gray; }");
        
        // 使用账本管理器计算金额
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
        
        // 清空输入框，但保留定期余额为当前值
        ui->totalDepositSpinBox->setValue(0);
        ui->salarySpinBox->setValue(0);
        // 保留定期余额为当前值，下次新增时默认使用
        ui->expenseSpinBox->setValue(0);
        ui->noteLineEdit->clear();
        
        // 重新计算，确保状态正确
        calculateAmounts();
    }
}