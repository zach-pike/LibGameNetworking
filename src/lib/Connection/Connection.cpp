#include "Connection.hpp"

#include <iostream>

Connection::Connection(asio::io_context& ctx, asio::ip::tcp::socket skt, TSQueue<OwnedMessage>& inq, Owner own):
    context(ctx),
    socket(std::move(skt)),
    incomingQueue(inq),
    owner(own) {}

Connection::~Connection() {}

void Connection::readHeader() {
    asio::async_read(socket, asio::buffer(&tempIncomingMessage.header, sizeof(Message::Header)), 
    [this](std::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "Socket Read Error!\n";

            socket.close();
            return;
        }

        // No Error

        if (tempIncomingMessage.header.size > 0) {
            tempIncomingMessage.body.resize(tempIncomingMessage.header.size);

            readBody();
        } else {
            // No Data!

            addMessageToQueue();
        }
    });
}

void Connection::readBody() {
    asio::async_read(socket, asio::buffer(tempIncomingMessage.body.data(), tempIncomingMessage.header.size), 
    [this](std::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "Socket Read Error!\n";

            socket.close();
            return;
        }

        // No Error
        // Done reading message
        addMessageToQueue();
    });
}

void Connection::writeHeader() {
    asio::async_write(socket, asio::buffer(&outgoingQueue.front().header, sizeof(Message::Header)),
    [this](std::error_code ec, std::size_t length) {
        if (ec) {

            std::cout << "Error writing to socket!\n";

            return;
        }

        // No Error

        if (outgoingQueue.front().body.size() > 0) {
            writeBody();
        } else {
            outgoingQueue.pop_front();
            
			// If the queue is not empty, there are more messages to send, so
			// make this happen by issuing the task to send the next header.
			if (!outgoingQueue.empty()) {
				writeHeader();
			}
        }

    });
}

void Connection::writeBody() {
    asio::async_write(socket, asio::buffer(outgoingQueue.front().body.data(), outgoingQueue.front().body.size()),
    [this](std::error_code ec, std::size_t length) {
        if (ec) {

            std::cout << "Socket Write Error!\n";
            return;
        }

        // No error

        // Pop the message off the front
        outgoingQueue.pop_front();

        // If the queue still has messages in it, then issue the task to 
		// send the next messages' header.
		if (!outgoingQueue.empty()) {
			writeHeader();
		}

    });
}

void Connection::addMessageToQueue() {
    if(owner == Owner::Server)
		incomingQueue.push_back({ this->shared_from_this(), tempIncomingMessage });
	else
		incomingQueue.push_back({ nullptr, tempIncomingMessage });

	readHeader();
}

void Connection::connectToClient() {
    if (socket.is_open() && owner == Owner::Server) {
        readHeader(); // Start le loop
    }
}

void Connection::connectToServer() {

}

void Connection::sendMessage(Message message) {
    asio::post(context,
	[this, message]()
	{
		// If the queue has a message in it, then we must 
		// assume that it is in the process of asynchronously being written.
		// Either way add the message to the queue to be output. If no messages
		// were available to be written, then start the process of writing the
		// message at the front of the queue.
		bool bWritingMessage = !outgoingQueue.empty();
		outgoingQueue.push_back(message);
		if (!bWritingMessage)
		{
			writeHeader();
		}
	});
}