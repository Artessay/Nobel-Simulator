#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core.h"
#include "Ring.h"
#include "config.h"
#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"
#include "CubeMap.h"
#include "Texture.h"
#include "callbacks.h"

#include <vector>
#include <iostream>

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// key
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

int core()
{
    // glfw: initialize and configure
    // ------------------------------
    if (glfwInit() == GLFW_FALSE)
    {
        std::cout << "GLFW initialize failed,invoke glfwInit()......Error file:" << __FILE__ << "......Error line:" << __LINE__ << std::endl;
		glfwTerminate();
		return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, NULL, NULL);
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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	// configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("./res/shader.vs", "./res/shader.fs");
	Shader skyShader("./res/skybox.vs", "./res/skybox.fs");
	Shader lightShader("./res/light.vs", "./res/light.fs");
	
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    

	// load and create a texture 
    // -------------------------
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

	Texture texture_sun			("./res/planets/sun.jpg");
	Texture texture_earth 		("./res/planets/earth.jpg");
	Texture texture_moon 		("./res/planets/moon.jpg");
	Texture texture_mars 		("./res/planets/mars.jpg");

    Sphere Sun		(100.0f, 180);
	Sphere Earth	(11.8f);
	Sphere Mars		( 8.0f);
	Sphere Moon		( 5.5f);
	
	camera.LookAtPos = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.Position = glm::vec3(0.0f, 250.0f, -450.0f);
	camera.Yaw = 90.0f;
	camera.Pitch = -40.0f;
	camera.ProcessMouseMovement(xoff, yoff);

	
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		// camera state 
		// --------------------

		// camera speed control
		if (!camera.FreeCamera)
		{
			if (camera.Position.y <  50.0f)
				camera.MovementSpeed = 50.0f;
			else if (camera.Position.y >= 50.0f && camera.Position.y < 100.f)
				camera.MovementSpeed = 100.0f;
			else if (camera.Position.y >= 100.f && camera.Position.y < 200.0f)
				camera.MovementSpeed = 200.0f;
			else if (camera.Position.y >= 200.f && camera.Position.y < 300.0f)
				camera.MovementSpeed = 300.0f;
			else if (camera.Position.y >= 300 && camera.Position.y < 400.0f)
				camera.MovementSpeed = 400.0f;
			else
				camera.MovementSpeed = 500.0f;
		}

		// ban drag rotate when camera is free
        if (camera.FreeCamera)
		{
			SceneRotateZ = 0.0f;
			SceneRotateX = 0.0f;
		}
		
		// cursor display
		if (camera.FreeCamera)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // input
        // -----
        processInput(window);

		// render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 10000.0f);
		
		ourShader.use();
		ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		// SUN
		texture_sun.use();
		glm::mat4 model_sun;
		model_sun = glm::rotate(model_sun, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_sun = glm::rotate(model_sun, (float)glfwGetTime() * glm::radians(-23.5f) * 0.25f, glm::vec3(0.0f, 0.0f, 1.f));
		
		ourShader.setMat4("model", model_sun);
		Sun.render();

		float x, z;
		const static float PlanetSpeed = 0.1f;
		

		// EARTH 
		glm::mat4 model_earth;
		x = sin(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f *1.3f;
		z = cos(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f *1.3f;
		
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateZ), glm::vec3(1.0f, 0.0f, 0.0f));
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_earth = glm::translate(model_earth, glm::vec3(x, 0.0f, z));
		glm::vec3 EarthPoint = glm::vec3(x, 0.0f, z);
		
		model_earth = glm::rotate(model_earth, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_earth = glm::rotate(model_earth, glm::radians(33.25f), glm::vec3(0.0f, 1.0f, 0.f));
		model_earth = glm::rotate(model_earth, (float)glfwGetTime() * glm::radians(-33.25f) * 2.0f, glm::vec3(0.0f, 0.0f, 1.f));
		camera.LookAtPos = glm::vec3(model_earth[3][0], model_earth[3][1], model_earth[3][2]);
		ourShader.setMat4("model", model_earth);

		texture_earth.use();
		Earth.render();  
		
		// MOON
		glm::mat4 model_moon;
		x = sin(glfwGetTime() * PlanetSpeed * 67.55f) * 100.0f * 0.5f *1.3f;
		z = cos(glfwGetTime() * PlanetSpeed * 67.55f) * 100.0f * 0.5f *1.3f;
		
		model_moon = glm::rotate(model_moon, glm::radians(SceneRotateZ), glm::vec3(1.0f, 0.0f, 0.0f));
		model_moon = glm::rotate(model_moon, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_moon = glm::translate(model_moon, EarthPoint);
		model_moon = glm::translate(model_moon, glm::vec3(x, 0.0f, z));
		model_moon = glm::rotate(model_moon, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_moon = glm::rotate(model_moon, glm::radians(32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_moon = glm::rotate(model_moon, (float)glfwGetTime() * glm::radians(-32.4f) * 3.1f, glm::vec3(0.0f, 0.0f, 1.f));
		ourShader.setMat4("model", model_moon);

		texture_moon.use();
		Moon.render();

		// MARS
		glm::mat4 model_mars;
		x = sin(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f *1.3f;
		z = cos(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f *1.3f;
		
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateZ), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_mars = glm::translate(model_mars, glm::vec3(x, 0.0f, z));
		
		model_mars = glm::rotate(model_mars, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mars = glm::rotate(model_mars, glm::radians(32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_mars = glm::rotate(model_mars, (float)glfwGetTime() * glm::radians(-32.4f) * 2.1f, glm::vec3(0.0f, 0.0f, 1.f));
		ourShader.setMat4("model", model_mars);

		texture_mars.use();
		Mars.render();

		
		// background
		skyShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	static bool flip = true;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && flip == true)
	{
		camera.FreeCamera = !camera.FreeCamera;
		firstMouse = true;	// avoid suddent jump when entering

		flip = false;	// only flip state once
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && flip == false)
	{
		flip = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		camera.FreeCamera = true;
		camera.Position = glm::vec3(0.0f, 250.0f, -450.0f);
		camera.Yaw = 90.0f;
		camera.Pitch = -40.0f;
		
		SceneRotateZ = 0.0f;
		SceneRotateX = 0.0f;

		camera.ProcessMouseMovement(xoff, yoff);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// camera.ProcessMouseScroll(static_cast<float>(yoffset));

	if (yoffset == 1)
	{
		camera.ProcessKeyboard(SCROLL_FORWARD, deltaTime);
	}
	else
	{
		camera.ProcessKeyboard(SCROLL_BACKWARD, deltaTime);
	}
}

// glfw: whenever the mouse buttons, this callback is called
// ----------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (!camera.FreeCamera)
	{
		if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
			dragRotate = true;
		else dragRotate = false;
	}
}