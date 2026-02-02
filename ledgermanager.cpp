/*
 * @Author: yu.wang
 * @Date: 2026-02-01 17:42:19
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-02 14:02:25
 * @Description: 
 */
#include "ledgermanager.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTableView>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QStyleFactory>

LedgerManager::LedgerManager(QObject *parent)
    : QObject(parent)
{
    initModel();
}

LedgerManager::~LedgerManager()
{
    delete model;
}

QStandardItemModel* LedgerManager::getModel() const
{
    return model;
}

void LedgerManager::initModel()
{
    model = new QStandardItemModel(this);
    
    // 设置表头
    QStringList headers;
    headers << "记账日期" << "当前总存款金额" << "当月工资" << "定期余额" << "当月开支" << "当月存款" << "当月可支配额度" << "备注";
    model->setHorizontalHeaderLabels(headers);
}

void LedgerManager::loadData(const QString &filePath)
{
    currentFilePath = filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 如果文件不存在，创建一个新文件
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.close();
        return;
    }
    
    QTextStream in(&file);
    int row = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue; // 跳过空行
        
        QStringList fields = line.split(",");
        
        // 智能判断是否包含序号列（检查第一个字段是否为数字）
        bool hasIndexColumn = false;
        if (!fields.isEmpty()) {
            bool isNumber = false;
            fields[0].toInt(&isNumber);
            hasIndexColumn = isNumber;
        }
        
        if (hasIndexColumn) {
            // 包含序号列，从fields[1]开始加载
            int startCol = 1;
            int maxCol = qMin(fields.size() - startCol, 8); // 最多加载8列数据
            for (int col = 0; col < maxCol; ++col) {
                QStandardItem *item = new QStandardItem(fields[col + startCol]);
                model->setItem(row, col, item);
            }
            // 如果缺少字段，用空字符串填充
            for (int col = maxCol; col < 8; ++col) {
                QStandardItem *item = new QStandardItem("");
                model->setItem(row, col, item);
            }
            row++;
        } else if (fields.size() >= 8) {
            // 不包含序号列，直接加载前8个字段
            for (int col = 0; col < 8; ++col) {
                QStandardItem *item = new QStandardItem(fields[col]);
                model->setItem(row, col, item);
            }
            row++;
        }
        // 对于其他格式，跳过该行
    }
    
    file.close();
}

void LedgerManager::saveData(const QString &filePath)
{
    currentFilePath = filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showError("错误", "无法打开文件进行保存！");
        return;
    }
    
    QTextStream out(&file);
    
    for (int row = 0; row < model->rowCount(); ++row) {
        QString line;
        
        // 添加序号（格式：行号+1）
        line += QString::number(row + 1);
        
        // 添加其他字段
        for (int col = 0; col < model->columnCount(); ++col) {
            line += ",";
            
            QStandardItem *item = model->item(row, col);
            if (item) {
                line += item->text();
            } else {
                line += "";
            }
        }
        out << line << "\n";
    }
    
    file.close();
    showSuccess("成功", "记录已保存！");
}

double LedgerManager::calculateDisposableAmount(double totalDeposit, double fixedDeposit) const
{
    return totalDeposit - fixedDeposit;
}

void LedgerManager::calculateAmounts(double totalDeposit, double salary, double &expense, double &monthlyDeposit, bool hasPreviousRecord)
{
    if (hasPreviousRecord) {
        // 有上一次记录，自动计算当月开支
        double previousTotalDeposit = getPreviousTotalDeposit();
        expense = previousTotalDeposit + salary - totalDeposit;
    }
    
    // 计算当月存款 = 当月工资 - 当月开支
    monthlyDeposit = salary - expense;
}

bool LedgerManager::isFirstRecord() const
{
    return model->rowCount() == 0;
}

void LedgerManager::initTableView(QTableView *tableView) const
{
    // 设置模型
    tableView->setModel(const_cast<QStandardItemModel*>(model));
    
    // 禁用编辑功能
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置表格外观
    tableView->setAlternatingRowColors(true);
    
    // 应用暗色主题样式
    setupDarkThemeStyle(tableView);
    
    // 设置列宽
    tableView->resizeColumnsToContents();
    tableView->setColumnWidth(0, 120); // 日期列最小宽度
    
    // 设置自动拉伸
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setMinimumSectionSize(120);
}

void LedgerManager::configureUI(QDoubleSpinBox *monthlyDepositSpinBox, QDoubleSpinBox *disposableAmountSpinBox, QDoubleSpinBox *expenseSpinBox) const
{
    // 配置当月存款控件
    configureWidgetStyle(monthlyDepositSpinBox, true);
    
    // 配置可支配额度控件
    configureWidgetStyle(disposableAmountSpinBox, true);
    
    // 配置当月开支控件
    if (isFirstRecord()) {
        // 第一次填写，可编辑
        configureWidgetStyle(expenseSpinBox, false);
    } else {
        // 不是第一次填写，不可编辑
        configureWidgetStyle(expenseSpinBox, true);
    }
}

void LedgerManager::showError(const QString &title, const QString &message) const
{
    QMessageBox::warning(nullptr, title, message);
}

void LedgerManager::showSuccess(const QString &title, const QString &message) const
{
    QMessageBox::information(nullptr, title, message);
}

