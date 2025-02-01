//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GLEW/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>
#include "SkyBox.hpp"


#include <vector>

#include <windows.h>
#include <mmsystem.h>


#pragma comment(lib, "winmm.lib")

#include <glm/gtx/rotate_vector.hpp>

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2*4096;
const unsigned int SHADOW_HEIGHT = 2*4096;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
				glm::vec3(0.0f, 2.0f, 5.0f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 1.0f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D sceneObj;
gps::Model3D magicalObj;
gps::Model3D caruselObj2;
gps::Model3D caruselObj1;
gps::Model3D test;


gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;

gps::Shader depthMapShader;
gps::Shader magicalShader;
gps::Shader newLight;

GLuint shadowMapFBO;
GLuint depthMapTexture;

GLuint textureID;

bool showDepthMap;


gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
std::vector<const GLchar*> faces;

struct Projectile {
	glm::vec3 position;
	glm::vec3 direction;
	float speed;
};


struct Rain {
	float x, y, z;
	float velocity;
};

std::vector<Projectile> projectiles;

std::vector<Rain> rain;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}




bool toggle_light1 = false;

bool toggle_rain = true;
bool toggle_wireframe = false;
bool toggle_points = false;
bool activ_tr = false;
bool toggle_p = false;
float yaw = -90.0f;
float pitch = 0.0f;
bool toggle_presentation = false;
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	//NEW
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		Projectile newProj;
		newProj.position = myCamera.getCameraPosition();
		newProj.direction = glm::normalize(myCamera.getCameraTarget());
		newProj.speed = 100.0f;
		projectiles.push_back(newProj);
		printf("%lf ___ %lf ___ %lf", newProj.position.x, newProj.position.y, newProj.position.z);
	}



	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		toggle_rain = !toggle_rain;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		toggle_p = !toggle_p;
	}

	

	if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		if (!projectiles.empty()) {
			glm::vec3 newPos = projectiles.back().position;
			glm::vec3 newDir = projectiles.back().direction;
			projectiles.pop_back();
			gps::Camera newCam(newPos, newDir, glm::vec3(0.0f, 1.0f, 0.0f));
			myCamera = newCam;
			
		}
			
	}

	
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {


		toggle_light1 = !toggle_light1;
		
	
	}
	
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		toggle_wireframe = !toggle_wireframe;
		toggle_points = false;
		if (!toggle_wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		toggle_points = !toggle_points;
		toggle_wireframe = false;
		if (!toggle_points) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		toggle_presentation = !toggle_presentation;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}




void init_rain(int nr) {

	for (int i = 0; i < nr; i++) {
		Rain raindrops;

		raindrops.x = (rand() % 5000) ;
		raindrops.y = 20.0f + static_cast<float>(rand()) / RAND_MAX * 50.0f;
		raindrops.z = (rand() % 5000);
		raindrops.velocity = 10.0f + static_cast<float>(rand()) / RAND_MAX * 8.0f;
		rain.push_back(raindrops);
	}
}

void update_raindrops() {
	for (auto& drop : rain) {
		drop.y -= drop.velocity;
		if (drop.y < -1.0f) {
			drop.y = 20.0f + static_cast<float>(rand()) / RAND_MAX * 50.0f ;
			drop.x = rand() % 5000;
			drop.z = rand() % 5000;
		}
	}
}


void draw_raindrops() {
	
	if (toggle_rain) {
		for (auto& drop : rain) {
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(drop.x -  100 , drop.y, -drop.z + 100));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(lightShader.shaderProgram, "toggle_light"), toggle_light1);
			lightCube.Draw(lightShader);
			
		}

		
	}
	else {
		PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
		
	}
	
	
}

void updateProj(float time) {
	for (auto& projectile:projectiles) {
		projectile.position += projectile.direction * projectile.speed * time;
	}

	projectiles.erase(
		std::remove_if(projectiles.begin(), projectiles.end(),
			[](const Projectile& proj) {
				return glm::length(proj.position) > 1000.0f; 
			}),
		projectiles.end());
	
}
float lastY, lastX;
bool firstMouse = true;


void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if(!toggle_presentation){
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	float sens = 0.1f;

	xoffset *= sens;
	yoffset *= sens;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	myCamera.rotate(pitch, yaw);

	}

}

