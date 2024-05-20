#include <iostream>
#include <graphics.h>
#include <conio.h>

int main() {
    initgraph(1280,720);

    bool running = true;

    ExMessage msg;
    IMAGE img_background;

    loadimage(&img_background,_T("../img/background.png"));

    BeginBatchDraw();

    while(running)
    {
        DWORD start_time = GetTickCount();

        while(peekmessage(&msg))
        {

        }

        cleardevice();

        putimage(0,0,&img_background);

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
