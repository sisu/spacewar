#include "Vector.hpp"
#include "Process.hpp"
#include "SDL.h"
#include "draw.hpp"
#include "server.hpp"
#include "game.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>
#include <sys/time.h>
#include <fstream>
using namespace std;

vector<Planet> planets;
vector<Craft> crafts;

const float GROW_SPEED = 3;
const float MOVE_SPEED = 9;

ofstream replayOut;
ifstream replayIn;
bool isReplay = 0;
bool noGraphics = 0;

void hitPlanet(Craft& c, Planet& p) {
	if (c.owner == p.owner) {
		p.population++;
	} else {
		p.population--;
		if (p.population<0) {
			p.population = 1;
			p.owner = c.owner;
		} else if (p.population<1) {
			p.population = 0;
			p.owner = NONE;
		}
	}
}

void update(float dt) {
	for(size_t i=0; i<planets.size(); ++i) {
		Planet& p = planets[i];
		if (p.owner!=NONE)
			p.population += GROW_SPEED * dt;
	}
	for(size_t i=0; i<crafts.size(); ++i) {
		Craft& c = crafts[i];
		Planet& p = *c.dest;
		Vec3 dir = normalize(p.pos - c.pos);
		c.pos += dt * MOVE_SPEED * dir;

		if (norm(c.pos-p.pos) <= p.size) {
			hitPlanet(c, p);
			crafts[i] = crafts.back();
			crafts.pop_back();
			--i;
		}
	}
}

double randf() {
	return rand() / (double)RAND_MAX;
}
double randf2() {
	return 2*randf()-1;
}
Vec3 rvec() {
	return Vec3(randf2(), randf2(), randf2());
}
void sendCrafts(Planet& from, Planet& to, int count) {
	count = max(0, min(count, (int)from.population));
	for(int i=0; i<count; ++i) {
		Vec3 pos = from.pos + rvec();
		crafts.emplace_back(pos, from.owner, to);
	}
	from.population -= count;
#if 0
	if (from.population<1) {
		from.population = 0;
		from.owner = NONE;
	}
#endif
}

Planet makePlanet() {
	Planet p;
	p.owner = NONE;
	p.size = 2*(1+2*randf());
	p.population = rand() % (int)(5*p.size);
	p.pos = 30.f*rvec();
	return p;
}
void genPlanets(int n) {
	const double P = 40;
	const double S = 5;
	for(int i=0; i<2; ++i) {
		Planet p;
		p.pos = Vec3(P*(2.*i-1), 0., 0.);
		p.size = S;
		p.population = (int)(5*S);
		p.owner = (Player)(1+i);
		planets.push_back(p);
	}
	for(int i=2; i<n; ++i) {
again:
		Planet p;
		if (i&1) {
			p = planets.back();
			p.pos[0] *= -1;
		} else {
			p = makePlanet();
			if (i<n-1) {
				if (fabs(p.pos[0]) < p.size) goto again;
			} else {
				p.pos[0] = 0;
			}
			for(size_t j=0; j<planets.size(); ++j) {
				if (norm(planets[j].pos-p.pos) <= p.size + planets[j].size) {
					goto again;
				}
			}
		}
		planets.push_back(p);
	}
}

Process proc1, proc2;

int fixPl(Player real, Player who) {
	if (real==NONE) return NONE;
	return real==who ? P1 : P2;
}
string planetDesc(Player pl) {
	ostringstream oss;
	oss<<planets.size()<<'\n';
	for(size_t i=0; i<planets.size(); ++i) {
		Planet& p = planets[i];
		int owner = fixPl(p.owner, pl);;
		oss << p.pos[0]<<' '<<p.pos[1]<<' '<<p.pos[2]<<' '<<p.size<<' '<<(int)p.population<<' '<<owner<<'\n';
	}
	return oss.str();
}

string statusMessage(Player pl) {
	ostringstream oss;
	oss<<"PLANETS";
	for(size_t i=0; i<planets.size(); ++i) {
		Planet& p = planets[i];
		oss<<' '<<p.population<<' '<<fixPl(p.owner, pl);
	}
	oss<<'\n';
	return oss.str();
}
string sendMessage(int from, int to, int count, Player sender, Player pl) {
	ostringstream oss;
	oss<<"SEND "<<fixPl(sender, pl)<<' '<<from<<' '<<to<<' '<<count<<'\n';
	return oss.str();
}
double curTime;
void readInput(Process& proc, Player pl) {
	string s;
	int maxRead = 10;
	while(maxRead-- && !(s = proc.readLine()).empty()) {
//		cout<<"got message from "<<pl<<" : "<<s<<endl;
		istringstream iss(s);
		string msg;
		iss >> msg;
		if (msg=="STATUS") {
			proc.send(statusMessage(pl));
//			cout<<"sending status to "<<pl<<endl;
		} else if (msg=="SEND") {
			int from, to, count;
			iss >> from >> to >> count;
//			cout<<"send "<<pl<<": "<<from<<' '<<to<<' '<<count<<endl;
			Planet& fromP = planets[from];
			if (fromP.owner == pl) {
				sendCrafts(fromP, planets[to], count);
				proc1.send(sendMessage(from, to, count, pl, P1));
				proc2.send(sendMessage(from, to, count, pl, P2));
				string msg = sendMessage(from, to, count, pl, P1);
				replayOut << curTime<<' ' << msg;
				sendToObs(msg);
			}
		} else {
			cerr<<"fail "<<msg<<'\n';
		}
	}
}

