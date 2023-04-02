#include "window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

bool filt1Active, filt2Active, filt3Active, wolfActive, rabbitActive, windowRunning, simRunning, leftMousePressed, titleBarHovered;
int currYear;
int cursorPosX, cursorPosY, offsetPosX, offsetPosY;
int gridSizeX, gridSizeY;
int curCellSize, minCellSize, maxCellSize;
Seasons season;
Cell* cells;
int grassCellCol[3], wolfCellCol[3];

void InitGLFW()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void InitImGui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void StartFrame(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
}

void RenderWindow(GLFWwindow* window)
{
	ImGui::Render();
	int displayWidth, displayHeight;
	glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
	//std::cout << displayWidth << "\n" << displayHeight << "\n\n";
	glViewport(0, 0, displayWidth, displayHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MoveWindow(GLFWwindow* window)
{
	int windowPosX, windowPosY;
	glfwGetWindowPos(window, &windowPosX, &windowPosY);
	glfwSetWindowPos(window, windowPosX + offsetPosX, windowPosY + offsetPosY);
	offsetPosX = 0, offsetPosY = 0;
	cursorPosX += offsetPosX;
	cursorPosY += offsetPosY;
}

void CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (leftMousePressed)
	{
		offsetPosX = xPos - cursorPosX;
		offsetPosY = yPos - cursorPosY;
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (titleBarHovered)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			leftMousePressed = true;
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			cursorPosX = std::floor(x);
			cursorPosY = std::floor(y);
			//std::cout << "Cursor X: " << cursorPosX << "\nCursor Y: " << cursorPosY << std::endl;
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			leftMousePressed = false;
			cursorPosX = 0;
			cursorPosY = 0;
		}
	}
}

void Colors(ImGuiStyle& style)
{
	//ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImColor(83, 83, 83);
	style.Colors[ImGuiCol_ChildBg] = ImColor(37, 36, 42);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(255, 255, 255, 100);
	style.Colors[ImGuiCol_TableBorderStrong] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_TableBorderLight] = ImColor(0, 0, 0, 255);
	style.CellPadding = ImVec2(5, 3);
}

ImVec2 SetItemDimensions(int width, int height)
{
	ImGui::SetNextItemWidth(width);
	ImVec2 framePadding = ImVec2(0, (height - ImGui::GetFontSize() + 1) * 0.5f);

	return framePadding;
}

void ResetWindow()
{
	season = Summer;
	currYear = 1;
	filt1Active = false;
	filt2Active = false;
	filt3Active = false;
	wolfActive = false;
	rabbitActive = false;
}

void ResetGridSize(int cellCount, ImGuiStyle& style)
{
	if (sizeof(cells) != 0)
	{
		delete[] cells;
	}

	cells = new Cell[cellCount];
	int colCount = sqrt(cellCount);
	//std::cout << "col count: " << colCount << std::endl;
	int cPaddingSize = (((2 * style.CellPadding.x) * (colCount - 1)) + (2 * style.CellPadding.x));
	//std::cout << "cell pad: " << cPaddingSize << std::endl;
	minCellSize =  cPaddingSize / colCount;
	maxCellSize = minCellSize * 4;

	curCellSize = (maxCellSize + minCellSize) / 2;


	std::cout << "Min Cell Size: " << minCellSize << std::endl;
	std::cout << "Max Cell Size: " << maxCellSize << std::endl;
	std::cout << "Cur Cell Size: " << curCellSize << std::endl;

	for (int i = 0; i < cellCount; i++)
	{
		cells[i].selected = false;
		cells[i].color[0] = grassCellCol[0];
		cells[i].color[1] = grassCellCol[1];
		cells[i].color[2] = grassCellCol[2];
	}
}

void ClearGrid(int cellCount)
{
	for (int i = 0; i < cellCount; i++)
	{
		cells[i].selected = false;
		cells[i].color[0] = grassCellCol[0];
		cells[i].color[1] = grassCellCol[1];
		cells[i].color[2] = grassCellCol[2];
	}
}

void ChangeCell(int index, int cellCol[])
{
	cells[index].selected = !cells[index].selected;

	if (cells[index].selected)
	{
		cells[index].color[0] = cellCol[0];
		cells[index].color[1] = cellCol[1];
		cells[index].color[2] = cellCol[2];
	}
}

