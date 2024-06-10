#include "Message.hpp"

#include <string.h>

Message::Message() {}

Message::Message(std::uint32_t id, std::string str) {
    header.id = id;
    header.size = str.size();
    body.insert(body.end(), str.begin(), str.end());
}