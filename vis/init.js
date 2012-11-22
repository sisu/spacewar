var prog = null;
var textProg = null, planetProg = null, craftProg = null, particleProg = null;
var gl = null;
var textTex = null, planetTex = null, particleTex = null;
var palette = [null,null,null];

function makeProg(vs, fs) {
	var frag = getShader(fs);
	var vert = getShader(vs);

	var prog = gl.createProgram();
	gl.attachShader(prog, vert);
	gl.attachShader(prog, frag);
	gl.linkProgram(prog);
	if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
		alert("Failed initializing shader program.");
	}
	return prog;
}
function initShaders() {
	craftProg = makeProg("shader-vs", "shader-fs");
	planetProg = makeProg("planet-vs", "planet-fs");
	textProg = makeProg("text-vs", "text-fs");
	particleProg = makeProg("particle-vs", "particle-fs");
}
function makeNumTexture() {
	var canvas = document.createElement('canvas');
	var W = 12, H =20;
	canvas.width = 10*W;
	canvas.height = H;
	var ctx = canvas.getContext('2d');
	ctx.fillStyle = 'black';
	ctx.fillRect(0,0, canvas.width, canvas.height);
	ctx.fillStyle = 'white';
	ctx.font = '20px monospace'
	ctx.textBaseline = 'top';
	ctx.textAlign = 'center';
//	ctx.fillText("0123456789", 0, 0);
	for(var i=0; i<10; ++i) {
		ctx.fillText(i+'', W*i + W/2, 0);
	}

	/*
	var image = new Array(canvas.width * canvas.height);
	for(var i=0; i<canvas.height; ++i) {
		for(var j=0; j<canvas.width; ++j) {
			image[canvas.width*i+j] = i^j;
		}
	}*/

	textTex = gl.createTexture();
	gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
	gl.bindTexture(gl.TEXTURE_2D, textTex);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, canvas);
//	gl.texImage2D(gl.TEXTURE_2D, 0, gl.ALPHA, canvas.width, canvas.height, 0, gl.ALPHA, gl.UNSIGNED_BYTE, new Uint8Array(image));
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
}
function makePlanetTexture() {
	var s = 128;
	var image = new Array(s*s);
	for(var i=0; i<s; ++i) {
		for(var j=0; j<s; ++j) {
			var x = j, y = i;
//			image[s*i+j] = i^j;
			var f = (4.0 + Math.sin(x / 16.0) + Math.sin(y / 8.0) + Math.sin((x + y) / 16.0) + Math.sin(Math.sqrt(x * x + y * y) / 8.0)) / 8.0;
			image[s*i+j] = 255*f;
		}
	}
	planetTex = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, planetTex);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.ALPHA, s, s, 0, gl.ALPHA, gl.UNSIGNED_BYTE, new Uint8Array(image));
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
//	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
//	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

	var ps = 128;
	var pal0 = new Array(3*ps);
	var pal1 = new Array(3*ps);
	var pal2 = new Array(3*ps);
	for(var i=0; i<ps; ++i) {
		var fi = 2 * Math.PI * i / ps;

		pal0[3*i] = pal0[3*i+1] = pal0[3*i+2] = 128 + 128*.5*(1 + Math.sin(fi));

		pal1[3*i] = 16 * .5 * (1 + Math.sin(8*fi));
		pal1[3*i+1] = 300 * .5 * (1 + Math.sin(2*fi));
		pal1[3*i+2] = 255;

//		pal2[3*i] = 255 * .5 * (1 + Math.sin(fi));
		pal2[3*i] = 255;
		pal2[3*i+1] = 300 * .5 * (1 + Math.sin(2*fi));
		pal2[3*i+2] = 16 * .5 * (1 + Math.sin(8*fi));

	}
	function make(img) {
		var t = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, t);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, ps, 1, 0, gl.RGB, gl.UNSIGNED_BYTE, new Uint8Array(img));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		return t;
	}
	var imgs = [pal0, pal1, pal2];
	for(var i=0; i<3; ++i)
		palette[i] = make(imgs[i]);
}
function makeParticleTexture() {
	var s = 128;
	var img = new Array(s*s);
	for(var i=0; i<s; ++i) {
		for(var j=0; j<s; ++j) {
			var x = j/s-.5, y = i/s-.5;
			var c = .1/(x*x + y*y) - 1;
			img[s*i+j] = 255 * Math.max(0, Math.min(1, c));
		}
	}
	particleTex = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, particleTex);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.ALPHA, s, s, 0, gl.ALPHA, gl.UNSIGNED_BYTE, new Uint8Array(img));
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
}
function makeTextures() {
	makeNumTexture();
	makePlanetTexture();
	makeParticleTexture();
}

