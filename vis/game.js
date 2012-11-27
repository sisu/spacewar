var GROW_SPEED = 1, MOVE_SPEED = 18;
var game = {
	crafts: [],
//	pressedKeys: [],
	prevTime: new Date().getTime(),
	updateID: null,
	planets: [],
	particles: [],
	totalTime: 0.0,
	replayStr: null,
	replayLine: 0,

	init: function() {
		for(var i=0; i<1; ++i) {
//			var p = new Planet();
//			this.planets.push(p);
		}
//		this.particles.push(new Particle([1,0,0], [1,0,0], [1,0,0,1]));
	},
	update: function() {
		try {
			var time = new Date().getTime();
			var dt = (time-this.prevTime)/1000.;
			this.prevTime = time;
			this.totalTime += dt;

			this.runReplay();
			this.growPlanets(dt);
			this.moveCrafts(dt);
			this.moveParticles(dt);
			draw();
		} catch(err) {
			console.log('exception: '+err.stack);
			this.stop();
		}
	},
	start: function() {
		this.crafts = [];
		this.particles = [];
		if (this.updateID) return;
		this.prevTime = new Date().getTime();
		this.updateID = setInterval(function() {game.update();},30);
	},
	stop: function() {
		clearInterval(this.updateID);
		this.updateID = null;
		stopDraw();
	},
	growPlanets: function(dt) {
		for(var i=0; i<this.planets.length; ++i) {
			var p = this.planets[i];
			if (p.owner!=0) p.population += dt * p.size * GROW_SPEED;
		}
	},
	sendCrafts: function(who, from, to, count) {
//		console.log('sending crafts: '+count);
		for(var i=0; i<count; ++i) {
			var pos = ivadd(rvec(3), this.planets[from].pos);
			this.crafts.push(new Craft(pos, this.planets[to], who));
		}
		this.planets[from].population -= count;
	},
	moveCrafts: function(dt) {
		for(var i=0; i<this.crafts.length; ++i) {
			var c = this.crafts[i];
			var p = c.dest;
			var dir = normalized(vsub(p.pos, c.pos));
			c.pos = ivadd(c.pos, ivmul(dt*MOVE_SPEED, dir));
			if (norm(vsub(c.pos,p.pos)) <= p.size) {
				this.crafts[i] = this.crafts.last();
				this.crafts.pop();
				--i;
//				if (c.owner!=p.owner)
//					this.spawnParticles(c.pos, dir, c.owner);
			}
		}

		var SIZE=2;
		var PUSH=1;
		for(var i=0; i<this.crafts.length; ++i) {
			for(var j=0; j<i; ++j) {
				var a = this.crafts[i], b = this.crafts[j];
				var d = vsub(b.pos, a.pos);
				if (norm(d) <= SIZE) {
//					var ato = vsub(a.dest.pos, a.pos);
//					var bto = vsub(b.dest.pos, b.pos);

					normalize(d);
					b.pos = ivadd(b.pos, vmul(dt*PUSH, d));
					a.pos = ivadd(a.pos, vmul(-dt*PUSH, d));

//					a.pos = vadd(a.dest.pos, ivmul(norm(ato)/norm(, ato));
				}
			}
		}
	},
	moveParticles: function(dt) {
		for(var i=0; i<this.particles.length; ++i) {
			this.particles[i].update(dt);
		}
		this.particles = this.particles.filter(function(p) {
			return p.color[3] > 0;
		});
	},
	spawnParticles: function(pos, dir, owner) {
		for(var i=0; i<10; ++i) {
			var color = plColor[owner].copy();
			color.push(1);
			var p = new Particle(pos, rvec(3), color);
//			console.log('spawning: '+p.pos+' ; '+p.color);
			this.particles.push(p);
		}
	},
	runReplay: function() {
		if (this.replayStr==null) return;
		for(; this.replayLine < this.replayStr.length; ++this.replayLine) {
			var line = this.replayStr[this.replayLine].split(' ');
			var t = parseFloat(line[0]);
			line.shift();
			if (t > this.totalTime) break;
			++this.replayLine;
			handleSplitted(line);
		}
		if (this.replayLine>=this.replayStr.length) game.stop();
	},
	initReplay: function() {
		handleInitMessage(this.replayStr);
		this.replayLine = parseInt(this.replayStr[0]) + 1;
	}
};
