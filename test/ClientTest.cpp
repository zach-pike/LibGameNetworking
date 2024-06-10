#include "GameClient.hpp"

#include "Collatz.hpp"

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

    // Repeat message right back to server
    if (queue.front().message.header.id == UINT32_MAX) {
        // Convert to string then resend
        Message msg = queue.pop_front().message;

        std::uint32_t value = *(std::uint32_t*)msg.body.data();

        // Calculate Collatz number
        std::uint32_t calcValue = collatzNumber(value);

        client.send(Message(UINT32_MAX, (std::uint8_t*)&calcValue, 4));
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