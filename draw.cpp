#include "game.hpp"
#include "SDL.h"
#include "font.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
using namespace std;

vector<GLfloat> ballVerts;
vector<GLushort> ballInd;
void genBall() {
	const int R=16, S=16;
	for(int i=0; i<=R; ++i) {
		double fi = M_PI*i/R;
		double si = sin(fi);
		double y = sin(-.5*M_PI + fi);
		for(int j=0; j<S; ++j) {
			double fj = 2*M_PI*j/S;
			double x = si * cos(fj);
			double z = si * sin(fj);
			ballVerts.push_back(x);
			ballVerts.push_back(y);
			ballVerts.push_back(z);
		}
	}
	for(int i=0; i<R; ++i) {
		for(int j=0; j<S; ++j) {
			int jj = (j+1)%S;
			int a = i*S + j;
			int b = (i+1)*S + j;
			int a2 = i*S + jj;
			int b2 = (i+1)*S + jj;

			ballInd.push_back(a);
			ballInd.push_back(a2);
			ballInd.push_back(b);

			ballInd.push_back(a2);
			ballInd.push_back(b);
			ballInd.push_back(b2);
		}
	}
}

GLubyte fontImage[FONT_H][10*FONT_W];
GLuint textTex;
void genFont() {
	for(int i=0; i<10; ++i) {
		for(int y=0; y<FONT_H; ++y) {
			for(int x=0; x<FONT_W; ++x) {
				int c = 1&(font[i][y]>>x);
				fontImage[FONT_H-1-y][FONT_W*i + x] = c*255;
			}
		}
	}
	glGenTextures(1, &textTex);
	glBindTexture(GL_TEXTURE_2D, textTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 10*FONT_W, FONT_H, 0, GL_ALPHA, GL_UNSIGNED_BYTE, fontImage);
}

void openWindow(int w, int h) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(w, h, 0, SDL_OPENGL);

	genBall();
	genFont();
}

void drawTri(double x, double y, double z, double s) {
	glVertex3f(x-s,y-s,z);
	glVertex3f(x+s,y-s,z);
	glVertex3f(x,y+s,z);
}

GLfloat plColors[3][3] = {
	{.5,.5,.5},
	{.1,.1,1},
	{1,.1,.1},
};

void drawNumber(int num) {
	char buf[32];
	sprintf(buf, "%d", num);
	int n = strlen(buf);

	glBegin(GL_QUADS);
	const double W = .5;
	for(int i=0; i<n; ++i) {
		int x = buf[i]-'0';
		double sx = x/10., ex = (x+1)/10.;
		double pos = i*2*W - (n-1)*W;
		glTexCoord2f(ex,0); glVertex2f(pos-W,-1);
		glTexCoord2f(sx,0); glVertex2f(pos+W,-1);
		glTexCoord2f(sx,1); glVertex2f(pos+W,1);
		glTexCoord2f(ex,1); glVertex2f(pos-W,1);
	}
	glEnd();
}

void drawPlanets() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &ballVerts[0]);
	glNormalPointer(GL_FLOAT, 0, &ballVerts[0]);
	for(size_t i=0; i<planets.size(); ++i) {
		glPushMatrix();

		Planet& p = planets[i];
		glColor3fv(plColors[p.owner]);
		Vec3 pos = p.pos;
		glTranslatef(pos[0], pos[1], pos[2]);
		glScalef(p.size, p.size, p.size);

//		glDrawArrays(GL_POINTS, 0, ballVerts.size()/3);
		glDrawElements(GL_TRIANGLES, ballInd.size(), GL_UNSIGNED_SHORT, &ballInd[0]);

		glPopMatrix();
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

#if 0
	glBegin(GL_TRIANGLES);
	for(size_t i=0; i<planets.size(); ++i) {
		Planet& p = planets[i];
		glColor3fv(plColors[p.owner]);
		Vec3 pos = p.pos;
		drawTri(pos[0], pos[1], pos[2], p.size);
	}
	glEnd();
#endif

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, textTex);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(.8,.8,.8);
	glDisable(GL_LIGHTING);
	for(size_t i=0; i<planets.size(); ++i) {
		glPushMatrix();


		Planet& p = planets[i];
//		glColor3fv(plColors[p.owner]);
		Vec3 pos = p.pos;
		glTranslatef(pos[0], pos[1], pos[2]);
#if 1
		GLfloat curM[16];
		glGetFloatv(GL_MODELVIEW_MATRIX , curM);
		for(int y=0; y<3; ++y) for(int x=0; x<3; ++x)
			curM[4*y+x] = y==x;
		glLoadMatrixf(curM);
#endif
		glScalef(p.size, p.size, p.size);

		drawNumber(p.population);
#if 0
		glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex2f(-1,-1);
		glTexCoord2f(1,0); glVertex2f(1,-1);
		glTexCoord2f(1,1); glVertex2f(1,1);
		glTexCoord2f(0,1); glVertex2f(-1,1);
		glEnd();
#endif

		glPopMatrix();
	}
}
void drawCrafts() {
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	for(size_t i=0; i<crafts.size(); ++i) {
		Craft& c = crafts[i];
		glColor3fv(plColors[c.owner]);
		Vec3 pos = c.pos;
		drawTri(pos[0], pos[1], pos[2], 1);
	}
	glEnd();
}
void setProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 4./3., .1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void draw(double t) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjection();
	glLoadIdentity();
	glTranslatef(0,0,-75);
	double curRot = 30*t;
	glRotatef(curRot, 0, 1, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	GLfloat pos[4] = {1,1,1,0};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	drawPlanets();
	drawCrafts();

#if 0
	glBegin(GL_TRIANGLES);
	glVertex2f(-1,-1);
	glVertex2f(1,-1);
	glVertex2f(0,1);
	glEnd();
#endif
	GLuint err = glGetError();
	if (err) {
		cout<<"GL ERROR: "<<gluErrorString(err)<<endl;
	}

	SDL_GL_SwapBuffers();
}
