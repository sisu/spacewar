#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>
#include <string>

namespace net {

struct DataWriter;
struct Connection {
	Connection(): buf(new char[1<<20]), fd(-1), handShakeDone(0) {}
	~Connection();
#if 0
	Connection(Connection&& c):
		buf(c.buf), cur(c.cur), need(c.need), fd(c.fd),
		obuf(std::move(c.obuf))
	{
		c.buf=0;
		c.fd=0;
	}
#endif
	void close();
	bool open(const char* host, int port);
	bool read();
	void write(const void* s, int n);
	void write(DataWriter& w);
	void send(const void* s, int n) { write(s,n); }
	void send(DataWriter& w) { write(w); }
	void flush();

	char* buf;
	std::string data;
	std::vector<char> obuf;
	int fd;

	bool handShakeDone;

private:
	Connection(const Connection&);

	bool readSingle();
};

} // namespace net

#endif
