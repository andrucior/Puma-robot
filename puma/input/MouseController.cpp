#include "MouseController.h"

void MouseController::onSetCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    static bool first = true;

    if (first) {
        g_lastX = xpos;
        g_lastY = ypos;
        first = false;
    }

    float xoffset = xpos - g_lastX;
    float yoffset = g_lastY - ypos;

    g_lastX = xpos;
    g_lastY = ypos;

    g_dx = xoffset;
    g_dy = yoffset;
}

void MouseController::onSetMouseButtonCallback(GLFWwindow*, int btn, int action, int)
{
    if (btn == GLFW_MOUSE_BUTTON_LEFT)  g_leftDown = (action == GLFW_PRESS);
    if (btn == GLFW_MOUSE_BUTTON_RIGHT) g_rightDown = (action == GLFW_PRESS);
}

void MouseController::onScrollCallback(GLFWwindow*, double, double yoffset)
{
    g_scroll = (float)yoffset * 0.1f;
    g_scrolled = true;
}

void MouseController::flush()
{
    // Kliknięcie = był wciśnięty w poprzedniej klatce, teraz puszczony
    g_leftClicked = g_leftWasDown && !g_leftDown;
    g_leftWasDown = g_leftDown;
    g_dx = g_dy = g_scroll = 0;
    g_scrolled = false;
}