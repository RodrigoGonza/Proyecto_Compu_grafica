#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();
void animacionbarco();
void animacionBill();
void animaciontb();
void animacionpelota();

// Window dimensions
const GLuint WIDTH = 1080, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 20.0f, 0.0f)); //cerca del bote
//Camera  camera(glm::vec3(96.0f, 4.5f, 602.5f)); //cerca de la sala 
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(28.0f, -1.9f, 30.5f);
glm::vec3 PosB(0.0f, 2.0f, -10.0f);

//glm::vec3 PosInibarco(10.0f, -6.0f, -620.0f);
bool active;

int luz = 0;  //variable para la luz

GLint pue = 0;
GLint cab = 0;
GLint sky1 = 1;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(105.0f, 5.5f, 602.0f),
	glm::vec3(50.0f, 5.5f, 602.0f),
	glm::vec3(70.0f, 5.5f, 565.0f),
	glm::vec3(-50.0f,  5.5f,  369.0f)
};

glm::vec3 LightP1, LightP2, LightP3, LightP4;

//Animación 
float movKitX = 0.0;
float movKitZ = 0.0;
float rotKit = 0.0;

bool circuito = false;
bool recorrido1 = true;
bool recorrido2 = false;
bool recorrido3 = false;
bool recorrido4 = false;

//animacion barco
float movKitXbar = 0.0;
float movKitZbar = 0.0;
float rotKitbar = 0.0;

bool circuitobar = false;
bool recorrido1bar = true;
bool recorrido2bar = false;
bool recorrido3bar = false;
bool recorrido4bar = false;

//Animacion Bill
float movKitXBill = 0.0;
float movKitZBill = 0.0;

bool circuitoBill = false;
bool recorrido1Bill = true;
bool recorrido2Bill = false;
bool recorrido3Bill = false;
bool recorrido4Bill = false;

bool rotventi = false;

//Animacion tibu
float movKitXtb = 0.0;
float movKitYtb = 0.0;
float movKitZtb = 0.0;
float rotKittb = 0.0;

bool circuitotb = false;
bool recorrido1tb = true;
bool recorrido2tb = false;
bool recorrido3tb = false;
bool recorrido4tb = false;


//animacion pelota

float movKitXpel = 0.0;
float movKitZpel = 0.0;
float movKitYpel = 0.0;
float rotKitYpel = 0.0;

bool circuitopel = false;
bool recorrido1pel = true;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, posBillx=PosB.x, posBilly = PosB.y, posBillz = PosB.z,rotizq=0;
#define MAX_FRAMES 9
int i_max_steps = 50;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotizq;   //Variable para rotacion izquierdo
	float rotInc;   //Variable para rotacion gradual
	float posBillx;
	float posBilly;
	float posBillz;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 7;			//introducir datos
