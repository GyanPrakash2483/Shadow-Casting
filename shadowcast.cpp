#include <iostream>
#include <raylib.h>
#include <string>
#include <cstring>

enum TileComponent {
    FREE,
    BLOCK
};

void DrawTriangle2D(Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
    DrawTriangle(v1, v2, v3, color);
    DrawTriangle(v1, v3, v2, color);
}

void DrawQuadrilateral(Vector2 v1,Vector2 v2, Vector2 v3, Vector2 v4, Color color) {
    DrawTriangle2D(v1, v2, v3, color);
    DrawTriangle2D(v1, v3, v4, color);
}

typedef struct {
    float m;
    float c;
} Line;

Line LineFromPoints(Vector2 p1, Vector2 p2) {
    if(p2.x == p1.x) {
        return (Line) {
            9999,
            p1.y - 9999 * p1.x
        };
    }
    return (Line){
        (p2.y - p1.y) / (p2.x - p1.x),
        p1.y - ((p2.y - p1.y) / (p2.x - p1.x)) * p1.x
    };
}

Line LineFromPointsSlope(Vector2 point, float slope) {
    return (Line){
        slope,
        point.y - slope * point.x
    };
}

Vector2 CalcIntersection(Line l1, Line l2) {
    return (Vector2){
        ((l2.c - l1.c) / (l1.m - l2.m)),
        l1.m * ((l2.c - l1.c) / (l1.m - l2.m)) + l1.c
    };
}

int main(int argc, char *argv[]) {

    bool vsync = true;

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "--no-vsync") == 0) {
            vsync = false;
        }
    }
    
    InitWindow(800, 800, "Shadow Casting");

    unsigned int TileBox[80][80] = {0};

    Image lightimage = LoadImage("./assets/light.jpg");
    Texture light = LoadTextureFromImage(lightimage);
    UnloadImage(lightimage);

    if(vsync) {
        SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    }

    std::cout << "\nInstructions:\n\n"
    "Right Mouse Button: add obstruction\n"
    "Right Mouse Button + del: remove obstruction\n"
    "Left Mouse Button: light source " << std::endl;

    while(!WindowShouldClose()) {

        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            TileBox[GetMouseX() / 10][GetMouseY() / 10] = BLOCK;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && IsKeyDown(KEY_DELETE)) {
            TileBox[GetMouseX() / 10][GetMouseY() / 10] = FREE;
        }


        BeginDrawing();

        ClearBackground(BLACK);

        std::string fpstext = "FPS: ";
        fpstext.append(std::to_string(GetFPS()));

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            DrawTexture(light, GetMouseX() - light.width / 2, GetMouseY() - light.height / 2, WHITE);
        }
        for(int i = 0; i < 80; i++) {
            for(int j = 0; j < 80; j++) {
                if(TileBox[i][j] == BLOCK) {
                    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        Vector2 btl = (Vector2){i * 10, j * 10};
                        Vector2 btr = (Vector2){btl.x + 10, btl.y};
                        Vector2 bbl = (Vector2){btl.x, btl.y + 10};
                        Vector2 bbr = (Vector2){btl.x + 10, btl.y + 10};
                        
                        Vector2 mousepos = GetMousePosition();

                        if(mousepos.y > bbl.y) {

                            Line leftline = LineFromPoints(mousepos, bbl);
                            Line rightline = LineFromPoints(mousepos, bbr);
                            Line topline = LineFromPointsSlope((Vector2){0, 0}, 0);

                            Vector2 farleftpoint = CalcIntersection(topline, leftline);
                            Vector2 farrightpoint = CalcIntersection(topline, rightline);

                            DrawQuadrilateral(bbr, bbl, farleftpoint, farrightpoint, BLACK);
                        }
                        if(mousepos.y < btr.y) {

                            Line leftline = LineFromPoints(mousepos, btl);
                            Line rightline = LineFromPoints(mousepos, btr);
                            Line bottomline = LineFromPointsSlope((Vector2){0, 800}, 0);

                            Vector2 farleftpoint = CalcIntersection(bottomline, leftline);
                            Vector2 farrightpoint = CalcIntersection(bottomline, rightline);

                            DrawQuadrilateral(btr, btl, farleftpoint, farrightpoint, BLACK);
                        }
                        if(mousepos.x < btl.x) {

                            Line topline = LineFromPoints(mousepos, btl);
                            Line bottomline = LineFromPoints(mousepos, bbl);
                            Line rightline = LineFromPointsSlope((Vector2){800, 0}, 9999);

                            Vector2 fartoppoint = CalcIntersection(topline, rightline);
                            Vector2 farbottompoint = CalcIntersection(bottomline, rightline);

                            DrawQuadrilateral(btl, bbl, farbottompoint, fartoppoint, BLACK);
                        }
                        if(mousepos.x > btr.x) {

                            Line topline = LineFromPoints(mousepos, btr);
                            Line bottomline = LineFromPoints(mousepos, bbr);
                            Line leftline = LineFromPointsSlope((Vector2){0, 0}, 9999);

                            Vector2 fartoppoint = CalcIntersection(topline, leftline);
                            Vector2 farbottompoint = CalcIntersection(bottomline, leftline);

                            DrawQuadrilateral(btr, bbr, farbottompoint, fartoppoint, BLACK);
                        }
                    }
                }
            }
        }
        
        for(int i = 0; i < 80; i++) {
            for(int j = 0; j < 80; j++) {
                if(TileBox[i][j] == BLOCK) {
                    DrawRectangle(i * 10, j * 10, 10, 10, BLUE);
                }
            }
        }

        DrawText(fpstext.c_str(), 10, 10, 10, WHITE);

        EndDrawing();
    }

    UnloadTexture(light);
    CloseWindow();

    return 0;
}