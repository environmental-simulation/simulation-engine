#include "window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

int cursorPosX, cursorPosY, offsetPosX, offsetPosY;
bool windowRunning, simRunning, leftMousePressed, titleBarHovered;

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

Year::Year()
{
	cells = nullptr;
}

Year::Year(int seasonCount, int cellCount)
{
	cells = new CellData*[seasonCount];
	for (int i = 0; i < seasonCount; i++)
	{
		cells[i] = new CellData[cellCount];
		for (int j = 0; j < cellCount; j++)
		{
			cells[i][j].animalPresent = new bool[2];
			cells[i][j].animalPresent[0] = false; // Wolf
			cells[i][j].animalPresent[1] = false; // Rabbit
			cells[i][j].vegetationPresent = false;
			cells[i][j].preyPresent = false;
			cells[i][j].humanPresent = false;
			cells[i][j].cellPresent = false;
		}
	}
}

Window::Window()
{
	InitGLFW();
	winWidth = 2000, winHeight = 1200;
	window = glfwCreateWindow(winWidth, winHeight, "Environmental Sim", NULL, NULL);

	if (window == NULL) std::cout << "Window creation failed" << std::endl;
	else
	{
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwMakeContextCurrent(window);
		gladLoadGL();
		InitImGui();
		ImGuiStyle& style = ImGui::GetStyle();

		
		imageWidth = 0, imageHeight = 0;

		// =setting up animal visuals and arrays
		animalTypes[0] = "\tWolf", animalTypes[1] = "\tRabbit";
		animalCounts[0] = 0, animalCounts[1] = 0;
		animalFiles[0] = "EnvSim GUI\\images\\wolf.png", animalFiles[1] = "EnvSim GUI\\images\\rabbit.png";
		LoadTextureFromFile(animalFiles[0], &animalTextures[0], &imageWidth, &imageHeight);
		LoadTextureFromFile(animalFiles[1], &animalTextures[1], &imageWidth, &imageHeight);
		animalColors[0] = IM_COL32(255, 0, 0, 200), animalColors[1] = IM_COL32(0, 0, 255, 170);
		activeAnimals[0] = true, activeAnimals[1] = true;
		//animalIndex = 0;

		// used for showing and hiding filters/layers
		filterTypes[0] = "\tVegetation", filterTypes[1] = "\tPrey Density", filterTypes[2] = "\tHuman Population";
		filterFiles[0] = "EnvSim GUI\\images\\vegetation.png", filterFiles[1] = "EnvSim GUI\\images\\prey.jpg", filterFiles[2] = "EnvSim GUI\\images\\human.png";
		LoadTextureFromFile(filterFiles[0], &filterTextures[0], &imageWidth, &imageHeight);
		LoadTextureFromFile(filterFiles[1], &filterTextures[1], &imageWidth, &imageHeight);
		LoadTextureFromFile(filterFiles[2], &filterTextures[2], &imageWidth, &imageHeight);
		filterColors[0] = IM_COL32(0, 255, 0, 100), filterColors[1] = IM_COL32(235, 204, 52, 80), filterColors[2] = IM_COL32(255, 0, 251, 60);
		activeFilters[0] = true, activeFilters[1] = true, activeFilters[2] = true;

		// used for placing cells on the grid
		cellTypes[0] = "Wolf Cell", cellTypes[1] = "Rabbit Cell", cellTypes[2] = "Vegetation", cellTypes[3] = "Prey", cellTypes[4] = "Human";
		cellIndex = 0;

		
		// environmental filter count * 4 + 4
		int inSize = 3 * 4 + 4;

		// creating animal models
		animalModels[0] = new AnimalModel((char*)"wolf", 4, inSize, 5);
		animalModels[1] = new AnimalModel((char*)"rabbit", 6, inSize, 5);

		windowRunning = true, simRunning = false;
		leftMousePressed = false, titleBarHovered = false;

		currYear = 1, minYear = 1, maxYear = 5;
		seasons[0] = "Summer", seasons[1] = "Winter";
		seasonIndex = 0;

		gridSizeText[0] = "100x100";
		cellCounts[0] = 10000;
		sizeIndex = 0;
		cellColor = IM_COL32(49, 77, 57, 255);
		
		tableSizeX = 1140, tableSizeY = 860;
		framePadding = style.FramePadding;
		Colors(style);
		SetYears();
		SetGridSize(cellCounts[sizeIndex], style);
		RunWindow(style);
	}
}

