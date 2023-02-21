#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Environmental Sim", NULL, NULL);

	if (window == NULL)
	{
		return 1;
	}

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); // vsync

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	bool showDemo = false;

	enum Seasons { Spring, Summer, Autumn, Winter };
	Seasons season = Spring;

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showDemo)
		{
			ImGui::ShowDemoWindow(&showDemo);
		}

		ImGui::Begin("Environmental Sim");

		if (ImGui::Button("Spring"))
		{
			season = Spring;
		}
		if (ImGui::Button("Summer"))
		{
			season = Summer;
		}
		if (ImGui::Button("Fall"))
		{
			season = Autumn;
		}
		if (ImGui::Button("Winter"))
		{
			season = Winter;
		}

		if (season == Spring)
		{
			ImGui::Text("Spring");
		}
		else if (season == Summer)
		{
			ImGui::Text("Summer");
		}
		else if (season == Autumn)
		{
			ImGui::Text("Autumn");
		}
		else if (season == Winter)
		{
			ImGui::Text("Winter");
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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