bool play = false;
int playIndex = 0;

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;
	posBillx = KeyFrame[0].posBillx;
	posBilly = KeyFrame[0].posBilly;
	posBillz = KeyFrame[0].posBillz;
	rotizq = KeyFrame[0].rotizq;
}
//Funcion de interpolacion de Keyframes
void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posBillx - KeyFrame[playIndex].posBillx) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posBilly - KeyFrame[playIndex].posBilly) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posBillz - KeyFrame[playIndex].posBillz) / i_max_steps;

	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotizq - KeyFrame[playIndex].rotizq) / i_max_steps;
}

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Gravity Falls", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	

	Model pueblo((char*)"Models/GFT/gravity.obj");
	Model pueblo1((char*)"Models/1/2G.obj");
	Model casa((char*)"Models/misterio/cabana.obj");
	Model casa1((char*)"Models/misterio1/cabana.obj");
	Model mesa((char*)"Models/mesasala/mesasala.obj");
	Model cocinamueble((char*)"Models/cocina/cocina.obj");
	Model estufa((char*)"Models/estufa/estufa.obj");
	Model micro((char*)"Models/micro/micro.obj");
	Model cama((char*)"Models/cama/cuarto.obj");
	Model bano((char*)"Models/baño/bano.obj");
	Model cama2((char*)"Models/cama/cuarto2.obj");
	Model sala((char*)"Models/sala/sala.obj");
	Model iglesia((char*)"Models/iglesia/iglesia.obj");
	Model iglesia1((char*)"Models/iglesia1/iglesia.obj");
	Model bill((char*)"Models/personaje/bill.obj");
	Model ruinas((char*)"Models/ruinas/carros.obj");
	Model barco((char*)"Models/boat/boat.obj");
	Model venti((char*)"Models/ventilador/ventilador.obj");
	Model tibu((char*)"Models/tibu/tiburon.obj");
	Model bano2((char*)"Models/baño2/bano2.obj");
	Model pelota((char*)"Models/pelota/pelota.obj");

	//Inicializacion de los Keyframe para almacenar datos
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posBillx = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotInc = 0;
		KeyFrame[i].rotizq = 0;
	}
	//Valores que toman los Keyframe para animacion
	KeyFrame[0].rotizq = 0;
	KeyFrame[1].rotizq = 90;
	KeyFrame[2].rotizq = -90;
	KeyFrame[3].rotizq = 90;
	KeyFrame[4].rotizq = -90;
	// Build and compile our shader program


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();
		animacionbarco();
		animacionBill();
		animaciontb();
		animacionpelota();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Load Model


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		
		switch (luz)
		{
		case 0:
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.93f, 0.93f, 0.93f);
			break;
		case 1:
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.40f, 0.40f, 0.40f);
			break;
		default:
			break;

		}

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), LightP2.x, LightP2.y, LightP2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), LightP2.x, LightP2.y, LightP2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), LightP3.x, LightP3.y, LightP3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), LightP3.x, LightP3.y, LightP3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), LightP4.x, LightP4.y, LightP4.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), LightP4.x, LightP4.y, LightP4.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glm::mat4 model(1);

		//Carga de modelo 

		//pueblo

		glm::mat4 model1(1);
		model1 = glm::translate(model1, glm::vec3(0.0f, -1.75f, 0.0f));
		/*model1 = glm::scale(model1, glm::vec3(0.02f, 0.02f, 0.02f));	*/
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));

		switch (pue)
		{
		case 0:
			pueblo.Draw(lightingShader);


			break;
		case 1:

			pueblo1.Draw(lightingShader);

			break;
		default:
			break;



		}

		//cabaña del misterio

		glm::mat4 model2(1);
		model2 = glm::translate(model2, glm::vec3(80.0f, 1.7f, 589.0f));
		model2 = glm::scale(model2, glm::vec3(0.5f, 0.5f, 0.5f));
		model2 = glm::rotate(model2, -11.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

		switch (cab)
		{
		case 0:
			casa.Draw(lightingShader);


			break;
		case 1:

			casa1.Draw(lightingShader);

			break;
		default:
			break;
		}

		//iglesia

		glm::mat4 model11(1);
		model11 = glm::translate(model11, glm::vec3(55.0f, 2.7f, 74.0f));
		model11 = glm::scale(model11, glm::vec3(0.3f, 0.3f, 0.3f));
		model11 = glm::rotate(model11, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));

		switch (cab)
		{
		case 0:
			iglesia.Draw(lightingShader);


			break;
		case 1:

			iglesia1.Draw(lightingShader);

			break;
		default:
			break;
		}
		
		//estatua

		glm::mat4 model12(1);
		model12= glm::translate(model12, glm::vec3(0.0f, 2.0f, -10.0f) + glm::vec3(movKitXBill, 0, movKitZBill));
		model12 = glm::scale(model12, glm::vec3(0.6f, 0.6f, 0.6f));
		model12 = glm::rotate(model12, glm::radians(-rotizq), glm::vec3(0.0f, 1.0f, 0.0f));
		model12 = glm::translate(model12, glm::vec3(posBillx,posBilly,posBillz));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model12));
		
		switch (cab)
		{
		case 0:
			


			break;
		case 1:

			bill.Draw(lightingShader);

			break;
		default:
			break;
		}

		//mesa sala

		glm::mat4 model3(1);
		model3 = glm::translate(model3, glm::vec3(96.0f, 5.9f, 602.5f));
		model3 = glm::scale(model3, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
		mesa.Draw(lightingShader);

		//mueble cocina

		glm::mat4 model4(1);
		model4 = glm::translate(model4, glm::vec3(95.5f, 5.9f, 595.5f));
		model4 = glm::scale(model4, glm::vec3(2.0f, 2.0f, 2.0f));
		model4 = glm::rotate(model4, -22.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));
		cocinamueble.Draw(lightingShader);

		//estufa

		glm::mat4 model5(1);
		model5 = glm::translate(model5, glm::vec3(86.8f, 5.9f, 591.5f));
		model5 = glm::scale(model5, glm::vec3(0.04f, 0.035f, 0.037f));
		model5 = glm::rotate(model5, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		/*model5 = glm::rotate(model5, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));
		estufa.Draw(lightingShader);

		//microcomputadoras

		glm::mat4 model6(1);
		model6 = glm::translate(model6, glm::vec3(91.5f, 8.8f, 591.5f));
		model6 = glm::scale(model6, glm::vec3(0.5f, 0.5f, 0.5f));
		/*model6 = glm::rotate(model6, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));*/
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));
		micro.Draw(lightingShader);

		//Cuarto
		glm::mat4 model7(1);
		model7 = glm::translate(model7, glm::vec3(61.5f, 6.0f, 607.5f));
		model7 = glm::scale(model7, glm::vec3(1.0f));
		model7 = glm::rotate(model7, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));
		cama.Draw(lightingShader);
		//Baño
		glm::mat4 model8(1);
		model8 = glm::translate(model8, glm::vec3(86.5f, 6.0f, 608.0f));
		model8 = glm::scale(model8, glm::vec3(5.0f,5.0f,3.5f));
		model8 = glm::rotate(model8, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));
		bano.Draw(lightingShader);
		//cuarto arriba
		glm::mat4 model9(1);
		model9 = glm::translate(model9, glm::vec3(94.5f, 19.0f, 600.5f));
		model9 = glm::scale(model9, glm::vec3(1.0f));
		model9 = glm::rotate(model9, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));
		cama2.Draw(lightingShader);
		//sala
		glm::mat4 model10(1);
		model10 = glm::translate(model10, glm::vec3(66.5f, 6.0f, 582.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));
		sala.Draw(lightingShader);
		//ruinas
		glm::mat4 model13(1);
		model13 = glm::translate(model13, glm::vec3(540.5f, 2.0f, -440.0f));
		model13 = glm::scale(model13, glm::vec3(5.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));
		ruinas.Draw(lightingShader);

		//bote
		glm::mat4 model14(1);
		model14 = glm::translate(model14, glm::vec3(10.0f, -4.0f, -620.0f) + glm::vec3(movKitXbar, 0, movKitZbar));
		model14 = glm::rotate(model14, glm::radians(rotKitbar), glm::vec3(0.0f, 1.0f, 0.0));
		model14 = glm::scale(model14, glm::vec3(0.30f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));
		barco.Draw(lightingShader);

		//ventilador
		glm::mat4 model15(1);
		model15 = glm::translate(model15, glm::vec3(95.5f, 15.5f, 602.5f));
		model15 = glm::scale(model15, glm::vec3(0.032f, 0.032f, 0.032f));
		if (rotventi == true) {

			model15 = glm::rotate(model15, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));
		venti.Draw(lightingShader);

		//tiburon
		glm::mat4 model16(1);
		model16 = glm::translate(model16, glm::vec3(40.0f, -5.4f, -720.0f) + glm::vec3(movKitXtb, movKitYtb, movKitZtb));
		model16 = glm::rotate(model16, glm::radians(rotKittb), glm::vec3(0.0f, 1.0f, 0.0));
		model16 = glm::scale(model16, glm::vec3(0.40f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));
		tibu.Draw(lightingShader);
		
		//Baño2
		glm::mat4 model17(1);
		model17 = glm::translate(model17, glm::vec3(69.5f, 19.0f, 580.0f));
		model17 = glm::rotate(model17, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model17 = glm::scale(model17, glm::vec3(0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model17));
		bano2.Draw(lightingShader);

		//cargando modelo pelota
		glm::mat4 model18(1);
		model18 = glm::translate(model18, glm::vec3(60.0f, -5.4f, -720.0f) + glm::vec3(movKitXpel, movKitYpel, movKitZpel));
		model18 = glm::scale(model18, glm::vec3(0.020f, 0.020f, 0.020f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model18));
		pelota.Draw(lightingShader);
		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.02f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (keys[GLFW_KEY_I])
	{
		circuito = true;
	}

	if (keys[GLFW_KEY_O])
	{
		circuito = false;
	}

	if (keys[GLFW_KEY_N])
	{
		circuitobar = false;
	}

	if (keys[GLFW_KEY_M])
	{
		circuitobar = true;
	}
	//Valores para animacion por KeyFrames
	if (keys[GLFW_KEY_K])
	{
		rotizq = 0;
		circuitoBill = false;
	}
	if (keys[GLFW_KEY_L])
	{
		rotizq = 0;
		circuitoBill = true;
	}

	if (keys[GLFW_KEY_T])
	{
		rotventi = false;
	}

	if (keys[GLFW_KEY_G])
	{
		rotventi = true;
	}

	if (keys[GLFW_KEY_R])
	{
		circuitotb = false;
	}

	if (keys[GLFW_KEY_F])
	{
		circuitotb = true;
	}
	if (keys[GLFW_KEY_Z])
	{
		circuitopel = false;
	}

	if (keys[GLFW_KEY_X])
	{
		circuitopel = true;
	}
}

