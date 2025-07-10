# 程序设计大作业报告

#### 此项目为大一下程序设计的大作业。并且以README.MD文件作为项目报告

1：经过查询，使用SDL可以进行图片渲染，就在github上拉取了SDL3的库，（包括了SDL3_image）

2：使用过程中发现SDL3很多部分与SDL2不互通，将SDL3的库换成了SDL2的

- 此项目代码为c++格式，用cmake编译

- 查询使用后尝试对图片进行渲染，出现路径问题无法解决，后经查询

  ```
  char* basePath = SDL_GetBasePath();
      std::string basePathStr = basePath;
      SDL_free(basePath);
  ```

- 使用这个函数以及字符串的拼接可以解决路径问题（原理是找到可执行文件的目录）

#### 玩家和敌人

- 该部分内容使用类来实现，玩家单独一个类，植物（包括戴夫）一个类

  （写到后面才发现玩家和植物具有很多类似的属性，可以写成一个基类，再由另外的继承，可以减少代码）

- 给玩家定义了血量，以及给戴夫定义了血量，通过判断各自的血量值在界面的左上方进行相应图片的渲染，在代码中通过if和switch case实现

- 接下来是玩家的操作

  在player类中编写了movement函数，并在主函数中循环运行

  ```
  if(keyboardState[SDL_SCANCODE_W])
  ```

  使用类似的方式判断玩家的操作并进行状态判断和命令的执行

- 玩家和戴夫的掉血机制

  定义了碰撞检测函数

  ```
  bool checkCollision(SDL_FRect rect1, SDL_FRect rect2)
  {
      return (rect1.x < rect2.x + rect2.w &&
              rect1.x + rect1.w > rect2.x &&
              rect1.y < rect2.y + rect2.h &&
              rect1.y + rect1.h > rect2.y);
  }
  ```

  返回bool值，再在各自类中通过体积的碰撞判断实现掉血

  玩家和豌豆射手的额外攻击手段（包括戴夫的小推车）则是在类中多定义了一个攻击体积，通过判断是否在攻击状态判断是否对该部分进行渲染

- 无敌模式

  为了防止快速掉血，写了一个无敌的状态判断，如果无敌就定时对图片渲染，起到闪烁的效果，并且在掉血部分写定只在非无敌状态下进行判断，无敌状态下不掉血

  > [!IMPORTANT]
  >
  > 后面的代码内容由ai在我的源代码中选取配合报告使用，可能出现源代码中没有的注释或者其他东西

  ```
  class Player {
  private:
      bool wudi = false;
      float wuditime = 0;
  
  public:
      void movement(const Uint8 *keyboardState, float nettime, std::vector<Plant*>& plants)
      {
          // 无敌时间计时
          if (wuditime > 0)
          {
              wuditime -= nettime;
              wudi = true;
          }
          else
          {
              wudi = false;
              wuditime = 0;
          }
  
          // 碰撞检测
          for (const auto& plant : plants)
          {
              if (checkCollision(rect, plant->getRect()) || checkCollision(rect, plant->getFireRect()))
              {
                  if (!wudi && health > 0)
                  {
                      health -= 10;
                      wudi = true;
                      wuditime = 1;
                      if (health <= 0) health = 0;
                  }
              }
          }
      }
  
      void render(SDL_Renderer *renderer, float nettime)
      {
          if (wudi)
          {
              lighting_time += nettime;
              is_lighting = static_cast<int>(lighting_time * 10) % 2 == 1;
              if (lighting_time >= 0.2) lighting_time = 0;
          }
          else is_lighting = false;
      }
  };
  ```

  

  > [!NOTE]
  >
  > 小bug，在体积碰撞检测中，防止玩家和植物的图片重叠，在判断进行碰撞的时候让玩家的位置保持在重叠之前的位置，因此，玩家可以通过持续移动卡在植物的旁边，戴夫在移动的时候也会把玩家卡在原地（也可以当作游戏特性）

- 为了不那么粗糙，对玩家操作进行检测判断此时的行走方向，对玩家图像进行翻转，实现左右面向

- 植物包括戴夫的动图形式则是通过时间判断交替进行图片的转换，实现伪动图的效果（包括玩家的行走）

#### 重力和地面的实现

- 重力则通过在y轴上给一个持续向下的加速度
- 地面实现和防止图片重叠的原理类似，在y轴触碰到地面时重置到地面的位置
- 地面的选择，背景图片中有两个地面，通过读取键盘操作s键位和人物高度来进行地面选择

```
class Player {
private:
    // ...已有代码...
    float gravity = 800;
    float H_speed = 0;
    // ...已有代码...
public:
    // ...已有代码...
    void movement(const Uint8 *keyboardState,float nettime, std::vector<Plant*>& plants)
    {
        // ...已有代码...
        H_speed += gravity * nettime;
        rect.y += H_speed * nettime;
        // ...已有代码...
    }
    // ...已有代码...
};
```

### 兼容性

最难的部分，因为是在linux系统编写代码，使用的SDL库为整个源文件放在项目内部可以实现，但在兼容到windows系统时出现了严重的问题，cmakelist需要重新构建

经过查询后使用了mingw交叉编译，但是也出现了严重的问题，最后只能在windows下重新寻找编译方法。

在成功编译生成exe文件后出现了其他的报错，dll缺失等，下载了相应的dll文件后又出现报错，查询后发现是libstdc++-6的版本可能过旧，而我使用的是cxx20，修改之后也无法使用，最后请教到可以将动态连接库改成静态连接库的方式解决了在windows下的兼容