/*
 * Filename: vtslcd.c
 * Description: lcd,触摸屏模拟器
 * Author: heqingde
 * Created Date: 2024-06-23
 * Version: 1.0
 * License: Specify your license here (e.g., MIT, GPL, etc.)
 */


#include "vtslcd.h"
#include "ui_vtslcd.h"
#include <lcdconfig.h>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMouseEvent>
#include <QMenuBar>




VTSLcd::VTSLcd(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VTSLcd)
{
    ui->setupUi(this);
    //设置设备名称
    file.setFileName("/dev/mmap_device");
    if(!file.open(QIODevice::ReadWrite))
    {
        qDebug()<<"open dev error";
    }
    //ts_init();
    init_lcd_show();
    //启动定时器并且关联槽函数
    connect(&mtimer, &QTimer::timeout, this, &VTSLcd::update_show);
    mtimer.start(100);
    tsfd = -1;
}

VTSLcd::~VTSLcd()
{
    //释放映射
    file.unmap(mmp);
    file.close();
    ts_destory();
    delete ui;
}


void VTSLcd::ts_init()
{

    tsfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); // 以只写和非阻塞模式打开 uinput 设备文件
    if (tsfd < 0) // 如果打开失败
        qDebug("error: open"); // 打印错误信息并退出


    // 启用触摸事件
    if (ioctl(tsfd, UI_SET_EVBIT, EV_ABS) < 0) // 设置设备支持绝对位置事件
        qDebug("error: ioctl0");
    if (ioctl(tsfd, UI_SET_EVBIT, EV_SYN) < 0) // 设置设备支持绝对位置事件
        qDebug("error: ioctl1");
    if (ioctl(tsfd, UI_SET_ABSBIT, ABS_MT_POSITION_X) < 0) // 设置设备支持 ABS_MT_POSITION_X 事件
        qDebug("error: ioctl2");
    if (ioctl(tsfd, UI_SET_ABSBIT, ABS_MT_POSITION_Y) < 0) // 设置设备支持 ABS_MT_POSITION_Y 事件
        qDebug("error: ioctl3");
    if (ioctl(tsfd, UI_SET_ABSBIT, ABS_MT_PRESSURE) < 0) // 设置设备支持 ABS_MT_PRESSURE 事件
        qDebug("error: ioctl4");
    if (ioctl(tsfd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID) < 0)
        qDebug("error: ioctl5");

    memset(&uidev, 0, sizeof(uidev)); // 将 uidev 结构体清零
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Example Touchscreen"); // 设置设备名称
    uidev.id.bustype = BUS_USB; // 设置设备总线类型为 USB
    uidev.id.vendor = 0x1234; // 设置设备供应商 ID
    uidev.id.product = 0x5678; // 设置设备产品 ID
    uidev.id.version = 1; // 设置设备版本
    // 设置设备的绝对位置参数
    uidev.absmin[ABS_MT_POSITION_X] = 0; // X 轴最小值
    uidev.absmax[ABS_MT_POSITION_X] = this->lcd.resx; // X 轴最大值
    uidev.absmin[ABS_MT_POSITION_Y] = 0; // Y 轴最小值
    uidev.absmax[ABS_MT_POSITION_Y] = this->lcd.resy; // Y 轴最大值
    uidev.absmin[ABS_MT_PRESSURE] = 0; // 压力最小值
    uidev.absmax[ABS_MT_PRESSURE] = 255; // 压力最大值
    uidev.absmin[ABS_MT_TRACKING_ID] = -1;
    uidev.absmax[ABS_MT_TRACKING_ID] = 65535;

    if (write(tsfd, &uidev, sizeof(uidev)) < 0) // 将设备信息写入 uinput 设备文件
        qDebug("error: write5");

    if (ioctl(tsfd, UI_DEV_CREATE) < 0) // 创建 uinput 设备
        qDebug("error: ioctl6");
}

void VTSLcd::ts_destory()
{
    if (ioctl(tsfd, UI_DEV_DESTROY) < 0) // 销毁 uinput 设备
            qDebug("error: ioctl7");

     ::close(tsfd); // 关闭文件描述符

    tsfd = -1;
}


