#include "irc.hpp"
#include "mainwindow.hpp"
#include <qmessagebox.h>

ircmanager manager;
int lasthash{};

int pseudohash(std::unordered_map<int, ChIRC::PeerData> &data)
{
    int hash = 0;
    for (auto &i : data)
    {
        hash += i.first;
    }
    return hash;
}

void ircmanager::update()
{
    while (on)
    {
        std::this_thread::sleep_for(std::chrono_literals::operator""ms(200));
        std::lock_guard<std::mutex> lock(guard);
        if (!on)
            continue;
        irc->Update();

        auto peers = irc->getPeers();
        int hash   = pseudohash(peers);
        if (lasthash == hash)
            continue;
        lasthash = hash;

        QStringListModel &listobj = w->getBotList();
        QStringList list;

        for (auto &i : peers)
        {
            list.append(QString::fromStdString(std::to_string(i.first) + " " +
                                               i.second.nickname));
        }
        listobj.setStringList(list);
    }
}
