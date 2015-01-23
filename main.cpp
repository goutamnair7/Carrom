#include<iostream>
#include<stdlib.h>
#include<cmath>
#include<sys/time.h>
#include<GL/freeglut.h>
#include<unistd.h>
#include<string.h>
#include<cstdlib>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.1415926
#define DEG2RAD(deg) (deg * PI/180)
#define RAD2DEG(rad) (rad * 180/PI)

using namespace std;

class Board
{
    public:
        void init()
        {
            glPushMatrix();
            glTranslatef(-2.0f, 0.0f, -2.0f);
            glPopMatrix();

            glBegin(GL_QUADS);

            glColor3f(1.0f, 0.870f, 0.678f); //Board colour - Peach
            glVertex3f(-0.75f, -0.75f, -2.0f);
            glVertex3f(0.75f, -0.75f, -2.0f);
            glVertex3f(0.75f, 0.75f, -2.0f);
            glVertex3f(-0.75f, 0.75f, -2.0f);

            glEnd();

            glBegin(GL_QUADS);

            glColor3f(0.0f, 0.0f, 0.0f); //Board border - Black
            glVertex3f(-0.8f, -0.8f, -2.0f);
            glVertex3f(0.8f, -0.8f, -2.0f);
            glVertex3f(0.8f, 0.8f, -2.0f);
            glVertex3f(-0.8f, 0.8f, -2.0f);

            glEnd();
        }
};

class Circle                //Base class: Circle
{
    public: void setRadius(float r);
            void drawCircle();
            float getRadius();

    private:float radius;
};

void Circle::setRadius(float r)
{
    radius = r;
}

float Circle::getRadius()
{
    return radius;
}

void Circle::drawCircle()
{
    glBegin(GL_TRIANGLE_FAN);
    for(int i=0 ; i<360 ; i++) 
    {
        glVertex3f(radius * cos(DEG2RAD(i)), radius * sin(DEG2RAD(i)), -2.0f);
    }   
    glEnd(); 
}

class Coin: public Circle
{
    public: void setID(int);
            int getColour();
            float getXCoordinate();
            float getYCoordinate();
            void setCoordinates(float, float);
            float getXVel();
            float getYVel();
            void setVelocities(float, float);
            int stateX, stateY, hide;
            float ratio;
    private:float x, y, vel_x, vel_y;
            int ID;
};

void Coin::setVelocities(float x, float y)
{
    vel_x = x;
    vel_y = y;
}

float Coin::getXVel()
{
    return vel_x;
}

float Coin::getYVel()
{
    return vel_y;
}

void Coin::setID(int id)
{
    ID = id;
}

int Coin::getColour() /*Coin colour is based on ID. ID==0: queen, ID%2==0: white, ID%2!=0: black*/
{                                 
    if(ID == 0)
        return 0;
    else if(ID%2 == 0)
        return 1;
    else if(ID%2 == 1)
        return 2;
    return -1;
}

float Coin::getXCoordinate()
{
    return x;
}

float Coin::getYCoordinate()
{
    return y;
}

void Coin::setCoordinates(float X, float Y)
{
    x = X;
    y = Y;
}

Circle Hole[4];

Coin coins[9];

Coin striker;
float theta, power;
int StrikeFlag;
void updateCoin(int);
int StrikerCoin[9];
int CoinCoin[9][9];

//Initializes 3D rendering
void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL); //Enable color
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //Background - white
}

void initCoinPositions() //Initialise coin positions
{
    coins[0].setCoordinates(0.0f, 0.0f);
    coins[1].setCoordinates(0.11f, 0.0f);
    coins[2].setCoordinates(0.078f, 0.078f);
    coins[3].setCoordinates(0.0f, 0.11f);
    coins[4].setCoordinates(-0.078f, 0.078f);
    coins[5].setCoordinates(-0.11f, 0.0f);
    coins[6].setCoordinates(-0.078f, -0.078f);
    coins[7].setCoordinates(0.0f, -0.11f);
    coins[8].setCoordinates(0.078f, -0.078f);

    for(int i=0;i<9;++i)
    {
        coins[i].setRadius(0.023f);
        coins[i].stateX = coins[i].stateY = 0;
        coins[i].setID(i);
        coins[i].ratio = -1.0f;
        StrikerCoin[i] = 0;
        coins[i].hide = 0;
        for(int j=0;j<9;++j)
            CoinCoin[i][j] = 0;
    }

    for(int i=0;i<9;++i)
        glutTimerFunc(10, updateCoin, i);
}

void initStrikerValues() // Initialise striker values
{
    striker.setCoordinates(0.0f, -0.47f);
    striker.setRadius(0.03f);
    striker.stateX = striker.stateY = 0;
    theta = 0.0f;
    power = 2.0f;
    striker.setVelocities(0.0f, power/400);
    StrikeFlag = 0;
    striker.hide = 0;
    striker.ratio = -1.0f;
}

