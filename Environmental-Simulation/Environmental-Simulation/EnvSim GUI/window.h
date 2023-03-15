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

extern bool filt1Active, filt2Active, filt3Active, wolfActive, rabbitActive, windowRunning, simRunning;
extern int currYear;
extern ImVec2 cellSize;
extern Seasons season;
extern int grassCellCol[3], wolfCellCol[3];
extern Cell* cells;

// Initializes GLFW
void InitGLFW();

// Initializes ImGui
void InitImGui(GLFWwindow* window);

// Starts ImGui frame
void StartFrame();

// Rendering the window
void RenderWindow(GLFWwindow* window);

// Displays text on the window
//void ImGuiTextHandler(Seasons season, bool filt1Active, bool filt2Active, bool filt3Active);

// Setting colors
void Colors();

ImVec2 SetItemDimensions(int width, int height);

void ResetWindow();

void ResetGridSize(int cellCount);

void ClearGrid(int cellCount);

void ChangeCell(int index, int cellCol[]);

int RunWindow();

