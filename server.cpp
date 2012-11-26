#include "base64.hpp"
#include "net/ServerSocket.hpp"
#include "net/Connection.hpp"
#include "game.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>
#include <openssl/sha.h>
using namespace std;
net::ServerSocket server;

#if 0
string tmp = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
"IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
"dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
"dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
"ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";
#endif
string wsReply(string key) {
//	assert(encode(decode(key))==key);
//	cout<<decode(tmp)<<'\n';
//	string dec = decode(key);
	string magic = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned char buf[21] = {};
	SHA1((unsigned char*)magic.c_str(), magic.size(), buf);
	string hash((char*)buf, 20);
	return encode64(hash);
}

const char* wsResp = "HTTP/1.1 101 Switching Protocols\r\n"
"Upgrade: websocket\r\n"
"Connection: Upgrade\r\n"
"Sec-WebSocket-Accept: %s\r\n"
//"Sec-WebSocket-Protocol: chat\r\n"
"\r\n";

void startServer() {
//	cout<<wsReply("x3JJHMbDL1EzLkh9GBhXDw==")<<'\n';
	server.init(41629, 0);
}
struct WSHeader {
	unsigned int OP_CODE : 4;
	unsigned int RSV1 : 1;
	unsigned int RSV2 : 1;
	unsigned int RSV3 : 1;
	unsigned int FIN : 1;
	unsigned int PAYLOAD : 7;
	unsigned int MASK : 1;
};
void readWS(net::Connection& conn) {
	if (!conn.data.empty()) {
//		WSHeader* h = (WSHeader*)&conn.data[0];
//		cout<<"payload: "<<h->PAYLOAD<<" ; "<<h->OP_CODE<<'\n';
//		cout<<"mask "<<h->MASK<<'\n';
		char* mask = &conn.data[2];
		for(size_t i=6; i<conn.data.size(); ++i)
			conn.data[i] ^= mask[(i-6)%4];
#if 0
		cout<<"data: "<<conn.data.substr(6)<<'\n';
		for(size_t i=0; i<conn.data.size(); ++i) {
			cout<<(unsigned)(unsigned char)conn.data[i]<<' ';
		}
		cout<<'\n';
#endif
		conn.data.clear();
	}
}
void sendWS(net::Connection& conn, string msg) {
	WSHeader h;
	h.OP_CODE = 1;
	h.RSV1 = h.RSV2 = h.RSV3 = 0;
	h.FIN = 1;
	h.MASK = 0;
	string len;
	if (msg.size() < 126)
		h.PAYLOAD = msg.size();
	else {
		assert(msg.size() < 1<<16);
		h.PAYLOAD = 126;
		len += char(msg.size()>>8);
		len += char(msg.size()&255);
	}

	string m((char*)&h, 2);
	m += len;
	m += msg;
	conn.write(&m[0], m.size());
}

string planetDesc(Player pl);

void handshake(net::Connection& conn) {
	while(1) {
		size_t pos = conn.data.find_first_of('\n');
		if (pos==string::npos) break;
		string line = conn.data.substr(0, pos);
		conn.data = conn.data.substr(pos+1);

		istringstream iss(line);
		string msg;
		iss>>msg;
		if (msg=="Sec-WebSocket-Key:") {
			string key;
			iss>>key;
//				key = "x3JJHMbDL1EzLkh9GBhXDw==";
//			cout<<"key: "<<key<<'\n';
			char buf[1<<12];
			sprintf(buf, wsResp, wsReply(key).c_str());
//			cout<<"sending resp:\n" << buf<<'\n';
			conn.write(buf, strlen(buf));
			conn.handShakeDone = 1;

			sendWS(conn, planetDesc(P1));
		}
	}
}
void runServer() {
	int added = server.pollConnections();
	if (added) {
		cout<<"new observer connected"<<endl;
	}
	server.readSockets();
	for(size_t i=0; i<server.conns.size(); ++i) {
		net::Connection& conn = *server.conns[i];
		if (conn.handShakeDone) {
			readWS(conn);
		} else handshake(conn);
//		conn.read();
	}
}
void sendToObs(string msg) {
	for(size_t i=0; i<server.conns.size(); ++i) {
		net::Connection& conn = *server.conns[i];
		if (conn.handShakeDone)
			sendWS(conn, msg);
	}
}
