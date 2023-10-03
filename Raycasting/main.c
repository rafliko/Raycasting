#include "raylib.h"
#include "raymath.h"

#define mapWidth 24
#define mapHeight 24
#define screenWidth 1440
#define screenHeight screenWidth/2
#define unit2d screenHeight / mapHeight
#define degree 2*PI/360
#define fov 60
#define unit3d screenHeight/fov

int worldMap[mapHeight][mapWidth] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,4,0,3,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,2,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

Vector2 player = { 12, 12 };
float move_speed = 5.0f;
float rotation_speed = 200.0f;

Vector2 rotation_vector = { 0, -1 };

struct
{
    Vector2 vector;
    float len;
    Color col;
} ray[fov];

Color getColorFromVal(int val)
{
    switch (val)
    {
    default:
        return BLACK;
        break;
    case 1:
        return SKYBLUE;
        break;
    case 2:
        return GREEN;
        break;
    case 3:
        return ORANGE;
        break;
    case 4:
        return RED;
        break;
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "raylib - raycaster");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_LEFT)) rotation_vector = Vector2Rotate(rotation_vector, -degree * GetFrameTime() * rotation_speed);
        if (IsKeyDown(KEY_RIGHT)) rotation_vector = Vector2Rotate(rotation_vector, degree * GetFrameTime() * rotation_speed);
        if (IsKeyDown(KEY_UP))
        {
            player.x += rotation_vector.x * GetFrameTime() * move_speed;
            player.y += rotation_vector.y * GetFrameTime() * move_speed;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            player.x -= rotation_vector.x * GetFrameTime() * move_speed;
            player.y -= rotation_vector.y * GetFrameTime() * move_speed;
        }
        for (int i = 0; i < fov; i++)
        {
            bool detected = false;
            float tmpx = player.x;
            float tmpy = player.y;
            while (!detected)
            {
                tmpx += Vector2Rotate(rotation_vector, degree * (i-fov/2)).x * 0.1f;
                if (worldMap[(int)tmpy][(int)tmpx] != 0) // horizontal
                {
                    detected = true;
                    ray[i].vector = (Vector2){ tmpx, tmpy };
                    ray[i].len = sqrt(pow(tmpx - player.x, 2) + pow(tmpy - player.y, 2)) * cos(degree * (i - fov / 2));
                    ray[i].col = getColorFromVal(worldMap[(int)tmpy][(int)tmpx]);
                    break;
                }
                tmpy += Vector2Rotate(rotation_vector, degree * (i-fov/2)).y * 0.1f;
                if (worldMap[(int)tmpy][(int)tmpx] != 0) // vertical
                {
                    detected = true;
                    ray[i].vector = (Vector2){ tmpx, tmpy };
                    ray[i].len = sqrt(pow(tmpx - player.x, 2) + pow(tmpy - player.y, 2)) * cos(degree * (i - fov / 2));
                    ray[i].col = getColorFromVal(worldMap[(int)tmpy][(int)tmpx]);
                    ray[i].col.r -= 0.2f * ray[i].col.r;
                    ray[i].col.g -= 0.2f * ray[i].col.g;
                    ray[i].col.b -= 0.2f * ray[i].col.b;
                    break;
                }
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectangle(screenHeight, 0, screenHeight, screenHeight / 2, GRAY);
        DrawRectangle(screenHeight, screenHeight/2, screenHeight, screenHeight/2, LIGHTGRAY);

        for (int x = 0; x < mapWidth; x++)
        {
            for (int y = 0; y < mapHeight; y++)
            {
                Color c = getColorFromVal(worldMap[y][x]);
                DrawRectangle(x * unit2d, y * unit2d, unit2d, unit2d, c);
            }
        }

        DrawCircle(player.x * unit2d, player.y * unit2d, 5, YELLOW);
        for (int i = 0; i < fov; i++)
        {
            DrawLine(player.x * unit2d, player.y * unit2d, ray[i].vector.x * unit2d, ray[i].vector.y * unit2d, ray[i].col);
            DrawRectangle(i * unit3d+screenHeight, screenHeight/2-(1 / ray[i].len * 500)/2, unit3d, 1/ray[i].len * 500, ray[i].col);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}