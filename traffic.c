#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define MAX_CARS 4
#define PI 3.14159265

/* --- STRUCTURES --- */
typedef struct {
    float x, y;
    float speed;
    float maxSpeed;
    int direction; // 0: Right, 1: Left, 2: Up, 3: Down
    float r, g, b;
} Car;

/* --- GLOBAL VARIABLES --- */
Car cars[MAX_CARS];
int signalState = 0; // 0: NS Green, 1: NS Yellow, 2: EW Green, 3: EW Yellow
int timer = 0;

/* --- DRAWING FUNCTIONS --- */
void drawCircle(float x, float y, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * PI / 20.0f;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void initCars() {
    // Car 0: Horizontal Right
    cars[0].x = -1.2; cars[0].y = -0.15; cars[0].speed = 0.01; 
    cars[0].maxSpeed = 0.015; cars[0].direction = 0; 
    cars[0].r = 0.8; cars[0].g = 0.1; cars[0].b = 0.1;

    // Car 1: Horizontal Left
    cars[1].x = 1.2; cars[1].y = 0.15; cars[1].speed = 0.01; 
    cars[1].maxSpeed = 0.012; cars[1].direction = 1; 
    cars[1].r = 0.1; cars[1].g = 0.4; cars[1].b = 0.8;

    // Car 2: Vertical Up
    cars[2].x = 0.15; cars[2].y = -1.2; cars[2].speed = 0.01; 
    cars[2].maxSpeed = 0.018; cars[2].direction = 2; 
    cars[2].r = 1.0; cars[2].g = 0.7; cars[2].b = 0.0;

    // Car 3: Vertical Down
    cars[3].x = -0.15; cars[3].y = 1.2; cars[3].speed = 0.01; 
    cars[3].maxSpeed = 0.014; cars[3].direction = 3; 
    cars[3].r = 0.2; cars[3].g = 0.8; cars[3].b = 0.2;
}

void drawCar(int i) {
    glPushMatrix();
    glTranslatef(cars[i].x, cars[i].y, 0);
    
    if (cars[i].direction == 1) glRotatef(180, 0, 0, 1);
    else if (cars[i].direction == 2) glRotatef(90, 0, 0, 1);
    else if (cars[i].direction == 3) glRotatef(270, 0, 0, 1);

    // Body
    glColor3f(cars[i].r, cars[i].g, cars[i].b);
    glRectf(-0.1, -0.06, 0.1, 0.06);

    // Windshield
    glColor3f(0.7, 0.9, 1.0);
    glRectf(0.02, -0.05, 0.07, 0.05);

    // Headlights
    glColor3f(1.0, 1.0, 0.5);
    drawCircle(0.09, -0.04, 0.015);
    drawCircle(0.09, 0.04, 0.015);

    glPopMatrix();
}

void drawRoads() {
    // Asphalt
    glColor3f(0.2, 0.2, 0.2);
    glRectf(-1, -0.3, 1, 0.3); 
    glRectf(-0.3, -1, 0.3, 1); 

    // Zebra Crossings
    glColor3f(1, 1, 1);
    for(float i = -0.25; i < 0.3; i += 0.1) {
        glRectf(0.35, i, 0.45, i-0.05);   // Right
        glRectf(-0.35, i, -0.45, i-0.05); // Left
        glRectf(i, 0.35, i-0.05, 0.45);   // Top
        glRectf(i, -0.35, i-0.05, -0.45); // Bottom
    }
}

void drawSignals() {
    float pos[4][2] = {{0.35, 0.35}, {-0.35, -0.35}, {0.35, -0.35}, {-0.35, 0.35}};
    for(int i=0; i<4; i++) {
        glPushMatrix();
        glTranslatef(pos[i][0], pos[i][1], 0);
        
        glColor3f(0.1, 0.1, 0.1);
        glRectf(-0.04, -0.12, 0.04, 0.12);

        int isGreen = (i < 2 && signalState == 0) || (i >= 2 && signalState == 2);
        int isYellow = (i < 2 && signalState == 1) || (i >= 2 && signalState == 3);
        int isRed = (i < 2 && (signalState == 2 || signalState == 3)) || 
                    (i >= 2 && (signalState == 0 || signalState == 1));

        // Red Light
        glColor3f(isRed ? 1.0f : 0.2f, 0.0f, 0.0f); 
        drawCircle(0, 0.08, 0.03);
        
        // Yellow Light (Fix applied here)
        glColor3f(isYellow ? 1.0f : 0.2f, isYellow ? 1.0f : 0.2f, 0.0f); 
        drawCircle(0, 0, 0.03);
        
        // Green Light
        glColor3f(0.0f, isGreen ? 1.0f : 0.2f, 0.0f); 
        drawCircle(0, -0.08, 0.03);
        
        glPopMatrix();
    }
}

/* --- LOGIC --- */
void update(int v) {
    timer++;
    if (timer > 300) { timer = 0; signalState = (signalState + 1) % 4; }

    for (int i = 0; i < MAX_CARS; i++) {
        int stop = 0;
        
        // Stop Logic
        if (cars[i].direction == 0 && cars[i].x < -0.45 && cars[i].x > -0.6 && signalState != 0) stop = 1;
        if (cars[i].direction == 1 && cars[i].x > 0.45 && cars[i].x < 0.6 && signalState != 0) stop = 1;
        if (cars[i].direction == 2 && cars[i].y < -0.45 && cars[i].y > -0.6 && signalState != 2) stop = 1;
        if (cars[i].direction == 3 && cars[i].y > 0.45 && cars[i].y < 0.6 && signalState != 2) stop = 1;

        if (stop) {
            cars[i].speed *= 0.9; // Deceleration
            if (cars[i].speed < 0.001) cars[i].speed = 0;
        } else {
            if (cars[i].speed < cars[i].maxSpeed) cars[i].speed += 0.0008; 
        }

        if (cars[i].direction == 0) cars[i].x += cars[i].speed;
        if (cars[i].direction == 1) cars[i].x -= cars[i].speed;
        if (cars[i].direction == 2) cars[i].y += cars[i].speed;
        if (cars[i].direction == 3) cars[i].y -= cars[i].speed;

        if (cars[i].x > 1.2) cars[i].x = -1.2;
        if (cars[i].x < -1.2) cars[i].x = 1.2;
        if (cars[i].y > 1.2) cars[i].y = -1.2;
        if (cars[i].y < -1.2) cars[i].y = 1.2;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Grass background
    glColor3f(0.2, 0.5, 0.2);
    glRectf(-1, -1, 1, 1);
    
    drawRoads();
    drawSignals();
    for (int i = 0; i < MAX_CARS; i++) drawCar(i);
    
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Traffic Management Project");
    gluOrtho2D(-1, 1, -1, 1);
    
    initCars();
    
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}