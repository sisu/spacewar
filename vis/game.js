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

			this.updateMove();
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
	updateMove: function() {
	},
	moveCrafts: function(dt) {
		for(var i=0; i<this.crafts.length; ++i) {
//			moveUnit(this.crafts[i], dt, this.area, this.crafts);
		}
	}
};
