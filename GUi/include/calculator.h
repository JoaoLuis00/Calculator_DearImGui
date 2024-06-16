#pragma once

#include "imgui.h"
#include <imgui_internal.h>
#include "imgui_stdlib.h"
#include <string>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>

class Calculator {
public:
    Calculator();
    ~Calculator();

    void render();

private:
    std::string current_input;
    char last_operator;
    double result;
    bool update_cursor;

    void process_input(char input);
};

static  ImVec4 WHITE = ImVec4(244.f / 255.f, 242.f / 255.f, 222.f / 255.f, 210.f / 255.f);
static  ImVec4 ORANGE = ImVec4(233.f / 255.f, 179.f / 255.f, 132.f / 255.f, 210.f / 255.f);
static  ImVec4 GREEN = ImVec4(124.f / 255.f, 157.f / 255.f, 150.f / 255.f, 210.f / 255.f);
static  ImVec4 BLUE = ImVec4(161.f / 255.f, 204.f / 255.f, 209.f / 255.f, 210.f / 255.f);

enum font_size
{
    SMALL = 0,
    LARGE
};