#include <iostream>
#include "SDL.h"
#include "SDL_ttf.h"
using namespace std;
int main() {
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("font.ttf", 10);
	bool first=1;
	for(int i=0; i<10; ++i) {
		char text[2];
		text[0] = '0'+i;
		text[1] = 0;
		SDL_Color color = {0,0,0};
		SDL_Surface* s = TTF_RenderText_Solid(font, text, color);
		if (first) {
			cout<<"#pragma once\n";
			cout<<"const int FONT_W = "<<s->w<<", FONT_H = "<<s->h<<";\n";
			cout<<"const int font[10][FONT_H] = {\n";
			first=0;
		}
//		cout<<s->w<<' '<<s->h<<'\n';
		cout<<"{\n";
		for(int i=0; i<s->h; ++i) {
			cout<<"\t0b";
			for(int j=0; j<s->w; ++j) {
				int c = ((char*)s->pixels)[s->pitch*i + j];
				cout<<c;
			}
			cout<<",\n";
		}
		cout<<"},\n";
	}
	cout<<"};\n";
}