void drawCoins() // Initialising positions of coins
{
    
    float X, Y;
    if(coins[0].hide == 0)
    {
        glPushMatrix();
        X = coins[0].getXCoordinate();
        Y = coins[0].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        coins[0].drawCircle();
        glPopMatrix();
    }
    if(coins[1].hide == 0)
    {
        glPushMatrix();
        X = coins[1].getXCoordinate();
        Y = coins[1].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        coins[1].drawCircle();
        glPopMatrix();
    }
    if(coins[2].hide == 0)
    {
        glPushMatrix();
        X = coins[2].getXCoordinate();
        Y = coins[2].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.960f, 0.960f, 0.960f);
        coins[2].drawCircle();
        glPopMatrix();
    }
    if(coins[3].hide == 0)
    {
        glPushMatrix();
        X = coins[3].getXCoordinate();
        Y = coins[3].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        coins[3].drawCircle();
        glPopMatrix();
    }
    if(coins[4].hide == 0)
    {
        glPushMatrix();
        X = coins[4].getXCoordinate();
        Y = coins[4].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.960f, 0.960f, 0.960f);
        coins[4].drawCircle();
        glPopMatrix();
    }
    if(coins[5].hide == 0)
    {
        glPushMatrix();
        X = coins[5].getXCoordinate();
        Y = coins[5].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        coins[5].drawCircle();
        glPopMatrix();
    }
    if(coins[6].hide == 0)
    {
        glPushMatrix();
        X = coins[6].getXCoordinate();
        Y = coins[6].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.960f, 0.960f, 0.960f);
        coins[6].drawCircle();
        glPopMatrix();
    }
    if(coins[7].hide == 0)
    {
        glPushMatrix();
        X = coins[7].getXCoordinate();
        Y = coins[7].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        coins[7].drawCircle();
        glPopMatrix();
    }
    if(coins[8].hide == 0)
    {
        glPushMatrix();
        X = coins[8].getXCoordinate();
        Y = coins[8].getYCoordinate();
        glTranslatef(X, Y, 0.0f);
        glColor3f(0.960f, 0.960f, 0.960f);
        coins[8].drawCircle();
        glPopMatrix();
    }
}

void drawAngleLine() // Drawing a line that moves with angle
{
    float X, Y, rad;
    X = striker.getXCoordinate();
    Y = striker.getYCoordinate();
    rad = striker.getRadius();
    if(Y == -0.47f && StrikeFlag == 0)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex3f(X, Y + rad, -2.0f);
        glVertex3f(X + (rad*4.0f*tan(DEG2RAD(theta))), Y + 4.0f*rad, -2.0f);
        glEnd();
    }
}

void drawPowerLine() // Drawing power for striker
{
    glBegin(GL_POLYGON);
    if(power <= 4.0f)
        glColor3f(0.0f, 1.0f, 0.0f);
    else if(power <= 7.0f)
        glColor3f(1.0f, 0.549f, 0.0f);
    else
        glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.90f, -0.50f, -2.0f);
    glVertex3f(0.90f, ((power - 2.0f)/20.0f) + -0.30f, -2.0f);
    glVertex3f(0.93f, ((power - 2.0f)/20.0f) + -0.30f, -2.0f);
    glVertex3f(0.93f, -0.50f, -2.0f);
    glEnd();
}

void drawStriker() // Initialising position of striker
{
    float X, Y;
    X = striker.getXCoordinate();
    Y = striker.getYCoordinate();
    striker.setID(-1.0f);
    glPushMatrix();
    glTranslatef(X, Y, 0.0f);
    glColor3f(1.0f, 0.843f, 0.0f);
    striker.drawCircle();
    glPopMatrix();
}

void drawCentreCircle() // Draw circle boundary at the centre
{
    float rad = 0.13f;
    glColor3f(0.545f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<360;i+=1)
    {
        glVertex3f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)), -2.0f);
    }
    glEnd();
}

void drawPockets() // Draw pockets/holes
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.69f, 0.69f, 0.0f);
    Hole[0].setRadius(0.05f);
    Hole[0].drawCircle();
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.69f, 0.69f, 0.0f);
    Hole[1].setRadius(0.05f);
    Hole[1].drawCircle();
    glPopMatrix();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.69f, -0.69f, 0.0f);
    Hole[2].setRadius(0.05f);
    Hole[2].drawCircle();
    glPopMatrix();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.69f, -0.69f, 0.0f);
    Hole[3].setRadius(0.05f);
    Hole[3].drawCircle();
    glPopMatrix();
}

