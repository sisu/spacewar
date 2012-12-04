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

		LPSTR cmd = &s[0];
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.hStdOutput = outw;
		si.hStdInput = inr;
		si.dwFlags |= STARTF_USESTDHANDLES;

		bool ok = CreateProcess(0,
				cmd,
				0,
				0,
				1,
				0,
				0,
				0,
				&si,
				&pi);
		assert(ok);
	}
	std::string readLine() {
		char tmp[1024];
		while(1) {
			DWORD c;
			ReadFile(outr, tmp, sizeof(tmp), &c, 0);
			if (c<=0) break;
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
		WriteFile(inw, &s[0], s.size(), 0, 0);
	}


private:
	void init() {
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = 1;
		sa.lpSecurityDescriptor = 0;

		makeP(outr, outw);
		makeP(inr, inw);
	}
	void makeP(HANDLE& r, HANDLE& w) {
		CreatePipe(&r, &w, &sa, 0);
		SetHandleInformation(r, HANDLE_FLAG_INHERIT, 0);
	}

	SECURITY_ATTRIBUTES sa;
	HANDLE outr, outw, inr, inw;

	std::string buf;
};