void animacion()
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;
			posBillx += KeyFrame[playIndex].incX;
			posBilly += KeyFrame[playIndex].incY;
			posBillz += KeyFrame[playIndex].incZ;
			rotizq += KeyFrame[playIndex].rotInc;
			i_curr_steps++;
		}

	}
	//Movimiento lancha
	if (circuito)
	{
		if (recorrido1)
		{
			rotKit = 0;
			movKitZ -= 0.1f;
			if (movKitZ < -100.0)
			{
				recorrido1 = false;
				recorrido2 = true;
			}
		}

		if (recorrido2)
		{
			rotKit = 90;
			movKitX -= 0.1f;
			
			if (movKitX < -57.0)
			{
				recorrido2 = false;
				recorrido3 = true;

			}
		}

		if (recorrido3)
		{
			rotKit = 180;
			movKitZ += 0.1f;
			
			if (movKitZ > 5.0)
			{
				recorrido3 = false;
				recorrido4 = true;

			}
		}

		if (recorrido4)
		{
			rotKit = 270;
			movKitX += 0.1f;

			if (movKitX > 5.0)
			{
				recorrido4 = false;
				recorrido1 = true;

			}
		}
	}
}

void animacionbarco()
{
	//Movimiento barco
	if (circuitobar)
	{
		if (recorrido1bar)
		{
			rotKitbar = 0;
			movKitZbar -= 0.3f;
			if (movKitZbar < -250.0)
			{
				recorrido1bar = false;
				recorrido2bar = true;
			}
		}

		if (recorrido2bar)
		{
			rotKitbar = 90;
			movKitXbar += 0.3f;

			if (movKitXbar > 200.0)
			{
				recorrido2bar = false;
				recorrido3bar = true;

			}
		}
		
		if (recorrido3bar)
		{
			rotKitbar = 180;
			movKitZbar += 0.3f;

			if (movKitZbar > 0.0)
			{
				recorrido3bar = false;
				recorrido4bar = true;

			}
		}
		
		if (recorrido4bar)
		{
			rotKitbar = 270;
			movKitXbar -= 0.3f;

			if (movKitXbar < 0.0)
			{
				recorrido4bar = false;
				recorrido1bar = true;

			}
		}
	}
}