void drawStrikingLines() // Draw lines where the striker can be played from
{
    Circle c1;
    
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINES);
    glVertex3f(-0.45f, 0.5f, -2.0f);
    glVertex3f(0.45f, 0.5f, -2.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.45f, 0.44f, -2.0f);
    glVertex3f(-0.45f, 0.44f, -2.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(-0.45f, 0.47f, 0.0f);
    c1.setRadius(0.03f);
    glColor3f(0.545f, 0.0f, 0.0f);
    c1.drawCircle();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.45f, 0.47f, 0.0f);
    c1.setRadius(0.03f);
    c1.drawCircle();
    glPopMatrix();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.45f, -0.5f, -2.0f);
    glVertex3f(0.45f, -0.5f, -2.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.45f, -0.44f, -2.0f);
    glVertex3f(-0.45f, -0.44f, -2.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(-0.45f, -0.47f, 0.0f);
    glColor3f(0.545f, 0.0f, 0.0f);
    c1.setRadius(0.03f);
    c1.drawCircle();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.45f, -0.47f, 0.0f);
    c1.setRadius(0.03f);
    c1.drawCircle();
    glPopMatrix();
    
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.475f, 0.425f, -2.0f);
    glVertex3f(-0.475f, -0.425f, -2.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-0.535f, 0.425f, -2.0f);
    glVertex3f(-0.535f, -0.425f, -2.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(-0.505f, 0.425f, 0.0f);
    c1.setRadius(0.03f);
    glColor3f(0.545f, 0.0f, 0.0f);
    c1.drawCircle();
    c1.drawCircle();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.505f, -0.425f, 0.0f);
    c1.setRadius(0.03f);
    c1.drawCircle();
    glPopMatrix();
    
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.475f, 0.425f, -2.0f);
    glVertex3f(0.475f, -0.425f, -2.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.535f, 0.425f, -2.0f);
    glVertex3f(0.535f, -0.425f, -2.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.505f, 0.425f, 0.0f);
    c1.setRadius(0.03f);
    glColor3f(0.545f, 0.0f, 0.0f);
    c1.drawCircle();
    c1.drawCircle();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.505f, -0.425f, 0.0f);
    c1.setRadius(0.03f);
    c1.drawCircle();
    glPopMatrix();
}

struct timeval t3, t4;

