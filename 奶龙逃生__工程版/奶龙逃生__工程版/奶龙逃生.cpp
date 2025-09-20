#include <graphics.h>
#include <stdio.h>
#include "tools.hpp"
#include <time.h>
#include <math.h>
#include <conio.h>

bool running = 1, SPACEUP = 1;
ExMessage msg = { 0 };  MOUSEMSG m;
IMAGE menu, guide, hero[6], enemy[2], map[3], bullet[4], dieimage, vectory;// 设置图形变量
int x = 10, y = 690, vx = 0, vy = 0, speed = 5;// 主人公变量初始化

struct Enemy {
    int x, y;//敌人位置
    int vx, vy;//敌人速度
    int min, max;//敌人移动范围
    int health = 3; // 敌人血量
    bool active = true; // 敌人是否活跃
    int plus_minus;
};

Enemy map1enermy[6] = {
    {480,425,0,3,425,600,3,1,-1},
    {800,165,0,3,165,285,3,1,-1},
    {610,275,3,0,610,760,3,1,-1},
    {85,320,0,3,240,400,3,1,1},
    {270,120,0,3,85,200,3,1,1},
    {580,45,3,0,510,650,3,1,1} };//地图一的敌人初始化

struct Wallhero {
    int xl, xr, yu, yd;
    int hx = 0, hy = 0;
    int x = 0, y = 0;
};
struct Wallhero wall[23] = {
    {-75,225,95,210}, //横着
    {-75,250,405,535},
    {100,480,215,325},
    {270,440,500,590},
    {535,780,165,255},
    {405,755,290,420},
    {505,755,440,530},
    {460,640,600,695},
    {795,960,50,150},
    {800,960,370,490},
    {635,875,535,630 },
    //竖着
    { 60,170,345,675 },
    { 275,360,95,490 },
    { 215,310,610,800 },
    { 390,475,-75,160 },
    { 390,440,190,585 },
    { 500,600,50,360 },
    { 500,575,440,585 },
    { 525,635,675,800 },
    { 650,765,-75,275 },
    { 675,760,290,520 },
    { 690,795,640,800 },
    { 805,890,310,505 } };

const clock_t FPS = 1000 / 60;// 1s 60帧
int starttime = 0, delay = 0;// 防止丢失信息的延迟与帧率计算
int j = 0, LR = 2, UMD = 0, en = 6;// 判断变量（页面，主人公朝向，敌人数量）初始化
int score = 3,maxscore=0;//得分
struct Wallenemy
{
    int l, r, u, d;
}walls[23];

const int wallCount = sizeof(walls) / sizeof(walls[0]);

struct Bullet {
    int UDLR = 4;//子弹的朝向
    int x, y;       // 子弹的位置
    int vx, vy;     // 子弹的移动方向
    bool active = 0;    // 子弹是否激活
};
int consume, consume2 = 0;//子弹消耗
const int bulletSpeed = 8;  // 子弹速度
const int maxBullets = 3, bag = 4;  // 最大子弹数量
Bullet bullets[maxBullets]; // 子弹结构体数组
char rank;
char str[50] = { "Your rank is :" };
char str2[50] = { "Your score is :" };
char sc[10],msc[10];
char str3[50] = { "Bullets left :" };
char bu[10];

//////////////////// 子弹墙壁碰撞检测
void checkBulletCollisionWithWalls() {
    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < wallCount; j++) {
                // 检查子弹是否在墙壁的矩形区域内
                if (bullets[i].x >= walls[j].l && bullets[i].x <= walls[j].r &&
                    bullets[i].y >= walls[j].u && bullets[i].y <= walls[j].d) {
                    bullets[i].active = false; // 碰到墙壁，设置为非活跃状态
                    consume2++;
                    break; // 退出当前循环，检查下一个子弹
                }
            }
        }
    }
}

//////////////////////主人公墙壁碰撞
void wallcollison() {
    for (int i = 0; i < 23; i++)
    {
        wall[i].hx = wall[i].x; wall[i].hy = wall[i].y;//上一帧

        if (x <= wall[i].xr && x >= wall[i].xl)
            wall[i].x = 0;
        if (x > wall[i].xr)
            wall[i].x = 1;
        if (x < wall[i].xl)
            wall[i].x = -1;
        if (y <= wall[i].yd && y >= wall[i].yu)
            wall[i].y = 0;
        if (y > wall[i].yd)
            wall[i].y = 1;
        if (y < wall[i].yu)
            wall[i].y = -1;

        if (wall[i].hx == 0)
        {
            if (wall[i].x == 0 && wall[i].y == 0)
            {
                if (wall[i].hy == 1)
                    y = wall[i].yd;
                else if (wall[i].hy == -1)
                    y = wall[i].yu;
                else
                {
                    if (y == wall[i].yd + speed * vy || y == wall[i].yu + speed * vy)
                        y -= speed * vy;

                }
            }
        }
        if (wall[i].hy == 0)
        {
            if (wall[i].x == 0 && wall[i].y == 0)
            {
                if (wall[i].hx == 1)
                    x = wall[i].xr;
                if (wall[i].hx == -1)
                    x = wall[i].xl;
                else
                {
                    if (x == wall[i].xr + speed * vx || x == wall[i].xl + speed * vx)
                        x -= speed * vx;
                }
            }
        }
    }
}

