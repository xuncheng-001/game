#include <iostream>
#include "SDL.h"
#include "SDL_image.h"

class Button {
    private:
    SDL_Texture *texture;
    SDL_FRect rect;
    public:
    //接受图片路径，位置，大小
    Button(SDL_Renderer *renderer,const char* imagePath, int x, int y, int w, int h)
    {
        //加载图片,设置按钮的位置和大小
        texture = IMG_LoadTexture(renderer, imagePath);
        if (!texture)
        {
            std::cerr << "Failed to load button image" << std::endl;
            return;
        }
        rect.x = static_cast<float>(x);
        rect.y = static_cast<float>(y);
        rect.w = static_cast<float>(w);
        rect.h = static_cast<float>(h);
    }
    ~Button()
    {
        SDL_DestroyTexture(texture);
    }
    //返回是否点击成功
    bool click(double x, double y)
    {
        return (x >= rect.x && x <= rect.x + rect.w &&
                y >= rect.y && y <= rect.y + rect.h);
    }
    void render(SDL_Renderer *renderer)
    {
        SDL_RenderCopyF(renderer, texture, nullptr, &rect);
    }
    SDL_Texture *getTexture()const
    {
        return texture;
    }
};

class Player {
    private:
    SDL_Texture *texture;
    SDL_FRect rect;
    public:
    float speed = 0.01;
    Player(SDL_Renderer *renderer, const char* imagePath, int x, int y, int w, int h)
    {
        texture = IMG_LoadTexture(renderer, imagePath);
        if (!texture)
        {
            std::cerr << "Failed to load player image" << std::endl;
            return;
        }
        rect.x = static_cast<float>(x);
        rect.y = static_cast<float>(y);
        rect.w = static_cast<float>(w);
        rect.h = static_cast<float>(h);
    }
    ~Player()
    {
        SDL_DestroyTexture(texture);
    }
    //键盘控制移动和动作
    void movement(const Uint8 *keyboardState)
    {
        if (keyboardState[SDL_SCANCODE_A])
        {
            rect.x -= speed;
        }
        if (keyboardState[SDL_SCANCODE_D])
        {
            rect.x += speed;
        }
    }
    void render(SDL_Renderer *renderer)
    {
        SDL_RenderCopyF(renderer, texture, nullptr, &rect);
    }

};


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    //创建游戏窗口
    IMG_Init(IMG_INIT_PNG);
    if (!IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)
    {
        std::cerr << "Failed to initialize image" << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("僵尸大战植物",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,0);
    //创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Failed to create renderer" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //加载背景图片
    SDL_Texture* background1 = IMG_LoadTexture(renderer, "/home/xuncheng/game/c++game/photo/back1.png");
    if (!background1)
    {
        std::cerr << "Failed to load background" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //新的背景图片
    SDL_Texture* background2 = nullptr;
    //加载按钮图片
    Button button(renderer, "/home/xuncheng/game/c++game/photo/button1.png", 220, 200, 200, 80);
    //加载玩家图片
    Player player(renderer, "/home/xuncheng/game/c++game/photo/player.png", 50, 300, 50, 50);

    //用于判断是否点击按钮
    bool buttonClicked = false;

    bool running = true;
    //SDL 事件对象，用于接收用户输入
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (button.click(mouseX, mouseY))
                {
                    buttonClicked = true;
                    background2 = IMG_LoadTexture(renderer, "/home/xuncheng/game/c++game/photo/fireback.png");
                }

            }
        }

        player.movement(SDL_GetKeyboardState(nullptr));



        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        if (buttonClicked)
        {
            SDL_RenderCopy(renderer, background2, nullptr, nullptr);
            player.render(renderer);
        }
        else
        {
            SDL_RenderCopy(renderer, background1, nullptr, nullptr);
            button.render(renderer);
        }




        //更新渲染器
        SDL_RenderPresent(renderer);
    }



    //清理资源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}