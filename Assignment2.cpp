//Updated Version

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "shader.h"

const int windowWidth = 700;
const int windowHeight = 700;

struct Point {
	float x, y;
	bool isSelected = false;
};

std::vector<Point> movablePoints = {
	{-0.75f, -0.75f},
	{-0.25f, 0.75f},
	{0.25f, -0.75f},
	{0.75f,  0.75f}
};

bool actDragging = false;
int selectedPoint = -1;

GLuint vao, vbo;
GLuint curveProgram;

void actMouseClicking(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	xpos = (xpos / windowWidth) * 2 - 1;
	ypos = -((ypos / windowHeight) * 2 - 1);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		for (size_t i = 0; i < movablePoints.size(); ++i) {
			float dx = xpos - movablePoints[i].x;
			float dy = ypos - movablePoints[i].y;
			if (dx * dx + dy * dy < 0.02f) {
				actDragging = true;
				selectedPoint = i;
				return;
			}
		}
		if (movablePoints.size() < 4) {
			movablePoints.push_back({ (float)xpos, (float)ypos });
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		actDragging = false;
		selectedPoint = -1;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		for (size_t i = 0; i < movablePoints.size(); ++i) {
			float dx = xpos - movablePoints[i].x;
			float dy = ypos - movablePoints[i].y;
			if (dx * dx + dy * dy < 0.02f) {
				movablePoints.erase(movablePoints.begin() + i);
				if (selectedPoint == (int)i) selectedPoint = -1;
				return;
			}
		}
	}
}

void updatedMouseClicking(GLFWwindow* window, double xpos, double ypos) {
	if (actDragging && selectedPoint != -1) {
		xpos = (xpos / windowWidth) * 2 - 1;
		ypos = -((ypos / windowHeight) * 2 - 1);
		movablePoints[selectedPoint].x = (float)xpos;
		movablePoints[selectedPoint].y = (float)ypos;
	}
}

void updateBuffers() {
	std::vector<float> vertices;
	for (const auto& pt : movablePoints) {
		vertices.push_back(pt.x);
		vertices.push_back(pt.y);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
}

void drawPolygon() {
	glUseProgram(0);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (const auto& pt : movablePoints)
		glVertex2f(pt.x, pt.y);
	glEnd();

	glLineWidth(4.0f);
	glBegin(GL_LINE_STRIP);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (const auto& pt : movablePoints)
		glVertex2f(pt.x, pt.y);
	glEnd();
}

int main() {
	if (!glfwInit()) return -1;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "BEZIER CURVE", NULL, NULL);
	if (!window) { glfwTerminate(); return -1; }
	glfwMakeContextCurrent(window);
	glewInit();

	glfwSetMouseButtonCallback(window, actMouseClicking);
	glfwSetCursorPosCallback(window, updatedMouseClicking);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	curveProgram = LoadShaders("vertex.glsl", "fragment.glsl", "tessellation.glsl", "tesselval.glsl");
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	std::cout << "ESC: To Exit\n";
	std::cout << "Left Click: Drag existing point or create a new point.\n";
	std::cout << "Right Click: Remove any point by clicking on it.\n";

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		updateBuffers();
		drawPolygon();

		if (movablePoints.size() == 4) {
			glUseProgram(curveProgram);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glDrawArrays(GL_PATCHES, 0, 4);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
