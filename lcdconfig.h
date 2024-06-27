#ifndef LCDCONFIG_H
#define LCDCONFIG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlTableModel>
#include <common.h>

namespace Ui {
class LcdConfig;
}

class LcdConfig : public QDialog
{
    Q_OBJECT

public:
    explicit LcdConfig(QWidget *parent = nullptr);
    ~LcdConfig();

private slots:
    void on_addBt_clicked();

    void on_selectBt_clicked();

    void on_updateBt_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_quitBt_clicked();

    void on_deleteBt_clicked();

private:
    Ui::LcdConfig *ui;
    QSqlTableModel *model;
};

#endif // LCDCONFIG_H
