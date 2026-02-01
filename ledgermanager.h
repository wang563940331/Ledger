/*
 * @Author: yu.wang
 * @Date: 2026-02-01 17:41:35
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-01 22:24:29
 * @Description: 
 */
#ifndef LEDGERMANAGER_H
#define LEDGERMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QDate>

/*
    QStandardItemModel的作用是：
    数据存储：账本的所有记录（日期、收入、支出等）都存储在该模型中
    数据展示：通过tableView将模型中的数据以表格形式呈现给用户
    数据操作：支持添加新记录、修改现有数据、删除记录等操作
    样式控制：可以为不同的单元格设置样式（如交替行颜色、表头样式等）

    使用QStandardItemModel（而非直接操作UI）的好处：

    数据与界面分离：修改数据不影响UI，修改UI不影响数据
    代码复用：同一模型可用于不同类型的视图
    性能优化：大数据量时只更新变化的部分，而非整个界面
    扩展性：便于添加新的视图类型或数据处理逻辑

*/


class LedgerManager : public QObject
{
    Q_OBJECT

public:
    explicit LedgerManager(QObject *parent = nullptr);
    ~LedgerManager();

    QStandardItemModel* getModel() const;
    void loadData(const QString &filePath);
    void saveData(const QString &filePath);
    void calculateAmounts(double totalDeposit, double salary, double &expense, double &monthlyDeposit, bool hasPreviousRecord = false);
    double getPreviousTotalDeposit() const;
    double getPreviousFixedDeposit() const;
    bool addRecord(const QDate &date, double totalDeposit, double salary, double fixedDeposit, double expense, double monthlyDeposit, const QString &note);
    int getRowCount() const;

private:
    QStandardItemModel *model;
    QString currentFilePath;
    void initModel();
};

#endif // LEDGERMANAGER_H