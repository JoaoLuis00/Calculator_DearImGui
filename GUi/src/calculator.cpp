#include "Calculator.h"

Calculator::Calculator()
    : current_input(""), last_operator(0), result(0.0), update_cursor(false) {}

Calculator::~Calculator() {}

void Calculator::render()
{

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
    ImGui::InputText("##input", &current_input);

    //Pop pushed configs
    ImGui::PopStyleColor();
    ImGui::PopFont();

    //Advance cursor for better spacing
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30);

    remainingSpace = ImGui::GetContentRegionAvail();
    //std::cout << "x: " << remainingSpace.x << "   " << "y: " << remainingSpace.y << "\n";

    float calc_x = (remainingSpace.x - 3 * style.ItemSpacing.x) / 4; //divide by 4 to calculate individual 
    float calc_y = (remainingSpace.y - 5 * style.ItemSpacing.y) / 5;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15);

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

    if (ImGui::Button("=", ImVec2(calc_x * 3 + 2 * style.ItemSpacing.x, calc_y))) { process_input('='); } ImGui::SameLine();
    if (ImGui::Button("CE", ImVec2(calc_x, calc_y))) { process_input('E'); }

    ImGui::PopStyleVar();

    ImGui::End();
}

void Calculator::process_input(char input)
{
    // Handle numeric input and decimal point
    if (isdigit(input) || input == '.') {
        current_input += input;
    }
    // Handle operators
    else if (input == '+' || input == '-' || input == '*' || input == '/') {
        if (current_input.empty() || last_operator != 0) {
            current_input = "ERROR";
        }
        else {
            last_operator = input;
            current_input += input;
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
                    if (floor(result) == result) {
                        // If result is an integer
                        current_input = std::to_string(static_cast<int>(result));
                    }
                    else {
                        // If result has decimal values
                        current_input = std::to_string(result);
                    }
                }
                last_operator = 0;
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
    }
    // Handle clear all
    else if (input == 'E') {
        current_input.clear();
        last_operator = 0;
        result = 0.0;
    }
}


