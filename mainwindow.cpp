#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "irc.hpp"
extern std::unique_ptr<MainWindow> w;
void handleIRC(IRCMessage message, IRCClient *client);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->msgList->setModel(&list);
    manager.irc->installCallback("PRIVMSG", handleIRC);

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

    addToStringList(QString::fromStdString(manager.irc->getData().nick), text);
}

void handleIRC(IRCMessage message, IRCClient *client)
{
    std::string &cmd     = message.command;
    std::string &channel = message.parameters.at(0);
    std::string &rawmsg  = message.parameters.at(1);
    std::string &usr = message.prefix.nick;
    if (!ucccccp::validate(rawmsg))
        return;
    std::string msg(ucccccp::decrypt(rawmsg));
    if (msg == "Attempt at ucccccping and failing" ||
        msg == "Unsupported version")
        return;

    // Handle privmsg (Message to #channel)
    if (cmd == "PRIVMSG")
    {
        if (msg.empty() || usr.empty())
            return;
        // Handle public messages
        if (channel == manager.irc->getData().comms_channel)
        {
            if (msg.find("msg") == 0)
            {
                w->addToStringList(QString::fromStdString(usr), QString::fromStdString(msg.substr(3)));
                return;
            }
        }
     }
}

void MainWindow::addToStringList(QString user, QString msg)
{
    auto stringlist = list.stringList();
    stringlist.prepend(user + ": " + msg);
    list.setStringList(stringlist);
}

void MainWindow::on_lineEdit_returnPressed()
{
    MainWindow::on_sendMsg_clicked();
}
