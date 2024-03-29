#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <curl/curl.h>

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
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
    	if (client.showModal())
    		client.setShowModal(false);
    	else
    		glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
    	client.setShowModal(true);
#ifndef NDEBUG
    } else if (action == GLFW_PRESS && key == GLFW_KEY_X) {
    	client.setGridWidth(client.gridWidth() + 1);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_C) {
    	client.setGridWidth(client.gridWidth() - 1);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_Y) {
    	client.setGridHeight(client.gridHeight() + 1);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_U) {
    	client.setGridHeight(client.gridHeight() - 1);
#endif
    } else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
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
}

int main(int argc, char **argv)
{
	if (curl_global_init(CURL_GLOBAL_DEFAULT))
		return -1;

	GLFWwindow *window;

	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	window = glfwCreateWindow(ww, wh, "Connected Device Native Client", nullptr, nullptr);
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
    curl_global_cleanup();
	return 0;
}
