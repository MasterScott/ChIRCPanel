#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStringListModel>
#include <memory>

namespace Ui
{
class MainWindow;
}

struct IRCData
{
    std::string address;
    int port{};
    std::string cc_channel;
    std::string cc_password;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QStringListModel msglist;
    QStringListModel botlist;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addToStringList(QString user, QString msg);
    QStringListModel &getBotList();

private slots:
    void on_sendMsg_clicked();

    void on_lineEdit_returnPressed();

    void on_reconnectButton_clicked();

    void on_cmdLineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
};

extern std::unique_ptr<MainWindow> w;

#endif // MAINWINDOW_HPP
