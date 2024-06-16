#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Calculator.h"

#include <imconfig.h>

// GLFW window
GLFWwindow* window;

static void style_setup();
static void font_setup();

// ImGui context
void initImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    style_setup();
    font_setup();
}

void cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// Style setup
static void style_setup()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Button] = GREEN;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(ORANGE.x, ORANGE.y, ORANGE.z, 170.f / 255.f);
    style.Colors[ImGuiCol_ButtonActive] = ORANGE;
    style.Colors[ImGuiCol_FrameBgActive] = WHITE;
    style.Colors[ImGuiCol_FrameBg] = WHITE;
    style.Colors[ImGuiCol_FrameBgHovered] = WHITE;
    style.Colors[ImGuiCol_WindowBg] = WHITE;
    //style.FramePadding = ImVec2(15.f, 5.f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
}

static void font_setup()
{
    ImGuiIO& io = ImGui::GetIO();

    //load font
    io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 45.f); // base font
    io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 80.f); // large font
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main() {

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    window = glfwCreateWindow(400, 600, "Calculator", NULL, NULL);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    if (window == NULL) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    initImGui(window);

    Calculator calculator;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render calculator UI
        calculator.render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(WHITE.x, WHITE.y, WHITE.z, .5f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    cleanupImGui();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
