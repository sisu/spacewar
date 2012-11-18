#pragma once

#include <vector>

namespace net {

struct Connection;

struct ServerSocket {
	ServerSocket();
	~ServerSocket();
	bool init(int port, bool block);
	int pollConnections();
	void readSockets();
	void send(const void* data, int n);
	void flush();

	int sockfd;
	std::vector<Connection*> conns;
};

} // namespace net
