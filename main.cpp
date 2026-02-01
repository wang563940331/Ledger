/*
 * @Author: yu.wang
 * @Date: 2026-02-01 15:05:31
 * @LastEditors: yu.wang
 * @LastEditTime: 2026-02-01 21:51:41
 * @Description: 
 */
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置全局样式表
    QString styleSheet = R"(
        /* 主窗口样式 */
        QMainWindow {
            background-color: #1a1a1a;
            color: #ffffff;
            font-family: "Microsoft YaHei", "Segoe UI", Arial, sans-serif;
        }
        
        /* 标签页样式 */
        QTabWidget {
            background-color: #2a2a2a;
            color: #ffffff;
            border: none;
        }
        
        QTabBar {
            background-color: #2a2a2a;
        }
        
        QTabBar::tab {
            background-color: #3a3a3a;
            color: #cccccc;
            padding: 12px 24px;
            margin-right: 2px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
        }
        
        QTabBar::tab:selected {
            background-color: #4a4a4a;
            color: #ffffff;
            font-weight: bold;
        }
        
        QTabBar::tab:hover {
            background-color: #404040;
        }
        
        /* 表格样式 */
        QTableView {
            background-color: #2a2a2a;
            color: #ffffff;
            border: 1px solid #3a3a3a;
            gridline-color: #3a3a3a;
        }
        
        QTableView::header {
            background-color: #3a3a3a;
            color: #ffffff;
            font-weight: bold;
            border: none;
        }
        
        QTableView::item {
            background-color: #2a2a2a;
            color: #ffffff;
            border-bottom: 1px solid #3a3a3a;
        }
        
        QTableView::item:selected {
            background-color: #0078d7;
            color: #ffffff;
        }
        
        QTableView::item:hover {
            background-color: #3a3a3a;
        }
        
        /* 分组框样式 */
        QGroupBox {
            background-color: #2a2a2a;
            color: #ffffff;
            border: 1px solid #3a3a3a;
            border-radius: 8px;
            margin-top: 10px;
        }
        
        QGroupBox::title {
            color: #ffffff;
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        
        /* 输入控件样式 */
        QLineEdit, QDoubleSpinBox, QDateEdit {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #4a4a4a;
            border-radius: 6px;
            padding: 8px;
        }
        
        QLineEdit:focus, QDoubleSpinBox:focus, QDateEdit:focus {
            border: 1px solid #0078d7;
            background-color: #404040;
        }
        
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            background-color: #4a4a4a;
            border: none;
        }
        
        /* 按钮样式 */
        QPushButton {
            background-color: #0078d7;
            color: #ffffff;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #106ebe;
        }
        
        QPushButton:pressed {
            background-color: #005a9e;
        }
        
        /* 只读控件样式 */
        QDoubleSpinBox:read-only {
            background-color: #3a3a3a;
            color: #888888;
        }
        
        /* 标签样式 */
        QLabel {
            color: #ffffff;
        }
        
        /* 状态栏样式 */
        QStatusBar {
            background-color: #2a2a2a;
            color: #cccccc;
        }
            
        /* 消息框样式 */
        QMessageBox {
            background-color: #2a2a2a;
            color: #ffffff;
        }
        
        QMessageBox QLabel {
            color: #ffffff;
        }
        
        QMessageBox QPushButton {
            background-color: #0078d7;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 5px 15px;
        }
        
        QMessageBox QPushButton:hover {
            background-color: #106ebe;
        }
        
        QMessageBox QPushButton:pressed {
            background-color: #005a9e;
        }
        
        /* 对话框样式 */
        QDialog {
            background-color: #2a2a2a;
            color: #ffffff;
        }
        
        QDialog QLabel {
            color: #ffffff;
        }
        
        QDialog QPushButton {
            background-color: #0078d7;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 5px 15px;
        }
        
        QDialog QPushButton:hover {
            background-color: #106ebe;
        }
    )";
    a.setStyleSheet(styleSheet);
    MainWindow w;
    w.show();
    return a.exec();
}
