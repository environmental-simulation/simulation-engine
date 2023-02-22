#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cstring>


enum Seasons
{
	Summer,
	Winter
};
// Initializes GLFW
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
// Initializes ImGui
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
// Starts ImGui frame
void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(2000, 1200));
	//ImGui::SetNextWindowPos();
}
// Rendering the window
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
// Displays text on the window
void ImGuiTextHandler(Seasons season, bool filt1Active, bool filt2Active, bool filt3Active)
{
	if (season == Summer)
	{
		ImGui::Text("Summer");
	}
	else if (season == Winter)
	{
		ImGui::Text("Winter");
	}

	std::string text = filt1Active ? "Filter 1: true" : "Filter 1: false";
	const char* filtText = text.c_str();
	ImGui::Text(filtText);
	text = filt2Active ? "Filter 2: true" : "Filter 2: false";
	filtText = text.c_str();
	ImGui::Text(filtText);
	text = filt3Active ? "Filter 3: true" : "Filter 3: false";
	filtText = text.c_str();
	ImGui::Text(filtText);
}
// Setting colors
void Colors()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImColor(83, 83, 83);
	style.Colors[ImGuiCol_ChildBg] = ImColor(37, 36, 42);
	//style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0);
}
ImVec2 SetItemDimensions(int width, int height)
{
	ImGui::SetNextItemWidth(width);
	ImVec2 framePadding = ImVec2(0, (height - ImGui::GetFontSize() + 1) * 0.5f);

	return framePadding;
}
int main()
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

	bool showDemo = false;
	bool filt1Active = false, filt2Active = false, filt3Active = false;
	int currYear = 1;
	int currAnimal = 0;
	const char* animals[] = { "Wolf", "Rabbit" };

	Seasons season = Summer;
	const char* test = "test";

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 framePadding = style.FramePadding;

	
	// https://imgui-test.readthedocs.io/en/latest/#
	

	while (!glfwWindowShouldClose(window))
	{
		StartFrame();
		Colors();
		
		if (showDemo)
		{
			ImGui::ShowDemoWindow(&showDemo);
		}

		ImGui::Begin("Environmental Sim", NULL, ImGuiWindowFlags_NoResize);

		// Left panel
		ImGui::SetCursorPos(ImVec2(10, 30));
		ImGui::BeginChild("Left Panel", ImVec2(400, 1160));
		{
			ImGui::Combo("Animals", &currAnimal, animals, 2);

			ImGui::SetCursorPos(ImVec2(25, 1050));
			if (ImGui::Button("Show Demo", ImVec2(350, 100)))
			{
				showDemo = !showDemo;
			}
		}
		ImGui::EndChild();

		// Middle panel
		ImGui::SetCursorPos(ImVec2(420, 30));
		ImGui::BeginChild("Mid Panel", ImVec2(1160, 900));
		{
			ImGuiTextHandler(season, filt1Active, filt2Active, filt3Active);
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
		ImGui::SetCursorPos(ImVec2(1590, 30));
		ImGui::BeginChild("Right Panel", ImVec2(400, 1160));
		{
			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 10));
			ImGui::Checkbox("\tFilter 1", &filt1Active);
			style.FramePadding = framePadding;

			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 70));
			ImGui::Checkbox("\tFilter 2", &filt2Active);
			style.FramePadding = framePadding;

			style.FramePadding = SetItemDimensions(50, 50);
			ImGui::SetCursorPos(ImVec2(10, 130));
			ImGui::Checkbox("\tFilter 3", &filt3Active);
			style.FramePadding = framePadding;

			ImGui::SetCursorPos(ImVec2(25, 1050));
			if (ImGui::Button("Close Window", ImVec2(350, 100)))
			{
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
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