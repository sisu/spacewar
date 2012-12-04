#pragma once
#include <string>
void startServer();
void runServer();
#ifndef NONET
void sendToObs(std::string msg);
#else
void sendToObs(std::string) {}
#endif
