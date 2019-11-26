#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QFile>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);
    QStringList SL;
    SL << "总览" << "系统" << "处理器" << "主板" << "内存" << "硬盘" << "显卡" << "显示器" << "声卡" << "网卡" << "软件";
    ui->listWidget->addItems(SL);
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(currentRowChange(int)));
    refresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getCPUInfo()
{
    QFile file("/proc/cpuinfo");
    file.open(QIODevice::ReadOnly);
    QString content = file.readAll();
    file.close();
    QStringList lines = content.split("\n");
    QStringList modelline = lines.filter("model name");
    cpu_model = modelline.first().split(":").at(1);
    QStringList coreline = lines.filter("cpu cores");
    cpu_core = coreline.first().split(":").at(1);
    QStringList cacheline = lines.filter("cache size");
    cpu_cache = cacheline.first().split(":").at(1);
    QStringList bugsline = lines.filter("bugs");
    cpu_bugs = bugsline.first().split(":").at(1);
    QStringList flagsline = lines.filter("flags");
    cpu_flags = flagsline.first().split(":").at(1);

    file.setFileName("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies");
    file.open(QIODevice::ReadOnly);
    cpu_freqs = file.readAll();
    file.close();
}

void MainWindow::getGPUInfo()
{
    QProcess *process = new QProcess;
    process->start("lshw -C display");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QStringList SL = PO.split("\n");
    for(int i=0; i<SL.size(); i++){
        if (SL.at(i).contains("product:")) {
            gpu_model = SL.at(i);
            gpu_model.remove("product:");
        }
        if(SL.at(i).contains("vendor:")) {
            gpu_vendor = SL.at(i);
            gpu_vendor.remove("vendor:");
            gpu_vendor.remove("Corporation");
        }
        if(SL.at(i).contains("width:")) {
            gpu_width = SL.at(i);
            gpu_width.remove("width:");
        }
        if(SL.at(i).contains("clock:")) {
            gpu_clock = SL.at(i);
            gpu_clock.remove("clock:");
        }
        if(SL.at(i).contains("driver=")) {
            QStringList SL1 = SL.at(i).split(QRegExp("\\s{1,}")); // 第一个\表示转义字符，\s表示空格，｛1，｝表示一个以上
            //qDebug() << SL1;
            for(int j=0; j<SL1.size(); j++){
                if(SL1.at(j).contains("driver=")){
                    gpu_driver = SL1.at(j);
                    gpu_driver.remove("driver=");
                }
            }
        }
    }
}

void MainWindow::getSoundInfo()
{
    QProcess *process = new QProcess;
    process->start("lshw -C multimedia");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QStringList SL = PO.split("\n");
    for(int i=0; i<SL.size(); i++){
        if (SL.at(i).contains("product:")) {
            sound_model = SL.at(i);
            sound_model.remove("product:");
        }
        if(SL.at(i).contains("vendor:")) {
            sound_vendor = SL.at(i);
            sound_vendor.remove("vendor:");
        }
        if(SL.at(i).contains("width:")) {
            sound_width = SL.at(i);
            sound_width.remove("width:");
        }
        if(SL.at(i).contains("clock:")) {
            sound_clock = SL.at(i);
            sound_clock.remove("clock:");
        }
        if(SL.at(i).contains("driver=")) {
            QStringList SL1 = SL.at(i).split(QRegExp("\\s{1,}")); // 第一个\表示转义字符，\s表示空格，｛1，｝表示一个以上
            //qDebug() << SL1;
            for(int j=0; j<SL1.size(); j++){
                if(SL1.at(j).contains("driver=")){
                    sound_driver = SL1.at(j);
                    sound_driver.remove("driver=");
                }
            }
        }
    }
}

void MainWindow::getNetwork()
{
    QProcess *process = new QProcess;
    process->start("lshw -C network");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QStringList SL = PO.split("\n");
    for(int i=0; i<SL.size(); i++){
        if (SL.at(i).contains("product:")) {
            npu_model = SL.at(i);
            npu_model.remove("product:");
        }else if (SL.at(i).contains("vendor:")) {
            npu_vendor = SL.at(i);
            npu_vendor.remove("vendor:");
        }else if (SL.at(i).contains("capacity:")) {
            npu_capacity = SL.at(i);
            npu_capacity.remove("capacity:");
        }else if(SL.at(i).contains("width:")) {
            npu_width = SL.at(i);
            npu_width.remove("width:");
        }else if(SL.at(i).contains("clock:")) {
            npu_clock = SL.at(i);
            npu_clock.remove("clock:");
        }
    }
}

