var GROW_SPEED = 3, MOVE_SPEED = 9;
var game = {
	crafts: [],
//	pressedKeys: [],
	prevTime: new Date().getTime(),
	updateID: null,
	planets: [],

	init: function() {
		for(var i=0; i<1; ++i) {
//			var p = new Planet();
//			this.planets.push(p);
		}
	},
	update: function() {
		try {
			var time = new Date().getTime();
			var dt = (time-this.prevTime)/1000.;
			this.prevTime = time;

			this.growPlanets(dt);
			this.moveCrafts(dt);
			draw();
		} catch(err) {
			console.log('exception: '+err.stack);
			this.stop();
		}
	},
	start: function() {
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
			if (p.owner!=0) p.population += dt * GROW_SPEED;
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
	}
};
