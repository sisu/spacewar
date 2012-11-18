function getCamSpeed(ydiff, prevV) {
	return ydiff;
}
var camY=null, camYVel=0.;
var prevDrawTime=null;

var planetModel = null;

function getViewM() {
	var proj = perspectiveM(radians(60.0), 4./3., 0.1, 1000.);
	var trans = translateM(vec3(0.,0.,-75.));
	return mmmult(proj, trans);

	var ppos = vec3(0.,0.,0.);

	if (prevDrawTime===null) {
		prevDrawTime = new Date().getTime();
	}
	var time = new Date().getTime();
	var dt = (time-prevDrawTime)/1000.;
	prevDrawTime = time;

	if (camY===null) {
		camY = ppos[1];
		camYVel = 0.;
	}
	camYVel = getCamSpeed(ppos[1]-camY, camYVel);
	camY += dt*camYVel;
	ppos[1] = -camY;

	ppos[0] *= -1;
	var ptrans = translateM(ppos);
	var trans = translateM(vec3(0.,-1.,-15.));
	var rot = resizeM(rotateX(.5), 4);
	return mult(proj, trans, rot, ptrans);
}
function unitTransM(u) {
//	var curT = new Date().getTime(); var t = (curT-time0) / 1000.;
//	var rot = resizeM(rotateY(3*t), 4);
//	var rot2 = resizeM(rotateX(2*t), 4); var rotation = mmmult(rot2,rot);
	var ppos = u.pos.copy();
	ppos[2] *= -1;
	ppos[1] += .5*u.height;
	var trans = translateM(ppos);
	var scale = scaleM([u.rad, .5*u.height, u.rad]);
	return mmmult(trans, scale);
//	return mmmult(trans, rotation);
	return trans;
}
function planetTransM(p) {
	var s = p.size;
	var trans = translateM(p.pos);
	var scale = scaleM([s, s, s]);
	return mmmult(trans, scale);
}
function setTrans(proj, mat) {
	var tloc = gl.getUniformLocation(prog, 'trans');
	gl.uniformMatrix4fv(tloc, false, mmmult(proj, mat));
	var nloc = gl.getUniformLocation(prog, 'nmat');
	gl.uniformMatrix3fv(nloc, false, transpose(resizeM(mat,3)));
}
function setLight() {
	// TODO: transform light
	var lloc = gl.getUniformLocation(prog, 'ldir');
	var ldir = normalize(vec3(1., 1., 1));
	gl.uniform3f(lloc, ldir[0], ldir[1], ldir[2]);
}
function drawCraft(u, view) {
	var pltrans = unitTransM(u);
	setTrans(view, pltrans);
	var cloc = gl.getAttribLocation(prog, 'color');
	gl.disableVertexAttribArray(cloc);
	gl.vertexAttrib3f(cloc, .7, .4, .8);
	u.model.draw();
}
function drawPlanet(p, view) {
	setTrans(view, planetTransM(p));
	var cloc = gl.getAttribLocation(prog, 'color');
	gl.disableVertexAttribArray(cloc);
	if (p.owner==0)
		gl.vertexAttrib3f(cloc, .5, .5, .5);
	else if (p.owner==1)
		gl.vertexAttrib3f(cloc, .1, .1, .9);
	else
		gl.vertexAttrib3f(cloc, .9, .1, .1);
	planetModel.draw();
}

function draw() {
	if (planetModel==null) {
		planetModel = makeCube(1,1,1);
	}
	gl.clearColor(0,0,0,1);
	gl.clear(gl.COLOR_BUFFER_BIT);
	gl.flush();
	gl.enable(gl.DEPTH_TEST);
	setLight();

	var view = getViewM();

	for(var i=0; i<game.planets.length; ++i)
		drawPlanet(game.planets[i], view);

	for(var i=0; i<game.crafts.length; ++i)
		drawCraft(game.crafts[i], view);

//	setTrans(view, identityM(4));
//	setTrans(view, scaleM([1,1,-1]));

	var err = gl.getError();
	if (err!=0) {
		alert("GL error: "+err);
	}
}
function stopDraw() {
	prevDrawTime=null;
}
