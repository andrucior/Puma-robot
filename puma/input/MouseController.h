#pragma once
#include <glfw/glfw3.h>

class MouseController {
public:
    inline static bool  g_leftDown = false;
    inline static bool  g_rightDown = false;
    inline static bool  g_leftClicked = false; 
    inline static float g_lastX = 0, g_lastY = 0;
    inline static float g_scrolled = false;
    inline static float g_dx = 0, g_dy = 0, g_scroll = 0;
    inline static float g_cursorX = 0, g_cursorY = 0;
    
private:
    inline static float multiplicator = 1;
    inline static bool  g_leftWasDown = false;

public:
    static void onSetCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    static void onSetMouseButtonCallback(GLFWwindow*, int btn, int action, int);

    static void onScrollCallback(GLFWwindow*, double, double yoffset);

    static void flush();
};