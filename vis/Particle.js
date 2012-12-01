function Particle(pos, vel, color) {
	this.pos = pos;
	this.vel = vel;
	this.color = color;
}
Particle.prototype.update = function(dt) {
	this.pos = ivadd(this.pos, vmul(dt, this.vel));
	this.color[3] -= dt;
//	console.log('update '+this.pos+' '+this.color+' '+this.vel);
}
var particleId = null;
var particleArr = null;

function drawParticles() {
	var SEND_ONCE = 1024;
	if (particleId==null) {
		particleId = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, particleId);
		particleArr = new Float32Array(7*SEND_ONCE);
		var size = 7*SEND_ONCE*4;
		gl.bufferData(gl.ARRAY_BUFFER, size, gl.STREAM_DRAW);
	}
	gl.bindBuffer(gl.ARRAY_BUFFER, particleId);
	var mid = 3 * SEND_ONCE;
	var pidx = gl.getAttribLocation(prog, 'pos');
	var cidx = gl.getAttribLocation(prog, 'color');
	gl.enableVertexAttribArray(pidx);
	gl.enableVertexAttribArray(cidx);
	gl.vertexAttribPointer(pidx, 3, gl.FLOAT, false, 0, 0);
	gl.vertexAttribPointer(cidx, 4, gl.FLOAT, false, 0, mid*4);

	var n = game.particles.length;
	for(var k=0; k < Math.floor((n+SEND_ONCE-1)/SEND_ONCE); ++k) {
		var count = Math.min(n - k*SEND_ONCE, SEND_ONCE);
//		console.log('drawing: '+count);
		for(var i=0; i<count; ++i) {
			var p = game.particles[SEND_ONCE*k + i];
//			console.log('particle: '+p.pos+' '+p.color);
			for(var j=0; j<3; ++j) particleArr[3*i+j] = p.pos[j];
			for(var j=0; j<4; ++j) particleArr[mid+4*i+j] = p.color[j];
		}
		gl.bufferSubData(gl.ARRAY_BUFFER, 0, particleArr.subarray(0, 3*count));
		gl.bufferSubData(gl.ARRAY_BUFFER, mid*4, particleArr.subarray(mid, mid+4*count));
		gl.drawArrays(gl.POINTS, 0, count);
//		console.log('sending '+count+' particles');
	}
}
