#include <graphics.h>
#include <string>
#include <iostream>

int idx_current_anim = 0; //用于记录当前动画的索引

const int PLAYER_ANIM_NUM = 6; //玩家动画的数量

const int PLAYER_SPEED = 5; //player移动速度

bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

POINT player_pos = { 500,500 }; //玩家初始位置

//实现png透明通道混叠
#pragma comment(lib,"MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* img)
{
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
               GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

void LoadAnimation()
{
    for(size_t i=0;i<PLAYER_ANIM_NUM;i++)
    {
        std::string path = "../img/player_left_"+std::to_string(i)+".png";
        loadimage(&img_player_left[i], path.c_str());
    }
    for(size_t i=0;i<PLAYER_ANIM_NUM;i++)
    {
        std::string path = "../img/player_right_"+std::to_string(i)+".png";
        loadimage(&img_player_right[i],(const char*)path.c_str());
    }
}

int main() {
    initgraph(1280,720);

    bool running = true;

    ExMessage msg;
    IMAGE img_background;

    LoadAnimation();
    loadimage(&img_background,_T("../img/background.png"));

    BeginBatchDraw();

    while(running)
    {
        DWORD start_time = GetTickCount();

        while(peekmessage(&msg))
        {
            if (msg.message == WM_KEYDOWN)
            {
                switch (msg.vkcode)
                {
                    case VK_UP:
                        is_move_up = true;
                        break;
                    case VK_DOWN:
                        is_move_down = true;
                        break;
                    case VK_LEFT:
                        is_move_left = true;
                        break;
                    case VK_RIGHT:
                        is_move_right = true;
                        break;
                }
            }
            else if (msg.message == WM_KEYUP)
            {
                switch (msg.vkcode)
                {
                    case VK_UP:
                        is_move_up = false;
                        break;
                    case VK_DOWN:
                        is_move_down = false;
                        break;
                    case VK_LEFT:
                        is_move_left = false;
                        break;
                    case VK_RIGHT:
                        is_move_right = false;
                        break;
                }
            }

            if (is_move_up)
                player_pos.y -= PLAYER_SPEED;
            if (is_move_down)
                player_pos.y += PLAYER_SPEED;
            if (is_move_left)
                player_pos.x -= PLAYER_SPEED;
            if (is_move_right)
                player_pos.x += PLAYER_SPEED;
        }

        static int counter = 0; //用于记录动画帧一共播放了几个游戏帧
        if(++counter % 5 == 0)
            idx_current_anim++; //每5帧切换一次动画

        idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM; //确保索引在0~5之间


        cleardevice();

        putimage(0,0,&img_background);
        putimage_alpha(player_pos.x,player_pos.y,&img_player_right[idx_current_anim]);

        FlushBatchDraw();

        DWORD end_time = GetTickCount();
        DWORD delta_time = end_time - start_time;
        if(delta_time < 1000 / 144)
        {
            Sleep(1000 /144-delta_time);
        }
    }

    EndBatchDraw();

    return 0;
}