void processMovement()
{	
	if (!toggle_presentation) {
		

		if (pressedKeys[GLFW_KEY_J]) {
			lightAngle -= 1.0f;
			lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (pressedKeys[GLFW_KEY_L]) {
			lightAngle += 1.0f;
			lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (pressedKeys[GLFW_KEY_W]) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}

		if (pressedKeys[GLFW_KEY_S]) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		}

		if (pressedKeys[GLFW_KEY_A]) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		}

		if (pressedKeys[GLFW_KEY_D]) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		}
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}
void initSkyBox() {

	faces.push_back("skybox1/interstellar_ft.tga");
	faces.push_back("skybox1/interstellar_bk.tga");
	faces.push_back("skybox1/interstellar_up.tga");
	faces.push_back("skybox1/interstellar_dn.tga");
	faces.push_back("skybox1/interstellar_rt.tga");
	faces.push_back("skybox1/interstellar_lf.tga");
	mySkyBox.Load(faces);
}
void initObjects() {
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	sceneObj.LoadModel("objects/a_proj/project.obj");
	magicalObj.LoadModel("objects/time_orb/untitled.obj");
	caruselObj1.LoadModel("objects/a_proj/p1.obj");
	caruselObj2.LoadModel("objects/a_proj/p2.obj");
	test.LoadModel("objects/a_proj/test.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();

	depthMapShader.loadShader("shaders/newShader.vert", "shaders/newShader.frag");
	depthMapShader.useShaderProgram();

	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	newLight.loadShader("shaders/newLight.vert", "shaders/newLight.frag");

	magicalShader.loadShader("shaders/magical.vert", "shaders/magical.frag");
	magicalShader.useShaderProgram();
	
	

}

glm::vec3 l_pos[3] = {
	glm::vec3(-0.052500f,-0.764551f, 1.145364f),
	glm::vec3(5.884540f, 0.018360f,-13.666081f),
	glm::vec3(30.950098f,-0.015640f,12.460320f)
};


void initUniforms() {
	myCustomShader.useShaderProgram();
	activ_tr = false;
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "activ_tr"), activ_tr);
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));



	lightDir = glm::vec3(5.0f, 1.0f, 0.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	glm::vec3 l_color[3] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.5f, 0.0f, 1.0f)
	};


	GLint pointLightColorsLoc = glGetUniformLocation(myCustomShader.shaderProgram, "l_color");
	glUniform3fv(pointLightColorsLoc, 3, glm::value_ptr(l_color[0]));

	GLint pointLightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "l_pos");
	glUniform3fv(pointLightPosLoc, 3, glm::value_ptr(l_pos[0]));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	

	
}


void initUniforms2() {
	newLight.useShaderProgram();
	activ_tr = false;
	glUniform1i(glGetUniformLocation(newLight.shaderProgram, "activ_tr"), activ_tr);
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(newLight.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(newLight.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(newLight.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(newLight.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(5.0f, 1.0f, 0.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(newLight.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f,0.0f, 0.0f); 
	lightColorLoc = glGetUniformLocation(newLight.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	
	glm::vec3 l_color[3] = {
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.5f, 0.0f, 1.0f)
	};

	GLint pointLightColorsLoc = glGetUniformLocation(myCustomShader.shaderProgram, "l_color");
	glUniform3fv(pointLightColorsLoc, 3, glm::value_ptr(l_color[0]));

	GLint pointLightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "l_pos");
	glUniform3fv(pointLightPosLoc, 3, glm::value_ptr(l_pos[0]));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
	

	glm::vec3 transformedLightDir = glm::vec3(glm::inverseTranspose(lightRotation) * glm::vec4(lightDir, 0.0f));
	
	glm::mat4 lightView = glm::lookAt(
		transformedLightDir,
		glm::vec3(0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	const GLfloat near_plane = -10.0f, far_plane = 500.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void drawEnderPearl(gps::Shader shader) {

	
	for (const auto& projectile : projectiles) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), projectile.position);
		model = glm::scale(model, glm::vec3(0.1));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		activ_tr = true; 
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "activ_tr"), activ_tr);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		magicalObj.Draw(shader);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);


	}
}

glm::vec3 carusel_pos2 = {
	14.264766f,-0.55,3.485869f
};
glm::vec3 carusel_pos1 = {
	14.264766f,-0.6,3.485869f
};

void draw_carusel(gps::Shader shader) {
	static float angle = 0.0f;
	angle += 0.01f;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, carusel_pos2); 
	model = glm::rotate(model, angle, glm::vec3(0, 1, 0)); 
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	caruselObj2.Draw(shader);

	model = glm::mat4(1.0);
	model = glm::translate(model, carusel_pos1);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	caruselObj1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(21.016632, 0.265994, -39.149464));
	//model = glm::translate(model, glm::vec3(0, 3, 0));
	model = glm::rotate(model, angle + 0.01f, glm::vec3(1, 1, 1));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	test.Draw(shader);

}

