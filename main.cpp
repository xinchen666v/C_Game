#include <graphics.h>
#include <string>
#include <iostream>
#include <vector>
#include <valarray>

int idx_current_anim = 0; //用于记录当前动画的索引

const int WINDOW_WIDTH = 1280; //窗口宽度
const int WINDOW_HEIGHT = 720; //窗口高度

const int PLAYER_ANIM_NUM = 6; //玩家动画的数量

const int PLAYER_WIDTH = 80; //玩家宽度
const int PLAYER_HEIGHT = 80; //玩家高度

const int PLAYER_SHADOW_WIDTH = 32; //玩家阴影宽度

const int PLAYER_SPEED = 3; //player移动速度

bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];
IMAGE img_shadow;
IMAGE img_background;

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

//构造类渲染player动画
class Animation
{
public:
    Animation(LPCTSTR path,int num,int interval)
    {
        interval_ms = interval;

        TCHAR path_file[256];
        for(size_t i = 0;i < num;i++)
        {
            _stprintf_s(path_file,path,i);

            IMAGE* frame = new IMAGE();
            loadimage(frame,path_file);
            frame_list.push_back(frame);
        }
    }

    ~Animation()
    {
        for(size_t i = 0;i<frame_list.size();i++)
            delete frame_list[i];
    }

    void Play(int x,int y,int delta)
    {
        timer += delta;
        if (timer >= interval_ms)
        {
            idx_frame = (idx_frame + 1) % frame_list.size();
            timer = 0;
        }
        putimage_alpha(x,y,frame_list[idx_frame]);
    }

private:
    int timer = 0; //用于记录动画播放时间
    int idx_frame = 0; //用于记录当前播放的帧
    int interval_ms; //动画播放间隔
    std::vector<IMAGE*> frame_list; //存放动画帧的数组
};

//用于渲染player动画的类实例
Animation anim_player_left(_T("../img/player_left_%d.png"),PLAYER_ANIM_NUM,45);
Animation anim_player_right(_T("../img/player_right_%d.png"),PLAYER_ANIM_NUM,45);

void DrawPlayer(int delta,int dir_x)
{
    int pos_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - PLAYER_SHADOW_WIDTH / 2);
    int pos_shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
    putimage_alpha(pos_shadow_x,pos_shadow_y,&img_shadow);

    static bool facing_left = false;
    if (dir_x < 0)
        facing_left = true;
    else if (dir_x > 0)
        facing_left = false;

    if (facing_left)
        anim_player_left.Play(player_pos.x,player_pos.y,delta);
    else
        anim_player_right.Play(player_pos.x,player_pos.y,delta);
}

int main() {
    initgraph(WINDOW_WIDTH,WINDOW_HEIGHT);

    bool running = true;

    ExMessage msg;



    LoadAnimation();
    loadimage(&img_background,_T("../img/background.png"));
    loadimage(&img_shadow,_T("../img/shadow_player.png"));

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

        //确保运算时的速度方向向量是单位向量
        int dir_x = is_move_right - is_move_left;
        int dir_y = is_move_down - is_move_up;
        double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
        if(len_dir != 0)
        {
            double normalized_x = dir_x / len_dir;
            double normalized_y = dir_y / len_dir;
            player_pos.x += (int)(PLAYER_SPEED * normalized_x);
            player_pos.y += (int)(PLAYER_SPEED * normalized_y);
        }

        //保证玩家在画面内
        if(player_pos.x < 0)
            player_pos.x = 0;
        if(player_pos.y < 0)
            player_pos.y = 0;
        if(player_pos.x + PLAYER_WIDTH > WINDOW_WIDTH)
            player_pos.x = WINDOW_WIDTH - PLAYER_WIDTH;
        if(player_pos.y + PLAYER_HEIGHT > WINDOW_HEIGHT)
            player_pos.y = WINDOW_HEIGHT - PLAYER_HEIGHT;

        cleardevice();

        putimage(0,0,&img_background);
//        putimage_alpha(player_pos.x,player_pos.y,&img_player_right[idx_current_anim]);
        DrawPlayer(1000/144,is_move_right - is_move_left);

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
