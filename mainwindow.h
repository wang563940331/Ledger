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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_saveButton_clicked();
    void calculateAmounts();

private:
    Ui::MainWindow *ui;
    LedgerManager *ledgerManager;
    QString excelFilePath;
    void initLedger();
};
#endif // MAINWINDOW_H