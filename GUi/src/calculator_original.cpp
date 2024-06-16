#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include <imgui_internal.h>
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iostream>

#include <imconfig.h>

std::string current_input = "";
char last_operator = 0;
double result = 0.0;

static  ImVec4 WHITE  = ImVec4(244.f / 255.f, 242.f / 255.f, 222.f / 255.f, 210.f / 255.f);
static  ImVec4 ORANGE = ImVec4(233.f / 255.f, 179.f / 255.f, 132.f / 255.f, 210.f / 255.f);
static  ImVec4 GREEN = ImVec4(124.f / 255.f, 157.f / 255.f, 150.f / 255.f, 210.f / 255.f);
static  ImVec4 BLUE = ImVec4(161.f / 255.f, 204.f / 255.f, 209.f / 255.f, 210.f / 255.f);

bool refocus = false;

enum font_size
{
    SMALL = 0,
    LARGE
};

bool CustomButton(const char* label, const ImVec2& size_arg = ImVec2(0,0), ImGuiButtonFlags flags = ImGuiButtonFlags_None)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("[", "]");
    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

void process_input(char input) {
    // Handle numeric input and decimal point
    if (isdigit(input) || input == '.') {
        current_input += input;
        refocus = true;
    }
    // Handle operators
    else if (input == '+' || input == '-' || input == '*' || input == '/') {
        if (current_input.empty() || last_operator != 0) {
            current_input = "ERROR";
        }
        else {
            last_operator = input;
            current_input += input;
            refocus = true;
        }
    }
    // Handle equal sign
    else if (input == '=') {
        if (!current_input.empty() && last_operator != 0) {
            size_t operator_pos = current_input.find_last_of("+-*/");
            if (operator_pos != std::string::npos && operator_pos > 0 && operator_pos < current_input.length() - 1) {
                std::string val1_str = current_input.substr(0, operator_pos);
                std::string val2_str = current_input.substr(operator_pos + 1);

                double val1 = atof(val1_str.c_str());
                double val2 = atof(val2_str.c_str());

                switch (last_operator) {
                case '+': result = val1 + val2; break;
                case '-': result = val1 - val2; break;
                case '*': result = val1 * val2; break;
                case '/':
                    if (val2 != 0.0)
                        result = val1 / val2;
                    else
                        current_input = "ERROR";
                    break;
                }
                if (current_input != "ERROR") {
                    if (std::floor(result) == result) {
                        // If result is an integer
                        current_input = std::to_string(static_cast<int>(result));
                    }
                    else {
                        // If result has decimal values
                        current_input = std::to_string(result);
                    }
                }
                last_operator = 0;
                refocus = true;
            }
            else {
                current_input = "ERROR";
            }
        }
        else {
            current_input = "ERROR";
        }
    }
    // Handle clear last character
    else if (input == 'C') {
        if (current_input == "ERROR") {
            current_input.clear();
        }
        else if (!current_input.empty()) {
            current_input.pop_back();
        }
        refocus = true;
    }
    // Handle clear all
    else if (input == 'E') {
        current_input.clear();
        last_operator = 0;
        result = 0.0;
        refocus = true;
    }
}


static float CalculateTextWidth(const char* text, float font_size) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    float width = ImGui::CalcTextSize(text).x;
    ImGui::PopFont();
    return width;
}

