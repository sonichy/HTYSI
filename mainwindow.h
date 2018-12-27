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
    QString cpu_model, cpu_core, cpu_cache, cpu_bugs, cpu_flags, gpu_model, gpu_vendor, gpu_width, gpu_clock, gpu_driver, sound_model, sound_vendor, sound_width, sound_clock, sound_driver, npu_model, npu_vendor, npu_capacity, npu_width, npu_clock, software;
    void getCPUInfo();
    void getGPUInfo();
    void getSoundInfo();
    void getNetwork();
    void getSoftware();
    void refresh();

private slots:
    void currentRowChange(int row);
    void on_actionRefresh_triggered();

};

#endif // MAINWINDOW_H
