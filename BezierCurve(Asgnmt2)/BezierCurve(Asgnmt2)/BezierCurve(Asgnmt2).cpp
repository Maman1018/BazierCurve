#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>
#include "tessShader.h" 

struct Point {
    float x, y;
};

const int windowWidth = 700, windowHeight = 700;
std::vector<Point> fixPoints = {
    {-0.75f, -0.75f},
    {-0.25f,  0.75f},
    { 0.25f, -0.75f},
    { 0.75f,  0.75f}
};
bool isPointDragging = false;
int draggingIndex = -1;

GLuint vao, vbo, shaderProgram;

void updateBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * fixPoints.size(), fixPoints.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void redPoints() {
    glPointSize(10.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& pt : fixPoints)
        glVertex2f(pt.x, pt.y);
    glEnd();
}

void bluePolygonLine() {
    glLineWidth(5.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& pt : fixPoints)
        glVertex2f(pt.x, pt.y);
    glEnd();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


void mouseClickingButton(GLFWwindow* window, int button, int action, int mods) {

    double horPosition, verPosition;
    glfwGetCursorPos(window, &horPosition, &verPosition);
    horPosition = (horPosition / windowWidth) * 2.0 - 1.0;
    verPosition = -((verPosition / windowHeight) * 2.0 - 1.0);
    

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        for (int i = 0; i < fixPoints.size(); ++i) {
            float dx = horPosition - fixPoints[i].x;
            float dy = verPosition - fixPoints[i].y;
            if (dx * dx + dy * dy < 0.03f) {
                isPointDragging = true;
                draggingIndex = i;
                return;
            }
        }
        if (fixPoints.size() < 4) {
            fixPoints.push_back({ (float)horPosition, (float)verPosition });
            updateBuffer();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (fixPoints.size() > 2) {
            float minDist = 1000.0f;
            int closest = -1;
            for (int i = 0; i < fixPoints.size(); ++i) {
                float dx = horPosition - fixPoints[i].x;
                float dy = verPosition - fixPoints[i].y;
                float dist = dx * dx + dy * dy;
                if (dist < minDist) {
                    minDist = dist;
                    closest = i;
                }
            }
            if (closest != -1 && minDist < 0.03f) {
                fixPoints.erase(fixPoints.begin() + closest);
                updateBuffer();
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        isPointDragging = false;
        draggingIndex = -1;
    }
}

void movedCursor(GLFWwindow* window, double xpos, double ypos) {
    if (isPointDragging && draggingIndex != -1) {
        xpos = (xpos / windowWidth) * 2.0 - 1.0;
        ypos = -((ypos / windowHeight) * 2.0 - 1.0);
        fixPoints[draggingIndex].x = (float)xpos;
        fixPoints[draggingIndex].y = (float)ypos;
        updateBuffer();
    }
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Flexible Bezier Curve", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();

    std::cout << "Bezier Curve Interaction Controls" << std::endl;
    std::cout << "Left-Click: Drag the red-point" << std::endl;
    std::cout << "Right-Click: Click on the red point to delete one of the point (min 2)." << std::endl;
    std::cout << "Left-Click: To add a point back to the window. (max 4)" << std::endl;

    glfwSetMouseButtonCallback(window, mouseClickingButton);
    glfwSetCursorPosCallback(window, movedCursor);
    glfwSetKeyCallback(window, keyCallback);

    shaderProgram = LoadShaders("vertex.glsl", "fragment.glsl", "tess_control.glsl", "tess_eval.glsl");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * fixPoints.size(), fixPoints.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(0);
        glBindVertexArray(0);

        bluePolygonLine();
        redPoints();

        if (fixPoints.size() >= 2 && fixPoints.size() <= 4) {

            glUseProgram(shaderProgram);
            glBindVertexArray(vao);
            glPatchParameteri(GL_PATCH_VERTICES, fixPoints.size());
            glDrawArrays(GL_PATCHES, 0, fixPoints.size());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
