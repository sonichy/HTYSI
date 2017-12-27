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
    QStringList SL;
    SL << "总览" << "系统" << "处理器" << "主板" << "内存" << "硬盘" << "显卡" << "显示器" << "其他";
    ui->listWidget->addItems(SL);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);

    // CPU
    QFile file("/proc/cpuinfo");
    file.open(QIODevice::ReadOnly);
    QString content = file.readAll();
    file.close();
    QStringList lines = content.split("\n");
    QStringList modelline = lines.filter("model name");
    cpu_model = modelline.first().split(":").at(1).split("@").first();
    QStringList coreline = lines.filter("cpu cores");
    cpu_core = coreline.first().split(":").at(1);

    // GPU
    QProcess *process = new QProcess;
    process->start("lshw -C display");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    SL = PO.split("\n");
    for(int i=0; i<SL.size(); i++){
        if (SL.at(i).contains("product:")) {
            gpu_model = SL.at(i);
            gpu_model.remove("product:");
        }
        if(SL.at(i).contains("vendor:")) {
            gpu_vendor = SL.at(i);
            gpu_vendor.remove("vendor:");
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
            qDebug() << SL1;
            for(int j=0; j<SL1.size(); j++){
                if(SL1.at(j).contains("driver=")){
                    gpu_driver = SL1.at(j);
                    gpu_driver.remove("driver=");
                }
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getCPUModel()
{

}

void MainWindow::itemClick(QListWidgetItem *item)
{
    Q_UNUSED(item);
    switch (ui->listWidget->currentRow()) {
    case 0:
        ui->label->setText("系统：" + QSysInfo::prettyProductName()+"\nCPU：" + cpu_model);
        break;
    case 1:
        ui->label->setText("用户名：" + qgetenv("USER") + "\n核心类型：" + QSysInfo::kernelType() + "\n核心版本：" + QSysInfo::kernelVersion() + "\n发行版：" + QSysInfo::prettyProductName() + "\n发行版本号：" + QSysInfo::productVersion());
        break;
    case 2:
        ui->label->setText("型号：" + cpu_model + "\n核心数：" + cpu_core + "\n架构：" + QSysInfo::currentCpuArchitecture());
        break;
    case 3:
        ui->label->setText("");
        break;
    case 4:
        ui->label->setText("");
        break;
    case 5:
        ui->label->setText("");
        break;
    case 6:
        ui->label->setText("型号：" + gpu_model + "\n供应商：" + gpu_vendor + "\n带宽：" + gpu_width + "\n频率：" + gpu_clock + "\n驱动：" + gpu_driver);
        break;
    case 7:
        ui->label->setText("");
        break;
    case 8:
        ui->label->setText("");
        break;
    default:
        break;
    }
}