/////////////////地图一敌人移动位置函数
void map1enemyaction() {
    for (int i = 0; i < en; i++) {
        if (map1enermy[i].active) {
            if (map1enermy[i].vx == 0) {
                if (map1enermy[i].y >= map1enermy[i].max) { map1enermy[i].vy = -3; }
                if (map1enermy[i].y <= map1enermy[i].min) { map1enermy[i].vy = 3; }
                map1enermy[i].y += map1enermy[i].vy;
            }
            else if (map1enermy[i].vy == 0) {
                if (map1enermy[i].x >= map1enermy[i].max) { map1enermy[i].vx = -3; }
                if (map1enermy[i].x <= map1enermy[i].min) { map1enermy[i].vx = 3; }
                map1enermy[i].x += map1enermy[i].vx;
            }
        }
    }
}

/////////////////主人公死亡判断函数
void die() {
    for (int i = 0; i < en; i++) {
        if (map1enermy[i].active && fabs(map1enermy[i].x - x) <= 20 && fabs(map1enermy[i].y - y) <= 20) {
            cleardevice();
            putimage(0, 0, &dieimage);
            rank = 'E';
            setbkmode(TRANSPARENT);
            settextcolor(RED);
            settextstyle(50, 0, "Showcard Gothic");
            outtextxy(80, 330, str);
            settextstyle(90, 0, "Showcard Gothic");
            outtextxy(400, 305, rank);
            if (score > maxscore) {
                maxscore = score;
                settextcolor(BLUE);
                settextstyle(100, 0, "Arial");
                outtextxy(100, 400, "GOOD!MAXSCORE!");
            }
            Sleep(5000);
            j = 0;
        }
    }
}

/////////////////获取信息并处理函数
void messageprocessing() {
    peekmessage(&msg, EX_KEY);
    if (msg.message == WM_KEYDOWN) {
        switch (msg.vkcode) {
        case 'S':// 下
            vy = 1; UMD = 4;
            break;
        case 'W': // 上
            vy = -1; UMD = 2;
            break;
        case 'A':// 左
            vx = -1; LR = 1; UMD = 0;
            break;
        case 'D':// 右
            vx = 1; LR = 2; UMD = 0;
            break;
        case VK_SPACE: // 按下空格键射击
            if (SPACEUP) {
                if (consume < bag * maxBullets)
                {
                    for (int i = 0; i < maxBullets; i++) {
                        if (!bullets[i].active) {
                            bullets[i].x = x + 35; // 子弹从玩家中心射出
                            bullets[i].y = y + 35;
                            consume++;
                            // 根据玩家移动方向设置子弹的移动方向
                            if (LR + UMD == 1) { // 向左移动
                                bullets[i].vx = -bulletSpeed;
                                bullets[i].vy = 0;
                                bullets[i].UDLR = 3;
                            }
                            else if (LR + UMD == 2) { // 向右移动
                                bullets[i].vx = bulletSpeed;
                                bullets[i].vy = 0;
                                bullets[i].UDLR = 4;
                            }
                            else if (LR + UMD == 3 || LR + UMD == 4) { // 向上移动
                                bullets[i].vx = 0;
                                bullets[i].vy = -bulletSpeed;
                                bullets[i].UDLR = 1;
                            }
                            else if (LR + UMD == 5 || LR + UMD == 6) { // 向下移动
                                bullets[i].vx = 0;
                                bullets[i].vy = bulletSpeed;
                                bullets[i].UDLR = 2;
                            }
                            bullets[i].active = true;
                            break;
                        }
                    }
                }
            }
            SPACEUP = false;
            break;
        }
    }
    else if (msg.message == WM_KEYUP) {
        switch (msg.vkcode) {
        case 'S':
            vy = 0;
            break;
        case 'W':
            vy = 0;
            break;
        case 'A':
            vx = 0;
            break;
        case 'D':
            vx = 0;
            break;
        case VK_SPACE:
            SPACEUP = true;
        }
    }
}