glm::vec3 start1 = glm::vec3(0.0f, 4.0f, 0.0f);
glm::vec3 end1 = glm::vec3(30.0, 4.0, 0.0f);
glm::vec3 last_pos = start1;
glm::vec3 currPos; 
glm::vec3 inspection_target;

glm::vec3 start[5] = {
	glm::vec3(0.0f, 4.0f, 0.0f),
	glm::vec3(30.0f, 4.0f, 0.0f),
	glm::vec3(30.0f, 4.0f, -50.0),
	glm::vec3(30.0f ,  4.0f ,  -70.0f),
	glm::vec3(10.678177 , 2.671595 , -236.547440),
};
glm::vec3 end[5] = {
	glm::vec3(30.0f, 4.0f, 0.0f),
	glm::vec3(30.0f, 4.0f, -50.0f),
	glm::vec3(30.0f, 4.0f, -70.0f),
	glm::vec3(10.678177 , 2.671595 , -236.547440),
	glm::vec3(-0.332213  , 3.241317 , -287.579437),

};

float speed = 0.01f;
float final_speed = 0.0f;
float final_angle = 360.0f;
float angle;
bool inspect_1 = false;
int point = 0;
void presentation_mode() {
	if (toggle_presentation) {
		glm::vec3 midpoint;
			midpoint = (start[0] + end[0]) / 2.0f;
			if (!inspect_1) {

				final_speed += speed;
			
				currPos = start[0] + final_speed * (end[0] - start[0]);
				if (glm::distance(currPos, midpoint) < 0.1f && point % 2 != 1) { 
					inspect_1 = true; 
					angle = 0.0f; 
				}
				if (point != 5) {
					if (final_speed >= 1.0f) {
						//toggle_presentation = !toggle_presentation;
						point++;
						end[0] = end[point];
						start[0] = start[point];
						final_speed = 0.0f;
						currPos = end[0];
					}
				}
				else {
					toggle_presentation = !toggle_presentation;
					end[0] = end[point - 1];
					start[0] = end[point - 1];
					final_speed = 0.0f;
					currPos = end[0];
				}

				inspection_target = currPos;
			}
			else {
				angle += 0.01;

				float radius = glm::distance(midpoint, start[0]);
				currPos = midpoint + glm::vec3(
					radius * cos(angle),
					0.0f,
					radius * sin(angle)
				);

				inspection_target = midpoint - glm::vec3(0.0, 0.5, 0.0);
				if (angle >= glm::radians(final_angle)) {
					inspect_1 = false;
					angle = 0.0f; 
				}
			}
		
			myCamera = gps::Camera(last_pos, inspection_target, glm::vec3(0.0f, 1.0f, 0.0f));
			last_pos = currPos;
	
		}
}


void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	/*nanosuit.Draw(shader);*/
	//ground.Draw(shader);
	activ_tr = false;
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "activ_tr"), activ_tr);
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "toggle_p"), toggle_p);
	sceneObj.Draw(shader);
	drawEnderPearl(shader);
	draw_carusel(shader);
	draw_raindrops();
	presentation_mode();
	
}


void drawLight(gps::Shader shader) {
	
		shader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		lightCube.Draw(shader);
		
		
			
}

void renderScene(gps::Shader shader, gps:: Shader l_shader) {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map

	
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		//lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(shader, false);
		

		updateProj(0.01);

		update_raindrops();
		
		mySkyBox.Draw(skyboxShader, view, projection);

		
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		drawLight(l_shader);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		
	}
}


void render_multiple_shaders() {

	
	
	
	if(toggle_light1) {
		renderScene(newLight, lightShader);
		
	}
	else {
		renderScene(myCustomShader, lightShader);
	}


}
void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}
#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	__declspec(dllexport) int  AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif
#endif
int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initUniforms2();
	initFBO();
	initSkyBox();
	init_rain(10000);
	PlaySound(L"sound/rain.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		render_multiple_shaders();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
		
	}

	cleanup();
	
	return 0;
}
