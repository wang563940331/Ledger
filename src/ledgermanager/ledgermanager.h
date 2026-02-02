/*
 * @Author: yu.wang
 * @Date: 2026-02-01 17:41:35
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-02 20:54:38
 * @Description: 
 */
#ifndef LEDGERMANAGER_H
#define LEDGERMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QDate>
#include <QTableView>
#include <QDoubleSpinBox>
#include <QMessageBox>

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
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit LedgerManager(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~LedgerManager();

    // 模型和数据操作接口
    /**
     * @brief 获取账本数据模型
     * @return 返回QStandardItemModel指针
     */
    QStandardItemModel* getModel() const;
    
    /**
     * @brief 从文件加载账本数据
     * @param filePath 文件路径
     */
    void loadData(const QString &filePath);
    
    /**
     * @brief 保存账本数据到文件
     * @param filePath 文件路径
     */
    void saveData(const QString &filePath);
    
    /**
     * @brief 添加新的记账记录
     * @param date 记账日期
     * @param totalDeposit 当前总存款金额
     * @param salary 当月工资
     * @param fixedDeposit 定期余额
     * @param expense 当月开支
     * @param monthlyDeposit 当月存款
     * @param note 备注
     * @return 添加成功返回true，否则返回false
     */
    bool addRecord(const QDate &date, double totalDeposit, double salary, double fixedDeposit, double expense, double monthlyDeposit, const QString &note);
    
    // 计算相关接口
    /**
     * @brief 计算当月可支配额度
     * @param totalDeposit 当前总存款金额
     * @param fixedDeposit 定期余额
     * @return 返回可支配额度
     */
    double calculateDisposableAmount(double totalDeposit, double fixedDeposit) const;
    
    /**
     * @brief 计算当月开支和存款
     * @param totalDeposit 当前总存款金额
     * @param salary 当月工资
     * @param expense 当月开支（输出参数）
     * @param monthlyDeposit 当月存款（输出参数）
     * @param hasPreviousRecord 是否有上一次记录
     */
    void calculateAmounts(double totalDeposit, double salary, double &expense, double &monthlyDeposit, bool hasPreviousRecord = false);
    
    // 数据查询接口
    double getPreviousTotalDeposit() const;
    double getPreviousFixedDeposit() const;
    
    /**
     * @brief 获取上一次记录的日期
     * @return 返回上一次记录的日期
     */
    QDate getPreviousDate() const;
    int getRowCount() const;
    bool isFirstRecord() const;
    
    // UI初始化和配置接口
    void initTableView(QTableView *tableView) const;
    void configureUI(QDoubleSpinBox *monthlyDepositSpinBox, QDoubleSpinBox *disposableAmountSpinBox, QDoubleSpinBox *expenseSpinBox) const;
    
    // 错误提示接口
    void showError(const QString &title, const QString &message) const;
    void showSuccess(const QString &title, const QString &message) const;
    bool confirmOperation(const QString &title, const QString &message) const;

private:
    QStandardItemModel *model;
    QString currentFilePath;
    void initModel();
    void setupDarkThemeStyle(QTableView *tableView) const;
    void configureWidgetStyle(QWidget *widget, bool readOnly, const QString &readOnlyColor = "#3a3a3a", const QString &textColor = "#ffffff") const;
};

#endif // LEDGERMANAGER_H