#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

void glfw_error_callback(int error, const char* description)
{
  std::cerr << "Error: " << description << std::endl;
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

const unsigned ww(1280);
const unsigned wh(720);

int main(int argc, char **argv)
{
	GLFWwindow *window;

	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(ww, wh, "Connected Device Native Client", NULL, NULL);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, glfw_key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwWaitEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
