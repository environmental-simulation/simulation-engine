// Displays and runs the window for software

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cstring>
#include <string>

class Cell
{
public:
	bool selected;
	int color[3];
};

// todo: port this enum set over into the Reacting Phase map files
enum Seasons
{
	Summer,
	Winter
};

extern bool filt1Active, filt2Active, filt3Active, wolfActive, rabbitActive, windowRunning, simRunning, leftMousePressed, titleBarHovered;
extern int currYear;
extern int cursorPosX, cursorPosY, offsetPosX, offsetPosY;
extern ImVec2 cellSize;
extern Seasons season;
extern int grassCellCol[3], wolfCellCol[3];
extern Cell* cells;

// Initializes GLFW
void InitGLFW();

// Initializes ImGui
void InitImGui(GLFWwindow* window);

// Starts ImGui frame
void StartFrame(GLFWwindow* window);

// Rendering the window
void RenderWindow(GLFWwindow* window);

// Moves window by mouse drag
void MoveWindow(GLFWwindow* window);

// Used with glfwSetCursorPosCallback() to get cursor position
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);

// Used with glfwSetMouseButtonCallback() to get mouse inputs
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// Setting colors
void Colors();

ImVec2 SetItemDimensions(int width, int height);

void ResetWindow();

void ResetGridSize(int cellCount);

void ClearGrid(int cellCount);

void ChangeCell(int index, int cellCol[]);

int RunWindow();