/*
function initDebug() {
	debugElem = document.getElementById('debug');
	debugElem.setAttribute("style", "height:320px; width:600px; overflow:scroll;");
}
function debug(str) {
	debugElem.innerHTML += str+"<br/>";
	debugElem.scrollTop = debugElem.scrollHeight;
}*/

initDone = false;
function handleMessage(e) {
	var msg = e.data;
//	console.log("ws message: "+msg);

	if (!initDone) {
		var lines = msg.split('\n');
		var n = parseInt(lines[0]);
		console.log('planets: '+n);
		game.planets.length = n;
		for(var i=0; i<n; ++i) {
			var s = lines[i+1].split(' ');
			var fs = s.map(parseFloat);
			var x=fs[0], y=fs[1], z=fs[2], size=fs[3], pop=fs[4], owner=parseInt(s[5]);
			game.planets[i] = new Planet(x,y,z,size,pop,owner);
		}
		initDone = 1;
	} else {
		var s = msg.split(' ');
		if (s[0]=='PLANETS') {
//			console.log('planets: '+s);
//			console.log(game.planets.length);
			for(var i=0; i<game.planets.length; ++i) {
				var p = game.planets[i];
				p.population = parseInt(s[2*i+1]);
				p.owner = parseInt(s[2*i+2]);
//				console.log('setting owner: '+p.owner);
			}
		} else if (s[0]=='SEND') {
			s.shift();
//			var is = s.map(parseInt); // fails, chrome bug?
			var is = s.map(function(x){return parseInt(x);});
//			console.log(s);
//			console.log(is);
			var who = is[0], from = is[1], to = is[2], count = is[3];
			game.sendCrafts(who, from, to, count);
		} else if (s[0]=='STOP') {
			game.stop();
		} else {
			console.log('unknown msg: '+msg);
		}
	}
}

function initSocket() {
	ws = new WebSocket("ws://127.0.0.1:41629");
	ws.onopen = function(e) { console.log("ws open"); ws.send("lol"); }
	ws.onclose = function(e) { console.log("ws close"); game.stop(); }
	ws.onmessage = handleMessage;
	ws.onerror = function(e) { console.log("ws error"); }
}

var dragging = false;
var prevMouseX, prevMouseY;
function mouseMove(e) {
	if (!dragging) return;
//	console.log(e.screenY+' '+e.screenX);
	var dx = e.screenX - prevMouseX;
	var dy = e.screenY - prevMouseY;
	var speed = .03;
	camRotX += speed*dx;
	camRotY += speed*dy;
//	console.log(camRotX);
	prevMouseX = e.screenX;
	prevMouseY = e.screenY;
}

function init() {
	var replay = QueryString.replay;
	console.log('replay: '+replay);
	if (replay==undefined) {
		initSocket();
	} else {
		var c = new XMLHttpRequest();
		c.open('GET', replay);
		c.onreadystatechange = function() {
			console.log(c.responseText);
		}
	}

	var canvas = document.getElementById('canvas');
	canvas.onmousedown = function(e) {dragging = true; prevMouseX=e.screenX; prevMouseY=e.screenY;};
	canvas.onmouseup = function() {dragging =false;};
	canvas.onmousemove = mouseMove;
	gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
	assert(gl, 'gl');
//	initDebug();
	initShaders();
	makeTextures();
	game.init();
	draw();
	game.start();
	console.log("init done");
}

/*
window.onfocus = function() {
	game.start();
}
window.onblur = function() {
	game.stop();
}*/
