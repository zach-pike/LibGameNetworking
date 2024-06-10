#include "GameServer/GameServer.hpp"

#include <chrono>
#include <iostream>
#include <map>

class MyTestServer : public GameServer {
private:
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
    std::map<UUIDv4::UUID, std::shared_ptr<Connection>> uuidToConnection;
    std::mutex uuidToConnectionLock;

    std::map<std::shared_ptr<Connection>, UUIDv4::UUID> waitingToValidate;
    std::mutex waitingToValidateLock;
public:
    MyTestServer(std::uint16_t p):
        GameServer(p) {}

protected:
    bool onClientConnect(std::shared_ptr<Connection> client) override {
        // Generate a uuid
        UUIDv4::UUID id = uuidGenerator.getUUID();

        std::string uuidStr;
        id.bytes(uuidStr);

        // Send it to the client
        client->sendMessage(Message(UINT32_MAX, uuidStr));

        // Add user to list of people waiting to be validated
        std::scoped_lock lock(waitingToValidateLock);
        waitingToValidate.insert({ client, id });

        return true;
    }

	void onClientDisconnect(std::shared_ptr<Connection> client) override {

    }

    void onMessage(std::shared_ptr<Connection> client, Message& msg) override {
        // This means this is a metadata packet
        if (msg.header.id == UINT32_MAX) {
            std::scoped_lock lock(waitingToValidateLock);

            // check if connection is in list
            auto iter = waitingToValidate.begin();

            if ((iter = waitingToValidate.find(client)) != waitingToValidate.end()) {
                auto uuid = waitingToValidate.at(client);

                // It is, check if packet is a valid ack
                std::string incomingMessageStr((const char*)msg.body.data(), msg.body.size());

                // Regenerate the uuid str
                std::string correctValue;
                uuid.bytes(correctValue);

                if (incomingMessageStr == correctValue) {
                    // User validated!
                    std::scoped_lock lock2(uuidToConnectionLock);
                    uuidToConnection.insert({ uuid, client });

                    // Remove from unvalidated list
                    waitingToValidate.erase(iter);

                    client->sendMessage(Message(UINT32_MAX, "GAMER"));
                } else {
                    // Socket disconnect
                    waitingToValidate.erase(iter);
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