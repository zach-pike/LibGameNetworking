#pragma once

// ASIO
#define ASIO_STANDALONE
#include <asio.hpp>

#include <thread>
#include <vector>
#include <memory>
#include <cstdint>

#include "../Connection/Connection.hpp"
#include "../Message/Message.hpp"
#include "../TSQueue/TSQueue.hpp"

class GameServer {
private:
    asio::io_context context;
    std::thread contextThread;
    asio::ip::tcp::acceptor acceptor;

    TSQueue<OwnedMessage> incomingMessages;

    std::vector<std::shared_ptr<Connection>> connections;

    void acceptConnection();

public:
    GameServer(std::uint16_t port);
    ~GameServer();

    void startServer();
    void stopServer();

protected:
    virtual bool onClientConnect(std::shared_ptr<Connection> client) { return false; }
	virtual void onClientDisconnect(std::shared_ptr<Connection> client) {}

	virtual void onMessage(std::shared_ptr<Connection> client, Message& msg) {}
};