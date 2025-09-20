#include <graphics.h>
#include <stdio.h>
#include "tools.hpp"
#include <time.h>
#include <math.h>
#include <conio.h>

bool running = 1, SPACEUP = 1;
ExMessage msg = { 0 };  MOUSEMSG m;
IMAGE menu, guide, hero[6], enemy[2], map[3], bullet[4], dieimage, vectory;// ����ͼ�α���
int x = 10, y = 690, vx = 0, vy = 0, speed = 5;// ���˹�������ʼ��

struct Enemy {
    int x, y;//����λ��
    int vx, vy;//�����ٶ�
    int min, max;//�����ƶ���Χ
    int health = 3; // ����Ѫ��
    bool active = true; // �����Ƿ��Ծ
    int plus_minus;
};

Enemy map1enermy[6] = {
    {480,425,0,3,425,600,3,1,-1},
    {800,165,0,3,165,285,3,1,-1},
    {610,275,3,0,610,760,3,1,-1},
    {85,320,0,3,240,400,3,1,1},
    {270,120,0,3,85,200,3,1,1},
    {580,45,3,0,510,650,3,1,1} };//��ͼһ�ĵ��˳�ʼ��

struct Wallhero {
    int xl, xr, yu, yd;
    int hx = 0, hy = 0;
    int x = 0, y = 0;
};
struct Wallhero wall[23] = {
    {-75,225,95,210}, //����
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
    //����
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

const clock_t FPS = 1000 / 60;// 1s 60֡
int starttime = 0, delay = 0;// ��ֹ��ʧ��Ϣ���ӳ���֡�ʼ���
int j = 0, LR = 2, UMD = 0, en = 6;// �жϱ�����ҳ�棬���˹����򣬵�����������ʼ��
int score = 3,maxscore=0;//�÷�
struct Wallenemy
{
    int l, r, u, d;
}walls[23];

const int wallCount = sizeof(walls) / sizeof(walls[0]);

struct Bullet {
    int UDLR = 4;//�ӵ��ĳ���
    int x, y;       // �ӵ���λ��
    int vx, vy;     // �ӵ����ƶ�����
    bool active = 0;    // �ӵ��Ƿ񼤻�
};
int consume, consume2 = 0;//�ӵ�����
const int bulletSpeed = 8;  // �ӵ��ٶ�
const int maxBullets = 3, bag = 4;  // ����ӵ�����
Bullet bullets[maxBullets]; // �ӵ��ṹ������
char rank;
char str[50] = { "Your rank is :" };
char str2[50] = { "Your score is :" };
char sc[10],msc[10];
char str3[50] = { "Bullets left :" };
char bu[10];

//////////////////// �ӵ�ǽ����ײ���
void checkBulletCollisionWithWalls() {
    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < wallCount; j++) {
                // ����ӵ��Ƿ���ǽ�ڵľ���������
                if (bullets[i].x >= walls[j].l && bullets[i].x <= walls[j].r &&
                    bullets[i].y >= walls[j].u && bullets[i].y <= walls[j].d) {
                    bullets[i].active = false; // ����ǽ�ڣ�����Ϊ�ǻ�Ծ״̬
                    consume2++;
                    break; // �˳���ǰѭ���������һ���ӵ�
                }
            }
        }
    }
}

