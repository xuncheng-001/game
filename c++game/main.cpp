#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include <vector>
#include <ctime>
#include <cmath>
//全局变量，用于检测是否结束了游戏
bool game_finished = false;

//碰撞检测
bool checkCollision(SDL_FRect rect1, SDL_FRect rect2)
{
    return (rect1.x < rect2.x + rect2.w &&
            rect1.x + rect1.w > rect2.x &&
            rect1.y < rect2.y + rect2.h &&
            rect1.y + rect1.h > rect2.y);
}
class Plant
{
private:
    SDL_Texture *texture;
    SDL_Texture *FireTexture_;
    SDL_FRect rect;
    SDL_FRect FireTexture;
public:
    //攻击冷却
    float fire_nettime=0;
    //是否已经攻击
    bool is_firing = false;
    //豌豆射手中子弹的发射方向
    bool firing_facing = true;

    int type_;
    int health_;
    bool wudi=false;
    float wuditime=0;
    float fire_range;
    Plant(SDL_Renderer *renderer, const char* imagePath,
        const char* fire_imagePath ,int x, int y, int w, int h,int health,int type)
    {
        texture = IMG_LoadTexture(renderer, imagePath);
        FireTexture_ = IMG_LoadTexture(renderer, fire_imagePath);
        health_ = health;
        type_ = type;
        rect.x = static_cast<float>(x);
        rect.y = static_cast<float>(y);
        rect.w = static_cast<float>(w);
        rect.h = static_cast<float>(h);
    }
    SDL_FRect getRect() const
    {
        return rect;
    }
    SDL_FRect getFireRect() const
    {
        return FireTexture;
    }
    ~Plant()
    {
        SDL_DestroyTexture(texture);
    }
    void render(SDL_Renderer *renderer)
    {
        SDL_RenderCopyF(renderer, texture, nullptr, &rect);
        //攻击过程就渲染攻击特效
        if (is_firing)
        {
            SDL_RenderCopyF(renderer, FireTexture_, nullptr, &FireTexture);
        }
    }
    void takeDamage(int damage)
    {
        if (!wudi)
        {
            if (health_ >=0)
            {
                health_ -= damage;
                wudi=true;
                wuditime=0.3;
            }
            if (health_ <= 0)
            {
                health_=0;
            }
        }
    }
    void WuDitimeing(float nettime)
    {
        if (wuditime>0)
        {
            wuditime-=nettime;
        }
        if (wuditime<=0)
        {
            wuditime=0;
            wudi=false;
        }

    }
    //用于检测是否在攻击范围内,给豌豆射手攻击用的(戴夫躲避也可以使用)
    bool Inrange(const SDL_FRect player_rect)const
    {
        float x=player_rect.x+player_rect.w/2-(rect.x+rect.w/2);
        float y=player_rect.y+player_rect.h/2-(rect.y+rect.h/2);
        float distance=std::sqrt(x*x+y*y);
        return distance < fire_range;
    }
    void recover(Plant* plants,float recoverd_health)
    {
            plants->health_+=recoverd_health;
    }
    //不同的植物类型又有不同的特性
   void update(float nettime,const SDL_FRect player_rect,std::vector<Plant*> plants)
    {
        bool left=player_rect.x+player_rect.w/2<rect.x+rect.w/2;
        if (fire_nettime>0)
        {
            fire_nettime-=nettime;
        }
        if (fire_nettime<=0)
        {
            fire_nettime=0;
            is_firing=false;
        }
        //植物类型，0为豌豆射手
        if (type_==0)
        {
            fire_range=200;


            if (Inrange(player_rect) && fire_nettime==0)
            {
                if (left)
                {
                    FireTexture.w=20;
                    FireTexture.x=rect.x-FireTexture.w/2;
                    FireTexture.y=rect.y;
                    FireTexture.h=20;
                    firing_facing=true;
                }
                else
                {
                    FireTexture.w=20;
                    FireTexture.x=rect.x+FireTexture.w/2+rect.w;
                    FireTexture.y=rect.y;
                    FireTexture.h=20;
                    firing_facing=false;
                }
                is_firing=true;
                fire_nettime=10;
            }
            if (is_firing)
            {
                if (firing_facing)
                {
                    FireTexture.x-=100*nettime;
                }
                else
                {
                    FireTexture.x+=100*nettime;
                }
            }
        }
        //植物类型，1为向日葵
        if (type_==1)
        {
            if (health_>0 && fire_nettime==0)
            {
                fire_nettime=2;
                for (auto plant : plants)
                {
                    recover(plant,10);
                }
            }
        }
        //懒得写多一个类，就把戴夫和推车放在这里了
        //植物类型，2为戴夫
        if (type_==2)
        {
            fire_range=200;
            if (health_==0)
            {
                game_finished=true;
            }
            int random=rand()%3;
            if (random==0)
            {
                rect.x-=30*nettime;
            }
            if (random==1)
            {
                rect.x+=30*nettime;
            }
        }

    }

};
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
    SDL_Texture *texture_right;
    SDL_Texture *walktexture;
    SDL_Texture *walktexture_right;
    SDL_Texture *zhuahentexture;
    SDL_FRect rect;
    public:
    //地面选择(false为下地面)
    bool ground_chosen=false;
    //地面高度
    float ground_height ;
    float ground_height2;
    float time=0;
    //面向右还是左
    bool face=false;
    //是否按j键
    bool pin_a=false;
    //走路
    bool onfoot=false;
    //血量和无敌状态,剩余的无敌时间
    int health=100;
    bool wudi=false;
    float wuditime=0;
    //攻击范围
    SDL_FRect F_rect;
    //满足跳跃的参数
    float speed = 0.03;
    float gravity = 800;
    float jumpSpeed = 600;
    float H_speed = 0;
    bool jumping = false;
    Player(SDL_Renderer *renderer, const char* imagePath,
        const char* right_imagePath,
        const char* walkimagePath,
        const char* walkimagePath_right,
        const char* zhuahenimagePath,
        int x, int y, int w, int h)
    {
        texture = IMG_LoadTexture(renderer, imagePath);
        texture_right = IMG_LoadTexture(renderer, right_imagePath);
        walktexture = IMG_LoadTexture(renderer, walkimagePath);
        walktexture_right = IMG_LoadTexture(renderer, walkimagePath_right);
        zhuahentexture = IMG_LoadTexture(renderer, zhuahenimagePath);
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
    void movement(const Uint8 *keyboardState,float nettime, std::vector<Plant*>& plants)
    {
        float oldx = rect.x;
        float oldy = rect.y;
        //平a的伤害范围
        F_rect.x= rect.x + rect.w;
        F_rect.y= rect.y ;
        F_rect.w= 10;
        F_rect.h= rect.h;
        if (!keyboardState[SDL_SCANCODE_J])
        {
            pin_a = false;
        }
        if (!keyboardState[SDL_SCANCODE_A] && !keyboardState[SDL_SCANCODE_D])
        {
            onfoot=false;
        }
        //无敌时间和无敌状态
        if (wuditime>0)
        {
            wuditime-=nettime;
            wudi=true;
        }
        else
        {
            wudi=false;
            wuditime=0;
        }
        if (keyboardState[SDL_SCANCODE_A])
        {
            onfoot=true;
            face=false;
            rect.x -= speed;
        }
        if (keyboardState[SDL_SCANCODE_D])
        {
            onfoot=true;
            face=true;
            rect.x += speed;
        }
        if (keyboardState[SDL_SCANCODE_W] && !jumping)
        {
            //只在非跳跃状态下跳跃，防止二段跳
            jumping = true;
            H_speed = -jumpSpeed;
        }

        if (keyboardState[SDL_SCANCODE_J])
        {
            pin_a = true;
            for (auto& plant : plants)
            {

                if (checkCollision(F_rect,plant->getRect()))
                {
                    plant->takeDamage(10);

                }
            }
        }
        H_speed += gravity * nettime;
        rect.y += H_speed * nettime;

        //碰撞检测
        for (const auto& plant : plants)
        {
            if (checkCollision(rect, plant->getRect()))
            {
                rect.x = oldx;
                rect.y = oldy;
                jumping = false;
                H_speed = 0;
                if (!wudi)
                {
                    if (health>0)
                    {
                        health-=10;
                        wudi=true;
                        wuditime=1;
                    }
                    if (health<=0)
                    {
                        health=0;
                    }
                }
            }
            if (checkCollision(rect,plant->getFireRect()))
            {
                if (!wudi)
                {
                    if (health>0)
                    {
                        health-=10;
                        wudi=true;
                        wuditime=1;
                    }
                    if (health<=0)
                    {
                        health=0;
                    }
                }
            }
        }

        //不同的地面高度
        if (rect.x >=0 && rect.x<200)
        {
            ground_height= 820-0.7*rect.x;
        }
        if (rect.x>200 && rect.x<960)
        {
            ground_height= 680;
        }
        if (rect.x>=960 && rect.x<1160)
        {
            ground_height= 680+(rect.x-960)*0.8;
        }
        //上层的地面高度
        if (rect.x>=0 && rect.x<=180)
        {
            ground_height2= 200;
        }
        if (rect.x>180 && rect.x<580)
        {
            ground_height2= 200+(rect.x-180)*0.8;
        }
        if (rect.x>=940 && rect.x<1140)
        {
            ground_height2= 520-(rect.x-940)*0.8;
        }
        if (rect.x>=1140 )
        {
            ground_height2= 360;
        }


        if (rect.y<ground_height2)
        {
            ground_chosen=true;
        }
        if (keyboardState[SDL_SCANCODE_S])
        {
            ground_chosen =false;
        }

        //当玩家触地时，重置跳跃状态和垂直速度
        if (!ground_chosen)
        {
            if (rect.y >= ground_height)
            {
                rect.y = ground_height;
                jumping = false;
                H_speed = 0;
            }
        }
        else
        {
            if (rect.y >= ground_height2)
            {
                rect.y = ground_height2;
                jumping = false;
                H_speed = 0;
            }
        }

    }
    void render(SDL_Renderer *renderer,float nettime)
    {
        if (onfoot)
        {
            if (face)
            {
                if (time <= 0.2)
                {
                    SDL_RenderCopyF(renderer,texture_right,nullptr,&rect);
                    time += nettime;
                }
                if (time < 0.4 && time >= 0.2)
                {
                    SDL_RenderCopyF(renderer, walktexture_right, nullptr, &rect);
                    time += nettime;
                }
                if (time >=0.4)
                {
                    time = 0;
                }
            }
            else
            {
                if (time <= 0.2)
                {
                    SDL_RenderCopyF(renderer,texture,nullptr,&rect);
                    time += nettime;
                }
                if (time < 0.4 && time >= 0.2)
                {
                    SDL_RenderCopyF(renderer, walktexture, nullptr, &rect);
                    time += nettime;
                }
                if (time >=0.4)
                {
                    time = 0;
                }
            }

        }
        else
        {
            if (face)
            {
                SDL_RenderCopyF(renderer, texture_right, nullptr, &rect);
            }
            else
            {
                SDL_RenderCopyF(renderer, texture, nullptr, &rect);
            }

        }
        if (pin_a)
        {
            SDL_RenderCopyF(renderer, zhuahentexture, nullptr, &F_rect);
        }


    }

};


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    //创建游戏窗口
    IMG_Init(IMG_INIT_PNG);
    SDL_Window *window = SDL_CreateWindow("僵尸大战植物",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1920,1080,0);
    //创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED);
    //加载背景图片
    SDL_Texture* background1 = IMG_LoadTexture(renderer, "/home/xuncheng/game/c++game/photo/back1.png");
    //新的背景图片
    SDL_Texture* background2 = nullptr;
    //加载按钮图片
    Button button(renderer, "/home/xuncheng/game/c++game/photo/button1.png", 220, 200, 200, 80);
    //加载玩家图片
    Player player(renderer, "/home/xuncheng/game/c++game/photo/player2.png",
        "/home/xuncheng/game/c++game/photo/player2_right2.png",
        "/home/xuncheng/game/c++game/photo/walk2.png",
        "/home/xuncheng/game/c++game/photo/walk2_right.png",
        "/home/xuncheng/game/c++game/photo/zhuahen.png" ,
        200, 500, 100, 100);
    //加载植物图片
    Plant plant1(renderer, "/home/xuncheng/game/c++game/photo/plant1.png",
        "/home/xuncheng/game/c++game/photo/back2.png",1140, 400, 50, 50,50,0);
    Plant plant2(renderer, "/home/xuncheng/game/c++game/photo/plant1.png",
        "/home/xuncheng/game/c++game/photo/back2.png",400, 300, 50, 50,50,1);
    Plant daifu(renderer, "/home/xuncheng/game/c++game/photo/plant1.png",
        "/home/xuncheng/game/c++game/photo/back2.png",300, 680, 50, 50,50,2);
    //用于判断是否点击按钮
    bool buttonClicked = false;

    bool running = true;
    //SDL 事件对象，用于接收用户输入
    SDL_Event event;
    //时间计算中的上一帧
    Uint32 lastTime = 0;
    //植物总和
    std::vector<Plant*> plants;
    //将植物加入到数组里面
    plants.push_back(&plant1);
    plants.push_back(&plant2);
    plants.push_back(&daifu);
    while (running)
    {

        //引入时间，使玩家跳跃更自然
        Uint32 currentTime = SDL_GetTicks();
        float nettime = (currentTime - lastTime)/1000.0f;
        lastTime = currentTime;

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
                    background2 = IMG_LoadTexture(renderer, "/home/xuncheng/game/c++game/photo/game_back1.png");
                }

            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        if (buttonClicked)
        {
            player.movement(SDL_GetKeyboardState(nullptr),nettime,plants);
            //更新植物无敌时间数据
            for (auto& plant : plants)
            {
                plant->WuDitimeing(nettime);
                plant->update(nettime,player.F_rect,plants);
            }

            SDL_RenderCopy(renderer, background2, nullptr, nullptr);
            plant1.render(renderer);
            plant2.render(renderer);
            daifu.render(renderer);
            player.render(renderer,nettime);
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