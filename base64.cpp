#include <string>
#include <iostream>
using namespace std;

namespace {
const char encTable[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char decTable[256] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
		,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
		,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
		,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1};
}

string decode64(string s) {
	int cur=0;
	int curS=0;
	string r;
	for(size_t i=0; i<s.size(); ++i) {
		unsigned char c = s[i];
		int x = decTable[c];
//		cout<<"x "<<x<<' '<<s[i]<<" ; "<<cur<<'\n';
		if (x<0) continue;
		cur = (cur<<6) | x;
		curS += 6;
		if (curS>=8) {
			r += cur >> (curS-8);
			cur &= (1<<(curS-8))-1;
			curS -= 8;
		}
	}
	return r;
}
string encode64(string s) {
	string r;
	int cur=0;
	int curS=0;
	for(size_t i=0; i<s.size(); ++i) {
		unsigned char c = s[i];
		cur = (cur<<8) | c;
		curS += 8;
		while(curS>=6) {
			r += encTable[cur >> (curS-6)];
			cur &= (1<<(curS-6))-1;
			curS -= 6;
		}
	}
//	cout<<"curS "<<curS<<'\n';
	if (curS==2) {
		r += encTable[cur<<4];
		r+='=';
		r+='=';
	} else if (curS==4) {
		r += encTable[cur<<2];
		r+='=';
	}
//	cout<<"k "<<s.size()<<' '<<r.size()<<'\n';
	return r;
}
