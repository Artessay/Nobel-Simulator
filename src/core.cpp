#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "core.h"
#include "Bomb.h"
#include "Model.h"
#include "config.h"
#include "Shader.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "BezierSurface.h"
#include "Camera.h"
#include "Box.h"
#include "CubeMap.h"
#include "Texture.h"
#include "callbacks.h"
#include "Picture.h"
#include "ObjState.h"

#include <vector>
#include <iostream>

// camera
Camera camera(initial_position);
vector<ObjState*> objects;

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

// bomb 

// // objects
// // pos  size    angle   axis
// // pos[0] +right    pos[1] +up  pos[2] +back
// Cylinder cylinder1(glm::vec3(-3.0f, 0.1f, -1.3f), glm::vec3(1.0f, 2.0f, 1.0f),
//                     90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
// Cylinder cylinder2(glm::vec3(-1.0f, 0.6f, -3.0f), glm::vec3(1.0f, 1.5f, 1.0f));
// Box box1(glm::vec3(3.0f, 0.6f, -4.0f), glm::vec3(1.8f, 1.5f, 2.3f), 30.0f);
// // r(default 0.5) position
// Sphere sphere1(0.8f, glm::vec3(-2.0f, 0.4f, 5.0f));

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
	// glfwWindowHint(GLFW_SAMPLES, 4);

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
    Shader ourShader("./res/shaders/modelShader.vs", "./res/shaders/modelShader.fs");
    Shader skyShader("./res/shaders/skybox.vs", "./res/shaders/skybox.fs");
    Shader lightingShader("./res/shaders/light.vs", "./res/shaders/light.fs");
	Shader lightSourceShader("./res/shaders/lightsource.vs", "./res/shaders/lightsource.fs");
    // Shader transparentShader("./res/shaders/transparent.vs", "./res/shaders/transparent.fs");

    std::vector<std::string> skybox
	{
		"res/textures/skybox/background_right.png",
		"res/textures/skybox/background_left.png",
		"res/textures/skybox/background_top.png",
		"res/textures/skybox/background_bottom.png",
		"res/textures/skybox/background_front.png",
		"res/textures/skybox/background_back.png",
	};
    
    CubeMap sky_texture(skybox);

    //Bezier Surface
    // float controlPoints[] = {
    //     -1.5, -1.5,  0.0,   -0.5, -1.5,  0.0,   0.5, -1.5,  0.0,   1.2, -1.5,  0.0,
    //     -1.5, -0.5,  0.0,   -0.0, -0.5,  1.0,   1.0, -0.5,  2.5,   1.2, -0.5,  0.0,
    //     -1.5,  0.5,  0.0,   -0.0,  0.5,  1.0,   1.0,  0.5,  2.5,   1.2,  0.5,  0.0, 
    //     -1.5,  1.5,  0.0,   -0.5,  1.5,  0.0,   0.5,  1.5,  0.0,   1.2,  1.5,  0.0
    // };
    // BezierSurface bezierS(controlPoints);
    

    Texture water_texture("res/textures/blue.jpg");
    Texture leaf_texture("res/textures/leaf.jpg");
    Texture bomb_texture("res/textures/bomb.png");
    Texture ruin_texture("res/textures/ruin.png");
    
    // load models
    // -----------
    Model street("./res/models/street/street.obj"); 
    Model tree("./res/models/Tree/tree.obj");
    Model machine3("./res/models/machine1/m1.obj");
    // Model cadillac("./res/models/Cadillac.obj");

    // explodable objects
    // pos  size    angle   axis
    // pos[0] +right    pos[1] +up  pos[2] +back
    Cylinder cylinder1(glm::vec3(-3.0f, 0.1f, -1.3f), glm::vec3(1.0f, 2.0f, 1.0f),
                       90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    ObjState* c1_state = new ObjState;
    c1_state->setState(0,ObjType::cylinder,cylinder1.getPosition(),cylinder1.getSize(),
                       cylinder1.getAngle(),cylinder1.getAxis());
    objects.push_back(c1_state);
    // cout << "num = " << objects.size() << endl;

    Cylinder cylinder2(glm::vec3(-1.0f, 0.6f, -3.0f), glm::vec3(1.0f, 1.5f, 1.0f));
    ObjState* c2_state = new ObjState;
    c2_state->setState(1,ObjType::cylinder,cylinder2.getPosition(),cylinder2.getSize(),
                       cylinder2.getAngle(),cylinder2.getAxis());
    objects.push_back(c2_state);
    // cout << "num = " << objects.size() << endl;

    Box box1(glm::vec3(3.0f, 0.6f, -4.0f), glm::vec3(1.8f, 1.5f, 2.3f), 30.0f);
    ObjState* b1_state = new ObjState;
    b1_state->setState(2,ObjType::box,box1.getPosition(),box1.getSize(),
                       box1.getAngle(),box1.getAxis());
    objects.push_back(b1_state);
    // cout << "num = " << objects.size() << endl;

    // r(default 0.5) position
    Sphere sphere1(0.5f, glm::vec3(-2.0f, 0.4f, 5.0f), glm::vec3(1.6f, 1.6f, 1.6f));
    ObjState* s1_state = new ObjState;
    s1_state->setState(3,ObjType::sphere,sphere1.getPosition(),sphere1.getSize(),
                       sphere1.getAngle(),sphere1.getAxis());
    objects.push_back(s1_state);
    // cout << "num = " << objects.size() << endl;

    // Picture a;

    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (camera.FreeCamera)
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

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        // transparentShader.use();
        // transparentShader.setMat4("view", view);
        // transparentShader.setMat4("projection", projection);

        lightingShader.use();
        lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);

        lightSourceShader.use();
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);

        // basic config
        lightingShader.use();
        lightingShader.setInt("material.diffuse", 0.5);
        lightingShader.setInt("material.specular", 0.5);
        lightingShader.setInt("NR_POINT_BOMBS", Bomb::getBombNumber());

        // light source config
        
        // directional light
        lightingShader.setVec3("dirLight.direction", -200.0f, -200.0f,  0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light
        set<Bomb*>::const_iterator it = Bomb::bombSet.begin();
        for (int i = 0; i < Bomb::getBombNumber(); ++i)
        {
            if (*it == NULL)
            {
                cout << "Bomb Program Error" << endl;
                continue;
            }
            (*it)->setObjState(objects);
            
            string attribute = "pointLights";
            attribute = attribute + "[" + to_string(i) + "].";
            lightingShader.setVec3(attribute + "position", (*it)->getPosition());
            lightingShader.setVec3(attribute + "ambient", 0.05f, 0.05f, 0.05f);
            lightingShader.setVec3(attribute + "diffuse", 0.8f, 0.8f, 0.8f);
            lightingShader.setVec3(attribute + "specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat(attribute + "constant", 1.0f);
            lightingShader.setFloat(attribute + "linear", 0.09f);
            lightingShader.setFloat(attribute + "quadratic", 0.032f);

            ++it;
        }

        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f))); 

        // render street
        // 
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
            // ourShader.setMat4("model", model);
            lightingShader.setMat4("model", model);
            // street.Draw(ourShader);
            street.Draw(lightingShader);
        }
        
        // explodable objects
        // render cylinder
        ourShader.use();
        // lightingShader.use();
        // leaf_texture.use();
        bomb_texture.use();
        {
            glm::mat4 model_cylinder1;
            model_cylinder1 = glm::translate(model_cylinder1, cylinder1.getPosition());
            model_cylinder1 = glm::rotate(model_cylinder1, glm::radians(cylinder1.getAngle()), cylinder1.getAxis());
            model_cylinder1 = glm::scale(model_cylinder1, cylinder1.getSize());
            ourShader.setMat4("model", model_cylinder1);
            cylinder1.render();
        }
        {
            glm::mat4 model_cylinder2;
            model_cylinder2 = glm::translate(model_cylinder2, cylinder2.getPosition());
            model_cylinder2 = glm::rotate(model_cylinder2, glm::radians(cylinder2.getAngle()), cylinder2.getAxis());
            model_cylinder2 = glm::scale(model_cylinder2, cylinder2.getSize());
            ourShader.setMat4("model", model_cylinder2);
            cylinder2.render();
        }
        //render sphere
        {
            glm::mat4 model_sphere1;
            model_sphere1 = glm::translate(model_sphere1, sphere1.getPosition());
            ourShader.setMat4("model", model_sphere1);
            sphere1.render();
        }
        //render box
        {
            glm::mat4 model_box1;
            model_box1 = glm::translate(model_box1, box1.getPosition());
            model_box1 = glm::rotate(model_box1, glm::radians(box1.getAngle()), box1.getAxis());
            model_box1 = glm::scale(model_box1, box1.getSize());
            ourShader.setMat4("model", model_box1);
            box1.render();
        }

        // render cube
        bomb_texture.use();
        // {
            // glm::mat4 model_bomb;
            // model_bomb = glm::translate(model_bomb, glm::vec3(3.0f, 3.3f, 3.0f));
            // ourShader.setMat4("model", model_bomb);
            // a.render();

            // model_bomb = glm::rotate(model_bomb,glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // ourShader.setMat4("model", model_bomb);
            // a.render();

            // model_bomb = glm::rotate(model_bomb,glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            // ourShader.setMat4("model", model_bomb);
            // a.render();
        // }
        

        // Bezier Surface
        // {
        //     glm::mat4 model_bazier;
        //     model_bazier = glm::rotate(model_bazier, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //     model_bazier = glm::translate(model_bazier, glm::vec3(-2.2f, -1.0f, 0.0f));
        //     model_bazier = glm::scale(model_bazier, glm::vec3(0.8f, 0.5f, 1.0f));
        //     ourShader.setMat4("model", model_bazier);
        //     bezierS.render();
        // }

        lightingShader.use();
		// render machine
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			machine3.Draw(ourShader);
		}

        // render tree 1
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(25.0f, -0.5f, 10.0f));
            model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
            lightingShader.setMat4("model", model);
            tree.Draw(ourShader);
        }
        
        // render tree 2
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(30.0f, -0.5f, 10.0f));
            model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
            lightingShader.setMat4("model", model);
            tree.Draw(ourShader);
        }     

        // // rencer car
        // glm::mat4 cadillac_matrix = glm::mat4(1.0f);
        // cadillac_matrix = glm::translate(cadillac_matrix, glm::vec3(1.0f, 0.4f, -5.0f)); // translate it down so it's at the center of the scene
        // cadillac_matrix = glm::scale(cadillac_matrix, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
        // ourShader.setMat4("model", cadillac_matrix);
        // water_texture.use();
        // cadillac.Draw(ourShader);
        

        lightSourceShader.use();
        bomb_texture.use();
        Bomb::draw(lightSourceShader);

        // transparentShader.use();
        ruin_texture.use();
        // Bomb::drawRuin(transparentShader);
        Bomb::drawRuin(lightSourceShader);
        
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
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && flip == true)
	{
		camera.FreeCamera = !camera.FreeCamera;
		firstMouse = true;	// avoid suddent jump when entering

		flip = false;	// only flip state once
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && flip == false)
	{
		flip = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		camera.FreeCamera = true;
		camera.Position = initial_position;
		camera.Yaw = YAW;
		camera.Pitch = PITCH;
		
		SceneRotateZ = 0.0f;
		SceneRotateX = 0.0f;

		camera.ProcessMouseMovement(xoff, yoff);
	}

    // put bomb
    static bool bomb_flip = true;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && bomb_flip == true)
    {
        Bomb::placeBomb(camera.Position, camera.Front, camera.Ahead);
        // cout << "Bomb Number: " << bomb_number << endl;
        bomb_flip = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && bomb_flip == false)
	{
		bomb_flip = true;
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
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
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