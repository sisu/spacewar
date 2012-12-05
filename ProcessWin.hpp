#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <cassert>

struct Process {
	Process() {}
	~Process() {}

	void start(std::string s) {
		init();
		std::cout<<"starting "<<s<<std::endl;

		size_t pos = s.find_last_of("/\\");
		std::string dir;
		if (pos!=std::string::npos) {
			dir = s.substr(0,pos);
		}

		LPSTR cmd = &s[0];
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.hStdOutput = outw;
		si.hStdInput = inr;
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si.dwFlags |= STARTF_USESTDHANDLES;

		bool ok = CreateProcess(0,
				cmd,
				0,
				0,
				1,
				0,
				0,
				dir.c_str(),
				&si,
				&pi);
		assert(ok);
	}
	std::string readLine() {
		char tmp[1024];
		while(1) {
			DWORD c=0;
			PeekNamedPipe(outr, 0, 0, 0, &c, 0);
			if (!c) break;
			if (c>(DWORD)sizeof(tmp)) c=sizeof(tmp);
			bool ok = ReadFile(outr, tmp, c, &c, 0);
			if (c<=0 || !ok) break;
			buf.append(tmp, tmp+c);
		}
		size_t pos = buf.find_first_of('\n');
//		std::cout<<"read res: "<<buf<<' '<<pos<<std::endl;
		if (pos==std::string::npos) return "";
		std::string res = buf.substr(0,pos);
		buf = buf.substr(pos+1);
		return res;
	}
	void send(std::string s) {
//		std::cout<<"sending message "<<s<<std::endl;
		DWORD c=0;
		bool ok = WriteFile(inw, &s[0], s.size(), &c, 0);
		assert(ok);
		assert(c==(DWORD)s.size());
		ok = FlushFileBuffers(inw);
		assert(ok);
	}


private:
	void init() {
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = 1;
		sa.lpSecurityDescriptor = 0;

		CreatePipe(&outr, &outw, &sa, 0);
		SetHandleInformation(outr, HANDLE_FLAG_INHERIT, 0);
		CreatePipe(&inr, &inw, &sa, 0);
		SetHandleInformation(inw, HANDLE_FLAG_INHERIT, 0);
	}

	SECURITY_ATTRIBUTES sa;
	HANDLE outr, outw, inr, inw;

	std::string buf;
};