int checkjumpStrikerCollision(int i)
{
    float strikerX = striker.getXCoordinate();
    float strikerY = striker.getYCoordinate();
    float coinX = coins[i].getXCoordinate();
    float coinY = coins[i].getYCoordinate();
    float ratio = striker.getXVel() / striker.getYVel();
    float factor = 0.001333333f;
    float dist;
    float rad = striker.getRadius();
    if(strikerX == coinX)
    {
        if(striker.stateY == 0)
        {
            strikerY += striker.getYVel()*0.4f + factor;
            dist = sqrt(pow((strikerX - coinX), 2) + pow((strikerY - coinY), 2));
            if(dist - 0.02f < rad + coins[i].getRadius())
                return 1;
        }
        else if(striker.stateY == 1)
        {
            strikerY -= (striker.getYVel()*0.4f - factor);
            dist = sqrt(pow((strikerX - coinX), 2) + pow((strikerY - coinY), 2));
            if(dist - 0.02f < rad + coins[i].getRadius())
                return 1;
        }
    }
    else
    {
        if(striker.stateX == 0)
        {
            strikerX += striker.getXVel()*0.4f + factor;
            strikerY += ratio*strikerX;
            dist = sqrt(pow((strikerX - coinX), 2) + pow((strikerY - coinY), 2));
            if(dist - 0.02f < rad + coins[i].getRadius())
                return 1;
        }
        else if(striker.stateX == 1)
        {
            strikerX += striker.getXVel()*0.4f + factor;
            strikerY += ratio*strikerX;
            dist = sqrt(pow((strikerX - coinX), 2) + pow((strikerY - coinY), 2));
            if(dist - 0.02f < rad + coins[i].getRadius())
                return 1;
        }
    }
    return 0;
}
int flag = 0;
void checkStrikerCollisions(int i, float dist_x, float dist_y)
{
    if(i >= 9 || i < 0)
        return;
    float strikerX, strikerY, rad, coinX, coinY, dist, dist2;
    float strikerVelX, strikerVelY, coinVelX, coinVelY, impactAngle, slope;
    float strikerUx, strikerUy, strikerV, coinUx, coinUy, coinV;
        strikerX = striker.getXCoordinate();
        strikerY = striker.getYCoordinate();
        strikerVelX = striker.getXVel();
        strikerVelY = striker.getYVel();
        
        rad = striker.getRadius();
        
        coinX = coins[i].getXCoordinate();
        coinY = coins[i].getYCoordinate();
        coinVelX = coins[i].getXVel();
        coinVelY = coins[i].getYVel();
        
        if(striker.stateX == 1)
            dist_x *= -1;
        if(striker.stateY == 1)
            dist_y *= -1;

        dist = sqrt(pow((strikerX - coinX), 2) + pow((strikerY - coinY), 2));
        dist2 = sqrt(pow(((strikerX+dist_x) - coinX), 2) + pow(((strikerY+dist_y) - coinY), 2));
        {
            if(((dist < rad + coins[i].getRadius()) || (dist2 < rad + coins[i].getRadius())) && StrikerCoin[i] == 0)
            {
                if(strikerX == coinX)
                {
                    impactAngle = 90.0f;
                    striker.ratio = coins[i].ratio = -1.0f;
                    strikerV = (strikerVelY + 2*coinVelY)/3.0;
                    coinV = (coinVelY*(-1) + 4*strikerVelY)/3.0;
                    striker.setVelocities(strikerX, strikerV);
                    coins[i].setVelocities(coinX, coinV);
                }
                else
                {
                    slope = ((float)(strikerY - coinY))/((float)(strikerX - coinX));
                    impactAngle = RAD2DEG(atanf(slope));
                    if(impactAngle < 0.0f)
                        impactAngle = 180 - abs(impactAngle);

                    float cosTheta = cos(DEG2RAD(impactAngle));
                    float sinTheta = sin(DEG2RAD(impactAngle));

                    strikerUx = strikerVelX*cosTheta + strikerVelY*sinTheta;
                    strikerUy = strikerVelY*cosTheta - strikerVelX*sinTheta;

                    coinUx = coinVelX*cosTheta + coinVelY*sinTheta;
                    coinUy = coinVelY*cosTheta - coinVelX*sinTheta;
                
                    strikerV = (strikerUx + 2*coinUx)/3.0;
                    coinV = (coinUx*(-1) + 4*strikerUx)/3.0;

                    striker.setVelocities(strikerV*cosTheta - strikerUy*sinTheta, strikerUy*cosTheta + strikerV*sinTheta);
                
                    if(abs(striker.getYVel()) <= 0.000001f)
                        striker.ratio = -1.0f;
                    else
                        striker.ratio = striker.getXVel()/striker.getYVel();
            
                    coins[i].setVelocities(coinV*cosTheta - coinUy*sinTheta, coinUy*cosTheta + coinV*sinTheta);
            
                    if(abs(coins[i].getYVel()) <= 0.000001f)
                        coins[i].ratio = -1.0f;
                    else
                        coins[i].ratio = coins[i].getXVel()/coins[i].getYVel();
                }

                if(striker.getXVel() < 0.0f)
                    striker.stateX = 1;
                else
                    striker.stateX = 0;

                if(striker.getYVel() < 0.0f)
                    striker.stateY = 1;
                else
                    striker.stateY = 0;

                if(coins[i].getXVel() < 0.0f)
                    coins[i].stateX = 1;
                else
                    coins[i].stateX = 0;

                if(coins[i].getYVel() < 0.0f)
                    coins[i].stateY = 1;
                else
                    coins[i].stateY = 0;
            
                StrikerCoin[i] = 1;
                //updateCoin(i);
            }
            else if(((dist > rad + coins[i].getRadius()) || (dist2 > rad + coins[i].getRadius())) && StrikerCoin[i] == 1)
                StrikerCoin[i] = 0;
        }
}

int checkjumpCollision(int i, int j)
{
    float coin1X = coins[i].getXCoordinate();
    float coin1Y = coins[i].getYCoordinate();
    float coin2X = coins[j].getXCoordinate();
    float coin2Y = coins[j].getYCoordinate();
    float factorX, factorY, dist;
    float ratio = coins[i].getXVel() / coins[i].getYVel();
    factorX = factorY = 0.001333333f;
    
    if(coin1X == coin2X)
    {
        if(coins[i].stateY == 0)
        {
            coin1Y += coins[i].getYVel()*0.2f + factorY;
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }
        else if(coins[i].stateY == 1)
        {
            coin1Y -= (coins[i].getYVel()*0.2f - factorY);
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }

        if(coins[j].stateY == 0)
        {
            coin2Y += coins[j].getYVel()*0.2f + factorY;
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }
        else if(coins[j].stateY == 1)
        {
            coin2Y -= (coins[j].getYVel()*0.2f - factorY);
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }
    }
    else
    {
        if(coins[i].stateX == 0)
        {
            coin1X += coins[i].getXVel()*0.2f + factorX;
            coin1Y += ratio*coin1X;
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }
        else if(coins[i].stateX == 1)
        {
            coin1X -= (coins[i].getXVel()*0.2f - factorX);
            coin1Y += ratio*coin1X;
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }

        if(coins[j].stateX == 0)
        {
            coin2X += coins[j].getXVel()*0.2f + factorX;
            coin2Y += ratio*coin2X;
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }
        else if(coins[j].stateX == 1)
        {
            coin2X -= (coins[j].getXVel()*0.2f - factorX);
            coin2Y += ratio*coin2X;
            dist = sqrt(pow((coin1X - coin2X), 2) + pow((coin1Y - coin2Y), 2));
            if(dist - 0.01f < 2*coins[i].getRadius())
                return 1;
        }
    }
    return 0;
}

