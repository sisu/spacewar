import java.util.*;
public class Bot {
	public static void main(String[] args) {
		new Bot().run();
	}
	Scanner s = new Scanner(System.in);
	Random rng = new Random();
	Planet[] planets;

	void run() {
		s.useLocale(Locale.US);
		planets = new Planet[s.nextInt()];
		for(int i=0; i<planets.length; ++i) {
			Planet p = new Planet();
			p.x = s.nextDouble();
			p.y = s.nextDouble();
			p.z = s.nextDouble();
			p.size = s.nextDouble();
			p.population = s.nextDouble();
			p.owner = s.nextInt();
			planets[i] = p;
		}

		while(true) {
			readInput();
			play();
		}
	}

	void readInput() {
		System.out.println("STATUS");
		while(true) {
			String msg = s.next();
			if (msg=="PLANETS") {
				for(int i=0; i<planets.length; ++i) {
					Planet p = planets[i];
					p.population = s.nextDouble();
					p.owner = s.nextInt();
				}
				break;
			} else if (msg=="SEND") {
				int owner = s.nextInt();
				int from = s.nextInt();
				int to = s.nextInt();
				int count = s.nextInt();
			}
		}
	}

	void play() {
		for(int i=0; i<planets.length; ++i) {
			Planet p = planets[i];
			if (p.owner==1 && p.population>=20) {
				int count = rng.nextInt((int)p.population);
				int to = rng.nextInt(planets.length);

				System.out.printf("SEND %d %d %d\n", i, to, count);
				p.population -= count;
			}
		}
	}
}

class Planet {
	double x,y,z;
	double size;
	double population;
	int owner;
}
