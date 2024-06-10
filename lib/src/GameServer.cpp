#include "GameServer.hpp"

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

std::vector<std::shared_ptr<Connection>>& GameServer::getConnectionList() {
	return connections;
}

void GameServer::messageClient(std::shared_ptr<Connection> client, const Message& msg) {
	// Check client is legitimate...
	if (client && client->isConnected()) {
		client->sendMessage(msg);
	} else {
		// If we cant communicate with client then we may as 
		// well remove the client - let the server know, it may
		// be tracking it somehow
		onClientDisconnect(client);

		// Off you go now, bye bye!
		client.reset();

		std::scoped_lock lock(connectionsMutex);

		// Then physically remove it from the container
		connections.erase(
			std::remove(connections.begin(), connections.end(), client), connections.end());
	}
}

void GameServer::messageAllClients(const Message& msg, std::shared_ptr<Connection> pIgnoreClient) {
	bool bInvalidClientExists = false;

	// Iterate through all clients in container
	for (auto& client : connections) {
		// Check client is connected...
		if (client && client->isConnected()) {
			// ..it is!
			if(client != pIgnoreClient)
				client->sendMessage(msg);
		} else {
		    // The client couldnt be contacted, so assume it has
			// disconnected.
			onClientDisconnect(client);
			client.reset();

			// Set this flag to then remove dead clients from container
			bInvalidClientExists = true;
		}
	}

	// Remove dead clients, all in one go - this way, we dont invalidate the
	// container as we iterated through it.
	if (bInvalidClientExists) {
		std::scoped_lock lock(connectionsMutex);

		connections.erase(
			std::remove(connections.begin(), connections.end(), nullptr), connections.end());

	}
}

void GameServer::update() {
	// Process as many messages as you can up to the value
	// specified
	while (!incomingMessages.empty()) {
		// Grab the front message
		auto msg = incomingMessages.pop_front();

		// Pass to message handler
		onMessage(msg.connection, msg.message);
	}

	// Loop over the connections and erase the non open ones
	for(int i=0; i<connections.size(); i++) {
		if (!connections[i]->isConnected()) {

			onClientDisconnect(connections[i]);

			connections.erase(connections.begin() + i);

			break;
		}
	}
}