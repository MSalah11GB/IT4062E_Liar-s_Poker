#include "raylib/src/raylib.h"
#include "raygui/src/raygui.h"
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER 32

int main(void)
{
    // Initialize window
    InitWindow(800, 600, "Raygui Login Example");
    SetTargetFPS(60);

    // Buffers for user input
    char username[MAX_BUFFER] = "";
    char password[MAX_BUFFER] = "";

    // For masked password display
    char masked[MAX_BUFFER];

    while (!WindowShouldClose())
    {
        // Mask password characters
        int len = strlen(password);
        for (int i = 0; i < len; i++)
            masked[i] = '*';
        masked[len] = '\0';

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Username
        DrawText("Username:", 50, 50, 20, BLACK);
        GuiTextBox((Rectangle){150, 50, 200, 30}, username, MAX_BUFFER, true);

        // Password
        DrawText("Password:", 50, 100, 20, BLACK);
        GuiTextBox((Rectangle){150, 100, 200, 30}, masked, MAX_BUFFER, true);

        // Login button
        if (GuiButton((Rectangle){150, 150, 100, 30}, "Login"))
        {
            printf("Username: %s\nPassword: %s\n", username, password);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
