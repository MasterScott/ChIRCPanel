#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "irc.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->msgList->setModel(&list);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sendMsg_clicked()
{
    QString text = ui->lineEdit->text();
    if (text.isEmpty())
        return;
    ui->lineEdit->clear();
    manager.sendmsg(text.toStdString());

    auto stringlist = list.stringList();
    stringlist.prepend(text);
    list.setStringList(stringlist);
}

void MainWindow::on_lineEdit_returnPressed()
{
    MainWindow::on_sendMsg_clicked();
}
