#pragma once

#include "ChIRC.hpp"
#include <mutex>
#include <thread>

class ircmanager
{
    std::atomic_bool on{true};
    std::thread ircthread;
    std::mutex guard;
public:
    std::unique_ptr<ChIRC::ChIRC> irc;
    void update();
    ircmanager()
    {
        irc = std::make_unique<ChIRC::ChIRC>();

        irc->UpdateData("ChIRC-Pannel", "ChIRC-Pannel", "#cat_comms", "", "", "cathook.irc.inkcat.net", 8080);
        irc->Connect();
        ircthread = std::thread(&ircmanager::update, this);
    }
    ~ircmanager()
    {
        std::lock_guard<std::mutex> lock(guard);
        on = false;
        irc.reset();
        ircthread.join();
    }
    bool sendmsg(std::string msg)
    {
        std::string raw = "msg" + msg;
        if (irc->privmsg(raw))
            return true;
        return false;
    }
};

extern ircmanager manager;
