// Displays and runs the window for software
#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/stb_image.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cstring>
#include <string>
#include <unordered_map>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height); // Used to load images from file
#endif

extern bool leftMousePressed, titleBarHovered;
extern int cursorPosX, cursorPosY, offsetPosX, offsetPosY;

void CursorPosCallback(GLFWwindow* window, double xPos, double yPos); // Used with glfwSetCursorPosCallback() to get cursor position
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods); // Used with glfwSetMouseButtonCallback() to get mouse inputs

struct CellData
{
public:
	bool wolfPresent;
	bool rabbitPresent;
	bool vegetationPresent;
	bool preyPresent;
	bool humanPresent;
	bool animalPresent;
};

class Year
{
public:
	CellData** cells; // dynamic 2d array of cells for each year and season
	Year();
	Year(int seasonCount, int cellCount);
};

class Window
{
public:
	Window(); // Initializes all variables and creates the window
	void RunWindow(); // Runs the window and handles window function
	// Receives output data from simulation
	// Will only work if the grid is a perfect square and the array passed has the same number of rows/cols
	void SetData(int year, int season, CellData** data); 
private:
	int winWidth, winHeight;
	GLFWwindow* window;
	bool windowRunning, simRunning;
	int tableSizeX, tableSizeY;
	const char* gridSizeText[1];
	int cellCounts[1];
	int sizeIndex;
	int curCellSize, minCellSize, maxCellSize;
	ImU32 cellColor;
	int currYear, minYear, maxYear;
	std::unordered_map<int, Year> years;
	bool activeAnimals[2];
	bool activeFilters[3];
	const char* animalTypes[2];
	const char* filterTypes[3];
	int animalIndex;
	const char* seasons[2];
	int seasonIndex;
	ImGuiStyle& style;
	ImVec2 framePadding;
	const char* animalFiles[2];
	const char* filterFiles[3];
	int imageWidth, imageHeight;
	GLuint animalTextures[2];
	GLuint filterTextures[3];
	ImU32 animalColors[2];
	ImU32 filterColors[3];
	
	void InitGLFW(); // Initializes GLFW
	void InitImGui(); // Initializes ImGui
	void Colors(); // Setting UI colors and cell padding
	void SetYears(); // Initializes the years unordered_map. at the moment this only works if years increment by 1
	void SetGridSize(int cellCount); // Can be used to set the grid size or clear the grid by reseting all CellData bools to false
	void StartFrame(); // Starts ImGui frame
	void MoveWindow(); // Moves window by mouse drag
	void RunSim(); // Sends input data and runs the simulation
	ImVec2 SetItemDimensions(int w, int h); // Helps set the dimensions of UI items like checkboxes and sliders
	void CreateMenuBar(); // Creates menu bar
	void CreateUpperLeftPanel(); // Creates upper left panel and items
	void CreateLowerLeftPanel(); // Creates lower left panel and items
	void CreateMidPanel(ImGuiTableFlags flags); // Creates mid panel and items
	void CreateLowerPanel(); // Creates lower panel and items
	void CreateRightPanel(); // Creates right panel and items
	void ResetWindow(); // Resets window filters and sliders to default values
	void PlaceAnimal(int index); // Allows the user to place an animal at a certain grid cell
	void RenderWindow(); // Rendering the window
};