//////////////////////���˹�ǽ����ײ
void wallcollison() {
    for (int i = 0; i < 23; i++)
    {
        wall[i].hx = wall[i].x; wall[i].hy = wall[i].y;//��һ֡

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

/////////////////��ͼһ�����ƶ�λ�ú���
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

/////////////////���˹������жϺ���
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

/////////////////��ȡ��Ϣ��������
void messageprocessing() {
    peekmessage(&msg, EX_KEY);
    if (msg.message == WM_KEYDOWN) {
        switch (msg.vkcode) {
        case 'S':// ��
            vy = 1; UMD = 4;
            break;
        case 'W': // ��
            vy = -1; UMD = 2;
            break;
        case 'A':// ��
            vx = -1; LR = 1; UMD = 0;
            break;
        case 'D':// ��
            vx = 1; LR = 2; UMD = 0;
            break;
        case VK_SPACE: // ���¿ո�����
            if (SPACEUP) {
                if (consume < bag * maxBullets)
                {
                    for (int i = 0; i < maxBullets; i++) {
                        if (!bullets[i].active) {
                            bullets[i].x = x + 35; // �ӵ�������������
                            bullets[i].y = y + 35;
                            consume++;
                            // ��������ƶ����������ӵ����ƶ�����
                            if (LR + UMD == 1) { // �����ƶ�
                                bullets[i].vx = -bulletSpeed;
                                bullets[i].vy = 0;
                                bullets[i].UDLR = 3;
                            }
                            else if (LR + UMD == 2) { // �����ƶ�
                                bullets[i].vx = bulletSpeed;
                                bullets[i].vy = 0;
                                bullets[i].UDLR = 4;
                            }
                            else if (LR + UMD == 3 || LR + UMD == 4) { // �����ƶ�
                                bullets[i].vx = 0;
                                bullets[i].vy = -bulletSpeed;
                                bullets[i].UDLR = 1;
                            }
                            else if (LR + UMD == 5 || LR + UMD == 6) { // �����ƶ�
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

/////////////////���˹��ƶ�λ�ú���
void heroaction() {
    x += speed * vx;
    y += speed * vy;
    wallcollison();
    if (y > 44) {
        if (x > 875) { x = 875; } // ��ײǽ��
    }
    if (y < 650) {
        if (x < 30) { x = 30; } // ��ײǽ��
    }
    if (y > 695) { y = 695; } // ��ײǽ��
    if (y < 44) { y = 44; } // ��ײǽ��
    if (y < 700 && y > 649 && x < -55) {
        j--;
    }
    if (y >= 0 && y <= 50 && x >= 960)
    {
        j = 2;
    }
}

///////////////// ����ӵ��͵����Ƿ���ײ
bool checkCollision(int bulletX, int bulletY, int enemyX, int enemyY) {
    // �ӵ��ľ�������
    int bulletLeft = bulletX + 5; // ��С��ײ����
    int bulletRight = bulletX + 15;
    int bulletTop = bulletY + 5;
    int bulletBottom = bulletY + 15;

    // ���˵ľ�������
    int enemyLeft = enemyX + 10; // ��С��ײ����
    int enemyRight = enemyX + 65;
    int enemyTop = enemyY + 10;
    int enemyBottom = enemyY + 65;

    // �������Ƿ��ص�
    if (bulletRight > enemyLeft && bulletLeft < enemyRight &&
        bulletBottom > enemyTop && bulletTop < enemyBottom) {
        return true; // ������ײ
    }
    return false; // δ������ײ
}

/////////////////�ӵ��ƶ�λ�ú���
void bulletaction() {
    if (consume2 < bag * maxBullets)
    {
        for (int i = 0; i < maxBullets; i++) {
            if (bullets[i].active) {
                bullets[i].x += bullets[i].vx; // �����ӵ�λ��
                bullets[i].y += bullets[i].vy;

                // ����ӵ��Ƿ���е���
                for (int j = 0; j < en; j++) {
                    if (map1enermy[j].active && checkCollision(bullets[i].x, bullets[i].y, map1enermy[j].x, map1enermy[j].y)) {
                        printf("Bullet%d hitenemy %d\n", i, j); // ������Ϣ
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
                            printf("Enemy%ddestroyed\n", j); // ������Ϣ
                        }
                        bullets[i].active = false;
                        break;
                    }
                }

                // ����ӵ�������Ļ����������Ϊδ����
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
    // ��ʼ��ͼ�ν���
    initgraph(960, 800, EX_SHOWCONSOLE);
    // ����ͼƬ
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
        // �˵�
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
                        en = 6;//��ʼ��
                        // ���õ���״̬
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

        // ��ͼ01
        while (j == 1) {
            starttime = clock();
            messageprocessing();//��Ϣ��ȡ������
            heroaction();
            map1enemyaction();
            bulletaction(); // ����λ��
            sprintf_s(sc, "%d", score);
            sprintf_s(bu, "%d", maxBullets * bag - consume);
            // ͼ�����
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
                    drawImg(bullets[i].x, bullets[i].y, &bullet[bullets[i].UDLR - 1]); // �����ӵ�
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
            die();//�����ж�
            delay = clock() - starttime;
            if (delay < FPS) {
                Sleep(FPS - delay);
            }
        }

        // �淨ָ��
        if (j == 4) {
            cleardevice();
            putimage(0, 0, &guide);
            while (j == 4) {
                m = GetMouseMsg();
                if (m.uMsg == WM_LBUTTONDOWN) { j = 0; }
            }
        }

        //ʤ������
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