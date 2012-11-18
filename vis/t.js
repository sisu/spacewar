var prog = null;
var model = null;
var gl = null;

function makeModel() {
	var m = new Model();
	var verts = [
		-1,-1,0,
		1,-1,0,
		0,1,0
		];
	m.vattrs.pos = new Float32Array(verts);
	m.indices.push(0,1,2);
	return m;
}
function initShaders() {
	var frag = getShader("shader-fs");
	var vert = getShader("shader-vs");

	prog = gl.createProgram();
	gl.attachShader(prog, vert);
	gl.attachShader(prog, frag);
	gl.linkProgram(prog);
	if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
		alert("Failed initializing shader program.");
	}
	gl.useProgram(prog);
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
			var is = s.map(parseInt);
			var who = is[1], from = is[2], to = is[3], count = is[4];
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

function init() {
	initSocket();
	var canvas = document.getElementById('canvas');
	gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
	assert(gl, 'gl');
//	initDebug();
	initShaders();
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
