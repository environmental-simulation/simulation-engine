#include "window.h"

bool filt1Active, filt2Active, filt3Active, wolfActive, rabbitActive, windowRunning, simRunning;
int currYear;
ImVec2 cellSize;
Seasons season;
Cell* cells;
int grassCellCol[3], wolfCellCol[3];

void InitGLFW()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_FLOATING, true);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_MAXIMIZED, true);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
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

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(2000, 1200));
	//ImGui::SetNextWindowPos();
}

void RenderWindow(GLFWwindow* window)
{
	ImGui::Render();
	int displayWidth, displayHeight;
	glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
	glViewport(0, 0, displayWidth, displayHeight);
	//glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//void ImGuiTextHandler(Seasons season, bool filt1Active, bool filt2Active, bool filt3Active)
//{
//	if (season == Summer)
//	{
//		ImGui::Text("Summer");
//	}
//	else if (season == Winter)
//	{
//		ImGui::Text("Winter");
//	}
//
//	std::string text = filt1Active ? "Filter 1: true" : "Filter 1: false";
//	const char* filtText = text.c_str();
//	ImGui::Text(filtText);
//	text = filt2Active ? "Filter 2: true" : "Filter 2: false";
//	filtText = text.c_str();
//	ImGui::Text(filtText);
//	text = filt3Active ? "Filter 3: true" : "Filter 3: false";
//	filtText = text.c_str();
//	ImGui::Text(filtText);
//}

void Colors()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImColor(83, 83, 83);
	style.Colors[ImGuiCol_ChildBg] = ImColor(37, 36, 42);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(255, 255, 255, 100);
	style.Colors[ImGuiCol_TableBorderStrong] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_TableBorderLight] = ImColor(0, 0, 0, 255);
	//std::cout << style.CellPadding.x << std::endl;
	//std::cout << style.CellPadding.y << std::endl;
	style.CellPadding = ImVec2(4, 3);
	//style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0);
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

void ResetGridSize(int cellCount)
{
	if (sizeof(cells) != 0)
	{
		delete[] cells;
	}

	cells = new Cell[cellCount];

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

	int width = glfwGetVideoMode(monitor)->width;
	int height = glfwGetVideoMode(monitor)->height;

	GLFWwindow* window = glfwCreateWindow(width, height, "Environmental Sim", NULL, NULL);
	if (window == NULL)
	{
		return 1;
	}

	glfwSetWindowAttrib(window, GLFW_DECORATED, false);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	//glfwSwapInterval(1); // vsync

	InitImGui(window);

	filt1Active = false, filt2Active = false, filt3Active = false, wolfActive = false, rabbitActive = false, windowRunning = true, simRunning = false;
	currYear = 1;
	season = Summer;
	int sizeIndex = 0;
	const char* gridSizeText[] = { "5x5", "10x10", "15x15", "20x20" };
	int cellCounts[] = { 25, 100, 225, 400 };
	int animalCellIndex = 0;
	const char* animalCellType[] = { "Wolf", "Rabbit" };

	//ImColor emptyCellCol = ImColor(255, 255, 255);
	grassCellCol[0] = 6; grassCellCol[1] = 188; grassCellCol[2] = 0;
	wolfCellCol[0] = 128; wolfCellCol[1] = 128; wolfCellCol[2] = 128;

	//cellSize = ImVec2(220,80);
	
	//cells = ResetGridSize(cellCount[sizeIndex]);
	ResetGridSize(cellCounts[sizeIndex]);

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 framePadding = style.FramePadding;
	Colors();


	while (!glfwWindowShouldClose(window))
	{
		StartFrame();

		ImGui::Begin("Environmental Sim", &windowRunning, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

		if (!windowRunning)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
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
			//ImGui::Combo("Animals", &currAnimal, animals, 2);
			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 10));
			ImGui::Checkbox("\tWolf", &wolfActive);
			style.FramePadding = framePadding;

			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 70));
			ImGui::Checkbox("\tRabbit", &rabbitActive);
			style.FramePadding = framePadding;
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
				//cells = ResetGridSize(cellCount[sizeIndex]);
				ResetGridSize(cellCounts[sizeIndex]);
			}

			ImGui::SetCursorPos(ImVec2(10, 30));
			ImGui::Combo("Animal Cell", &animalCellIndex, animalCellType, 2);
		}
		ImGui::EndChild();

		// Middle panel
		ImGui::SetCursorPos(ImVec2(420, 50));
		ImGui::BeginChild("Mid Panel", ImVec2(1160, 880));
		{
			ImGui::SetCursorPos(ImVec2(10, 10));
			int columnCount = sqrt(cellCounts[sizeIndex]);
			if (ImGui::BeginTable("Grid", columnCount, 1920 | 64, ImVec2(1140.0f, 860.0f)))
			{
				for (int i = 0; i < columnCount; i++)
				{
					ImGui::TableSetupColumn(NULL, 16);
				}

				for (int i = 0; i < cellCounts[sizeIndex]; i++)
				{
					ImGui::TableNextColumn();
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					drawList->ChannelsSplit(2);
					drawList->ChannelsSetCurrent(1);
					ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(cells[i].color[0], cells[i].color[1], cells[i].color[2], 255));
					if (ImGui::Selectable(std::to_string(i).c_str(), cells[i].selected, 0, ImVec2((1130 / columnCount), (830 / columnCount))))
					{
						if (!simRunning && currYear == 1)
						{
							ChangeCell(i, wolfCellCol);
							std::cout << 1130 / columnCount << std::endl;
							std::cout << 830 / columnCount << std::endl;
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