void Window::RunWindow(ImGuiStyle& style)
{
	if (window != NULL)
	{
		while (!glfwWindowShouldClose(window))
		{
			StartFrame();

			if (leftMousePressed && titleBarHovered) MoveWindow();
			else titleBarHovered = false;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;
			ImGui::Begin("Environmental Sim", &windowRunning, windowFlags);
			if (ImGui::IsItemHovered())
			{
				if (!windowRunning) glfwSetWindowShouldClose(window, GLFW_TRUE);
				titleBarHovered = true;
			}
			CreateMenuBar(style);
			ImGui::SetCursorPos(ImVec2(10, 50));
			CreateUpperLeftPanel(style);
			ImGui::SetCursorPos(ImVec2(10, 800));
			CreateLowerLeftPanel(style);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
			ImGui::SetCursorPos(ImVec2(420, 50));
			CreateMidPanel(tableFlags, style);
			ImGui::SetCursorPos(ImVec2(420, 940));
			CreateLowerPanel(style);
			ImGui::SetCursorPos(ImVec2(1590, 50));
			CreateRightPanel(style);
			ImGui::End();
			RenderWindow();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		if (years.at(currYear).cells != nullptr)
		{
			for (auto& year : years)
			{
				delete[] year.second.cells;
			}
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

void Window::SetData(int year, int season, CellData** data)
{
	int size = sqrt(cellCounts[sizeIndex]);
	int index = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			years.at(year).cells[season][index] = data[i][j];
			index++;
		}
	}
}

void Window::InitGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Window::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Window::Colors(ImGuiStyle& style)
{
	style.Colors[ImGuiCol_WindowBg] = ImColor(83, 83, 83);
	style.Colors[ImGuiCol_ChildBg] = ImColor(37, 36, 42);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(255, 255, 255, 100);
	style.Colors[ImGuiCol_TableBorderStrong] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_TableBorderLight] = ImColor(0, 0, 0, 255);
	style.CellPadding = ImVec2(5, 3);
}

void Window::SetYears()
{
	for (int i = minYear; i <= maxYear; i++)
	{
		Year year;
		years.insert({i, year});
	}
}

void Window::SetGridSize(int cellCount, ImGuiStyle& style)
{
	int seasonCount = sizeof(seasons) / sizeof(const char*);

	if (years.at(currYear).cells != nullptr)
	{
		for (auto& year : years)
		{
			delete[] year.second.cells;
			year.second = Year(seasonCount, cellCount);
		}
	}
	else
	{
		for (auto& year : years)
		{
			year.second = Year(seasonCount, cellCount);
		}
	}

	int colCount = sqrt(cellCount);
	//std::cout << "col count: " << colCount << std::endl;
	int cPaddingSize = (((2 * style.CellPadding.x) * (colCount - 1)) + (2 * style.CellPadding.x));
	//std::cout << "cell pad: " << cPaddingSize << std::endl;
	minCellSize = cPaddingSize / colCount;
	maxCellSize = minCellSize * 4;

	curCellSize = (maxCellSize + minCellSize) / 2;

	//std::cout << "Min Cell Size: " << minCellSize << std::endl;
	//std::cout << "Max Cell Size: " << maxCellSize << std::endl;
	//std::cout << "Cur Cell Size: " << curCellSize << std::endl;
}

void Window::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
}

void Window::MoveWindow()
{
	int windowPosX, windowPosY;
	glfwGetWindowPos(window, &windowPosX, &windowPosY);
	glfwSetWindowPos(window, windowPosX + offsetPosX, windowPosY + offsetPosY);
	offsetPosX = 0, offsetPosY = 0;
	cursorPosX += offsetPosX;
	cursorPosY += offsetPosY;
}