int RunWindow()
{
	InitGLFW();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (monitor == NULL)
	{
		return 0;
	}

	int width = 2000, height = 1200;

	GLFWwindow* window = glfwCreateWindow(width, height, "Environmental Sim", NULL, NULL);
	if (window == NULL)
	{
		return 1;
	}

	glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	glfwMakeContextCurrent(window);
	gladLoadGL();

	InitImGui(window);

	filt1Active = false, filt2Active = false, filt3Active = false, wolfActive = false, rabbitActive = false, windowRunning = true, simRunning = false, leftMousePressed = false, titleBarHovered = false;
	currYear = 1;
	season = Summer;
	int sizeIndex = 0;
	const char* gridSizeText[] = { "100x100" };
	int cellCounts[] = { 10000 };

	int animalCellIndex = 0;
	const char* animalCellType[] = { "Wolf", "Rabbit" };

	//ImColor emptyCellCol = ImColor(255, 255, 255);
	grassCellCol[0] = 6; grassCellCol[1] = 188; grassCellCol[2] = 0;
	wolfCellCol[0] = 128; wolfCellCol[1] = 128; wolfCellCol[2] = 128;

	//ResetGridSize(cellCounts[sizeIndex]);
	gridSizeX = 1140, gridSizeY = 860;
	//cellSize = (((2*)))

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 framePadding = style.FramePadding;
	Colors(style);
	ResetGridSize(cellCounts[sizeIndex], style);

	const char* filename = "EnvSim GUI\\images\\wolf.png";
	const char* filename2 = "EnvSim GUI\\images\\filter.jpg";
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;
	GLuint my_image_texture2 = 0;
	bool ret = LoadTextureFromFile(filename, &my_image_texture, &my_image_width, &my_image_height);
	bool ret2 = LoadTextureFromFile(filename2, &my_image_texture2, &my_image_width, &my_image_height);
	IM_ASSERT(ret);
	IM_ASSERT(ret2);

	while (!glfwWindowShouldClose(window))
	{
		StartFrame(window);
		
		if (leftMousePressed && titleBarHovered)
		{
			MoveWindow(window);
		}
		else
		{
			titleBarHovered = false;
		}

		ImGui::Begin("Environmental Sim", &windowRunning, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);
		if (ImGui::IsItemHovered())
		{
			if (!windowRunning)
			{
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}

			titleBarHovered = true;
		}

		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close")) 
				{ 
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Run"))
			{
				if (!simRunning)
				{
					if (ImGui::MenuItem("Run Simulation"))
					{
						ResetWindow();
						simRunning = true;
					}
				}
				else
				{
					// just ends/resets the simulation. grid size stays the same
					if (ImGui::MenuItem("End Simulation"))
					{
						simRunning = false;
						ResetWindow();
					}

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Upper left panel
		ImGui::SetCursorPos(ImVec2(10, 50));
		ImGui::BeginChild("Upper Left Panel", ImVec2(400, 740));
		{
			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 10));
			ImGui::Checkbox("\tWolf", &wolfActive);
			style.FramePadding = framePadding;

			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 70));
			ImGui::Checkbox("\tRabbit", &rabbitActive);
			style.FramePadding = framePadding;

			if (ImGui::Button("Zoom In"))
			{
				int sizeChange = std::ceil(maxCellSize * 0.1f);
				if (curCellSize + sizeChange < maxCellSize)
				{
					curCellSize += sizeChange;
				}
				else
				{
					curCellSize = maxCellSize;
				}

				/*std::cout << "Min Cell Size: " << minCellSize << std::endl;
				std::cout << "Max Cell Size: " << maxCellSize << std::endl;
				std::cout << "Cur Cell Size: " << curCellSize << std::endl;*/
			}

			if (ImGui::Button("Zoom Out"))
			{
				int sizeChange = std::ceil(maxCellSize * 0.1f);
				if (curCellSize - sizeChange > minCellSize)
				{
					curCellSize -= sizeChange;
				}
				else
				{
					curCellSize = minCellSize;
				}

				/*std::cout << "Min Cell Size: " << minCellSize << std::endl;
				std::cout << "Max Cell Size: " << maxCellSize << std::endl;
				std::cout << "Cur Cell Size: " << curCellSize << std::endl;*/
			}
		}
		ImGui::EndChild();

		// Lower left panel
		ImGui::SetCursorPos(ImVec2(10, 800));
		ImGui::BeginChild("Lower Left Panel", ImVec2(400, 390));
		{
			ImGui::SetCursorPos(ImVec2(10, 10));
			int temp = sizeIndex;
			ImGui::Combo("Grid Size", &sizeIndex, gridSizeText, 4);
			if (simRunning && ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Can't Change Size While Running");
			}

			if (simRunning && temp != sizeIndex)
			{
				sizeIndex = temp;
			}
			else if(!simRunning && temp != sizeIndex)
			{
				ResetWindow();
				ResetGridSize(cellCounts[sizeIndex], style);
			}

			ImGui::SetCursorPos(ImVec2(10, 30));
			ImGui::Combo("Animal Cell", &animalCellIndex, animalCellType, 2);

			if (animalCellIndex == 1)
			{
				ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(10, 10));
			}
		}
		ImGui::EndChild();

		// Middle panel
		ImGui::SetCursorPos(ImVec2(420, 50));
		ImGui::BeginChild("Mid Panel", ImVec2(1160, 880));
		{
			ImGui::SetCursorPos(ImVec2(10, 10));
			int columnCount = sqrt(cellCounts[sizeIndex]);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
			if (ImGui::BeginTable("Grid", columnCount, tableFlags, ImVec2(gridSizeX, gridSizeY)))
			{
				for (int i = 0; i < columnCount; i++)
				{
					ImGui::TableSetupColumn(NULL, 16);
				}

				for (int i = 0; i < cellCounts[sizeIndex]; i++)
				{
					ImGui::TableNextColumn();
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					drawList->ChannelsSplit(3);
					drawList->ChannelsSetCurrent(1);
					ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(cells[i].color[0], cells[i].color[1], cells[i].color[2], 255));
					if (ImGui::Selectable(std::to_string(i).c_str(), cells[i].selected, 0, ImVec2(curCellSize, curCellSize)))
					{
						if (!simRunning && currYear == 1)
						{
							ChangeCell(i, wolfCellCol);
							//std::cout << "Cell " << i << " has been selected" << std::endl;
							//ImGui::SetItemAllowOverlap();
							//std::cout <<  << std::endl;
							//std::cout << (800 / columnCount) - 6 << std::endl;
						}
					}
					ImGui::PopStyleColor();

					if (!ImGui::IsItemHovered() && !cells[i].selected)
					{
						drawList->ChannelsSetCurrent(0);
						ImVec2 posMin = ImGui::GetItemRectMin();
						ImVec2 posMax = ImGui::GetItemRectMax();
						ImGui::GetWindowDrawList()->AddRectFilled(posMin, posMax, IM_COL32(grassCellCol[0], grassCellCol[1], grassCellCol[2], 255));
					}
					if (!simRunning && currYear != 1 && ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("Animals Must Be Placed In Year 1");
					}

					if (cells[i].selected)
					{
						drawList->ChannelsSetCurrent(2);
						drawList->AddImage((void*)(intptr_t)my_image_texture, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

						if (filt1Active)
						{
							drawList->AddImage((void*)(intptr_t)my_image_texture, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0,0), ImVec2(1,1), IM_COL32(25, 80, 145, 90));
						}
						//ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(10, 10));
					}

					drawList->ChannelsMerge();

					
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();

		// Lower Panel
		ImGui::SetCursorPos(ImVec2(420, 940));
		ImGui::BeginChild("Lower Panel", ImVec2(1160, 250));
		{
			ImGui::SetCursorPos(ImVec2(10, 10));
			if (ImGui::Button("Summer", ImVec2(565, 100)))
			{
				season = Summer;
			}
			ImGui::SetCursorPos(ImVec2(585, 10));
			if (ImGui::Button("Winter", ImVec2(565, 100)))
			{
				season = Winter;
			}

			style.FramePadding = SetItemDimensions(1050, 80);
			ImGui::SetCursorPos(ImVec2(10, 140));
			ImGui::SliderInt("Year", &currYear, 1, 5);
			style.FramePadding = framePadding;
		}
		ImGui::EndChild();

		// Right Panel
		ImGui::SetCursorPos(ImVec2(1590, 50));
		ImGui::BeginChild("Right Panel", ImVec2(400, 1140));
		{
			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 10));
			ImGui::Checkbox("\tVegitation", &filt1Active);
			style.FramePadding = framePadding;

			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 70));
			ImGui::Checkbox("\tFilter 2", &filt2Active);
			style.FramePadding = framePadding;

			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 130));
			ImGui::Checkbox("\tFilter 3", &filt3Active);
			style.FramePadding = framePadding;
		}
		ImGui::EndChild();

		ImGui::End();
		

		RenderWindow(window);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