void checkCoinCollisions(int i, int j, float dist_x, float dist_y)
{
    if(i >= 9 || j >= 9 || i < 0 || j < 0)
        return;
    
    float coin1X, coin1Y, coin1Ux, coin1Uy, coin2X, coin2Y, coin2Ux, coin2Uy, X, Y;
    float slope, impactAngle, cosTheta, sinTheta, coin1Vx, coin1Vy, coin2Vx, coin2Vy, coin1V, coin2V, dist, dist2;
    
    coin1X = coins[i].getXCoordinate();
    coin1Y = coins[i].getYCoordinate();
    X = coin1X + dist_x - 0.00133333f;
    Y = coin1Y + dist_y - 0.00133333f;
    coin1Ux = coins[i].getXVel();
    coin1Uy = coins[i].getYVel();

    coin2X = coins[j].getXCoordinate();
    coin2Y = coins[j].getYCoordinate();
    coin2Ux = coins[j].getXVel();
    coin2Uy = coins[j].getYVel();
    
    dist = sqrt(pow((coin2X - coin1X), 2) + pow((coin2Y - coin1Y), 2));
    dist2 = sqrt(pow((coin2X - X), 2) + pow((coin2Y - Y), 2));
     
    //if(CoinCoin[i][j] == 0 && CoinCoin[j][i] == 0)
    {
        if(((dist < 2*coins[i].getRadius()) || (dist2 < 2*coins[i].getRadius())) && CoinCoin[i][j] == 0 && CoinCoin[j][i] == 0)
        {
            if(coin1X == coin2X)
            {
                impactAngle = 90.0f;
                coins[i].ratio = coins[j].ratio = -1.0f;
                coins[i].setVelocities(coins[i].getXVel(), coin2Uy);
                coins[j].setVelocities(coins[j].getXVel(), coin1Uy);
            }
            else
            {
                if(coin1Y == coin2Y)
                    impactAngle = 0.0f;
                else
                {
                    slope = ((float)(coin1Y - coin2Y))/((float)(coin1X - coin2X));
                    impactAngle = RAD2DEG(atanf(slope));
                    if(impactAngle < 0.0f)
                        impactAngle = 180 - abs(impactAngle);
                }

                cosTheta = cos(DEG2RAD(impactAngle));
                sinTheta = sin(DEG2RAD(impactAngle));

                coin1Vx = coin1Ux*cosTheta + coin1Uy*sinTheta;
                coin1Vy = coin1Uy*cosTheta - coin1Ux*sinTheta;
                
                coin2Vx = coin2Ux*cosTheta + coin2Uy*sinTheta;
                coin2Vy = coin2Uy*cosTheta - coin2Ux*sinTheta;

                coin1V = coin2Vx;
                coin2V = coin1Vx;

                coins[i].setVelocities(coin1V*cosTheta - coin1Vy*sinTheta, coin1Vy*cosTheta + coin1V*sinTheta);

                if(abs(coins[i].getYVel()) <= 0.000001f)
                    coins[i].ratio = -1.0f;
                else
                    coins[i].ratio = coins[i].getXVel() / coins[i].getYVel();

                coins[j].setVelocities(coin2V*cosTheta - coin2Vy*sinTheta, coin2Vy*cosTheta + coin2V*sinTheta);

                if(abs(coins[j].getYVel()) <= 0.000001f)
                    coins[j].ratio = -1.0f;
                else
                    coins[j].ratio = coins[j].getXVel() / coins[j].getYVel();
                
                if(coins[i].getXVel() < 0.0f)
                    coins[i].stateX = 1;
                else
                    coins[i].stateX = 0;
            }
            if(coins[i].getYVel() < 0.0f)
                coins[i].stateY = 1;
            else
                coins[i].stateY = 0;
            
            if(coins[j].getXVel() < 0.0f)
                coins[j].stateX = 1;
            else
                coins[j].stateX = 0;

            if(coins[j].getYVel() < 0.0f)
                coins[j].stateY = 1;
            else
                coins[j].stateY = 0;

            CoinCoin[i][j] = 1;
            CoinCoin[j][i] = 1;
        }
        else if(((dist > 2*coins[i].getRadius()) || (dist2 > 2*coins[i].getRadius())) && CoinCoin[i][j] == 1)
            CoinCoin[i][j] = CoinCoin[j][i] = 0;
    }
    /*else if(dist > coins[i].getRadius() + coins[j].getRadius() && CoinCoin[i][j] == 1)
        CoinCoin[i][j] = CoinCoin[j][i] = 0;*/

}

int scoreTime = 30;
int timeFlag = 0;
void reduceTime(int value)
{
    scoreTime -= 1;
    timeFlag = 0;
}