void Window::RunSim()
{
	int size = sqrt(cellCounts[sizeIndex]);

	// initializing layers using the Layer class in Environment.h and EnvironmentLayer struct in Simulation.h
	Layer vegLayer, preyLayer, humanLayer;
	EnvironmentLayer *vegEnvLayer = new EnvironmentLayer, *preyEnvLayer = new EnvironmentLayer, *humanEnvLayer = new EnvironmentLayer;

	vegLayer.name = (char*)"vegetation", preyLayer.name = (char*)"prey", humanLayer.name = (char*)"human";
	vegEnvLayer->name = (char*)"vegetation", preyEnvLayer->name = (char*)"prey", humanEnvLayer->name = (char*)"human";

	vegLayer.len = 10, preyLayer.len = 4, humanLayer.len = 5;

	vegLayer.xSize = size, preyLayer.xSize = size, humanLayer.xSize = size;
	vegEnvLayer->xSize = size, preyEnvLayer->xSize = size, humanEnvLayer->xSize = size;

	vegLayer.ySize = size, preyLayer.ySize, humanLayer.ySize = size;
	vegEnvLayer->ySize = size, preyEnvLayer->ySize, humanEnvLayer->ySize = size;

	int index = 0;
	vegLayer.layer = new double* [size], preyLayer.layer = new double* [size], humanLayer.layer = new double* [size];
	vegEnvLayer->grid = new double* [size], preyEnvLayer->grid = new double* [size], humanEnvLayer->grid = new double* [size];

	for (int i = 0; i < size; i++)
	{
		vegLayer.layer[i] = new double[size], preyLayer.layer[i] = new double[size], humanLayer.layer[i] = new double[size];
		vegEnvLayer->grid[i] = new double[size], preyEnvLayer->grid[i] = new double[size], humanEnvLayer->grid[i] = new double[size];

		for (int j = 0; j < size; j++)
		{
			vegLayer.layer[i][j] = (double)years.at(minYear).cells[0][index].vegetationPresent;
			vegEnvLayer->grid[i][j] = (double)years.at(minYear).cells[0][index].vegetationPresent;

			preyLayer.layer[i][j] = (double)years.at(minYear).cells[0][index].preyPresent;
			preyEnvLayer->grid[i][j] = (double)years.at(minYear).cells[0][index].preyPresent;

			humanLayer.layer[i][j] = (double)years.at(minYear).cells[0][index].humanPresent;
			humanEnvLayer->grid[i][j] = (double)years.at(minYear).cells[0][index].humanPresent;

			index++;
		}
	}

	// haven't added mute functionality yet, only the ability to show and hide filters
	vegLayer.muted = false, preyLayer.muted = false, humanLayer.muted = false;

	// initializing grid
	Grid* grid = new Grid;
	grid->xSize = size, grid->ySize = size;
	grid->speciesCount = 2;
	grid->cellCount = new int[2] { animalCounts[0], animalCounts[1] };
	grid->name = (char*)"Test";
	grid->len = 4;
	grid->layers = new Layer[3]{ vegLayer, preyLayer, humanLayer };

	// initializing simulation and adding animals and layers
	Simulation sim = Simulation(grid, maxYear);
	sim.AddAnimal(animalModels[0]);
	sim.AddAnimal(animalModels[1]);
	sim.AddLayer(vegEnvLayer);
	sim.AddLayer(preyEnvLayer);
	sim.AddLayer(humanEnvLayer);

	// getting coordinates for the all cells placed for each animal
	for (int i = 0; i < 2; i++)
	{
		if (animalCounts[i] > 0)
		{
			int* xCoords = new int[animalCounts[i]];
			int* yCoords = new int[animalCounts[i]];
			int idx = 0;

			for (int j = 0; j < size; j++)
			{
				for (int k = 0; k < size; k++)
				{
					if (years.at(minYear).cells[j][k].animalPresent[i])
					{
						xCoords[idx] = j;
						yCoords[idx] = k;
						idx++;
					}
				}

				if (idx == animalCounts[i])
				{
					break;
				}
			}

			sim.AddCells(i, 4, xCoords, yCoords);
		}
	}
}

