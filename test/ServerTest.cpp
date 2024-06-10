
#include <chrono>
#include <iostream>
#include <map>

#include "GameServer.hpp"
#include "Collatz.hpp"

class MyTestServer : public GameServer {
private:
    std::vector<std::shared_ptr<Connection>> verifiedConnections;
    std::map<std::shared_ptr<Connection>, std::uint32_t> unverifiedConnections;

public:
    MyTestServer(std::uint16_t p):
        GameServer(p) {}

protected:
    bool onClientConnect(std::shared_ptr<Connection> client) override {
        srand(time(nullptr));

        std::uint32_t num = rand() % 100000;
        std::uint32_t collatz = collatzNumber(num);

        client->sendMessage(Message(UINT32_MAX, (std::uint8_t*)&num, 4));

        unverifiedConnections.insert({ client, collatz });

        return true;
    }

	void onClientDisconnect(std::shared_ptr<Connection> client) override {
        // If a client disconnects, check if they were on the list of validated or waiting to validate
        auto unverifiedIter = unverifiedConnections.begin();
        if ((unverifiedIter = unverifiedConnections.find(client)) != unverifiedConnections.end()) {
            // Erase it
            unverifiedConnections.erase(unverifiedIter);
        }

        auto verifiedIter = verifiedConnections.begin();
        if ((verifiedIter = std::find(verifiedConnections.begin(), verifiedConnections.end(), client)) != verifiedConnections.end()) {
            // Erase it
            verifiedConnections.erase(verifiedIter);
        }

    }

    void onMessage(std::shared_ptr<Connection> client, Message& msg) override {
        // This means this is a metadata packet
        if (msg.header.id == UINT32_MAX) {
            // check if connection is in list
            auto iter = unverifiedConnections.begin();

            if ((iter = unverifiedConnections.find(client)) != unverifiedConnections.end()) {
                auto collatzNumber = unverifiedConnections.at(client);

                std::uint32_t valueFromClient = *(std::uint32_t*)msg.body.data();

                if (collatzNumber == valueFromClient) {
                    // User validated!

                    // Remove from unvalidated list
                    unverifiedConnections.erase(iter);

                    // Add to verified connections
                    verifiedConnections.push_back(client);

                    client->sendMessage(Message(UINT32_MAX, "GAMER"));
                } else {
                    // Socket disconnect
                    unverifiedConnections.erase(iter);
                    client->disconnect();
                }
            }
        } else {
            // Normal user logic here
            std::string s((char*)msg.body.data(), msg.body.size());

            std::cout << s << '\n';

            client->sendMessage(Message(0, "Hello world from server!"));
        }
    }
};

int main() {
    using namespace std::chrono_literals;

    MyTestServer server(9000);
    server.startServer();

    std::thread pt([&]() {
        while(true) {
            std::cout << "Client Count: " << server.getConnectionList().size() << '\n';
            std::this_thread::sleep_for(1s);
        }
    });

    while (true) {
		server.update();
	}
}