void writeTime(int value)
{
    char s[] = "Score: ";
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos3f(-1.15, 0.0f, -2.0f);
    int l = strlen(s);
    for(int i=0;i<l;++i)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    
    int temp = abs(scoreTime), k=0;
    char ints[3];
    while(temp != 0)
    {
        ints[k] = temp%10 + '0';
        temp = temp/10;
        k += 1;
    }
    if(scoreTime < 0)
        ints[k++] = '-';
    else if(scoreTime == 0)
        ints[k++] = '0';
    ints[k] = '\0';
    l = strlen(ints);
    for(int i=l-1;i>=0;--i)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ints[i]);
    if(timeFlag == 0)
    {
        glutTimerFunc(1000, reduceTime, value);
        timeFlag = 1;
    }
}

void drawSpokes()
{
    float inboard_length = 0.7f;
    float angle = 0.0f;
    for(;angle<=360;angle+=45)
    {
        glLoadIdentity();
        glPushMatrix();
        glRotatef((float)angle,0.0,0.0,1.0);
        glTranslatef(0.0f,0.0f,-2.0f);
        glColor3f(0.0f,0,0);
        glBegin(GL_LINE_LOOP);
        glVertex3f(0,0,0);
        glVertex3f(0,inboard_length*0.18,0);
        glVertex3f(inboard_length*0.02,inboard_length*0.05,0);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex3f(0,0,0);
        glVertex3f(0,inboard_length*0.18,0);
        glVertex3f(-inboard_length*0.02,inboard_length*0.05,0);
        glEnd();
        glPopMatrix();
   }
}

void DrawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    drawCoins();
    drawStriker();
    
    drawAngleLine();
    drawPowerLine();
    
    writeTime(0);
    
    //drawLines();

    drawCentreCircle();
    drawPockets();
    drawStrikingLines();

    drawSpokes();
    Board board;
    board.init();

    glutSwapBuffers();
}

//Handle window resizes
void handleResize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0); 
}

float a = 0.01f;
struct timeval t1, t2;
// stateY:0 - Up; stateY:1 - Down;
// stateX:0 - Right; stateX:1 - Left

void checkPocketed(int value)
{
    if(coins[value].hide == 0)
    {
        float dist1, dist2, dist3, dist4;
        dist1 = sqrt(pow((coins[value].getXCoordinate() + 0.69f), 2) + pow((coins[value].getYCoordinate() - 0.69f), 2));
        dist2 = sqrt(pow((coins[value].getXCoordinate() + 0.69f), 2) + pow((coins[value].getYCoordinate() + 0.69f), 2));
        dist3 = sqrt(pow((coins[value].getXCoordinate() - 0.69f), 2) + pow((coins[value].getYCoordinate() - 0.69f), 2));
        dist4 = sqrt(pow((coins[value].getXCoordinate() - 0.69f), 2) + pow((coins[value].getYCoordinate() + 0.69f), 2));

        if(dist1 + 0.01f < coins[value].getRadius() + 0.05f)
            coins[value].hide = 1;
        else if(dist2 + 0.01f < coins[value].getRadius() + 0.05f)
            coins[value].hide = 1;
        else if(dist3 + 0.01f < coins[value].getRadius() + 0.05f)
            coins[value].hide = 1;
        else if(dist4 + 0.01f < coins[value].getRadius() + 0.05f)
            coins[value].hide = 1;
        if(coins[value].hide == 1)
        {
            coins[value].setCoordinates(10.0f, 10.0f);

            if(coins[value].getColour() == 0)
                scoreTime += 50;
            else if(coins[value].getColour() == 1)
                scoreTime += 10;
            else if(coins[value].getColour() == 2)
                scoreTime -= 5;
        }
    }
}