void animacionBill()
{
	if (circuitoBill)
	{
		if (recorrido1Bill)
		{
			rotizq = 0;
			movKitZBill -= 0.3f;
			if (movKitZBill < -310.0)
			{
				recorrido1Bill = false;
				recorrido2Bill = true;
			}
		}

		if (recorrido2Bill)
		{
			rotizq = 90;
			movKitXBill += 0.3f;

			if (movKitXBill > 330.0)
			{
				recorrido2Bill = false;
				recorrido3Bill = true;

			}
		}

		if (recorrido3Bill)
		{
			rotizq = 180;
			movKitZBill += 0.3f;

			if (movKitZBill > 330.0)
			{
				recorrido3Bill = false;
				recorrido4Bill = true;

			}
		}

		if (recorrido4Bill)
		{
			rotizq = 270;
			movKitXBill -= 0.3f;

			if (movKitXBill < 0.0)
			{
				recorrido4Bill = false;
				recorrido1Bill = true;
			}
		}
	}
}
void animaciontb()
{
	//Movimiento tibu
	if (circuitotb)
	{
		if (recorrido1tb)
		{
			rotKittb = 0;
			movKitXtb -= 0.3f;
			movKitYtb -= 0.01f;
			if (movKitXtb < -35.0 && movKitYtb < -0.2)
			{
				recorrido1tb = false;
				recorrido2tb = true;
			}
		}

		if (recorrido2tb)
		{
			rotKittb = 90;
			movKitZtb += 0.3f;
			movKitYtb += 0.1f;

			if (movKitZtb > 2.0 && movKitYtb > 0.0)
			{
				recorrido2tb = false;
				recorrido3tb = true;

			}
		}
		
		if (recorrido3tb)
		{
			rotKittb = 180;
			movKitXtb += 0.3f;
			movKitYtb -= 0.01;

			if (movKitXtb > 0.0 && movKitYtb < -2.0)
			{
				recorrido3tb = false;
				recorrido4tb = true;

			}
		}
		
		if (recorrido4tb)
		{
			rotKittb = 270;
			movKitZtb -= 0.3f;
			movKitYtb += 0.1f;

			if (movKitZtb < 0.0 && movKitYtb > 0.0)
			{
				recorrido4tb = false;
				recorrido1tb = true;

			}
		}
	}
}

