function makeCube() {
	var m = new Model();
	var verts = [
		-1,-1,-1,
		1,-1,-1,
		1,1,-1,
		-1,1,-1,
		-1,-1,1,
		1,-1,1,
		1,1,1,
		-1,1,1];
	m.vattrs.pos = new Float32Array(verts);
	m.vattrs.normal = new Float32Array(verts);
	var quads = [
		[0,1,2,3],
		[4,5,6,7],
		[0,1,5,4],
		[2,3,7,6],
		[0,3,7,4],
		[1,5,6,2]];
	for(var i=0; i<quads.length; ++i) {
		var q = quads[i];
		m.indices.push(q[0],q[1],q[2],q[0],q[2],q[3]);
	}
	m.load();
	return m;
}
function makeSphere() {
	var m = new Model();
	var R=16, S=16;
	var verts = [];
	for(var i=0; i<=R; ++i) {
		var fi = Math.PI*i/R;
		var si = Math.sin(fi);
		var y = Math.sin(fi - .5*Math.PI);
		for(var j=0; j<S; ++j) {
			var fj = 2*Math.PI*j/S;
			var x = si * Math.cos(fj);
			var z = si * Math.sin(fj);
			verts.push(x,y,z);
		}
	}
	for(var i=0; i<R; ++i) {
		for(var j=0; j<S; ++j) {
			var jj = (j+1)%S;
			var a = i*S + j;
			var b = (i+1)*S + j;
			var a2 = i*S + jj;
			var b2 = (i+1)*S + jj;
			m.indices.push(a,a2,b, a2,b,b2);
		}
	}
	m.vattrs.pos = new Float32Array(verts);
	m.vattrs.normal = new Float32Array(verts);
	m.load();
	return m;
}
function makeQuad() {
	var m = new Model();
	var vs = [-1,-1,0, 1,-1,0, 1,1,0, -1,1,0];
	m.vattrs.pos = new Float32Array(vs);
	m.indices.push(0,1,2, 0,2,3);
	m.load();
	return m;
}
