//Версия 0.2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

SDL_Surface *screen;
SDL_Event Event;
int bol, pause, speed, scen, x, y, Height, Width, CountFood, coobHW;

struct snakes{
    Uint8 S, body[300];
    int x, y;
} snake;

struct foods{
    Uint8 x, y;
} food[50];

Uint32 times, step;
Uint8 *keys;
SDL_Surface *img;
SDL_Rect rimg;
FILE *config;

inline int InitGame();
inline int InitProgram();
inline int coob(Uint8, Uint8, Uint8);
inline int clear();
inline int divW(int);
inline int divH(int);
inline int GoSnake(struct snakes *);
inline int RespEve();
inline int KeySnake(struct snakes *);
inline int eat(struct snakes *, struct foods *);


int main(){
    printf("begin\n");
    InitProgram();

    while(!bol){
           keys = SDL_GetKeyState(NULL);
        if(SDL_GetTicks() > times){ // Обновление экрана
            //if(SDL_MUSTLOCK(screen)) if(SDL_LockSurface(screen) < 0) return 3;
            times += 10;
            //if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
            SDL_UpdateRect(screen, 0, 0, coobHW * Height, coobHW * Width);
        }

        if(scen == 1){
        }

        if(scen == 2){ // Сцена 1
            if(SDL_GetTicks() > step && !pause){ // Игровой такт
                step += keys[SDLK_SPACE]? speed / 2: speed;

                GoSnake(&snake);
                eat(&snake, &food[0]);
            }
        }

        if(scen == 3){
                clear();
                SDL_BlitSurface(img, NULL, screen, &rimg); // Рисуем картинку на экране
                scen = 0;
        }

        RespEve();
        SDL_Delay(1);
    }
    printf("end\n");
    return 0;
}



inline int InitProgram(){
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Ошибка инициализации SDL.\n");
        return 1;
    }

    config = fopen("config.ini", "r");
    fscanf(config, "%d%d%d%d", &Width, &Height, &coobHW, &CountFood);
    printf("CountFood: %d\n", CountFood);

    atexit(SDL_Quit);

    //-------------------

    screen = SDL_SetVideoMode(coobHW * Height, coobHW * Width, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL){
        printf("Невозможно установить разрешение!\n");
        return 2;
    }


    srand(time(NULL));
    InitGame();

    img = SDL_LoadBMP("GameOver.bmp");
       if(img == NULL) return 3;
    else {
        rimg.x = 0;
            rimg.y = 0;
        rimg.w = img->w; // Высота
        rimg.h = img->h; // Ширина
    }

    return 0;
}

inline int InitGame(){
    bol = 0;
    pause = 0;
    speed = 200;
    scen = 2;

    snake.S = 3;
    snake.x = 5;
    snake.y = 12;
    snake.body[0] = 3;
    Uint16 i;
    for(i = 1; i < snake.S; snake.body[i++] = 3);

    for(i = 0; i < CountFood; i++){
        food[i].x = rand() % Height;
        food[i].y = rand() % Width;
    }

    x = snake.x;
    y = snake.y;

    times = SDL_GetTicks();
    step = SDL_GetTicks() + 300;

    //clear(screen);
    return 0;
}

inline int coob(Uint8 x, Uint8 y, Uint8 c){
    Uint32 color;
    if(!c) color = 0;
    else if(c == 1) color = SDL_MapRGB(screen->format, 255, 0, 0);
    else if(c == 2) color = SDL_MapRGB(screen->format, 0, 255, 0);
    else if(c == 3) color = SDL_MapRGB(screen->format, 0, 0, 255);
    Uint16 i, j;
    Uint32 *buf;
    for(i = x * coobHW + 2; i < x * coobHW + coobHW - 2; i++)
        for(j = y * coobHW + 2; j < y * coobHW + coobHW - 2; j++){
            buf = (Uint32 *) screen->pixels + j * screen->pitch / 4 + i;
            *buf = color;
        }

    return 0;
}

inline int clear(){
    Uint32 color = 0;
    Uint16 i, j;
    Uint32 *buf;
    for(i = 0; i < coobHW * Height; i++) for(j = 0; j < coobHW * Width; j++){
        buf = (Uint32 *) screen->pixels + j * screen->pitch / 4 + i;
        *buf = color;
    }

    return 0;
}

inline int divW(int y){
    if(y >= Width) y %= Width;
    else if(y < 0) y = Width - 1;
    return y;
}

inline int divH(int x){
    if(x >= Height) x %= Height;
    else if(x < 0) x = Height - 1;
    return x;
}

inline int GoSnake(struct snakes *snake){

    KeySnake(snake);

    coob(snake->x, snake->y, 1); // Рисуем голову
    Uint16 i;
    for(i = 1; i < snake->S; i++){

        switch(snake->body[i]){ // Расщёт координатов позвонков
            case 0: y--; break;
            case 1: x++; break;
            case 2: y++; break;
            case 3: x--; break;
        }

        if(x == snake->x && y == snake->y) scen = 3; // Авария
        x = divH(x);
        y = divW(y);
    }
    for(i = snake->S - 1; i > 0; i--) snake->body[i] = snake->body[i - 1];
    coob(x, y, 0); // Стираем хвост

    for(i = 0; i < CountFood; i++){
        coob(food[i].x, food[i].y, 2); // Рисуем еду
    }

    switch(snake->body[0]){  // Ползём
        case 0: snake->y++; break;
        case 1: snake->x--; break;
        case 2: snake->y--; break;
        case 3: snake->x++; break;
    }
    snake->x = divH(snake->x);
    snake->y = divW(snake->y);
    x = snake->x; // Позиция позвонка
    y = snake->y;

    return 0;
}

inline int RespEve(){
    while(SDL_PollEvent(&Event)){
        switch(Event.type){
            case SDL_QUIT: //Exit
                bol = 1;
                break;
            case SDL_KEYDOWN:
                if(Event.key.keysym.sym == SDLK_F3) speed -= speed / 10;
                else if(Event.key.keysym.sym == SDLK_F2 && speed < 500) speed += speed / 10? speed / 10: 1;
                if(Event.key.keysym.sym == SDLK_F4)snake.body[snake.S++] = 3;
                else if(Event.key.keysym.sym == SDLK_F5){
                    if(!pause) pause = 1;
                    else{
                        pause = 0;
                        step = SDL_GetTicks() + speed / 2;
                    }
                }
                else if(Event.key.keysym.sym == SDLK_F6) InitGame();
                break;
        }
    }

    return 0;
}

inline int KeySnake(struct snakes *snake){
    if(Event.key.keysym.sym == SDLK_UP && snake->body[0] != 0) snake->body[0] = 2;
    else if(Event.key.keysym.sym == SDLK_DOWN && snake->body[0] != 2) snake->body[0] = 0;
    else if(Event.key.keysym.sym == SDLK_LEFT && snake->body[0] != 3) snake->body[0] = 1;
    else if(Event.key.keysym.sym == SDLK_RIGHT && snake->body[0] != 1) snake->body[0] = 3;

    return 0;
}

inline int eat(struct snakes *snake, struct foods *food){
    Uint8 i;
    for(i = 0; i < CountFood; i++){
        if(x == (food + i)->x && y == (food + i)->y){ // Если сели
            coob(x, y, 1);
            snake->body[snake->S] = snake->body[snake->S - 1];
            snake->S++;
            printf("S = %d\n", snake->S - 1);
            (food + i)->x = rand() % Height; // Новая еда
            (food + i)->y = rand() % Width; // Новая еда
        }
    }

    return 0;
}