void VTSLcd::mousePressEvent(QMouseEvent *event)
{
    QPoint lpoint = ui->label->mapFromParent(event->pos());


    memset(&ev, 0, sizeof(ev));
    gettimeofday(&ev.time, NULL);

    ev.type = EV_ABS;
    ev.code = ABS_MT_TRACKING_ID;
    ev.value = 0; // 跟踪 ID
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send tracking ID event");
        return ;
    }

    ev.type = EV_ABS;
    ev.code = ABS_MT_POSITION_X;
    ev.value = lpoint.x(); // X 坐标
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send X position event");
        return ;
    }

    ev.type = EV_ABS;
    ev.code = ABS_MT_POSITION_Y;
    ev.value =lpoint.y()-this->menuBar()->height(); // Y 坐标
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send Y position event");
        return ;
    }

    ev.type = EV_ABS;
    ev.code = ABS_MT_PRESSURE;
    ev.value = 100; // 压力值
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send pressure event");
        return ;
    }

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send sync event");
        return ;
    }
}

void VTSLcd::mouseReleaseEvent(QMouseEvent *event)
{

    // 发送触摸点释放事件
    ev.type = EV_ABS;
    ev.code = ABS_MT_TRACKING_ID;
    ev.value = -1; // 释放跟踪 ID
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send tracking ID release event");
        return ;
    }

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send sync event");
        return ;
    }

}

void VTSLcd::mouseMoveEvent(QMouseEvent *event)
{

    QPoint lpoint = ui->label->mapFromParent(event->pos());
    ev.type = EV_ABS;
    ev.code = ABS_MT_POSITION_X;
    ev.value = lpoint.x(); // X 坐标
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send X position event");
        return ;
    }

    ev.type = EV_ABS;
    ev.code = ABS_MT_POSITION_Y;
    ev.value =lpoint.y()-this->menuBar()->height(); // Y 坐标
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send Y position event");
        return ;
    }

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(tsfd, &ev, sizeof(ev)) < 0) {
        qDebug("Failed to send sync event");
        return ;
    }
}

void VTSLcd::init_lcd_show()
{
    //清理
    lcds.clear();
    lcdActions.clear();
    ui->open_menu->clear();
    //查询数据库
    QSqlTableModel model;
    model.setTable("device");
    model.select();
    int rows = model.rowCount();
    for(int i=0; i<rows; i++)
    {
        QSqlRecord record = model.record(i);
        //创建一个LcdDevice
        LcdDevice lcd;
        lcd.devname = record.value("devname").toString();
        lcd.resx = record.value("resx").toInt();
        lcd.resy = record.value("resy").toInt();
        lcd.perlbits = record.value("perlbits").toInt();
        lcds.append(lcd);
        //把lcd信息打包创建一个QAction对添加到menu中
        QString name = QString("%1:%2X%3位宽:%4位").arg(lcd.devname).arg(lcd.resx).arg(lcd.resy).arg(lcd.perlbits);
        QAction *action = ui->open_menu->addAction(name);
        lcdActions.append(action);//把创建好的action添加到容器中

        //把所有的action关联同一个槽函数
        connect(action, &QAction::triggered, this, &VTSLcd::select_lcd_open);
    }
}

void VTSLcd::select_lcd_open()
{
    file.unmap(mmp);
    if(tsfd > 0){
        ts_destory();
    }
    ts_init();
    //获取信号发送者
    QAction *action = (QAction*)sender();
    //查找当前action对应的设备信息
    int row = lcdActions.indexOf(action);
    //在通过行找到设备信息
    this->lcd = lcds.at(row);
    QString name = QString("%1:%2X%3位宽:%4位").arg(lcd.devname).arg(lcd.resx).arg(lcd.resy).arg(lcd.perlbits);
    ui->statusBar->showMessage(name);
    //ui->centralWidget->resize(lcd.resx, lcd.resy);
    ui->label->resize(lcd.resx, lcd.resy+ui->statusBar->height()+ui->menuBar->height());
    //映射
    mmp = file.map(0, lcd.resx*lcd.resy*lcd.perlbits/8);
}

//更新显示
void VTSLcd::update_show()
{
    //lcd里面的像素----[32---RGBA]
    QImage image(mmp,lcd.resx, lcd.resy, QImage::Format_RGB32);
    //QImage image(lcd.resx, lcd.resy,QImage::Format_RGB888);
    //image.fill(QColor(qrand()%255, qrand()%255, qrand()%255));
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void VTSLcd::on_add_dev_action_triggered()
{
    //创建添加对话框
    LcdConfig config;
    config.exec();
    init_lcd_show();
}

//关闭模拟器
void VTSLcd::on_close_action_triggered()
{
    this->close();
}
