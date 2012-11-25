#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;
struct Planet {
	double x,y,z;
	double size;
	double population;
	int owner;
};
int nPlanets;
vector<Planet> planets;

void readInput() {
	cout<<"STATUS"<<endl;
	while(1) {
		string msg;
		cin >> msg;
		if (msg=="PLANETS") {
			for(int i=0; i<nPlanets; ++i) {
				Planet& p = planets[i];
				cin >> p.population >> p.owner;
//				cerr<<"planet "<<i<<' '<<p.population<<' '<<p.owner<<'\n';
			}
			break;
		} else if (msg=="SEND") {
			int sender,from,to,count;
			cin >> sender >> from >> to >> count;
		} else {
			cerr<<"FAIL: "<<msg<<'\n';
		}
	}
}
void play() {
	for(int i=0; i<nPlanets; ++i) {
		Planet& p = planets[i];
//		cerr<<"testing plane "<<p.owner<<' '<<p.population<<'\n';
		if (p.owner == 1 && p.population>=20) {
			int count = rand() % (int)p.population;
			int to = rand() % nPlanets;
			cout<<"SEND "<<i<<' '<<to<<' '<<count<<endl;
//			cerr<<"SEND "<<i<<' '<<to<<' '<<count<<" ; "<<p.population<<endl;
			p.population -= count;
		}
	}
}

int main() {
	cin >> nPlanets;
	for(int i=0; i<nPlanets; ++i) {
		Planet p;
		cin >> p.x >> p.y >> p.z >> p.size >> p.population >> p.owner;
		planets.push_back(p);
	}
//	cerr<<"read planets done "<<nPlanets<<' '<<!!cin<<endl;

	while(1) {
		readInput();
		play();
	}
}
