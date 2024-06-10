#include "Message.hpp"

#include <string.h>

Message::Message() {}

Message::Message(std::uint32_t id) {
    header.id = id;
    header.size = 0;
}

Message::Message(std::uint32_t id, std::string str) {
    header.id = id;
    header.size = str.size();
    body.insert(body.end(), str.begin(), str.end());
}

Message::Message(std::uint32_t id, const std::uint8_t* data, std::size_t size) {
    header.id = id;
    header.size = size;
    body.resize(size);

    memcpy(body.data(), data, size);
}