void create_calculator_ui() {

    // Get the current viewport size
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2(0, 0);
    ImVec2 window_size = io.DisplaySize;

    // Create a full-screen ImGui window without decorations
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Calculator", nullptr, ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoCollapse);

    //Get style to try and calculate the button sizes to the given window width and height
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 remainingSpace = ImGui::GetContentRegionAvail();

    //Advance cursor for better spacing
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    //Load font to change size
    ImFont* big_font = io.Fonts->Fonts[LARGE];

    //Push font color just for the input text widget
    ImGui::PushStyleColor(ImGuiCol_Text, ORANGE);

    //Update font size to increase InputText Widget overall height
    ImGui::PushFont(big_font);

    ImGui::SetNextItemWidth(remainingSpace.x);
    if (ImGui::InputText("##input", &current_input)) {
        refocus = false;
    }

    
    //Pop pushed configs
    ImGui::PopStyleColor();
    ImGui::PopFont();

    //Advance cursor for better spacing
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30);
 
    remainingSpace = ImGui::GetContentRegionAvail();
    //std::cout << "x: " << remainingSpace.x << "   " << "y: " << remainingSpace.y << "\n";

    float calc_x = (remainingSpace.x - 3 * style.ItemSpacing.x) / 4; //divide by 4 to calculate individual 
    float calc_y = (remainingSpace.y - 5 * style.ItemSpacing.y) / 5;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,10);

    if (ImGui::Button("7", ImVec2(calc_x, calc_y))) { process_input('7'); } ImGui::SameLine();
    if (ImGui::Button("8", ImVec2(calc_x, calc_y))) { process_input('8'); } ImGui::SameLine();
    if (ImGui::Button("9", ImVec2(calc_x, calc_y))) { process_input('9'); } ImGui::SameLine();
    if (ImGui::Button("/", ImVec2(calc_x, calc_y))) { process_input('/'); }

    if (ImGui::Button("4", ImVec2(calc_x, calc_y))) { process_input('4'); } ImGui::SameLine();
    if (ImGui::Button("5", ImVec2(calc_x, calc_y))) { process_input('5'); } ImGui::SameLine();
    if (ImGui::Button("6", ImVec2(calc_x, calc_y))) { process_input('6'); } ImGui::SameLine();
    if (ImGui::Button("*", ImVec2(calc_x, calc_y))) { process_input('*'); }

    if (ImGui::Button("1", ImVec2(calc_x, calc_y))) { process_input('1'); } ImGui::SameLine();
    if (ImGui::Button("2", ImVec2(calc_x, calc_y))) { process_input('2'); } ImGui::SameLine();
    if (ImGui::Button("3", ImVec2(calc_x, calc_y))) { process_input('3'); } ImGui::SameLine();
    if (ImGui::Button("-", ImVec2(calc_x, calc_y))) { process_input('-'); }

    if (ImGui::Button("0", ImVec2(calc_x, calc_y))) { process_input('0'); } ImGui::SameLine();
    if (ImGui::Button(".", ImVec2(calc_x, calc_y))) { process_input('.'); } ImGui::SameLine();
    if (ImGui::Button("C", ImVec2(calc_x, calc_y))) { process_input('C'); } ImGui::SameLine();
    if (ImGui::Button("+", ImVec2(calc_x, calc_y))) { process_input('+'); }

    if (ImGui::Button("=", ImVec2(calc_x*3 + 2 * style.ItemSpacing.x, calc_y))) { process_input('='); } ImGui::SameLine();
    if (ImGui::Button("CE", ImVec2(calc_x, calc_y))) { process_input('E'); }

    ImGui::PopStyleVar();

    ImGui::End();
}

static void style_setup()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Button] = GREEN;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4( ORANGE.x, ORANGE.y, ORANGE.z, 170.f/255.f);
    style.Colors[ImGuiCol_ButtonActive] = ORANGE;
    style.Colors[ImGuiCol_FrameBgActive] = WHITE;
    style.Colors[ImGuiCol_FrameBg] = WHITE;
    style.Colors[ImGuiCol_FrameBgHovered] = WHITE;
    style.Colors[ImGuiCol_WindowBg] = WHITE;
    //style.FramePadding = ImVec2(15.f, 5.f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int mainn() {

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    
    GLFWwindow* window = glfwCreateWindow(400, 600, "Calculator", NULL, NULL);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    if (window == NULL) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //ImGui::StyleColorsDark();
    style_setup();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    //load font
    io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 45.f); // base font
    io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 80.f); // large font

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create the calculator UI
        create_calculator_ui();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(WHITE.x, WHITE.y, WHITE.z, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
