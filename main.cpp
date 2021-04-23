#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "Client.h"

const unsigned ww(1280);
const unsigned wh(720);

Client client(ww, wh);

void glfw_error_callback(int error, const char* description)
{
  std::cerr << "Error: " << description << std::endl;
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (action == GLFW_PRESS || action == GLFW_REPEAT)
    	switch (key) {
    	case GLFW_KEY_UP:
    	case GLFW_KEY_DOWN:
    	case GLFW_KEY_LEFT:
    	case GLFW_KEY_RIGHT:
    		client.moveCursor(key);
    		break;
    	default:
    		break;
    	}
}

int main(int argc, char **argv)
{
	GLFWwindow *window;

	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	window = glfwCreateWindow(ww, wh, "Connected Device Native Client", NULL, NULL);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, glfw_key_callback);

	glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	glfwSwapInterval(1);

    glViewport(0, 0, ww, wh);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	client.init();
	glfwSwapBuffers(window);
	client.load();

	while (!glfwWindowShouldClose(window)) {
		client.draw();
		glfwSwapBuffers(window);
		glfwWaitEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
