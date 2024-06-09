#include "Server.hpp"

#include <iostream>

GameServer::GameServer(std::uint16_t port):
    acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) 
{}

GameServer::~GameServer() {}

void GameServer::acceptConnection() {
    acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (ec) {
            std::cout << "Connection Error " << ec << "\n";

            // Retrigger accept
            acceptConnection();
            return;
        }

        // No Error!

        std::shared_ptr<Connection> conn 
            = std::make_shared<Connection>(context, std::move(socket), incomingMessages, Connection::Owner::Server);


        if (onClientConnect(conn)) {
            connections.push_back(std::move(conn));

            connections.back()->connectToClient();

            std::cout << "Client Successfully connected!\n";
        }

        // If client was not allowed to connect the conn obj will go out of scope and close the socket
        


        acceptConnection();
    });
}

void GameServer::startServer() {
    acceptConnection(); // Start accept loop once io context starts

    contextThread = std::thread([&]() { context.run(); });
}

void GameServer::stopServer() {
    context.stop();

    if (contextThread.joinable())
        contextThread.join();
}

