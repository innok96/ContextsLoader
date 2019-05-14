#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadButton_clicked();

    void on_saveButton_clicked();

    void on_saveContentButton_clicked();

private:
    Ui::MainWindow *ui;
    DataLoader m_dataLoader;
    std::vector<QString> m_aKeywords;
};

#endif // MAINWINDOW_H
