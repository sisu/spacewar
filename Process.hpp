#pragma once
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string>
#include <iostream>

struct Process {
	Process(): pid(0) {}
	~Process() {
		if (pid) kill();
	}
	void start(std::string s) {
		name = s;

		size_t pos = s.find_last_of('/');
		std::string dir;
		if (pos!=std::string::npos) {
			dir = s.substr(0,pos);
			s = "./"+s.substr(pos+1);
		}

		init();

		int pid = fork();
		if (!pid) {
			if (!dir.empty()) chdir(dir.c_str());
			dup2(out[1], STDOUT_FILENO);
			dup2(in[0], STDIN_FILENO);
			system(s.c_str());
			perror(name.c_str());
			abort();
		}
		this->pid = pid;
		std::cout<<"got pid "<<this->pid<<std::endl;

		fcntl(out[0], F_SETFL, O_NONBLOCK | fcntl(out[0], F_GETFL, 0));
	}
	std::string readLine() {
		char tmp[1024];
		while(1) {
			int c = read(out[0], tmp, sizeof(tmp));
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
		write(in[1], &s[0], s.size());
	}
	void init() {
		pipe(in);
		pipe(out);
	}
	void kill() {
		std::cout<<"terminating process "<<pid<<' '<<name<<std::endl;
		::kill(pid, SIGKILL);
	}
	int in[2];
	int out[2];
	std::string name;
	int pid;

	std::string buf;
};
