#pragma once

#include <cstdint>
#include <vector>
#include <memory>
// #include "../Connection/Connection.hpp"

class Connection; // I hate c++

class Message {
public:
    struct Header {
        std::uint32_t id;
        std::uint32_t size;
    };

    Header header;
    std::vector<std::uint8_t> body;
};

class OwnedMessage {
public:
    std::shared_ptr<Connection> connection = nullptr;
    Message                     message;
};