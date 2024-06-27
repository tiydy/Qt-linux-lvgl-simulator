/*
 * Filename: vtslcd.h
 * Description: lcd,触摸屏模拟器
 * Author: heqingde
 * Created Date: 2024-06-23
 * Version: 1.0
 * License: Specify your license here (e.g., MIT, GPL, etc.)
 */


#ifndef VTSLCD_H
#define VTSLCD_H

#include <QMainWindow>
#include <common.h>
#include <QTimer>
#include <QFile>

#include <stdio.h>  // 标准输入输出库
#include <stdlib.h> // 标准库
#include <string.h> // 字符串处理库
#include <unistd.h> // UNIX 标准库
#include <fcntl.h>  // 文件控制库
#include <linux/uinput.h> // uinput 头文件

namespace Ui {
class VTSLcd;
}

class VTSLcd : public QMainWindow
{
    Q_OBJECT

public:
    explicit VTSLcd(QWidget *parent = nullptr);
    ~VTSLcd();

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
private slots:
    void on_add_dev_action_triggered();
    void on_close_action_triggered();
    void init_lcd_show();
    void select_lcd_open();
    void update_show();
    void ts_init();
    void ts_destory();
private:
    Ui::VTSLcd *ui;
    QVector<LcdDevice> lcds;
    QVector<QAction*> lcdActions;
    QTimer mtimer;
    LcdDevice lcd;
    QFile file;//操作lcd设备
    uchar *mmp ;//保存映射空间

    struct uinput_user_dev uidev; // 定义 uinput 设备结构体
    struct input_event ev; // 定义输入事件结构体

    int tsfd;
};

#endif // VTSLCD_H
