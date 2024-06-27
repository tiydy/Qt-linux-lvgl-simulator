#include "lcdconfig.h"
#include "ui_lcdconfig.h"

#include <QSqlRecord>

LcdConfig::LcdConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LcdConfig)
{
    ui->setupUi(this);
    model = new QSqlTableModel();//一定要在数据库db打开后才有效
    model->setTable("device");
    model->select();
    //设置水平表格头
    model->setHeaderData(1,Qt::Orientation::Horizontal, "设备名称");
    model->setHeaderData(2,Qt::Orientation::Horizontal, "显示器宽");
    model->setHeaderData(3,Qt::Orientation::Horizontal, "显示器高");
    model->setHeaderData(4,Qt::Orientation::Horizontal, "显示位深");


    //设置模型事务提交方式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置手动提交

    //把模型添加到视图上显示
    ui->tableView->setModel(model);
    //隐藏对应的列显示--view
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(5);

    ui->tableView->setWordWrap(true);
}

LcdConfig::~LcdConfig()
{
    delete ui;
    delete model;
}

//插入数据
void LcdConfig::on_addBt_clicked()
{
    LcdDevice lcd;
    lcd.devname = ui->nameEdit->text();
    lcd.resx = ui->resxEdit->text().toInt();
    lcd.resy = ui->resyEdit->text().toInt();
    lcd.perlbits = ui->perlbits_cbb->currentText().toInt();
    if(lcd.devname.isEmpty() || lcd.resx==0 || lcd.resy==0 || lcd.perlbits==0) return;

    //从model里面获取一个记录
    QSqlRecord record = model->record();
    //填充数据
    record.setValue("devname", lcd.devname);
    record.setValue("resx", lcd.resx);
    record.setValue("resy", lcd.resy);
    record.setValue("perlbits", lcd.perlbits);
    record.setValue("type", 0);
    //把数据记录项添加到model中
    bool flg = model->insertRecord(0,record);
    qDebug()<<flg;

    model->submitAll();//
    model->select();//查询并且显示更新--从把模型数据重置
}

void LcdConfig::on_selectBt_clicked()
{
    model->select();
}

void LcdConfig::on_updateBt_clicked()
{
    int row = ui->tableView->currentIndex().row();//获取当前所选的行
    if(row < 0)return ;

    LcdDevice lcd;
    lcd.devname = ui->nameEdit->text();
    lcd.resx = ui->resxEdit->text().toInt();
    lcd.resy = ui->resyEdit->text().toInt();
    lcd.perlbits = ui->perlbits_cbb->currentText().toInt();
    if(lcd.devname.isEmpty() || lcd.resx==0 || lcd.resy==0 || lcd.perlbits==0) return;
    //提取这一行记录
    QSqlRecord record = model->record(row);
    //修改记录里面的数据
    record.setValue("devname", lcd.devname);
    record.setValue("resx", lcd.resx);
    record.setValue("resy", lcd.resy);
    record.setValue("perlbits", lcd.perlbits);
    //把数据放回模型中--QMessageBox提示
    model->setRecord(row, record);
    model->submitAll();
}

void LcdConfig::on_tableView_clicked(const QModelIndex &index)
{
    qDebug()<<"index"<<index.row();
    QSqlRecord record = model->record(index.row());//获取当前点击的行的数据
    ui->nameEdit->setText(record.value("devname").toString());
    ui->resxEdit->setText(record.value("resx").toString());
    ui->resyEdit->setText(record.value("resy").toString());
    ui->perlbits_cbb->setCurrentText(record.value("perlbits").toString());
}

void LcdConfig::on_quitBt_clicked()
{
    this->accept();
}

void LcdConfig::on_deleteBt_clicked()
{
    int row = ui->tableView->currentIndex().row();//获取当前所选的行
    if(row < 0)return ;
    model->removeRow(row);
    model->submitAll();
}
