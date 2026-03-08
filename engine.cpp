#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include "utf8.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <emscripten/html5.h>

EM_JS(int, js_win_w, (), {
    return window.visualViewport ? Math.round(window.visualViewport.width)  : window.innerWidth;
});
EM_JS(int, js_win_h, (), {
    return window.visualViewport ? Math.round(window.visualViewport.height) : window.innerHeight;
});

int scale = 3;
int a = 0;
SDL_Rect ARect; SDL_Rect BRect;
SDL_Texture* texCenter  = nullptr;
SDL_Texture* rus  = nullptr;

SDL_Window*   gWindow   = nullptr;
SDL_Renderer* gRenderer = nullptr;

std::unordered_map<std::string,unsigned short> f;

int gW = 0, gH = 0;

SDL_Texture* loadTex(const char* path) {
    SDL_Surface* s = IMG_Load(path);
    SDL_Texture* t = SDL_CreateTextureFromSurface(gRenderer, s);
    SDL_FreeSurface(s);
    return t;
}

void DrawText(unsigned int nado, std::string text, std::unordered_map<std::string,unsigned short> f, 
              SDL_Renderer* renderer, SDL_Texture* rus, int x, int y, double time) {

    unsigned int counter=0;
    std::vector<std::string> letters;
    auto it = text.begin();
    try {while (it != text.end() and counter<nado) {
      auto start = it; utf8::next(it, text.end());
      letters.emplace_back(start, it); if (letters[letters.size()-1]!="$") {counter++;}}}
    catch (...) {std::cout << "UTF-8 parsing error!" << std::endl;return;}
    //dialog.sound=1;
    if (letters.size()!=0 and (letters[letters.size()-1]=="|" or letters[letters.size()-1]==" ")) {
    //dialog.sound=0;
    }

    unsigned int shifting = 0;
    unsigned int up = 0;
    for (unsigned int i = 0; i<letters.size(); i++) {
    if (i!=0 and i-1 < letters.size()) {
        auto it_find = f.find(letters[i-1]);
        if (it_find != f.end() && it_find->second == 69) {
        up=(int)round(sin(i/2.0+time/8.0)*2);}
        else {
        up=0;}
    if (it_find != f.end()) {
        if (it_find->second==66) {shifting+=6;}
        else if (it_find->second==69) {shifting+=6;}
        else if (it_find->second==80) {shifting+=4;}
        else if (it_find->second==81) {shifting+=2;}
        else if (it_find->second==82) {shifting+=4;}
        else if (it_find->second==83) {shifting+=3;}
        else if (it_find->second==85) {shifting+=4;}
        else if (it_find->second==88) {shifting+=2;}}}
    auto current_find = f.find(letters[i]);
    if (current_find != f.end()) {
      ARect = {current_find->second*6,0,5,9};
      BRect = {x+(int)i*6*scale-(int)shifting*scale,y+(int)up-scale*4,5*scale,9*scale};
      SDL_RenderCopy(renderer, rus, &ARect, &BRect);}}
}

void render(SDL_Texture* texture,
            float x, float y,
            unsigned char fx, unsigned char fy,
            unsigned char nfx, unsigned char nfy,
            bool cx=0, bool cy=0) {

    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
    ARect = {textW/fx*nfx, textH/fy*nfy, textW/fx, textH/fy};
    BRect = {
        static_cast<int>((int)x - round((textW/fx*1)/2)*cx),
        static_cast<int>((int)y - round((textH/fy*1)/2)*cy),
        textW/fx*1,
        textH/fy*1
    };
    SDL_RenderCopy(gRenderer, texture, &ARect, &BRect);
}

void main_loop() {
    a++;
    if (a>200) {a=0;}
    int w = js_win_w();
    int h = js_win_h();

    if (w != gW || h != gH) {
        gW = w;
        gH = h;
        SDL_SetWindowSize(gWindow, gW, gH);
        SDL_RenderSetViewport(gRenderer, nullptr);
    }

    if (gW<320 or gH<180) {scale=2;}
    else if (gW<640 or gH<360) {scale=3;}
    else if (gW<960 or gH<540) {scale=4;}
    else {scale=5;}

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) emscripten_cancel_main_loop();
    }

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    int size = 15*scale;
    SDL_Rect rect = {
        gW / 2 - size / 2,
        gH / 2 - size / 2,
        size, size
    };
    SDL_SetRenderDrawColor(gRenderer, 255, 100, 50, 255);
    //SDL_RenderFillRect(gRenderer, &rect);

    //render(texCenter, gW / 2, gH / 2, 1, 1, 0, 0, true, true);

    DrawText(999,"10.07.202$Х",f,gRenderer,rus,gW/2-3*scale*9,gH/2,a/4);

    SDL_RenderPresent(gRenderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    gW = js_win_w();
    gH = js_win_h();

    gWindow   = SDL_CreateWindow("Something", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gW, gH, 0);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    texCenter = loadTex("assets/player.jpg");
    rus = loadTex("assets/rus2.png");

    f["А"]=0;  f["а"]=1;  f["Б"]=2;  f["б"]=3;  f["В"]=4;  f["в"]=5;  f["Г"]=6;  f["г"]=7;  f["Д"]=8;  f["д"]=9;
    f["Е"]=10; f["е"]=11; f["Ё"]=12; f["ё"]=13; f["Ж"]=14; f["ж"]=15; f["З"]=16; f["з"]=17; f["И"]=18; f["и"]=19;
    f["Й"]=20; f["й"]=21; f["К"]=22; f["к"]=23; f["Л"]=24; f["л"]=25; f["М"]=26; f["м"]=27; f["Н"]=28; f["н"]=29;
    f["О"]=30; f["о"]=31; f["П"]=32; f["п"]=33; f["Р"]=34; f["р"]=35; f["С"]=36; f["с"]=37; f["Т"]=38; f["т"]=39;
    f["У"]=40; f["у"]=41; f["Ф"]=42; f["ф"]=43; f["Х"]=44; f["х"]=45; f["Ц"]=46; f["ц"]=47; f["Ч"]=48; f["ч"]=49;
    f["Ш"]=50; f["ш"]=51; f["Щ"]=52; f["щ"]=53; f["Ъ"]=54; f["ъ"]=55; f["Ы"]=56; f["ы"]=57; f["Ь"]=58; f["ь"]=59;
    f["Э"]=60; f["э"]=61; f["Ю"]=62; f["ю"]=63; f["Я"]=64; f["я"]=65; f["|"]=66; f["&"]=67; f["@"]=68; f["$"]=69;
    f["0"]=70; f["1"]=71; f["2"]=72; f["3"]=73; f["4"]=74; f["5"]=75; f["6"]=76; f["7"]=77; f["8"]=78; f["9"]=79;
    f[":"]=80; f["-"]=81; f["."]=82; f[","]=83; f["?"]=84; f["!"]=85; f[" "]=86; f["~"]=87; f["'"]=88; f["#"]=89;

    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}