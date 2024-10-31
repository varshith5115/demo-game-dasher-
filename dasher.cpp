#include "raylib.h"
struct AnimData
{
    Rectangle Rec;
    Vector2 Pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.Pos.y >= windowHeight - data.Rec.height;
} 

AnimData updateAnimData(AnimData data,float deltaTime, int maxFrame)
{
    //update running time
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update animation frame
        data.Rec.x = data.frame * data.Rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    int  windowDimensions[2];
    windowDimensions[0] = 510;//width
    windowDimensions[1] = 350;//height
    
    InitWindow(windowDimensions[0],windowDimensions[1],"dasher");
    //acceleration due to gravity
    const int gravity(1'000);
    //nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{3};
    AnimData nebulae[sizeOfNebulae]{};
    
    for(int i=0; i < sizeOfNebulae; i++)
    {
        nebulae[i].Rec.x = 0.0;
        nebulae[i].Rec.y = 0.0;
        nebulae[i].Rec.width = nebula.width/8;
        nebulae[i].Rec.height = nebula.height/8;
        nebulae[i].Pos.y = windowDimensions[1]-nebula.height/8;
        nebulae[i].Pos.x = windowDimensions[0] + i * 400;
        nebulae[i].frame = 0.0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/16.0;
        
    }

    float finishLine{nebulae[sizeOfNebulae - 1].Pos.x + 150};

    //nebula velocity
    int nebVel{-200};

    //scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.Rec.width = scarfy.width/6;
    scarfyData.Rec.height = scarfy.height;
    scarfyData.Rec.x=0;
    scarfyData.Rec.y=0;
    scarfyData.Pos.x = windowDimensions[0]/2 - scarfyData.Rec.width/2;
    scarfyData.Pos.y = windowDimensions[1] - scarfyData.Rec.height;
    scarfyData.frame=0;
    scarfyData.updateTime=1.0/12.0;
    scarfyData.runningTime=0;
    
    //is rect in air
    bool inAir{};
    //jump velocity
    const int jumpvel{-600};

    int velocity{0};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgx{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgx{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgx;

    bool collision{};    

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {   
        const float dt{GetFrameTime()};
        BeginDrawing();
        ClearBackground(WHITE);
        
        // scroll background
        bgx -= 20 * dt;
        if(bgx <= -background.width*2)
        {
            bgx =0.0;
        }

        mgx -= 40 * dt;
        if(mgx <= -midground.width*2)
        {
            mgx =0.0;
        }

        fgx -= 60 * dt;
        if(fgx <= -foreground.width*2)
        {
            fgx =0.0;
        }

        //draw the background
        Vector2 bg1Pos{bgx,0.0};
        DrawTextureEx(background, bg1Pos,0.0,2.0,WHITE);
        Vector2 bg2Pos{bgx + background.width * 2,0.0};
        DrawTextureEx(background,bg2Pos,0.0,2.0,WHITE);

        Vector2 mg1Pos{mgx,0.0};
        DrawTextureEx(midground,mg1Pos,0.0,2.0,WHITE);
        Vector2 mg2Pos{mgx + midground.width * 2,0.0};
        DrawTextureEx(midground,mg2Pos,0.0,2.0,WHITE);

        Vector2 fg1Pos{fgx,0.0};
        DrawTextureEx(foreground, fg1Pos,0.0,2.0,WHITE);
        Vector2 fg2Pos{fgx + foreground.width * 2,0.0};
        DrawTextureEx(foreground,fg2Pos,0.0,2.0,WHITE);

        if(isOnGround(scarfyData,windowDimensions[1]))
        {
            velocity=0;
            inAir=false;
        }
        else
        {
            velocity+=gravity*dt;
            inAir=true;
        }
        if(IsKeyPressed(KEY_SPACE)&&!inAir)
        {
            velocity+=jumpvel;
        }

        for(int i=0; i < sizeOfNebulae; i++)
        {
            //update nebula positon
            nebulae[i].Pos.x += nebVel*dt;
        }

        //upadte finish line
        finishLine += nebVel * dt;
        //update scarfy postion
        scarfyData.Pos.y += velocity*dt;

        //update scarfy animation
        if(!inAir)
        {
            scarfyData = updateAnimData(scarfyData,dt,5);
        }

        for(int i=0; i < sizeOfNebulae; i++)
        {
           nebulae[i] = updateAnimData(nebulae[i],dt,7);
        }
        
        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.Pos.x + pad,
                nebula.Pos.y + pad,
                nebula.Rec.width - 2*pad,
                nebula.Rec.height - 2*pad
            };
            Rectangle scarfyRec{
                scarfyData.Pos.x,
                scarfyData.Pos.y,
                scarfyData.Rec.width,
                scarfyData.Rec.height
            };
            if(CheckCollisionRecs(nebRec,scarfyRec))
            {
                collision = true;
            }
        }

        if(collision)
        {
            //lost
            DrawText("Game Over!",windowDimensions[0]/4,windowDimensions[1]/2,40,RED);
        }

        else if (scarfyData.Pos.x >= finishLine)
        {
            //Won
            DrawText("You Won!",windowDimensions[0]/4,windowDimensions[1]/2,40,RED);
        }
        
        else
        {
            for(int i=0; i < sizeOfNebulae; i++)
            {
                //drawing nebula rectangle
                DrawTextureRec(nebula,nebulae[i].Rec,nebulae[i].Pos,WHITE);
            }

            DrawTextureRec(scarfy,scarfyData.Rec,scarfyData.Pos,WHITE);
        }
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

}