#include "GameClient/GameClient.hpp"

#include "uuid_v4.h"

#include <chrono>

int main() {
    using namespace std::chrono_literals;
    GameClient client;

    client.connectToServer("0.0.0.0", 9000);

    // Wait for connection
    while (!client.isConnected()) std::this_thread::sleep_for(100ms);

    auto& queue = client.getIncomingMessageQueue();

    // Wait for message
    while (queue.count() == 0) std::this_thread::sleep_for(100ms);

    // Client UUID
    UUIDv4::UUID uuid;

    // Repeat message right back to server
    if (queue.front().message.header.id == UINT32_MAX) {
        // Convert to string then resend
        Message msg = queue.pop_front().message;

        std::string strMsg((const char*)msg.body.data(), msg.body.size());

        uuid = UUIDv4::UUID(strMsg);

        client.send(Message(UINT32_MAX, strMsg));
    } else {
        exit(1);
    }

    // Wait for OK
    while (queue.count() == 0) std::this_thread::sleep_for(100ms);

    // Repeat message right back to server
    if (queue.front().message.header.id == UINT32_MAX) {
        // Convert to string then resend
        Message msg = queue.pop_front().message;

        std::string strMsg((const char*)msg.body.data(), msg.body.size());

        if (strMsg == "GAMER")
            std::cout << "Handshake OK\n";
    } else {
        exit(1);
    }

    client.send(Message(0, "Hello from client!"));

    while (queue.count() == 0) std::this_thread::sleep_for(100ms);

    // Get a message
    Message msg = queue.pop_front().message;

    std::string s((char*)msg.body.data(), msg.body.size());

    std::cout << s << '\n';

    client.disconnect();
}