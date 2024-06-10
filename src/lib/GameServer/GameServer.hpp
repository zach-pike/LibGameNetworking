#pragma once

// ASIO
#define ASIO_STANDALONE
#include <asio.hpp>

#include "uuid_v4.h"

#include <thread>
#include <vector>
#include <memory>
#include <cstdint>
#include <iostream>
#include <random>

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
    std::mutex connectionsMutex;


    void acceptConnection();

public:
    GameServer(std::uint16_t port);
    ~GameServer();

    void startServer();
    void stopServer();

    std::vector<std::shared_ptr<Connection>>& getConnectionList();

    void messageClient(std::shared_ptr<Connection> client, const Message& msg);
			
	// Send message to all clients
	void messageAllClients(const Message& msg, std::shared_ptr<Connection> pIgnoreClient = nullptr);

	// Force server to respond to incoming messages
	void update();

protected:
    virtual bool onClientConnect(std::shared_ptr<Connection> client) { return false; }

	virtual void onClientDisconnect(std::shared_ptr<Connection> client) {}

	virtual void onMessage(std::shared_ptr<Connection> client, Message& msg) {}
};