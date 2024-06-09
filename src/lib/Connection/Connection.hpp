#pragma once

// ASIO
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>

#include "../Message/Message.hpp"
#include "../TSQueue/TSQueue.hpp"

class Connection : public std::enable_shared_from_this<Connection> {
public:
    enum class Owner {
        Server,
        Client
    };
private:
    asio::ip::tcp::socket  socket;
    asio::io_context&      context;


    TSQueue<Message> outgoingQueue;
    TSQueue<OwnedMessage>& incomingQueue;

    Owner owner;

    // Storage for current incoming message
    Message tempIncomingMessage;

    void readHeader();
    void readBody();

    void writeHeader();
    void writeBody();

    void addMessageToQueue();

public:
    Connection(asio::io_context& context, asio::ip::tcp::socket socket, TSQueue<OwnedMessage>& incomingQueue, Owner owner);
    ~Connection();

    void connectToClient();
    void connectToServer();

    void sendMessage(Message message);
};