ImVec2 Window::SetItemDimensions(int w, int h)
{
	ImGui::SetNextItemWidth(w);
	ImVec2 fPadding = ImVec2(0, (h - ImGui::GetFontSize() + 1) * 0.5f);
	return fPadding;
}

void Window::CreateMenuBar(ImGuiStyle& style)
{
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
					RunSim();
				}
			}
			else
			{
				if (ImGui::MenuItem("End Simulation"))
				{
					simRunning = false;
					SetGridSize(cellCounts[sizeIndex], style);
					ResetWindow();
				}

			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Window::CreateUpperLeftPanel(ImGuiStyle& style)
{
	ImGui::BeginChild("Upper Left Panel", ImVec2(400, 740));
	{
		style.FramePadding = SetItemDimensions(50, 50);
		ImGui::SetCursorPos(ImVec2(10, 10));
		ImGui::Checkbox(animalTypes[0], &activeAnimals[0]);
		style.FramePadding = framePadding;

		style.FramePadding = SetItemDimensions(50, 50);
		ImGui::SetCursorPos(ImVec2(10, 70));
		ImGui::Checkbox(animalTypes[1], &activeAnimals[1]);
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
		}
	}
	ImGui::EndChild();
}

void Window::CreateLowerLeftPanel(ImGuiStyle& style)
{
	ImGui::BeginChild("Lower Left Panel", ImVec2(400, 390));
	{
		ImGui::SetCursorPos(ImVec2(10, 10));
		int temp = sizeIndex;
		ImGui::Combo("Grid Size", &sizeIndex, gridSizeText, 1);
		if (simRunning && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Can't Change Size While Running");
		}

		if (simRunning && temp != sizeIndex)
		{
			sizeIndex = temp;
		}
		else if (!simRunning && temp != sizeIndex)
		{
			ResetWindow();
			SetGridSize(cellCounts[sizeIndex], style);
		}

		ImGui::SetCursorPos(ImVec2(10, 80));
		ImGui::Combo("Cell Types", &cellIndex, cellTypes, 5);
	}
	ImGui::EndChild();
}

void Window::CreateMidPanel(ImGuiTableFlags flags, ImGuiStyle& style)
{
	ImGui::BeginChild("Mid Panel", ImVec2(1160, 880));
	{
		ImGui::SetCursorPos(ImVec2(10, 10));
		int columnCount = sqrt(cellCounts[sizeIndex]);
		if (ImGui::BeginTable("Grid", columnCount, flags, ImVec2(tableSizeX, tableSizeY)))
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
				ImGui::PushStyleColor(ImGuiCol_Header, cellColor);
				CellData currCell = years.at(currYear).cells[seasonIndex][i];
				if(ImGui::Selectable(std::to_string(i).c_str(), currCell.cellPresent, 0, ImVec2(curCellSize, curCellSize)))
				{
					if (!simRunning && currYear == minYear)
					{
						PlaceAnimal(i);
					}
				}
				ImGui::PopStyleColor();

				if (!ImGui::IsItemHovered() && !currCell.cellPresent)
				{
					drawList->ChannelsSetCurrent(0);
					ImVec2 posMin = ImGui::GetItemRectMin();
					ImVec2 posMax = ImGui::GetItemRectMax();
					ImGui::GetWindowDrawList()->AddRectFilled(posMin, posMax, cellColor);
				}
				if (!simRunning && currYear != minYear && ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Animals Must Be Placed In Year 1");
				}
				
				drawList->ChannelsSetCurrent(2);

				if (currCell.animalPresent[0] && activeAnimals[0])
				{
					drawList->AddImage((void*)(intptr_t)animalTextures[0], ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), animalColors[0]);
				}

				if (currCell.animalPresent[1] && activeAnimals[1])
				{
					drawList->AddImage((void*)(intptr_t)animalTextures[1], ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), animalColors[1]);
				}

				if (currCell.vegetationPresent && activeFilters[0])
				{
					drawList->AddImage((void*)(intptr_t)filterTextures[0], ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), filterColors[0]);
				}

				if (currCell.preyPresent && activeFilters[1])
				{
					drawList->AddImage((void*)(intptr_t)filterTextures[1], ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), filterColors[1]);
				}

				if (currCell.humanPresent && activeFilters[2])
				{
					drawList->AddImage((void*)(intptr_t)filterTextures[2], ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), filterColors[2]);
				}

				drawList->ChannelsMerge();


			}
			ImGui::EndTable();
		}
	}
	ImGui::EndChild();
}

