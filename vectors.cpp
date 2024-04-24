#include <raylib.h>
#include <iostream>
#include <math.h>
#include <random>

using namespace std;

const int winHeight = 800;
const int winWidth = 1200;
const int fps = 100;
const double deltaTime = 0.01;
static int cursorIsOff = 1;

class hole
{
public:
    Vector2 positon;
    int radius;

    hole() {}

    hole(Vector2 positon, int radius)
    {
        this->positon = positon;
        this->radius = radius;
    }
};

class particle
{
public:
    Vector2 maxVelocity = {16, 16};
    Vector2 position;
    int radius;
    Vector2 velocity = {0, 0};
    double drag = 0.5;
    particle() {}

    particle(Vector2 position, int radius)
    {
        this->position = position;
        this->radius = radius;
    }

    void checkCollosionWithWall()
    {
        if (((position.x + radius) >= winWidth && velocity.x > 0) || ((position.x - radius) <= 0 && velocity.x < 0))
        {
            velocity.x = -velocity.x;
        }
        if (((position.y + radius) >= winHeight && velocity.y > 0) || ((position.y - radius) <= 0 && velocity.y < 0))
        {
            velocity.y = -velocity.y;
        }
    }

    void drawTrail(Vector2 resulantVeclocity)
    {
        DrawLine(position.x, position.y, position.x + resulantVeclocity.x / 2, position.y + resulantVeclocity.y / 2, WHITE);
    }

    Vector2 motionInitilization(Vector2 mouse)
    {
        Vector2 resultantVector = {position.x - mouse.x, position.y - mouse.y};
        velocity.x = resultantVector.x * 0.01;
        velocity.y = resultantVector.y * 0.01;

        if (velocity.x < -1 * maxVelocity.x)
            velocity.x = -1 * maxVelocity.x;
        if (velocity.x > maxVelocity.x)
            velocity.x = maxVelocity.x;

        if (velocity.y < -1 * maxVelocity.y)
            velocity.y = -1 * maxVelocity.y;
        if (velocity.y > maxVelocity.y)
            velocity.y = maxVelocity.y;

        drawTrail({velocity.x, velocity.y});
        return resultantVector;
    }

    void move()
    {
        position.x += velocity.x;
        position.y += velocity.y;
        velocity.x = velocity.x > 0 ?velocity.x - ( velocity.x  * drag )/fps : velocity.x + (abs(velocity.x) * drag )/ fps;
        velocity.y = velocity.y > 0 ?velocity.y - (velocity.y  * drag )/fps : velocity.y +(abs(velocity.y) * drag )/ fps;

        if(abs(velocity.x) <1) velocity.x = 0;
        if(abs(velocity.y) <1) velocity.y = 0;
        checkCollosionWithWall();
    }

    void gotoHole(hole thisHole)
    {
        position = thisHole.positon;
        position = thisHole.positon;
    }
};

class obstracle
{
public:
    Vector2 position;
    int width;
    int height;
    vector<Rectangle> recStuff;

    obstracle() {}

    // obstracle(Vector2 position, int width, int height)
    // {
    //     this->position = position;
    //     this->width = width;
    //     this->height = height;
    // }

    obstracle (int level)
    {
        for(int i=0;i<level ;i++)
        {
            Rectangle rec = {(float)GetRandomValue(0, winWidth),(float)GetRandomValue(0, winHeight),(float)GetRandomValue(0, 100),(float)GetRandomValue(0, 100)};
            recStuff.push_back(rec);
        }
    }
};

void resetScreen(particle &ball, hole &thisHole,vector<Rectangle> &ob, int level)
{
    obstracle newOb(level);
    ob = newOb.recStuff;
    ball.velocity ={0,0};

    ball.position ={(float)GetRandomValue((int)ball.radius,(int)(winWidth-ball.radius)),(float)GetRandomValue((int)ball.radius,(int)(winHeight-ball.radius))};
    thisHole.positon ={(float)GetRandomValue((int)ball.radius,(int)(winWidth-ball.radius)),(float)GetRandomValue((int)ball.radius,(int)(winHeight-ball.radius))};
}

void reflectFromObstracle(Rectangle ob1, particle &ball)
{
    if ((ball.position.x + ball.radius) >= (ob1.x + ob1.width) || ((ball.position.x - ball.radius) <= ob1.x))
    {
        ball.velocity.x = -ball.velocity.x;
    }
    if ((ball.position.y + ball.radius) >= (ob1.y + ob1.height) || ((ball.position.y - ball.radius) <= ob1.y))
    {
        ball.velocity.y = -ball.velocity.y;
    }
}

int main()
{

    SetTargetFPS(fps);
    InitWindow(winWidth, winHeight, "Vectors");

    particle ball({winWidth / 2, winHeight / 2}, 12);

    hole thisHole({100, 600}, ball.radius + 5);

    int level = 1;
    obstracle obs(level);

    Texture2D backgroundImage = LoadTexture("Grass.png");
    

    while (!WindowShouldClose())
    {

        DrawTexture(backgroundImage, 0, 0, WHITE);
        BeginDrawing();

        DrawCircle(ball.position.x, ball.position.y, ball.radius, WHITE);
        DrawCircle(thisHole.positon.x, thisHole.positon.y, thisHole.radius, RED);
        

        // initilize same no. of obstracle as that of level;
        for(int i=0;i<level;i++)
        {
            DrawRectangleRec(obs.recStuff[i],WHITE);
        }

        Vector2 mouse = GetMousePosition();
        Vector2 ResultantVector;

        

        if (IsMouseButtonDown(0))
        {
            ResultantVector = ball.motionInitilization(mouse);
            ball.drawTrail(ResultantVector);
        }
    
        if (!IsMouseButtonDown(0))
        {
            ball.move();
        }



        if((ball.velocity.x ==0 && ball.velocity.y ==0) || ((int)(ball.velocity.x )== (int)(ResultantVector.x * 0.01)))
        {
            if(cursorIsOff == 1)
            {
                EnableCursor();
                cursorIsOff = 0;
            }
        }
        else{
            DisableCursor();
            cursorIsOff = 1;
        }
        // Collosion With Obstracle
        for(int i=0;i<level;i++)
        {
            if(CheckCollisionCircleRec(ball.position,ball.radius,obs.recStuff[i]))
            {
                reflectFromObstracle(obs.recStuff[i],ball);
            }
        }
        

        // Check PAR
        if (CheckCollisionCircles(ball.position, ball.radius, thisHole.positon, thisHole.radius))
        {
            ball.gotoHole(thisHole);
            level+=1;
            resetScreen(ball, thisHole,obs.recStuff,level);
        }

        ClearBackground(BLACK);
        EndDrawing();
    }
}