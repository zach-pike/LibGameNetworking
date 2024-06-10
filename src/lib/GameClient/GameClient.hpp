#pragma once

// ASIO
#define ASIO_STANDALONE
#include <asio.hpp>

#include <thread>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>

#include "../Connection/Connection.hpp"
#include "../TSQueue/TSQueue.hpp"
#include "../Message/Message.hpp"

class GameClient {
private:
    asio::io_context context;
    std::thread contextThread;

    TSQueue<OwnedMessage> incomingMessages;

    std::unique_ptr<Connection> connection;
public:
    GameClient();
    ~GameClient();

    bool connectToServer(std::string host, std::uint16_t port);

    void disconnect();
    bool isConnected();

    void send(const Message& msg);

    TSQueue<OwnedMessage>& getIncomingMessageQueue();
};