void Window::CreateLowerPanel(ImGuiStyle& style)
{
	ImGui::BeginChild("Lower Panel", ImVec2(1160, 250));
	{
		ImGui::SetCursorPos(ImVec2(10, 10));
		if (ImGui::Button(seasons[0], ImVec2(565, 100)))
		{
			seasonIndex = 0;
		}
		ImGui::SetCursorPos(ImVec2(585, 10));
		if (ImGui::Button(seasons[1], ImVec2(565, 100)))
		{
			seasonIndex = 1;
		}

		style.FramePadding = SetItemDimensions(1050, 80);
		ImGui::SetCursorPos(ImVec2(10, 140));
		ImGui::SliderInt("Year", &currYear, minYear, maxYear);
		style.FramePadding = framePadding;
	}
	ImGui::EndChild();
}

void Window::CreateRightPanel(ImGuiStyle& style)
{
	ImGui::SetCursorPos(ImVec2(1590, 50));
	ImGui::BeginChild("Right Panel", ImVec2(400, 1140));
	{
		style.FramePadding = SetItemDimensions(50, 50);
		ImGui::SetCursorPos(ImVec2(10, 10));
		ImGui::Checkbox(filterTypes[0], &activeFilters[0]);
		style.FramePadding = framePadding;

		style.FramePadding = SetItemDimensions(50, 50);
		ImGui::SetCursorPos(ImVec2(10, 70));
		ImGui::Checkbox(filterTypes[1], &activeFilters[1]);
		style.FramePadding = framePadding;

		style.FramePadding = SetItemDimensions(50, 50);
		ImGui::SetCursorPos(ImVec2(10, 130));
		ImGui::Checkbox(filterTypes[2], &activeFilters[2]);
		style.FramePadding = framePadding;
	}
	ImGui::EndChild();
}

void Window::ResetWindow()
{
	activeAnimals[0] = true, activeAnimals[1] = true;
	animalCounts[0] = 0, animalCounts[1] = 0;
	animalIndex = 0;

	activeFilters[0] = true, activeFilters[1] = true, activeFilters[2] = true;

	currYear = 1;
	seasonIndex = 0;

	cellIndex = 0;
}

void Window::PlaceAnimal(int index)
{
	int seasonCount = sizeof(seasons) / sizeof(const char*);

	if(cellIndex == 0 || cellIndex == 1)
	{
		for (int i = 0; i < seasonCount; i++)
		{
			years.at(currYear).cells[i][index].animalPresent[cellIndex] = !years.at(currYear).cells[i][index].animalPresent[cellIndex];

			if (years.at(currYear).cells[i][index].animalPresent[cellIndex])
			{
				animalCounts[cellIndex]++;
			}
			else
			{
				animalCounts[cellIndex]--;
			}
		}
	}
	else if (cellIndex == 2)
	{
		for (int i = 0; i < seasonCount; i++)
		{
			years.at(currYear).cells[i][index].vegetationPresent = !years.at(currYear).cells[i][index].vegetationPresent;
		}
	}
	else if (cellIndex == 3)
	{
		for (int i = 0; i < seasonCount; i++)
		{
			years.at(currYear).cells[i][index].preyPresent = !years.at(currYear).cells[i][index].preyPresent;
		}
	}
	else if (cellIndex == 4)
	{
		for (int i = 0; i < seasonCount; i++)
		{
			years.at(currYear).cells[i][index].humanPresent = !years.at(currYear).cells[i][index].humanPresent;
		}
	}
}

void Window::RenderWindow()
{
	ImGui::Render();
	int displayWidth, displayHeight;
	glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
	glViewport(0, 0, displayWidth, displayHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}