void updateCoin(int value)
{
    if(value >= 9 || value < 0)
        return;
    float X = coins[value].getXCoordinate();
    float Y = coins[value].getYCoordinate();
    float rad = coins[value].getRadius();
    float velx, vely, dist_x = 0.0f, dist_y = 0.0f;
    long long elapsed = 20;
    if((Y + rad + dist_y > 0.73f && coins[value].stateY == 0) || (Y - rad - dist_y < -0.73f && coins[value].stateY == 1))
    {
        coins[value].setVelocities(coins[value].getXVel(), coins[value].getYVel() * -1);
        coins[value].stateY = (coins[value].stateY + 1)%2;
    }
    if((X + rad + dist_x > 0.73f && coins[value].stateX == 0) || (X - rad - dist_x < -0.73f && coins[value].stateX == 1))
    {
        coins[value].setVelocities(coins[value].getXVel() * -1, coins[value].getYVel());
        coins[value].stateX = (coins[value].stateX + 1)%2;
    }
    
    if(coins[value].getXVel() > 0.0f && coins[value].stateX == 0)
    {
        velx = coins[value].getXVel() - a*(float(elapsed/1000.0));
        if(velx <= 0.0f)
            velx = 0.0f;
    }
    else if(coins[value].getXVel() < 0.0f && coins[value].stateX == 1)
    {
        velx = coins[value].getXVel() + a*(float(elapsed/1000.0));
        if(velx >= 0.0f)
            velx = 0.0f;
    }
    else
        velx = 0.0f;
    
    if(coins[value].ratio != -1.0f)
    {
        vely = velx / coins[value].ratio;
        if(velx == 0.0f)
            vely = 0.0f;
        else if(coins[value].getYVel() > 0.0f && vely < 0.0f)
            vely *= -1;
        else if(coins[value].getYVel() < 0.0f && vely > 0.0f)
            vely *= -1;
    }
    else
    {
        if(coins[value].getYVel() > 0.0f && coins[value].stateY == 0)
            vely = coins[value].getYVel() - a*(float(elapsed/1000.0));
        else if(coins[value].getYVel() < 0.0f && coins[value].stateY == 1)
            vely = coins[value].getYVel() + a*(float(elapsed/1000.0));
        else
            vely = 0.0f;
    }
    
    if(abs(coins[value].getXVel()) <= 0.000001f)
        dist_x = 0.0f;
    else if(coins[value].getXVel() > 0.0f)
        dist_x = coins[value].getXVel()*(elapsed/100.0) + 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    else if(coins[value].getXVel() < 0.0f)
        dist_x = coins[value].getXVel()*(elapsed/100.0) - 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    
    if(abs(coins[value].getYVel()) <= 0.000001f)
        dist_y = 0.0f;
    else if(coins[value].getYVel() > 0.0f)
        dist_y = coins[value].getYVel()*(elapsed/100.0) + 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    else if(coins[value].getYVel() < 0.0f)
        dist_y = coins[value].getYVel()*(elapsed/100.0) - 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    
    coins[value].setVelocities(velx, vely);
    
    X += dist_x;
    Y += dist_y;
    
    coins[value].setCoordinates(X, Y);
    for(int i=0;i<9;++i)
    {
        if(i != value)
            checkCoinCollisions(i, value, dist_x, dist_y);
    }
    
    /*for(int i=0;i<9;++i)
        CoinCoin[value][i] = 0;*/
    
    checkPocketed(value);

    glutTimerFunc(10, updateCoin, value);
}

void updateStriker(int value)
{
    if(value >= 9 || value < 0 || StrikeFlag == 0)
        return;
    float X = striker.getXCoordinate();
    float Y = striker.getYCoordinate();
    float rad = striker.getRadius();
    float velx, vely, dist_x=0.0f, dist_y=0.0f;
    gettimeofday(&t2, NULL);
    
    long long t1_time = t1.tv_sec * 1000 + t1.tv_usec / 1000;
    long long t2_time = t2.tv_sec * 1000 + t2.tv_usec / 1000;
    long long elapsed = t2_time - t1_time;
    
    if((Y + rad + dist_y > 0.73f && striker.stateY == 0) || (Y - rad - dist_y < -0.73f && striker.stateY == 1))
    {
        striker.setVelocities(striker.getXVel(), striker.getYVel() * -1);
        striker.stateY = (striker.stateY + 1)%2;
    }
    if((X + rad + dist_x > 0.73f && striker.stateX == 0) || (X - rad - dist_x < -0.73f && striker.stateX == 1))
    {
        striker.setVelocities(striker.getXVel() * -1, striker.getYVel());
        striker.stateX = (striker.stateX + 1)%2;
    }
    
    if(striker.getXVel() > 0.0f && striker.stateX == 0)
    {
        velx = striker.getXVel() - a*(float(elapsed/1000.0));
        if(velx <= 0.0f)
            velx = 0.0f;
    }
    else if(striker.getXVel() < 0.0f && striker.stateX == 1)
    {
        velx = striker.getXVel() + a*(float(elapsed/1000.0));
        if(velx >= 0.0f)
            velx = 0.0f;
    }
    else
        velx = 0.0f;
    
    if(striker.ratio != -1)
    {
        vely = velx / striker.ratio;
        if(velx == 0.0f)
            vely = 0.0f;
        else if(striker.getYVel() > 0.0f && vely < 0.0f)
            vely *= -1;
        else if(striker.getYVel() < 0.0f && vely > 0.0f)
            vely *= -1;
    }
    else
    {
        if(striker.getYVel() > 0.0f && striker.stateY == 0)
            vely = striker.getYVel() - a*(float(elapsed/1000.0));
        else if(striker.getYVel() < 0.0f && striker.stateY == 1)
            vely = striker.getYVel() + a*(float(elapsed/1000.0));
        else
            vely = 0.0f;
    }

    if(abs(striker.getXVel()) <= 0.000001f)
        dist_x = 0.0f;
    else if(striker.getXVel() > 0.0f)
        dist_x = striker.getXVel()*(elapsed/50.0) + 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    else if(striker.getXVel() < 0.0f)
        dist_x = striker.getXVel()*(elapsed/50.0) - 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    
    if(abs(striker.getYVel()) <= 0.000001f)
        dist_y = 0.0f;
    else if(striker.getYVel() > 0.0f)
        dist_y = striker.getYVel()*(elapsed/50.0) + 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    else if(striker.getYVel() < 0.0f)
        dist_y = striker.getYVel()*(elapsed/50.0) - 0.5*a*(elapsed/50.0)*(elapsed/30.0);
    
    striker.setVelocities(velx, vely);
    
    X += dist_x;
    Y += dist_y;
    striker.setCoordinates(X, Y);

    flag = 0;
    for(int i=0;i<9;++i)
            checkStrikerCollisions(i, dist_x, dist_y);

    /*for(int i=0;i<9;++i)
        StrikerCoin[i] = 0;*/
    
    t1 = t2;

    glutTimerFunc(10, updateStriker, 0);
}