/////////////////主人公移动位置函数
void heroaction() {
    x += speed * vx;
    y += speed * vy;
    wallcollison();
    if (y > 44) {
        if (x > 875) { x = 875; } // 碰撞墙壁
    }
    if (y < 650) {
        if (x < 30) { x = 30; } // 碰撞墙壁
    }
    if (y > 695) { y = 695; } // 碰撞墙壁
    if (y < 44) { y = 44; } // 碰撞墙壁
    if (y < 700 && y > 649 && x < -55) {
        j--;
    }
    if (y >= 0 && y <= 50 && x >= 960)
    {
        j = 2;
    }
}

///////////////// 检测子弹和敌人是否碰撞
bool checkCollision(int bulletX, int bulletY, int enemyX, int enemyY) {
    // 子弹的矩形区域
    int bulletLeft = bulletX + 5; // 缩小碰撞区域
    int bulletRight = bulletX + 15;
    int bulletTop = bulletY + 5;
    int bulletBottom = bulletY + 15;

    // 敌人的矩形区域
    int enemyLeft = enemyX + 10; // 缩小碰撞区域
    int enemyRight = enemyX + 65;
    int enemyTop = enemyY + 10;
    int enemyBottom = enemyY + 65;

    // 检测矩形是否重叠
    if (bulletRight > enemyLeft && bulletLeft < enemyRight &&
        bulletBottom > enemyTop && bulletTop < enemyBottom) {
        return true; // 发生碰撞
    }
    return false; // 未发生碰撞
}

/////////////////子弹移动位置函数
void bulletaction() {
    if (consume2 < bag * maxBullets)
    {
        for (int i = 0; i < maxBullets; i++) {
            if (bullets[i].active) {
                bullets[i].x += bullets[i].vx; // 更新子弹位置
                bullets[i].y += bullets[i].vy;

                // 检测子弹是否击中敌人
                for (int j = 0; j < en; j++) {
                    if (map1enermy[j].active && checkCollision(bullets[i].x, bullets[i].y, map1enermy[j].x, map1enermy[j].y)) {
                        printf("Bullet%d hitenemy %d\n", i, j); // 调试信息
                        map1enermy[j].health--;
                        if (map1enermy[j].health <= 0) {
                            map1enermy[j].active = false;
                            consume2++;
                            if (map1enermy[j].plus_minus == 1)
                            {
                                score += 2;
                            }
                            if (map1enermy[j].plus_minus == -1)
                            {
                                score -= 1;
                            }
                            printf("%d", score);
                            printf("Enemy%ddestroyed\n", j); // 调试信息
                        }
                        bullets[i].active = false;
                        break;
                    }
                }

                // 如果子弹超出屏幕，则将其设置为未激活
                if (bullets[i].x < 0 || bullets[i].x > 960 || bullets[i].y < 0 || bullets[i].y > 800) {
                    bullets[i].active = false;
                    consume2++;
                }
                checkBulletCollisionWithWalls();
            }
        }
    }
    else
    {
        for (int i = 0; i < maxBullets; i++)
        {
            bullets[i].active = false;
        }
    }
}