double getTime() {
	timeval tp;
	gettimeofday(&tp, 0);
	return tp.tv_sec + tp.tv_usec/1e6;
}
double replayTime;
void runReplay() {
	while(replayTime < curTime) {
		string cmd;
		replayIn >> cmd;
//		cout<<"k "<<replayTime<<' '<<curTime<<": "<<cmd<<'\n';

		if (cmd=="END") {
			exit(0);
		} else if (cmd=="PLANETS") {
			for(size_t i=0; i<planets.size(); ++i) {
				Planet& p = planets[i];
				int own;
				replayIn >> p.population >> own;
//				cout<<"own "<<own<<'\n';
				p.owner = (Player)own;
			}
		} else if (cmd=="SEND") {
			int who, from, to, count;
			replayIn >> who>>from>>to>>count;
			planets[from].owner = (Player)who;
			sendCrafts(planets[from], planets[to], count);
		} else {
			cout<<"Invalid message: "<<cmd<<endl;
			abort();
		}

		replayIn >> replayTime;
	}
}
bool testEnd(double t) {
	if (t>=60) return 1;

	int counts[3] = {};
	for(size_t i=0; i<planets.size(); ++i) ++counts[planets[i].owner];
	for(size_t i=0; i<crafts.size(); ++i) ++counts[crafts[i].owner];
//	cout<<"end: "<<t<<' '<<counts[P1]<<' '<<counts[P2]<<'\n';
	if (counts[P1]==0 || counts[P2]==0) return 1;
	return 0;
}
void runGame() {
	double prevT = getTime();
	double t0 =prevT;
	double lastWrite=0;
	double lastSend=0;
	while(1) {
		if (!isReplay) {
			readInput(proc1, P1);
			readInput(proc2, P2);
			runServer();
		} else {
			runReplay();
		}

		double curT = getTime();
		curTime = curT - t0;
		double dt = curT - prevT;
		prevT = curT;
//		cout<<"dt: "<<dt<<'\n';

		if (testEnd(curTime)) break;

		if (dt<=0) continue;
		update(dt);

		if (!noGraphics)
			draw(curTime);
		SDL_Delay(1);

		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if (e.type==SDL_QUIT) return;
		}

		if (!isReplay) {
			if (curTime-lastWrite >= .1) {
				string msg = statusMessage(P1);
				replayOut <<curTime<<' ' << msg;
				replayOut.flush();
				lastWrite = curTime;

			}
		}
		if (curTime-lastSend >= .1) {
			sendToObs(statusMessage(P1));
			lastSend = curTime;
		}
		usleep(1000);
	}
//	cout<<"quitting\n";
}

void sighandler(int sig)
{
	cout<<"caught signal "<<sig<<endl;
	exit(-1);
}

int main(int argc, char* argv[]) {
	// At least chrome doesn't seem to close connections properly so ignoring
	// broken pipe errors seems to be the only way to avoid crashing after
	// observer has disconnected.
	signal(SIGPIPE, SIG_IGN);

	signal(SIGTERM, sighandler);

	srand(time(0));
	string replayFile = "game.replay";
	vector<string> bots;

	for(int i=1; i<argc; ++i) {
		string s = argv[i];
		if (s=="-r") {
			isReplay = 1;
		} else if (s=="-n") {
			noGraphics = 1;
		} else if (s=="-f") {
			replayFile = argv[++i];
		} else {
			bots.push_back(s);
		}
	}

	if (isReplay) {
		replayIn.open(argv[2]);
		assert(replayIn.good());
		isReplay = 1;

		int n;
		replayIn >> n;
		for(int i=0; i<n; ++i) {
			Planet p;
			double x,y,z;
			int who;
			replayIn>>x>>y>>z>>p.size>>p.population>>who;
			p.owner = (Player)who;
			p.pos = Vec3(x,y,z);
			planets.push_back(p);
//			cout<<"planet "<<x<<' '<<y<<' '<<z<<' '<<p.size<<' '<<p.population<<' '<<who<<'\n';
		}
		replayIn >> replayTime;
	} else {
		assert(bots.size()==2);
		proc1.start(bots[0]);
		proc2.start(bots[1]);

		replayOut.open(replayFile);

		genPlanets(13);
		proc1.send(planetDesc(P1));
		proc2.send(planetDesc(P2));
		replayOut << planetDesc(P1);

	}
	startServer();
	if (!noGraphics)
		openWindow(800, 600);

	runGame();

	if (!isReplay) {
		replayOut << curTime<<" END\n";
	}

	int pcounts[3] = {};
	double ucounts[3] = {};
	for(size_t i=0; i<planets.size(); ++i) {
		Planet& p = planets[i];
		++pcounts[p.owner];
		ucounts[p.owner] += (int)p.population;
	}
	for(size_t i=0; i<crafts.size(); ++i)
		++ucounts[crafts[i].owner];
//	for(int i=0; i<3; ++i) cout<<pcounts[i]<<' '<<ucounts[i]<<' ';cout<<'\n';
//	bool p1win = pcounts[P1]!=pcounts[P2] ? pcounts[P1]>pcounts[P2] : ucounts[P1]>ucounts[P2];
//	bool p1win = ucounts[P1]>ucounts[P2];
//	cout<<"WINNER: "<<(p1win ? "1" : "2")<<'\n';
	int c1 = ucounts[P1], c2 = ucounts[P2];
	int s1 = 100*c1/(c1+c2);
	int s2 = 100*c2/(c1+c2);
	while(s1+s2<100) {
		if (s1<s2) ++s1;
		else ++s2;
	}
	cout<<"SCORE: "<<s1<<' '<<s2<<'\n';

	if (!isReplay) {
		ofstream out("result.txt");
		out<<s1<<' '<<s2<<'\n';
	}
}
