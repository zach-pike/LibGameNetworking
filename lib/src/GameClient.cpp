#include "GameClient.hpp"

GameClient::GameClient() {}
GameClient::~GameClient() {}

bool GameClient::connectToServer(std::string host, std::uint16_t port) {
    try {
		// Resolve hostname/ip-address into tangiable physical address
		asio::ip::tcp::resolver resolver(context);
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

		// Create connection
		connection = std::make_unique<Connection>(context, asio::ip::tcp::socket(context), incomingMessages, Connection::Owner::Client);
					
		// Tell the connection object to connect to server
		connection->connectToServer(endpoints);

		// Start Context Thread
		contextThread = std::thread([this]() { context.run(); });
    } catch (std::exception& e) {
		std::cout << "Client Exception: " << e.what() << "\n";
		return false;
	}
	return true;
}

void GameClient::disconnect() {
	// If connection exists, and it's connected then...
	if(isConnected()) {
		// ...disconnect from server gracefully
		connection->disconnect();
	}

	// Either way, we're also done with the asio context...				
	context.stop();

	// ...and its thread
	if (contextThread.joinable())
		contextThread.join();

	// Destroy the connection object
	connection.release();
}

// Check if client is actually connected to a server
bool GameClient::isConnected() {
	if (connection)
		return connection->isConnected();
	else
		return false;
}

TSQueue<OwnedMessage>& GameClient::getIncomingMessageQueue() {
    return incomingMessages;
}

void GameClient::send(const Message& msg) {
	if (isConnected())
		connection->sendMessage(msg);
}