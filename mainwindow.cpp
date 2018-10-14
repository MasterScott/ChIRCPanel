#include "mainwindow.hpp"
#include <ui_mainwindow.h>
#include "irc.hpp"
#include <qmessagebox.h>
#include <exception>
#include "ChIRC/ucccccp/ucccccp.hpp"

void handleIRC(IRCMessage message, IRCClient *client);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->msgList->setModel(&msglist);
    ui->botList->setModel(&botlist);
    ui->msgList->setSelectionBehavior(QAbstractItemView::SelectColumns);
    manager.irc->installCallback("PRIVMSG", handleIRC);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    auto stringlist = msglist.stringList();
    stringlist.prepend(user + ": " + msg);
    msglist.setStringList(stringlist);
}

QStringListModel &MainWindow::getBotList()
{
    return botlist;
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

void MainWindow::on_lineEdit_returnPressed()
{
    MainWindow::on_sendMsg_clicked();
}

void MainWindow::on_reconnectButton_clicked()
{
    std::string text = ui->addressField->text().toStdString();
    std::string address = text.substr(0, text.find_last_of(':'));
    int port;
    try {
        port = std::stoul(text.substr(text.find_last_of(':')+1));
    } catch (std::invalid_argument) {
        QMessageBox::warning(w.get(), "Error", "Port must be integer!");
        return;
    }
    std::string channel = ui->channelField->text().toStdString();
    std::string password = ui->passwordField->text().toStdString();
    std::string user = ui->usernameField->text().toStdString();

    manager.irc->Disconnect();
    manager.irc->UpdateData(user, user, "#cat_comms", channel, password, address, port);
    manager.irc->Connect();
}

void MainWindow::on_cmdLineEdit_returnPressed()
{
    if (ui->sendallBox->checkState())
        manager.irc->privmsg("cc_cmd$cmd" + ui->cmdLineEdit->text().toStdString(), true);
    else
    {
        QModelIndex index = ui->botList->currentIndex();
        int columnid = index.column();
        if (columnid == -1)
        {
            QMessageBox::warning(w.get(), "Error", "No bot selected.");
            return;
        }

            int i = 0;
            int id = -1;
            for (auto &j : manager.irc->getPeers())
            {
                if (i == columnid)
                    id = j.first;
            }
            if (id == -1)
            {
                QMessageBox::warning(w.get(), "Error", "Invalid selection. (This should never happen)");
                return;
            }
            manager.irc->privmsg("cc_cmd$id" + std::to_string(id) + "$cmd" + ui->cmdLineEdit->text().toStdString(), true);
    }
    ui->cmdLineEdit->clear();
}
