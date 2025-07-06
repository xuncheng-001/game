#include <iostream>
#include <SDL3/SDL.h>
#define SDL_IMAGE_USE_COMMON_OPENGL_HEADERS
#include <SDL3_image/SDL_image.h>

class Button {
    private:
    SDL_Texture *texture;
    SDL_FRect rect;
    public:
    //接受图片路径，位置，大小
    Button(SDL_Renderer *renderer,const char* imagePath, int x, int y, int w, int h)
    {
        if (!texture)
        {
            std::cerr << "Failed to load button image" << std::endl;
            return;
        }

        //加载图片,设置按钮的位置和大小
        texture = IMG_LoadTexture(renderer, imagePath);
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
        SDL_RenderTexture(renderer, texture, nullptr, &rect);
    }
};
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    //创建游戏窗口
    SDL_Window *window = SDL_CreateWindow("僵尸大战植物",640, 480, 0);
    //创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Failed to create renderer" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* background = IMG_LoadTexture(renderer, "button.png");
    if (!background)
    {
        std::cerr << "Failed to load background" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //加载图片
    Button button(renderer, "photo/button.png", 220, 200, 200, 80);


    bool running = true;
    //SDL 事件对象，用于接收用户输入
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

            }
        }

        SDL_RenderTexture(renderer, background, nullptr, nullptr);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        button.render(renderer);
        SDL_RenderPresent(renderer);
    }



    //清理资源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}