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

#include <cmath>
#include <vector>
#include <wtypes.h>
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

void GetDesktopResolution(float& horizontal, float& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
	
}

int core()
{
    float scr_width, scr_height;
    GetDesktopResolution(scr_width, scr_height); // get resolution for create window
    SCREEN_WIDTH = (int)scr_width;
    SCREEN_HEIGHT = (int)scr_height;

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
    // Shader lightingShader("./res/shaders/light.vs", "./res/shaders/light.fs");
	Shader lightSourceShader("./res/shaders/lightsource.vs", "./res/shaders/lightsource.fs");
    Shader shader("res/shaders/point_shadows.vs", "res/shaders/point_shadows.fs");
    Shader simpleDepthShader("res/shaders/point_shadows_depth.vs", "res/shaders/point_shadows_depth.fs", "res/shaders/point_shadows_depth.gs");

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("depthMap", 1);

    // lighting info
    // -------------
    glm::vec3 lightPos(0.0f, 10.0f, 10.0f);

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

    Texture water_texture("res/textures/water.png");
    Texture leaf_texture("res/textures/leaf.jpg");
    Texture bomb_texture("res/textures/bomb.png");
    Texture ruin_texture("res/textures/ruin.png");
    Texture wall_texture("res/textures/wall.png");
    Texture wall_texture1("res/textures/wall1.jpg");
    Texture wall_texture2("res/textures/wall2.jpg");  
	Texture earth_texture("res/textures/planets/earth.jpg");
    Texture box_texture("res/textures/box.png");
    Texture box_texture1("res/textures/boxburnt.png");cout<<"KKKKKKK ";
    Texture fire_texture("res/textures/fire.png");

    // load models
    // -----------
    Model street("./res/models/street/street.obj"); 
    Model tree("./res/models/Tree/tree.obj");
    Model machine3("./res/models/machine1/m1.obj");
    cout<<"aaaaa"<<endl;
    glm::vec3 explode_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    int burning[7] = {-1};
    burning[6]=-1;
    Picture fire(0.5f, 0.5f);

    

    // explodable objects
    // pos  size    angle   axis
    // pos[0] +right    pos[1] +up  pos[2] +back
    Box cylinder1(glm::vec3(-4.5f, 0.0f, -9.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    ObjState* c1_state = new ObjState;
    c1_state->setState(0,ObjType::cylinder,cylinder1.getPosition(),cylinder1.getSize(),
                       cylinder1.getAngle(),cylinder1.getAxis());
    objects.push_back(c1_state); //0

    Box cylinder2(glm::vec3(-3.0f, 0.0f, -9.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    ObjState* c2_state = new ObjState;
    c2_state->setState(1,ObjType::cylinder,cylinder2.getPosition(),cylinder2.getSize(),
                       cylinder2.getAngle(),cylinder2.getAxis());
    objects.push_back(c2_state); //1

    Box box1(glm::vec3(-1.5f, 0.0f, -9.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    ObjState* b1_state = new ObjState;
    b1_state->setState(2,ObjType::box,box1.getPosition(),box1.getSize(),
                       box1.getAngle(),box1.getAxis());
    objects.push_back(b1_state); //2

    // r(default 0.5) position
    Sphere sphere1(0.5f, glm::vec3(-2.0f, 0.4f, 5.0f), glm::vec3(1.6f, 1.6f, 1.6f));
    ObjState* s1_state = new ObjState;
    s1_state->setState(3,ObjType::sphere,sphere1.getPosition(),sphere1.getSize(),
                       sphere1.getAngle(),sphere1.getAxis());
    objects.push_back(s1_state); //3
    
    Sphere sphere2(0.5f, glm::vec3(-8.0f, 0.1f, 6.0f));
    ObjState* s2_state = new ObjState;
    s2_state->setState(4,ObjType::sphere,sphere2.getPosition(),sphere2.getSize(),
                       sphere2.getAngle(),sphere2.getAxis());
    objects.push_back(s2_state); //4

    //machine, bbox type is box
    Box box2(glm::vec3(1.0f, 0.2f, 0.0f), glm::vec3(1.2f, 1.2f, 0.8f));
    ObjState* b2_state = new ObjState;
    b2_state->setState(5,ObjType::box,box2.getPosition(),box2.getSize(),
                       box2.getAngle(),box2.getAxis());
    objects.push_back(b2_state); //5

    Box box3(glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(1.2f, 1.2f, 1.2f));
    ObjState* b3_state = new ObjState;
    b3_state->setState(6,ObjType::box,box3.getPosition(),box3.getSize(),
                       box3.getAngle(),box3.getAxis());
    objects.push_back(b3_state); //6 wood box

    camera.setObjState(objects);

    cout<<"bbbbb"<<endl;
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // move light position over time
        lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.1) * 10.0);
        float lightIntensity = static_cast<float>(sin(glfwGetTime() * 0.1) * 0.6 + 0.5);
        
        // @shadow   
        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", lightPos);
        
        // renders the 3D scene
        // --------------------
        {
            Shader &shader_ = simpleDepthShader;
            shader_.setInt("reverse_normals", 0);
            // glEnable(GL_CULL_FACE);
            // render street
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(30.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
                // ourShader.setMat4("model", model);
                shader_.setMat4("model", model);
                // street.Draw(ourShader);
                street.Draw(shader_);
            }

            // explodable objects
            // --------------------
            
            //render sphere
            earth_texture.use();
            {
                glm::mat4 model_sphere2;
                model_sphere2 = glm::translate(model_sphere2, sphere2.getPosition());
                shader_.setMat4("model", model_sphere2);
                sphere2.render();
            }

            //render sphere
            leaf_texture.use();
            {
                glm::mat4 model_sphere1;
                model_sphere1 = glm::translate(model_sphere1, sphere1.getPosition());
                model_sphere1 = glm::scale(model_sphere1, sphere1.getSize());
                shader_.setMat4("model", model_sphere1);
                sphere1.render();
            }

            if(objects[0]->bomb_affected == 0) wall_texture.use();
            else if(objects[0]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
        
{
            glm::mat4 model_cylinder1;
            model_cylinder1 = glm::translate(model_cylinder1, cylinder1.getPosition());
            model_cylinder1 = glm::rotate(model_cylinder1, glm::radians(cylinder1.getAngle()), cylinder1.getAxis());
            model_cylinder1 = glm::scale(model_cylinder1, cylinder1.getSize());
            shader_.setMat4("model", model_cylinder1);
            cylinder1.render();
        }
        if(objects[1]->bomb_affected == 0) wall_texture.use();
            else if(objects[1]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
        {
            glm::mat4 model_cylinder2;
            model_cylinder2 = glm::translate(model_cylinder2, cylinder2.getPosition());
            model_cylinder2 = glm::rotate(model_cylinder2, glm::radians(cylinder2.getAngle()), cylinder2.getAxis());
            model_cylinder2 = glm::scale(model_cylinder2, cylinder2.getSize());
            shader_.setMat4("model", model_cylinder2);
            cylinder2.render();
        }
            //render box
            if(objects[2]->bomb_affected == 0) wall_texture.use();
            else if(objects[2]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
            {
                glm::mat4 model_box1;
                model_box1 = glm::translate(model_box1, box1.getPosition());
                model_box1 = glm::rotate(model_box1, glm::radians(box1.getAngle()), box1.getAxis());
                model_box1 = glm::scale(model_box1, box1.getSize());
                shader_.setMat4("model", model_box1);
                box1.render();
            }

            //render box
            if(objects[2]->bomb_affected == 0) wall_texture.use();
            else if(objects[2]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
            {
                glm::mat4 model_box1;
                model_box1 = glm::translate(model_box1, box1.getPosition());
                model_box1 = glm::rotate(model_box1, glm::radians(box1.getAngle()), box1.getAxis());
                model_box1 = glm::scale(model_box1, box1.getSize());
                shader_.setMat4("model", model_box1);
                box1.render();
            }
            
            // render machine
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
                shader_.setMat4("model", model);
                machine3.Draw(shader_);
            }
        fire_texture.use();
                glm::mat4 model_fire = glm::mat4(1.0f);
                model_fire = glm::translate(model_fire, objects[6]->getPos());
                shader_.setMat4("model", model_fire);
                fire.render();

                box_texture.use();

            if(burning[6] != 0)box_texture.use();
            else {
                box_texture1.use();
            }
                {
                    glm::mat4 model_box3 = glm::mat4(1.0f);
                    model_box3 = glm::translate(model_box3, objects[6]->getPos()); // translate it down so it's at the center of the scene
                    shader_.setMat4("model", model_box3);
                    box3.render();
                }
        
            {
                glm::mat4 model_box3 = glm::mat4(1.0f);
                model_box3 = glm::translate(model_box3, objects[6]->getPos()); // translate it down so it's at the center of the scene
                shader_.setMat4("model", model_box3);
                box3.render();
            }
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal 
        // -------------------------
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set lighting uniforms
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        shader.setFloat("pointLight.ambient", lightIntensity);
        shader.setFloat("far_plane", far_plane);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

        // renders the 3D scene
        // --------------------
        {
            Shader &shader_ = shader;
            shader_.setInt("reverse_normals", 0);
            // glEnable(GL_CULL_FACE);
            // render street
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(30.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
                // ourShader.setMat4("model", model);
                shader_.setMat4("model", model);
                // street.Draw(ourShader);
                street.Draw(shader_);
            }

            // explodable objects
            // --------------------
            
            //render sphere
            earth_texture.use();
            {
                glm::mat4 model_sphere2;
                model_sphere2 = glm::translate(model_sphere2, sphere2.getPosition());
                shader_.setMat4("model", model_sphere2);
                sphere2.render();
            }

            //render sphere
            leaf_texture.use();
            {
                glm::mat4 model_sphere1;
                model_sphere1 = glm::translate(model_sphere1, sphere1.getPosition());
                model_sphere1 = glm::scale(model_sphere1, sphere1.getSize());
                shader_.setMat4("model", model_sphere1);
                sphere1.render();
            }
            if(objects[0]->bomb_affected == 0) wall_texture.use();
            else if(objects[0]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
        
{
            glm::mat4 model_cylinder1;
            model_cylinder1 = glm::translate(model_cylinder1, cylinder1.getPosition());
            model_cylinder1 = glm::rotate(model_cylinder1, glm::radians(cylinder1.getAngle()), cylinder1.getAxis());
            model_cylinder1 = glm::scale(model_cylinder1, cylinder1.getSize());
            shader_.setMat4("model", model_cylinder1);
            cylinder1.render();
        }
        if(objects[1]->bomb_affected == 0) wall_texture.use();
            else if(objects[1]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
        {
            glm::mat4 model_cylinder2;
            model_cylinder2 = glm::translate(model_cylinder2, cylinder2.getPosition());
            model_cylinder2 = glm::rotate(model_cylinder2, glm::radians(cylinder2.getAngle()), cylinder2.getAxis());
            model_cylinder2 = glm::scale(model_cylinder2, cylinder2.getSize());
            shader_.setMat4("model", model_cylinder2);
            cylinder2.render();
        }
            //render box
            if(objects[2]->bomb_affected == 0) wall_texture.use();
            else if(objects[2]->bomb_affected == 1) {wall_texture1.use();}
            else {wall_texture2.use();}
            {
                glm::mat4 model_box1;
                model_box1 = glm::translate(model_box1, box1.getPosition());
                model_box1 = glm::rotate(model_box1, glm::radians(box1.getAngle()), box1.getAxis());
                model_box1 = glm::scale(model_box1, box1.getSize());
                shader_.setMat4("model", model_box1);
                box1.render();
            }
            
            // render machine
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
                shader_.setMat4("model", model);
                machine3.Draw(shader_);
            }

            if(burning[6] != 0)box_texture.use();
            else {
                box_texture1.use();
            }
            {
                glm::mat4 model_box3 = glm::mat4(1.0f);
                model_box3 = glm::translate(model_box3, objects[6]->getPos()); // translate it down so it's at the center of the scene
                shader_.setMat4("model", model_box3);
                box3.render();
            }

            water_texture.use();
            {
                glm::mat4 model_cylinder1;
                model_cylinder1 = glm::translate(model_cylinder1, cylinder1.getPosition());
                model_cylinder1 = glm::rotate(model_cylinder1, glm::radians(cylinder1.getAngle()), cylinder1.getAxis());
                model_cylinder1 = glm::scale(model_cylinder1, cylinder1.getSize());
                shader.setMat4("model", model_cylinder1);
                cylinder1.render();
            }
            {
                glm::mat4 model_cylinder2;
                model_cylinder2 = glm::translate(model_cylinder2, cylinder2.getPosition());
                model_cylinder2 = glm::rotate(model_cylinder2, glm::radians(cylinder2.getAngle()), cylinder2.getAxis());
                model_cylinder2 = glm::scale(model_cylinder2, cylinder2.getSize());
                shader.setMat4("model", model_cylinder2);
                cylinder2.render();
            }
        }
        

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        
        lightSourceShader.use();
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);

        // lightingShader.use();
        // lightingShader.setMat4("view", view);
		// lightingShader.setMat4("projection", projection);


        // // basic config
        // lightingShader.use();
        // lightingShader.setInt("material.diffuse", 0.5);
        // lightingShader.setInt("material.specular", 0.5);
        // lightingShader.setInt("NR_POINT_BOMBS", Bomb::getBombNumber());

        // // light source config
        // lightingShader.setVec3("viewPos", camera.Position);
        // lightingShader.setFloat("material.shininess", 32.0f);
        
        // // directional light
        // lightingShader.setVec3("dirLight.direction", -200.0f, -200.0f,  0.0f);
        // lightingShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
        // lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        // lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

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
            
            // string attribute = "pointLights";
            // attribute = attribute + "[" + to_string(i) + "].";
            // lightingShader.setVec3(attribute + "position", (*it)->getPosition());
            // lightingShader.setVec3(attribute + "ambient", 0.05f, 0.05f, 0.05f);
            // lightingShader.setVec3(attribute + "diffuse", 0.8f, 0.8f, 0.8f);
            // lightingShader.setVec3(attribute + "specular", 1.0f, 1.0f, 1.0f);
            // lightingShader.setFloat(attribute + "constant", 1.0f);
            // lightingShader.setFloat(attribute + "linear", 0.09f);
            // lightingShader.setFloat(attribute + "quadratic", 0.032f);

            ++it;
        }

        // spotLight
        // lightingShader.setVec3("spotLight.position", camera.Position);
        // lightingShader.setVec3("spotLight.direction", camera.Front);
        // lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        // lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        // lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        // lightingShader.setFloat("spotLight.constant", 1.0f);
        // lightingShader.setFloat("spotLight.linear", 0.09f);
        // lightingShader.setFloat("spotLight.quadratic", 0.032f);
        // lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        // lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f))); 

        // render cylinder
        ourShader.use();
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

        ourShader.use();
        // render tree 1
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(25.0f, -0.5f, 10.0f));
            model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
            ourShader.setMat4("model", model);
            tree.Draw(ourShader);
        }
        
        // render tree 2
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(30.0f, -0.5f, 10.0f));
            model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
            ourShader.setMat4("model", model);
            tree.Draw(ourShader);
        }     
        

        lightSourceShader.use();
        bomb_texture.use();
        explode_pos = Bomb::draw(lightSourceShader);

        auto obj_it = objects.begin();
        for(;obj_it != objects.end();obj_it++){
            glm::vec3 obj_pos = (*obj_it)->getPos();
            if(abs(obj_pos.x - explode_pos.x) + abs(obj_pos.y - explode_pos.y)
                + abs(obj_pos.z - explode_pos.z) < 3.0f){
                    (*obj_it)->bomb_affected ++;
                }
        }

        // transparentShader.use();
        ruin_texture.use();
        // Bomb::drawRuin(transparentShader);
        Bomb::drawRuin(lightSourceShader);

        if(burning[6] != 0 && objects[6]->bomb_affected != 0){
            
            if(burning[6] == -1)burning[6] = 300;
            else burning[6]--;
            fire_texture.use();
            glm::mat4 model_fire = glm::mat4(1.0f);
            model_fire = glm::translate(model_fire, objects[6]->getPos());
            model_fire = glm::translate(model_fire, glm::vec3(0.0f,0.0f,objects[6]->getSize().z/2 + 0.001f));
            model_fire = glm::scale(model_fire, glm::vec3(1.0f + 0.005 * ((burning[6]/10)%2) ));
            lightSourceShader.setMat4("model", model_fire);
            fire.render();

            model_fire = glm::translate(model_fire, glm::vec3(0.0f,0.0f,-objects[6]->getSize().z - 0.002f));
            model_fire = glm::scale(model_fire, glm::vec3(1.0f + 0.005 * ((burning[6]/10)%2) ));
            lightSourceShader.setMat4("model", model_fire);
            fire.render();

            model_fire = glm::mat4(1.0f);
            model_fire = glm::translate(model_fire, objects[6]->getPos());
            model_fire = glm::rotate(model_fire, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f) );
            model_fire = glm::translate(model_fire, glm::vec3(0.0f,0.0f,objects[6]->getSize().z/2 + 0.001f));
            model_fire = glm::scale(model_fire, glm::vec3(1.0f + 0.005 * ((burning[6]/10)%2) ));
            lightSourceShader.setMat4("model", model_fire);
            fire.render();

            model_fire = glm::translate(model_fire, glm::vec3(0.0f,0.0f,-objects[6]->getSize().z - 0.002f));
            model_fire = glm::scale(model_fire, glm::vec3(1.0f + 0.005 * ((burning[6]/10)%2) ));
            lightSourceShader.setMat4("model", model_fire);
            fire.render();
        }
        
        
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