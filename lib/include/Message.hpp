#pragma once

#include <cstdint>
#include <vector>
#include <memory>

class Connection; // I hate c++

class Message {
public:
    struct Header {
        std::uint32_t id;
        std::uint32_t size;
    };

    Header header;
    std::vector<std::uint8_t> body;

    Message();

    Message(std::uint32_t id, std::string str);
    Message(std::uint32_t id, const std::uint8_t* data, std::size_t size);
};

class OwnedMessage {
public:
    std::shared_ptr<Connection> connection = nullptr;
    Message                     message;
};