void MainWindow::getSoftware()
{
    QProcess *process = new QProcess;
    process->start("dpkg -l");
    process->waitForFinished();
    software = process->readAllStandardOutput();
}

void MainWindow::currentRowChange(int row)
{
    ui->label_brand->setPixmap(QPixmap(""));
    switch (row) {
    case 0:
        ui->textBrowser->setText("系统：" + QSysInfo::prettyProductName() + "\n处理器：" + cpu_model + "\n显卡：" + gpu_vendor + gpu_model + "\n声卡：" + sound_vendor + sound_model);
        break;
    case 1:
        ui->textBrowser->setText("主机名：" + QSysInfo::machineHostName() + "\n用户名：" + qgetenv("USER") + "\n内核：" + QSysInfo::kernelType() + " " + QSysInfo::kernelVersion() + "\n架构：" + QSysInfo::currentCpuArchitecture() + "\n版本名称：" + QSysInfo::prettyProductName() + "\n版本号：" + QSysInfo::productVersion());
        if(QSysInfo::prettyProductName().contains("Deepin")){
            ui->label_brand->setPixmap(QPixmap(":/icon/deepin.jpg").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        break;
    case 2:
        ui->textBrowser->setText("型号：" + cpu_model + "\n核心数：" + cpu_core + "\n缓存：" + cpu_cache + "\n架构：" + QSysInfo::currentCpuArchitecture() + "\n特征：" + cpu_flags + "\n缺陷：" + cpu_bugs + "\n工作频率(KHz)：" + cpu_freqs);
        if(cpu_model.contains("Celeron")){
            ui->label_brand->setPixmap(QPixmap(":/icon/celeron.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }else if(cpu_model.contains("Pentium")){
            ui->label_brand->setPixmap(QPixmap(":/icon/pentium.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }else if(cpu_model.contains("AMD")){
            ui->label_brand->setPixmap(QPixmap(":/icon/AMD.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        break;
    case 3:
        ui->textBrowser->setText("");
        break;
    case 4:
        ui->textBrowser->setText("");
        break;
    case 5:
        ui->textBrowser->setText("");
        break;
    case 6:
        ui->textBrowser->setText("型号：" + gpu_model + "\n厂商：" + gpu_vendor + "\n带宽：" + gpu_width + "\n频率：" + gpu_clock + "\n驱动：\t" + gpu_driver);
        if(gpu_vendor.contains("Intel")){
            ui->label_brand->setPixmap(QPixmap(":/icon/intel.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }else if(gpu_vendor.contains("Nvidia")){
            ui->label_brand->setPixmap(QPixmap(":/icon/NVIDIA.svg").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }else if(gpu_vendor.contains("Radeon") || gpu_model.contains("Radeon")){
            ui->label_brand->setPixmap(QPixmap(":/icon/radeon.jpg").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        break;
    case 7:
        ui->textBrowser->setText("");
        break;
    case 8:
        ui->textBrowser->setText("型号：" + sound_model + "\n厂商：" + sound_vendor + "\n带宽：" + sound_width + "\n频率：" + sound_clock + "\n驱动：\t" + sound_driver);
        break;
    case 9:
        ui->textBrowser->setText("型号：" + npu_model + "\n厂商：" + npu_vendor + "\n带宽：" + npu_width + "\n频率：" + npu_clock + "\n速率：" + npu_capacity);
        if(npu_vendor.contains("Realtek")){
            ui->label_brand->setPixmap(QPixmap(":/icon/realtek.gif").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        break;
    case 10:
        ui->textBrowser->setText(software);
        break;
    }
}

void MainWindow::refresh()
{
    ui->textBrowser->setText("正在获取硬件信息...");
    getCPUInfo();
    getGPUInfo();
    getSoundInfo();
    getNetwork();
    getSoftware();
    ui->listWidget->setCurrentRow(0);
}

void MainWindow::on_actionRefresh_triggered()
{
    refresh();
}
