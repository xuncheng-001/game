#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include <vector>
#include <ctime>
#include <cmath>
//全局变量，用于检测是否结束了游戏
bool game_finished = false;
bool game_failed = false;
std::string daifuxueliang;

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
    SDL_Texture *Firerect_;
    SDL_Texture *diafu_walkTexture_;
    SDL_FRect rect;
    SDL_FRect Firerect;
public:
    //戴夫的小推车
    int fire_random;
    float ground_tuiche;
    float tuiche_h_speed;
    float tuiche_time=0;
    //戴夫走路的图像定时器
    float daifu_walk = 0;
    bool daifu_walking = false;
    int jump_random=0;
    //地面选择(false为下地面)
    bool ground_chosen=false;
    float jumping_randomtime=0;
    //地面高度
    float ground_height ;
    float ground_height2;
    float speed = 0.03;
    float gravity = 800;
    float jumpSpeed = 600;
    float H_speed = 0;
    bool jumping = false;
    //daifu随机结果的持续时间
    float random_time=0;
    float random=0;
    //攻击冷却
    float fire_nettime=0;
    //是否已经攻击
    bool is_firing = false;
    //豌豆射手中子弹的发射方向
    bool firing_facing = true;

    int type_;
    int health_;
    int health_max;
    bool wudi=false;
    float wuditime=0;
    float fire_range;
    Plant(SDL_Renderer *renderer, const char* imagePath,
        const char* fire_imagePath ,
        const char* daifu_walk_imagePath,
        int x, int y, int w, int h,int health,int type)
    {
        texture = IMG_LoadTexture(renderer, imagePath);
        Firerect_ = IMG_LoadTexture(renderer, fire_imagePath);
        diafu_walkTexture_ = IMG_LoadTexture(renderer, daifu_walk_imagePath);
        health_ = health;
        health_max = health;
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
        return Firerect;
    }
    ~Plant()
    {
        SDL_DestroyTexture(texture);
    }
    void render(SDL_Renderer *renderer,float nettime)
    {

        //daifu独有的移动特效(顺便给豌豆和向日葵的切换图片用)
        daifu_walk += nettime;
        if (daifu_walk >= 0.15 &&daifu_walk <= 0.3)
        {
            daifu_walking = true;
        }
        if (daifu_walk>=0 && daifu_walk<0.15)
        {
            daifu_walking = false;
        }
        if (daifu_walk>0.3)
        {
            daifu_walk=0;
        }
        if (type_ == 2)
        {
            if (daifu_walking)
            {
                SDL_RenderCopyF(renderer, diafu_walkTexture_, nullptr, &rect);
            }
            else
            {
                SDL_RenderCopyF(renderer, texture, nullptr, &rect);
            }

            SDL_RenderCopyF(renderer, Firerect_, nullptr, &Firerect);
        }
        else
        {
            if (daifu_walking)
            {
                SDL_RenderCopyF(renderer, diafu_walkTexture_, nullptr, &rect);
            }
            else
            {
                SDL_RenderCopyF(renderer, texture, nullptr, &rect);
            }
            //攻击过程就渲染攻击特效
            if (is_firing)
            {
                SDL_RenderCopyF(renderer, Firerect_, nullptr, &Firerect);
            }
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
                wuditime=1;
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
    //用于检测是否在攻击范围内,给豌豆射手攻击用的
    bool Inrange(const SDL_FRect player_rect)const
    {
        float x=player_rect.x+player_rect.w/2-(rect.x+rect.w/2);
        float y=player_rect.y+player_rect.h/2-(rect.y+rect.h/2);
        float distance=std::sqrt(x*x+y*y);
        return distance < fire_range;
    }
    //给向日葵回血用的
    void recover(Plant* plants,float recoverd_health)
    {
        plants->health_+=recoverd_health;
        if (plants->health_>=plants->health_max)
        {
            plants->health_=plants->health_max;
        }
    }
    //不同的植物类型又有不同的特性
   void update(float nettime,const SDL_FRect player_rect,std::vector<Plant*> plants)
    {
        tuiche_time +=nettime;
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
                    Firerect.w=20;
                    Firerect.x=rect.x-Firerect.w/2;
                    Firerect.y=rect.y+40;
                    Firerect.h=20;
                    firing_facing=true;
                }
                else
                {
                    Firerect.w=20;
                    Firerect.x=rect.x+Firerect.w/2+rect.w;
                    Firerect.y=rect.y+40;
                    Firerect.h=20;
                    firing_facing=false;
                }
                is_firing=true;
                fire_nettime=10;
            }
            if (is_firing)
            {
                if (firing_facing)
                {
                    Firerect.x-=100*nettime;
                }
                else
                {
                    Firerect.x+=100*nettime;
                }
            }
        }
        //植物类型，1为向日葵
        if (type_==1)
        {
            if (health_>0 && fire_nettime==0)
            {
                fire_nettime=1;
                for (auto plant : plants)
                {
                    recover(plant,1);
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
            if (random_time==0)
            {
                random=rand()%2;
            }
            random_time+=nettime;
            if (random_time>2.5)
            {
                random_time=0;
            }
            if (random==0)
            {
                rect.x-=300*nettime;
            }
            if (random==1)
            {
                rect.x+=500*nettime;
            }
            if (rect.x<0)
            {
                rect.x=0;
            }
            if (rect.x>1870)
            {
                rect.x=1870;
            }
            H_speed += nettime*gravity;
            rect.y += H_speed * nettime;


            //下层的地面高度
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

            jumping_randomtime+=nettime;
            if (jumping_randomtime==0)
            {
                 jump_random=rand()%3;
            }
            if (jumping_randomtime>5)
            {
                jumping_randomtime=0;
            }
            if (jumping_randomtime>0)
            {
                jumping=true;
            }
            if (jump_random==1)
            {
                ground_chosen=false;
            }
            if (rect.y<ground_height2 && jump_random!=1)
            {
                ground_chosen=true;
            }
            if (jump_random==0 && !jumping)
            {
                H_speed-=jumpSpeed;
                jumping=true;
            }
            //戴夫的小推车
            if (tuiche_time>=5)
            {
                fire_random=rand()%2;
                tuiche_time=0;
                if (fire_random==0)
                {
                    tuiche_h_speed=0;

                    Firerect.w=50;
                    Firerect.x=0;
                    Firerect.y=200;
                    Firerect.h=50;


                }
                if (fire_random==1)
                {
                    tuiche_h_speed=0;

                    Firerect.w=50;
                    Firerect.x=0;
                    Firerect.y=820;
                    Firerect.h=50;


                }

            }

            if (fire_random==0)
            {
                if (Firerect.x>=0 && Firerect.x<=180)
                {
                    ground_tuiche= 200+50;
                }
                if (Firerect.x>180 && Firerect.x<580)
                {
                    ground_tuiche= 200+(Firerect.x-180)*0.8+50;
                }
                if (Firerect.x>=940 && Firerect.x<1140)
                {
                    ground_tuiche= 520-(Firerect.x-940)*0.8+50;
                }
                if (Firerect.x>=1140 )
                {
                    ground_tuiche= 360+50;
                }
            }
            if (fire_random==1)
            {
                if (Firerect.x >=0 && Firerect.x<200)
                {
                    ground_tuiche= 820-0.7*Firerect.x+50;
                }
                if (Firerect.x>200 && Firerect.x<960)
                {
                    ground_tuiche= 680+50;
                }
                if (Firerect.x>=960 && Firerect.x<1160)
                {
                    ground_tuiche= 680+(Firerect.x-960)*0.8+50;
                }
            }
            tuiche_h_speed+=nettime*gravity;
            Firerect.x+=400*nettime;
            fire_nettime+=nettime;
            Firerect.y+= tuiche_h_speed*nettime;
            if (Firerect.y>ground_tuiche)
            {
                Firerect.y=ground_tuiche;
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
class xueliang
{
    private:
    SDL_Texture *texture;
    SDL_FRect rect;
    public:
    xueliang(SDL_Renderer *renderer,const char* imagePath)
    {
        texture = IMG_LoadTexture(renderer, imagePath);
        rect.x = 0;
        rect.y = 100;
        rect.w = 400;
        rect.h = 50;
    }
    ~xueliang()
    {
        SDL_DestroyTexture(texture);
    }
    void render(SDL_Renderer *renderer)
    {
        SDL_RenderCopyF(renderer, texture, nullptr, &rect);
    }
};
class Player {
    private:
    SDL_Texture *texture;
    SDL_Texture *texture_right;
    SDL_Texture *walktexture;
    SDL_Texture *walktexture_right;
    SDL_Texture *zhuahentexture;
    SDL_Texture *pin_atexture;
    SDL_Texture *healthtexture1;
    SDL_Texture *healthtexture2;
    SDL_Texture *healthtexture3;
    SDL_Texture *healthtexture4;
    SDL_Texture *healthtexture5;
    SDL_Texture *healthtexture6;
    SDL_Texture *healthtexture7;
    SDL_Texture *healthtexture8;
    SDL_FRect rect;
    public:
    //闪烁的时间
    float lighting_time=0;
    bool is_lighting=false;
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
    int health=80;
    bool wudi=false;
    float wuditime=0;
    //攻击范围
    SDL_FRect F_rect;
    //生命值
    SDL_FRect health_rect;
    //满足跳跃的参数
    float speed = 0.2;
    float gravity = 800;
    float jumpSpeed = 600;
    float H_speed = 0;
    bool jumping = false;
    Player(SDL_Renderer *renderer, const char* imagePath,
        const char* right_imagePath,
        const char* walkimagePath,
        const char* walkimagePath_right,
        const char* zhuahenimagePath,
        const char* pin_aimagePath,
        const char* healthimagePath1,
        const char* healthimagePath2,
        const char* healthimagePath3,
        const char* healthimagePath4,
        const char* healthimagePath5,
        const char* healthimagePath6,
        const char* healthimagePath7,
        const char* healthimagePath8,
        int x, int y, int w, int h)
    {
        texture = IMG_LoadTexture(renderer, imagePath);
        texture_right = IMG_LoadTexture(renderer, right_imagePath);
        walktexture = IMG_LoadTexture(renderer, walkimagePath);
        walktexture_right = IMG_LoadTexture(renderer, walkimagePath_right);
        zhuahentexture = IMG_LoadTexture(renderer, zhuahenimagePath);
        pin_atexture= IMG_LoadTexture(renderer, pin_aimagePath);
        healthtexture1 = IMG_LoadTexture(renderer, healthimagePath1);
        healthtexture2 = IMG_LoadTexture(renderer, healthimagePath2);
        healthtexture3 = IMG_LoadTexture(renderer, healthimagePath3);
        healthtexture4 = IMG_LoadTexture(renderer, healthimagePath4);
        healthtexture5 = IMG_LoadTexture(renderer, healthimagePath5);
        healthtexture6 = IMG_LoadTexture(renderer, healthimagePath6);
        healthtexture7 = IMG_LoadTexture(renderer, healthimagePath7);
        healthtexture8 = IMG_LoadTexture(renderer, healthimagePath8);
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
        if (face)
        {
            F_rect.x= rect.x + rect.w;
            F_rect.y= rect.y ;
            F_rect.w= 60;
            F_rect.h= rect.h;
        }
        else
        {
            F_rect.x= rect.x - 60;
            F_rect.y= rect.y ;
            F_rect.w= 60;
            F_rect.h= rect.h;
        }
        //状态判断
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
        //边框限制
        if (rect.x<0)
        {
            rect.x=0;
        }
        if (rect.x>1870)
        {
            rect.x=1870;
        }

    }
    void render(SDL_Renderer *renderer,float nettime)
    {
        if (wudi)
        {
            lighting_time += nettime;
            // 每 0.1 秒切换一次闪烁状态
            is_lighting = static_cast<int>(lighting_time * 10) % 2 == 1;
            if (lighting_time >= 0.2)
            {
                lighting_time = 0;
            }
        }
        else
        {
            is_lighting = false;
        }
        if (!is_lighting)
        {
            SDL_Texture* currentTexture = nullptr;
            if (onfoot)
            {
                if (face)
                {
                    if (time <= 0.2)
                    {
                        currentTexture = texture_right;
                    }
                    else
                    {
                        currentTexture = walktexture_right;
                    }
                }
                else
                {
                    if (time <= 0.2)
                    {
                        currentTexture = texture;
                    }
                    else
                    {
                        currentTexture = walktexture;
                    }
                }
                time += nettime;
                if (time >= 0.4)
                {
                    time = 0;
                }
            }
            else
            {
                currentTexture = face ? texture_right : texture;
            }

            if (currentTexture)
            {
                SDL_RenderCopyF(renderer, currentTexture, nullptr, &rect);
            }

            if (pin_a)
            {
                SDL_RenderCopyF(renderer, zhuahentexture, nullptr, &F_rect);
            }
        }
        switch (health)
        {
            case 10:
                SDL_RenderCopyF(renderer, healthtexture1, nullptr, &health_rect);
                break;
            case 20:
                SDL_RenderCopyF(renderer, healthtexture2, nullptr, &health_rect);
                break;
            case 30:
                SDL_RenderCopyF(renderer, healthtexture3, nullptr, &health_rect);
                break;
            case 40:
                SDL_RenderCopyF(renderer, healthtexture4, nullptr, &health_rect);
                break;
            case 50:
                SDL_RenderCopyF(renderer, healthtexture5, nullptr, &health_rect);
                break;
            case 60:
                SDL_RenderCopyF(renderer, healthtexture6, nullptr, &health_rect);
                break;
            case 70:
                SDL_RenderCopyF(renderer, healthtexture7, nullptr, &health_rect);
                break;
            case 80:
                SDL_RenderCopyF(renderer, healthtexture8, nullptr, &health_rect);
                break;
        }

    }

};


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    //创建游戏窗口
    IMG_Init(IMG_INIT_PNG);
    //不知道为什么用相对路径无法显示图片，查到可以使用这个函数来解决
    char* basePath = SDL_GetBasePath();
    std::string basePathStr = basePath;
    SDL_free(basePath);
    std::string buttonPath = basePathStr + "photo/button1.png";
    std::string back1Path = basePathStr + "photo/back1.png";
    std::string game_back1Path = basePathStr + "photo/game_back1.png";
    std::string player2Path = basePathStr + "photo/player2.png";
    std::string player2Path2 = basePathStr + "photo/player2_right2.png";
    std::string walk2Path = basePathStr + "photo/walk2.png";
    std::string walk2_rightPath = basePathStr + "photo/walk2_right.png";
    std::string zhuahenPath = basePathStr + "photo/zhuahen.png";
    std::string gongji_leftPath = basePathStr + "photo/gongji_left.png";
    std::string health1Path = basePathStr + "photo/health1.png";
    std::string health2Path = basePathStr + "photo/health2.png";
    std::string health3Path = basePathStr + "photo/health3.png";
    std::string health4Path = basePathStr + "photo/health4.png";
    std::string health5Path = basePathStr + "photo/health5.png";
    std::string health6Path = basePathStr + "photo/health6.png";
    std::string health7Path = basePathStr + "photo/health7.png";
    std::string health8Path = basePathStr + "photo/health8.png";

    std::string plant1Path = basePathStr + "photo/plant1.png";
    std::string wandousheshou1Path = basePathStr + "photo/wandousheshou1.png";
    std::string wandousheshou2Path = basePathStr + "photo/wandousheshou2.png";
    std::string xiangrikui1Path = basePathStr + "photo/xiangrikui1.png";
    std::string xiangrikui2Path = basePathStr + "photo/xiangrikui2.png";
    std::string wandouPath = basePathStr + "photo/wandou.png";
    std::string daifu_walkPath = basePathStr + "photo/daifu_walk.png";
    std::string tuichePath = basePathStr + "photo/tuiche.png";
    std::string daifuPath = basePathStr + "photo/daifu2.png";

    std::string daifuhealth1 = basePathStr + "photo/daifu1health.png";
    std::string daifuhealth2 = basePathStr + "photo/daifu2health.png";
    std::string daifuhealth3 = basePathStr + "photo/daifu3health.png";
    std::string daifuhealth4 = basePathStr + "photo/daifu4health.png";
    std::string daifuhealth5 = basePathStr + "photo/daifu5health.png";
    std::string daifuhealth6 = basePathStr + "photo/daifu6health.png";
    std::string daifuhealth7 = basePathStr + "photo/daifu7health.png";
    std::string daifuhealth8 = basePathStr + "photo/daifu8health.png";

    std::string gameoverPath = basePathStr + "photo/gameover.png";
    std::string gamefailPath = basePathStr + "photo/gamefail.png";
    //初始化随机数种子
    srand(static_cast<unsigned int>(time(NULL)));
    SDL_Window *window = SDL_CreateWindow("僵尸大战植物",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1920,1080,0);
    //创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED);
    //加载背景图片
    SDL_Texture* background1 = IMG_LoadTexture(renderer, back1Path.c_str());
    //新的背景图片
    SDL_Texture* background2 = nullptr;
    //加载按钮图片
    Button button(renderer, buttonPath.c_str(), 220, 200, 200, 80);
    //加载玩家图片
    Player player(renderer, player2Path.c_str(),
        player2Path2.c_str(),
        walk2Path.c_str(),
        walk2_rightPath.c_str(),
        zhuahenPath.c_str() ,
        gongji_leftPath.c_str(),
        health1Path.c_str(),
        health2Path.c_str(),
        health3Path.c_str(),
        health4Path.c_str(),
        health5Path.c_str(),
        health6Path.c_str(),
        health7Path.c_str(),
        health8Path.c_str(),
        1000, 680, 100, 100);
    //加载植物图片
    Plant wandousheshou1(renderer, wandousheshou1Path.c_str(),
        wandouPath.c_str(),
        wandousheshou2Path.c_str(),1300, 360, 100, 100,100,0);
    Plant wandousheshou2(renderer, wandousheshou2Path.c_str(),
        wandouPath.c_str(),
        wandousheshou1Path.c_str(),300, 680, 100, 100,100,0);
    Plant xiangrikui(renderer, xiangrikui1Path.c_str(),
        wandouPath.c_str(),
        xiangrikui2Path.c_str(),100, 200, 100, 100,50,1);
    Plant daifu(renderer, daifuPath.c_str(),
        tuichePath.c_str(),
        daifu_walkPath.c_str(),1000, 0, 100, 100,80,2);
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
    plants.push_back(&wandousheshou1);
    plants.push_back(&wandousheshou2);
    plants.push_back(&xiangrikui);
    plants.push_back(&daifu);
    //血量的位置
    player.health_rect.x=0;
    player.health_rect.y=50;
    player.health_rect.w=400;
    player.health_rect.h=50;


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
                    background2 = IMG_LoadTexture(renderer, game_back1Path.c_str());
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {

                    running = false;
                }
            }
        }
        if (daifu.health_<=10 && daifu.health_>=0)
        {
            daifuxueliang=daifuhealth1;
        }
        else if (daifu.health_<=20 && daifu.health_>=10)
        {
            daifuxueliang=daifuhealth2;
        }
        else if (daifu.health_<=30 && daifu.health_>=20)
        {
            daifuxueliang=daifuhealth3;
        }
        else if (daifu.health_<=40 && daifu.health_>=30)
        {
            daifuxueliang=daifuhealth4;
        }
        else if (daifu.health_<=50 && daifu.health_>=40)
        {
            daifuxueliang=daifuhealth5;
        }
        else if (daifu.health_<=60 && daifu.health_>=50)
        {
            daifuxueliang=daifuhealth6;
        }
        else if (daifu.health_<=70 && daifu.health_>=60)
        {
            daifuxueliang=daifuhealth7;
        }
        else if (daifu.health_<=80 && daifu.health_>=70)
        {
            daifuxueliang=daifuhealth8;
        }
        else if (daifu.health_<=0)
        {
            game_finished=true;
        }
        if (player.health <=0)
        {
            game_failed=true;
        }
        xueliang daifuhealth(renderer, daifuxueliang.c_str());
        std::cerr<<daifu.health_<<std::endl;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        if (buttonClicked)
        {
            if (game_finished)
            {
                background2=IMG_LoadTexture(renderer, gameoverPath.c_str());
                SDL_RenderCopy(renderer,background2,nullptr,nullptr);
            }
            else if (game_failed)
            {
                background2=IMG_LoadTexture(renderer, gamefailPath.c_str());
                SDL_RenderCopy(renderer,background2,nullptr,nullptr);
            }
            else
            {
                player.movement(SDL_GetKeyboardState(nullptr),nettime,plants);
                //更新植物无敌时间数据
                for (auto& plant : plants)
                {
                    plant->WuDitimeing(nettime);
                    plant->update(nettime,player.F_rect,plants);
                }

                SDL_RenderCopy(renderer, background2, nullptr, nullptr);
                wandousheshou1.render(renderer,nettime);
                wandousheshou2.render(renderer,nettime);
                xiangrikui.render(renderer,nettime);
                daifu.render(renderer,nettime);
                player.render(renderer,nettime);
                daifuhealth.render(renderer);
            }

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