int main() {
    // 初始化图形界面
    initgraph(960, 800, EX_SHOWCONSOLE);
    // 加载图片
    loadimage(&hero[0], ".\\images\\hero1.png", 75, 75);
    loadimage(&hero[1], ".\\images\\hero2.png", 75, 75);
    loadimage(&hero[2], ".\\images\\hero3.png", 75, 75);
    loadimage(&hero[3], ".\\images\\hero4.png", 75, 75);
    loadimage(&hero[4], ".\\images\\hero5.png", 75, 75);
    loadimage(&hero[5], ".\\images\\hero6.png", 75, 75);
    loadimage(&map[0], ".\\images\\map01.png", 960, 800);
    loadimage(&menu, ".\\images\\menu.png", 960, 800);
    loadimage(&bullet[0], ".\\images\\bullet1.png", 20, 20);
    loadimage(&bullet[1], ".\\images\\bullet2.png", 20, 20);
    loadimage(&bullet[2], ".\\images\\bullet3.png", 20, 20);
    loadimage(&bullet[3], ".\\images\\bullet4.png", 20, 20);
    loadimage(&enemy[0], ".\\images\\enemy1.png", 75, 75);
    loadimage(&enemy[1], ".\\images\\enemy2.png", 75, 75);
    loadimage(&guide, ".\\images\\guide.png", 960, 800);
    loadimage(&dieimage, ".\\images\\die.png", 960, 800);
    loadimage(&vectory, ".\\images\\vectory.png", 960, 800);
    for (int i = 0; i < 23; i++)
    {
        walls[i].l = wall[i].xl + 75;
        walls[i].r = wall[i].xr;
        walls[i].u = wall[i].yu + 75;
        walls[i].d = wall[i].yd;
    }

    while (running) {
        // 菜单
        if (j == 0) {
            cleardevice();
            putimage(0, 0, &menu);
            sprintf_s(msc, "%d", maxscore);
            setbkmode(TRANSPARENT);
            settextcolor(YELLOW);
            settextstyle(50, 0, "Algerian");
            outtextxy(300, 700, "Maxscore is :");
            outtextxy(620, 700, msc);
            while (j == 0) {
                m = GetMouseMsg();
                if (m.uMsg == WM_LBUTTONDOWN) {
                    if (m.x >= 181 && m.x <= 429 && m.y >= 242 && m.y <= 310) {
                        msg.message = WM_KEYUP;
                        vx = 0, vy = 0;
                        j = 1;
                        x = 10, y = 690;
                        LR = 2, UMD = 0;
                        en = 6;//初始化
                        // 重置敌人状态
                        for (int i = 0; i < en; i++) {
                            map1enermy[i].active = true;
                            map1enermy[i].health = 3;
                        }
                    }
                    else if (m.x >= 231 && m.x <= 464 && m.y >= 388 && m.y <= 465) {
                        j = 4;
                    }
                    else if (m.x >= 203 && m.x <= 413 && m.y >= 538 && m.y <= 642) {
                        closegraph();
                        running = false;
                    }
                }
            }
        }

        // 地图01
        while (j == 1) {
            starttime = clock();
            messageprocessing();//信息获取并处理
            heroaction();
            map1enemyaction();
            bulletaction(); // 更新位置
            sprintf_s(sc, "%d", score);
            sprintf_s(bu, "%d", maxBullets * bag - consume);
            // 图像输出
            BeginBatchDraw();
            cleardevice();
            putimage(0, 0, &map[0]);
            drawImg(x, y, &hero[LR + UMD - 1]);
            for (int i = 0; i < en; i++) {
                if (map1enermy[i].active) {
                    drawImg(map1enermy[i].x, map1enermy[i].y, &enemy[i % 2]);
                }
            }
            for (int i = 0; i < maxBullets; i++) {
                if (bullets[i].active) {
                    drawImg(bullets[i].x, bullets[i].y, &bullet[bullets[i].UDLR - 1]); // 绘制子弹
                }
            }
            setbkmode(TRANSPARENT);
            settextcolor(BLACK);
            settextstyle(25, 0, "Arial");
            outtextxy(50, 50, str2);
            outtextxy(200, 50, sc);
            outtextxy(50, 85, str3);
            outtextxy(200, 85, bu);
            EndBatchDraw();
            die();//死亡判断
            delay = clock() - starttime;
            if (delay < FPS) {
                Sleep(FPS - delay);
            }
        }

        // 玩法指导
        if (j == 4) {
            cleardevice();
            putimage(0, 0, &guide);
            while (j == 4) {
                m = GetMouseMsg();
                if (m.uMsg == WM_LBUTTONDOWN) { j = 0; }
            }
        }

        //胜利界面
        if (j == 2) {
            sprintf_s(sc, "%d", score);
            initgraph(960, 800, EX_SHOWCONSOLE);
            cleardevice();
            setbkmode(TRANSPARENT);
            printf("%d", score);
            if (score == 9)
                rank = 'S';
            if (score == 7 || score == 8)
                rank = 'A';
            if (score == 5 || score == 6)
                rank = 'B';
            if (score == 3 || score == 4)
                rank = 'C';
            if (score == 0 || score == 1 || score == 2)
                rank = 'D';
            cleardevice();
            putimage(0, 0, &vectory);
            settextcolor(GREEN);
            settextstyle(50, 0, "Showcard Gothic");
            outtextxy(80, 330, str);
            outtextxy(80, 430, str2);
            settextstyle(90, 0, "Showcard Gothic");
            outtextxy(400, 415, sc);
            outtextxy(400, 315, rank);
            if (score > maxscore){
                maxscore = score;
                settextcolor(BLUE);
                settextstyle(100, 0, "Arial");
                outtextxy(100, 480, "GOOD!MAXSCORE!");
            }
            while (j == 2) {
                m = GetMouseMsg();
                if (m.uMsg == WM_LBUTTONDOWN) { j = 0; }
            }
        }
    }
    return 0;
}