void handleKeypress1(unsigned char key, int x, int y)
{
    if(key == 27)       //Escape key is pressed
        exit(0);
    else if(key == 32 && StrikeFlag == 0)  //Space key is pressed
    {
        gettimeofday(&t1, NULL);
        if(theta != 0.0f)
        {
            float temp = 90.0f - theta;
            striker.setVelocities(striker.getRadius() * cos(DEG2RAD(temp)) * power/2, striker.getRadius() * sin(DEG2RAD(temp)) * power/2);
            if(theta < 0.0f)
                striker.stateX = 1;
                else
                striker.stateX = 0;
            striker.ratio = striker.getXVel() / striker.getYVel();
        }
        else
            striker.setVelocities(striker.getXVel(), striker.getYVel() + ((power-2.0f)/100));
        StrikeFlag = 1;
        updateStriker(0);
    }
    else if(key == 97)
    {
        if(theta <= -75.0f)
            return;
        theta -= 5.0f;
    }
    else if(key == 99)
    {
        if(theta >= 75.0f)
            return;
        theta += 5.0f;
    }
    else if(key == 114)
        initStrikerValues();
}

void handleKeypress2(int key, int x, int y)
{
    float X, Y;
    X = striker.getXCoordinate();
    Y = striker.getYCoordinate();
    if(Y == -0.47f)
    {
        if(key == GLUT_KEY_LEFT)
        {
            if(X <= -0.45f)
                return;
            if(X <= -0.39f)
                X = -0.45f;
            else if(X == 0.45f)
                X -= 0.06f;
            else
                X -= 0.03f;
            striker.setCoordinates(X, Y);
        }
        else if(key == GLUT_KEY_RIGHT)
        {
            if(X >= 0.45f)
                return;
            if(X >= 0.39f)
                X = 0.45f;
            else if(X == -0.45f)
                X += 0.06f;
            else
                X += 0.03f;
            striker.setCoordinates(X, Y);
        }
        else if(key == GLUT_KEY_UP)
        {
            if(power >= 10.0f)
                return;
            power += 1.0f;
        }
        else if(key == GLUT_KEY_DOWN)
        {
            if(power <= 2.0f)
                return;
            power -= 1.0f;
        }
    }
}

void handleHover(int x, int y)
{
    x = x - 1365.0/2;
    y = 767/2.0 - y;
    float x1, y1;
    x1 = (float)((float)x/480.0);
    y1 = (float)((float)y/480.0);
    float temp = RAD2DEG(atanf((y1+0.45)/(x1)));
    if(abs(temp) <= 15.0f)
        return;

    if(temp > 0.0f)
        theta = 90.0f - temp;
    else
    {
        temp = 180 - abs(temp);
        theta = 90.0f - temp;
    }
}

void setDragFlag(int button, int state, int x, int y) 
{
    if(button == GLUT_LEFT_BUTTON) 
    {
        if(StrikeFlag == 0)
        {
            gettimeofday(&t1, NULL);
            if(theta != 0.0f)
            {
                float temp = 90.0f - theta;
                striker.setVelocities(striker.getRadius() * cos(DEG2RAD(temp)) * power/2, striker.getRadius() * sin(DEG2RAD(temp)) * power/2);
                if(theta < 0.0f)
                    striker.stateX = 1;
                    else
                    striker.stateX = 0;
                striker.ratio = striker.getXVel() / striker.getYVel();
            }
            else
                striker.setVelocities(striker.getXVel(), striker.getYVel() + ((power-2.0f)/100));
            StrikeFlag = 1;
            updateStriker(0);
        }
    }
}

int main(int argc, char **argv)
{
    //Initialise
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 1000);

    //Create the window
    glutCreateWindow("Carrom");
    glutFullScreen();
    initRendering();
    initCoinPositions();
    initStrikerValues();

    //Draw the scene
    glutDisplayFunc(DrawScene);
    glutIdleFunc(DrawScene);
    glutReshapeFunc(handleResize);
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutPassiveMotionFunc(handleHover);
    glutMouseFunc(setDragFlag);

    glutMainLoop();
    return 0;
}