void animacionpelota()
{

	//Movimiento pelota
	if (circuitopel)
	{
		if (recorrido1pel) {
			movKitXpel = 5 * cos((float)glfwGetTime() * 0.3);
			movKitYpel = 1 * sin((float)glfwGetTime() * 3);
			movKitZpel = 5 * cos((float)glfwGetTime() * 3);




		}

	}
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{

	if (keys[GLFW_KEY_L] || keys[GLFW_KEY_K])
	{
		if (play == false && (FrameIndex > 1))
		{

			posX = 0;
			posY = 0;
			posZ = 0;
			posBillx = 0;
			posBilly = 0;
			posBillz = 0;
			rotizq = 0;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}


	if (keys[GLFW_KEY_J])
	{
		pue = pue + 1;
		if (pue == 2) {
			pue = 0,
				LightP1 = glm::vec3(1.0f),
				LightP2 = glm::vec3(1.0f),
				LightP3 = glm::vec3(1.0f),
				LightP4 = glm::vec3(1.0f);
		}
	}

	if (keys[GLFW_KEY_H])
	{
		pue = pue - 1;
		if (pue == -1) {
			pue = 1,
				LightP1 == glm::vec3(1.0f),
				LightP2 = glm::vec3(1.0f),
				LightP3 = glm::vec3(1.0f),
				LightP4 = glm::vec3(1.0f);
		}
	}

	if (keys[GLFW_KEY_J])
	{
		active = !active;
		luz = luz + 1;
		if (luz == 2) {
			luz = 0;
			if (active)
				LightP1 = glm::vec3(1.0f),
				LightP2 = glm::vec3(1.0f),
				LightP3 = glm::vec3(1.0f),
				LightP4 = glm::vec3(1.0f);
			else
				LightP1 = glm::vec3(0.0f),
				LightP2 = glm::vec3(0.0f),
				LightP3 = glm::vec3(0.0f),
				LightP4 = glm::vec3(0.0f);
		}
	}

	if (keys[GLFW_KEY_H])
	{
		luz = luz - 1;
		if (luz == -1) {
			luz = 1;
			LightP1 = glm::vec3(1.0f);
			LightP2 = glm::vec3(1.0f);
			LightP3 = glm::vec3(1.0f);
			LightP4 = glm::vec3(1.0f);
		}
	}

	if (keys[GLFW_KEY_J])
	{
		cab = cab + 1;
		if (cab == 2) {
			cab = 0;
		}
	}

	if (keys[GLFW_KEY_H])
	{
		cab = cab - 1;
		if (cab == -1) {
			cab = 1;
		}
	}

	if (keys[GLFW_KEY_J])
	{
		sky1 = sky1 + 1;
		if (sky1 == 2) {
			sky1 = 0;
		}
	}

	if (keys[GLFW_KEY_H])
	{
		sky1 = sky1 - 1;
		if (sky1 == -1) {
			sky1 = 1;
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}