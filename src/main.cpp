#include <bits/stdc++.h>

#include "lib/Server/Server.hpp"

class myserver : public GameServer {
public:
    myserver(std::uint16_t p): GameServer(p) {}
protected:
    virtual bool onClientConnect(std::shared_ptr<Connection> client) {
        return true;
    }

	virtual void onClientDisconnect(std::shared_ptr<Connection> client) {

    }
	virtual void onMessage(std::shared_ptr<Connection> client, Message& msg) {

    }
};

int main() {
    myserver server(8000);

    server.startServer();

    using namespace std::chrono_literals;

    std::this_thread::sleep_for(20s);

    server.stopServer();
}

// #define ASIO_STANDALONE
// #include <asio.hpp>
// #include <asio/ts/buffer.hpp>
// #include <asio/ts/internet.hpp>

// std::vector<char> buffer(20*1024);

// void getSomeData(asio::ip::tcp::socket& socket) {
//     socket.async_read_some(asio::buffer(buffer.data(), buffer.size()), [&](std::error_code ec, std::size_t length) {
//         if (!ec) {
//             std::cout << "Read" << length << " bytes\n";

//             for (auto b : buffer) {
//                 std::cout << b;
//             }

//             getSomeData(socket);
//         }
//     });
// }

// int main() {
//     asio::error_code ec;

//     asio::io_context context;
//     asio::io_context::work idleWork(context);
//     std::thread thrContext = std::thread([&]() { context.run(); });

//     asio::ip::tcp::endpoint endpoint(asio::ip::make_address("172.253.62.139", ec), 80);
//     asio::ip::tcp::socket socket(context);
//     socket.connect(endpoint, ec);

//     if (!ec) {
//         std::cout << "Connected!" << std::endl;
//     } else {
//         std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
//     }

//     if (socket.is_open()) {
//         getSomeData(socket);

//         std::string req = "GET / HTTP/1.1\r\n"
//                           "Host: google.com\r\n"
//                           "Connection: close\r\n\r\n";
//         socket.write_some(asio::buffer(req.data(), req.size()), ec);
//     }

//     using namespace std::chrono_literals;

//     std::this_thread::sleep_for(20s);


//     return 0;
// }