bool LedgerManager::confirmOperation(const QString &title, const QString &message) const
{
    return QMessageBox::question(nullptr, title, message) == QMessageBox::Yes;
}

void LedgerManager::setupDarkThemeStyle(QTableView *tableView) const
{
    tableView->setStyleSheet(
        "QTableView { background-color: #2a2a2a; color: white; }"  // 主背景色和文字颜色
        "QTableView::item:alternate { background-color: #323232; }"  // 交替行背景色
        "QTableView { gridline-color: #4a4a4a; }"  // 网格线颜色
    );
    
    tableView->horizontalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: #3a3a3a; color: #ffffff; padding: 8px; border: 1px solid #4a4a4a; }"
    );
}

void LedgerManager::configureWidgetStyle(QWidget *widget, bool readOnly, const QString &readOnlyColor, const QString &textColor) const
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(widget);
    if (spinBox) {
        spinBox->setReadOnly(readOnly);
        
        if (readOnly) {
            spinBox->setStyleSheet(QString("QDoubleSpinBox:read-only { background-color: %1; color: #888888; }").arg(readOnlyColor));
        } else {
            spinBox->setStyleSheet(QString("QDoubleSpinBox { background-color: %1; color: %2; }").arg(readOnlyColor, textColor));
        }
    }
}

double LedgerManager::getPreviousTotalDeposit() const
{
    int rowCount = model->rowCount();
    if (rowCount == 0) {
        return 0.0;
    }
    
    QStandardItem *previousItem = model->item(rowCount - 1, 1); // 获取上一次记录的总存款金额
    if (previousItem) {
        return previousItem->text().toDouble();
    }
    
    return 0.0;
}

double LedgerManager::getPreviousFixedDeposit() const
{
    int rowCount = model->rowCount();
    if (rowCount == 0) {
        return 0.0;
    }
    
    QStandardItem *previousItem = model->item(rowCount - 1, 3); // 获取上一次记录的定期余额
    if (previousItem) {
        return previousItem->text().toDouble();
    }
    
    return 0.0;
}

bool LedgerManager::addRecord(const QDate &date, double totalDeposit, double salary, double fixedDeposit, double expense, double monthlyDeposit, const QString &note)
{
    // 数据验证
    if (!date.isValid()) {
        QMessageBox::warning(nullptr, "数据验证失败", "记账日期无效！");
        return false;
    }
    
    if (totalDeposit <= 0) {
        QMessageBox::warning(nullptr, "数据验证失败", "当前总存款金额必须大于0！");
        return false;
    }
    
    if (salary < 0) {
        QMessageBox::warning(nullptr, "数据验证失败", "当月工资不能为负数！");
        return false;
    }
    
    if (fixedDeposit < 0) {
        QMessageBox::warning(nullptr, "数据验证失败", "定期余额不能为负数！");
        return false;
    }
    
    if (fixedDeposit > totalDeposit) {
        QMessageBox::warning(nullptr, "数据验证失败", "定期余额不能大于当前总存款金额！");
        return false;
    }
    
    // 验证当前总存款金额是否小于等于上一次总存款金额 + 当月工资
    int rowCount = model->rowCount();
    if (rowCount > 0) {
        double previousTotalDeposit = getPreviousTotalDeposit();
        if (totalDeposit > previousTotalDeposit + salary) {
            QMessageBox::warning(nullptr, "数据验证失败", "当前总存款金额不能大于上一次总存款金额与当月工资之和！");
            return false;
        }
    }
    
    if (expense < 0) {
        int ret = QMessageBox::question(nullptr, "确认", "当月开支为负数，是否继续？");
        if (ret != QMessageBox::Yes) {
            return false;
        }
    }
    
    // 检查是否为第一次填写且没有输入当月开支
    if (getRowCount() == 0 && expense == 0.0) {
        QMessageBox::warning(nullptr, "警告", "这是第一次填写记录，当月开支为0，请确认是否正确！");
    }
    
    // 格式化数据
    QString dateStr = date.toString("yyyy/MM/dd");
    QString totalDepositStr = QString::number(totalDeposit, 'f', 2);
    QString salaryStr = QString::number(salary, 'f', 2);
    QString fixedDepositStr = QString::number(fixedDeposit, 'f', 2);
    QString expenseStr = QString::number(expense, 'f', 2);
    QString monthlyDepositStr = QString::number(monthlyDeposit, 'f', 2);
    
    // 计算当月可支配额度 = 当前总存款金额 - 定期余额
    double disposableAmount = totalDeposit - fixedDeposit;
    QString disposableAmountStr = QString::number(disposableAmount, 'f', 2);
    
    // 添加新行
    QList<QStandardItem*> items;
    items << new QStandardItem(dateStr);
    items << new QStandardItem(totalDepositStr);
    items << new QStandardItem(salaryStr);
    items << new QStandardItem(fixedDepositStr);
    items << new QStandardItem(expenseStr);
    items << new QStandardItem(monthlyDepositStr);
    items << new QStandardItem(disposableAmountStr);
    items << new QStandardItem(note);
    
    model->appendRow(items);
    
    return true;
}

int LedgerManager::getRowCount() const
{
    return model->rowCount();
}