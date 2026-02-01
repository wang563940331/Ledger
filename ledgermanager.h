#ifndef LEDGERMANAGER_H
#define LEDGERMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QDate>

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