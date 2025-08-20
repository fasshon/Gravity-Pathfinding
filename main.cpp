#include <iostream>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

#define M_PI 3.14159f

// -------------------- Vector2 --------------------
struct Vector2 {
    float x, y;
};

// -------------------- Globals --------------------
Vector2 ScreenSize = { 1400, 1000 };

struct Player {
    std::string Name;
    float X, Y;
    float Radius = 30.0f;
    int Segments = 50;
    Vector2 Velocity = { 0, 0 };
    float mass;
};

std::vector<Player> PlayerList;

// -------------------- Prototypes --------------------
GLFWwindow* StartGLFW();
float CalcDCOM(float FX, float FY, float SX, float SY);
void DrawCircle(float x, float y, float radius, int segments);
void LoadPlayers();
void UpdatePositions();
void Controls();
void CheckCollision();
void DrawPlayers();
void render();
void ApplyGravity();
// -------------------- Circle Rendering --------------------
void DrawCircle(float x, float y, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center
    for (int i = 0; i <= segments; i++) {
        float angle = i * 2.0f * M_PI / segments;
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// -------------------- Main --------------------
int main() {
    srand((unsigned)time(0));
    GLFWwindow* window = StartGLFW();
    if (!window) return -1;

    // Setup projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ScreenSize.x, 0, ScreenSize.y, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    LoadPlayers();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// -------------------- GLFW Init --------------------
GLFWwindow* StartGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }
    GLFWwindow* window = glfwCreateWindow(ScreenSize.x, ScreenSize.y, "Circle Collision Demo", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

// -------------------- Load Players --------------------
void LoadPlayers() {
    Player user;
    user.Name = "User";
    user.X = ScreenSize.x / 2;
    user.Y = ScreenSize.y / 2;
    user.mass = 10000;
    PlayerList.push_back(user);

    Player enemy;
    enemy.Name = "Enemy";
    enemy.X = rand() % (int)(ScreenSize.x - 100) + 50;
    enemy.Y = rand() % (int)(ScreenSize.y - 100) + 50;
    enemy.mass = 10000;
    PlayerList.push_back(enemy);
}

// -------------------- Controls --------------------
void Controls() {
    Player& user = PlayerList[0];

    if (GetAsyncKeyState('W') & 0x8000) user.Velocity.y = 5;
    if (GetAsyncKeyState('S') & 0x8000) user.Velocity.y = -5;
    if (GetAsyncKeyState('A') & 0x8000) user.Velocity.x = -5;
    if (GetAsyncKeyState('D') & 0x8000) user.Velocity.x = 5;
}

// -------------------- Update Positions --------------------
void UpdatePositions() {
    for (auto& p : PlayerList) {
        p.X += p.Velocity.x;
        p.Y += p.Velocity.y;

        // Simple friction
        p.Velocity.x *= 0.9f;
        p.Velocity.y *= 0.9f;

        // Keep inside screen
        if (p.X - p.Radius < 0 || p.X + p.Radius > ScreenSize.x) {
            p.Velocity.x *= -1;
        }
        if (p.Y - p.Radius < 0 || p.Y + p.Radius > ScreenSize.y) {
            p.Velocity.y *= -1;
        }
    }
}

// -------------------- Collision --------------------
void CheckCollision() {
    for (int i = 0; i < PlayerList.size(); i++) {
        for (int j = i + 1; j < PlayerList.size(); j++) {
            float dx = PlayerList[j].X - PlayerList[i].X;
            float dy = PlayerList[j].Y - PlayerList[i].Y;
            float distSq = dx * dx + dy * dy;
            float radiusSum = PlayerList[i].Radius + PlayerList[j].Radius;

            if (distSq <= radiusSum * radiusSum) {
                std::cout << "Collision: "
                    << PlayerList[i].Name << " <-> "
                    << PlayerList[j].Name << "\n";

                // Arcade bounce: swap velocities
                std::swap(PlayerList[i].Velocity, PlayerList[j].Velocity);
            }
        }
    }
}

// -------------------- Drawing --------------------
void DrawPlayers() {
    for (auto& p : PlayerList) {
        DrawCircle(p.X, p.Y, p.Radius, p.Segments);
    }
}

// -------------------- Render --------------------
void render() {
    Controls();
    UpdatePositions();
    CheckCollision();
    DrawPlayers();
    ApplyGravity();
}


void ApplyGravity()
{
    Player& user = PlayerList[0];
    Player& enemy = PlayerList[1];
    float G = 0.00095;
    float DCOM = CalcDCOM(user.X, user.Y, enemy.X, enemy.Y);
    
    float force = G * enemy.mass * user.mass / DCOM;

    float ax = force * (user.X - enemy.X) / DCOM / enemy.mass;
    float ay = force * (user.Y - enemy.Y) / DCOM / enemy.mass;

    enemy.Velocity.x += ax;
    enemy.Velocity.y += ay;

    enemy.X += enemy.Velocity.x;
    enemy.Y += enemy.Velocity.y;
}

float CalcDCOM(float FX, float FY,float SX,float SY)
{
    float dx = FX - SX;
    float dy = FY - SY;

    return sqrt(dx * dx + dy * dy);
}
