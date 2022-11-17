#include <GLad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "loadModel.h"
#include "config.h"
#include "CubeMap.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "callbacks.h"

#include <iostream>
#include <vector>


Camera camera1(glm::vec3(0.0f, 0.0f, 3.0f));

static bool keys[1024];

// mouse
static float lastX = (float)(SCREEN_WIDTH  / 2.0);	// mouse position in last frame
static float lastY = (float)(SCREEN_HEIGHT / 2.0);	// mouse position in last frame

static float SceneRotateZ = 0.0f;
static float SceneRotateX = 0.0f;

static bool firstMouse = true;	// get mouse position for the first time. 
static float xoff = 0.0f, yoff = 0.0f;	// mouse move offset

static bool dragRotate = false;

// timing
static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;	// time of last frame


int loadModel()
{
    // glfw: initialize and configure
    // ------------------------------
    if (glfwInit() == GLFW_FALSE)
    {
        std::cout << "GLFW initialize failed,invoke glfwInit()......Error file:" << __FILE__ << "......Error line:" << __LINE__ << std::endl;
		glfwTerminate();
		return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader ourShader("./res/modelShader1.vs", "./res/modelShader1.fs");


    Shader skyShader("./res/skybox.vs", "./res/skybox.fs");

    std::vector<std::string> skybox
	{
		"res/skybox/background_right.png",
		"res/skybox/background_left.png",
		"res/skybox/background_top.png",
		"res/skybox/background_bottom.png",
		"res/skybox/background_front.png",
		"res/skybox/background_back.png",
	};
    
    CubeMap sky_texture(skybox);




    // load models
    // -----------
    Model ourModel("./res/models/street/street.obj"); //
    Model tree("./res/models/Tree/tree.obj");
    Model m3("./res/models/machine1/m1.obj");


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (camera1.FreeCamera)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera1.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera1.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        

        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(1.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
        model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model2);
        m3.Draw(ourShader);

        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(25.0f, -0.5f, 10.0f));
        model3 = glm::scale(model3, glm::vec3(1.5f, 1.5f, 1.5f));
        ourShader.setMat4("model", model3);
        tree.Draw(ourShader);

        glm::mat4 model4 = glm::mat4(1.0f);
        model4 = glm::translate(model4, glm::vec3(30.0f, -0.5f, 10.0f));
        model4 = glm::scale(model4, glm::vec3(1.5f, 1.5f, 1.5f));
        ourShader.setMat4("model", model4);
        tree.Draw(ourShader);

        skyShader.use();
		view = glm::mat4(glm::mat3(camera1.GetViewMatrix()));
		skyShader.setMat4("view", view);
		skyShader.setMat4("projection", projection);

		sky_texture.render();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera1.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera1.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera1.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera1.ProcessKeyboard(RIGHT, deltaTime);

	static bool flip = true;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && flip == true)
	{
		camera1.FreeCamera = !camera1.FreeCamera;
		firstMouse = true;	// avoid suddent jump when entering

		flip = false;	// only flip state once
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && flip == false)
	{
		flip = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		camera1.FreeCamera = true;
		camera1.Position = glm::vec3(0.0f, 250.0f, -450.0f);
		camera1.Yaw = 90.0f;
		camera1.Pitch = -40.0f;
		
		SceneRotateZ = 0.0f;
		SceneRotateX = 0.0f;

		camera1.ProcessMouseMovement(xoff, yoff);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the keys are pressed or released, this callback is called
// ---------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	else
	{
		std::cout << "Key Callback: Out of Index" << std::endl;
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

	if (dragRotate)
	{
		SceneRotateZ += yoffset * 0.1f;
		SceneRotateX += xoffset * 0.1f;
	}

	camera1.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// camera.ProcessMouseScroll(static_cast<float>(yoffset));

	if (yoffset == 1)
	{
		camera1.ProcessKeyboard(SCROLL_FORWARD, deltaTime);
	}
	else
	{
		camera1.ProcessKeyboard(SCROLL_BACKWARD, deltaTime);
	}
}

// glfw: whenever the mouse buttons, this callback is called
// ----------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (!camera1.FreeCamera)
	{
		if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
			dragRotate = true;
		else dragRotate = false;
	}
}