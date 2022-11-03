#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include <GLFW/glfw3.h>

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window);

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// glfw: whenever the keys are pressed or released, this callback is called
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// glfw: whenever the mouse buttons, this callback is called
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

#endif