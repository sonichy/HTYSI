#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString getCPUModel(), cpu_model, cpu_core, gpu_model, gpu_vendor, gpu_width, gpu_clock, gpu_driver;

private slots:
    void itemClick(QListWidgetItem*);

};

#endif // MAINWINDOW_H
