#include "stdfx.h"
#include "util.h"
#include "model.h"
#include <FreeImage.h>
using namespace std;

extern int a;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::vec3 eye = glm::vec3(0, 0, 3);
glm::vec3 up = glm::vec3(0, 1, 0);
glm::vec3 front = glm::vec3(0, 0, -1);
glm::vec3 vec_right = glm::vec3(1, 0, 0);
int lightMode = 0;

float roughness = 1.0f;
int debug = 0;

#define PI 3.141692653
#define KEY_W 0 
#define KEY_S 1
#define KEY_A 2
#define KEY_D 3
#define KEY_UP 4
#define KEY_DOWN 5
#define KEY_DB 6
bool keys[7];
bool upflag, downflag, debugflag;

void do_movement();

// The MAIN function, from here we start the application and run the game loop
int main()
{
	/********************************************************************/
	//Init
	/********************************************************************/
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	if(!glfwInit())
		return -1;
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	/********************************************************************/
	//shader
	/********************************************************************/
	string currentProject = "gs";

	Shader shader((currentProject + "\\shader\\vs.vs").c_str(), (currentProject + "\\shader\\fs.fs").c_str());
	Shader skyboxShader((currentProject + "\\shader\\skybox.vs").c_str(), (currentProject + "\\shader\\skybox.fs").c_str());
	Shader lightingShader((currentProject + "\\shader\\lighting.vs").c_str(), (currentProject + "\\shader\\lighting.fs").c_str());
	Shader triangleShader((currentProject + "\\shader\\triangleVs.vs").c_str(), (currentProject + "\\shader\\triangleFs.fs").c_str());
	Shader pbrDirectShader((currentProject + "\\shader\\pbr_direct.vs").c_str(), (currentProject + "\\shader\\pbr_direct.fs").c_str());

	lightingShader.Use();

	/********************************************************************/
	//gBuffer
	/********************************************************************/
	//GLuint gBuffer;
	//glGenFramebuffers(1, &gBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	//GLuint gPosition, gNormal, gAlbedoSpec;

	////position
	//glGenTextures(1, &gPosition);
	//glBindTexture(GL_TEXTURE_2D, gPosition);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	////normal
	//glGenTextures(1, &gNormal);
	//glBindTexture(GL_TEXTURE_2D, gNormal);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	////normal
	//glGenTextures(1, &gAlbedoSpec);
	//glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	//GLuint attachments[3] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2 };
	//glDrawBuffers(3, attachments);

	////depth buffer
	//GLuint depthBuffer;
	//glGenRenderbuffers(1, &depthBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	////complete check 
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "Framebuffer not complete!" << std::endl;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/********************************************************************/
	//pbr
	/********************************************************************/

	/********************************************************************/
	//model
	/********************************************************************/
	//Model model("model//nanosuit.obj");
	Model model("model//sphere//sphere0.obj");

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// sky box
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};
	

	//quad
	glm::vec3 quadVert[] =
	{
		glm::vec3(-1, -1, 0),
		glm::vec3(-1, 1, 0),
		glm::vec3(1, 1, 0),
		glm::vec3(-1, -1, 0),
		glm::vec3(1, 1, 0),
		glm::vec3(1, -1, 0)
	}; 

	glm::vec2 quadUVs[] =
	{
		glm::vec2(0, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(1, 0)
	};

	GLuint quadVertexBuffer, quadUVBuffer, quadVertexBufferArray;
	glGenVertexArrays(1, &quadVertexBufferArray);
	glBindVertexArray(quadVertexBufferArray);

	glGenBuffers(1, &quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), &quadVert[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(0);
	
	glGenBuffers(1, &quadUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), &quadUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	
	//sky box
	GLuint skyboxVertexBufferArray, skyboxVertexBuffer; 
	glGenVertexArrays(1, &skyboxVertexBufferArray);
	glBindVertexArray(skyboxVertexBufferArray);

	glGenBuffers(1, &skyboxVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	/********************************************************************/
	//camera
	/********************************************************************/

	glm::mat4 proj = glm::perspective(45.0f, 1.0f, 0.01f, 10.0f);

	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	
	/********************************************************************/
	//sky box texture
	/********************************************************************/
	GLuint skyboxTex;
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	vector<string> skyboxTexNames;
	/*skyboxTexNames.push_back("image/right.jpg");
	skyboxTexNames.push_back("image/left.jpg");
	skyboxTexNames.push_back("image/top.jpg");
	skyboxTexNames.push_back("image/bottom.jpg");
	skyboxTexNames.push_back("image/back.jpg");
	skyboxTexNames.push_back("image/front.jpg");*/
	for (int i = 0; i < 6; ++i)
		skyboxTexNames.push_back("image/pisa_c0" + to_string(i) + ".hdr");
		//skyboxTexNames.push_back("image/"+to_string(i)+".jpg");
	unsigned char* image;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexStorage2D(GL_TEXTURE_CUBE_MAP, 10, GL_RGB32F, 512, 512);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 10, GL_RGBA8, 512, 512);
	for (int i = 0; i < 6; ++i)
	{
		FIBITMAP* bitmap = FreeImage_Load(
			FreeImage_GetFileType(skyboxTexNames[i].c_str(), 0),
			skyboxTexNames[i].c_str());
		int bitsPerPixel = FreeImage_GetBPP(bitmap);
		printf("%d\n", bitsPerPixel);
		GLubyte* bytes = FreeImage_GetBits(bitmap);
		//FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(bitmap);
		int nHeight = FreeImage_GetHeight(bitmap);
		printf("%d %d\n", nWidth, nHeight);

		/*image = SOIL_load_image(skyboxTexNames[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);*/

		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, nWidth, nHeight, GL_RGB, GL_FLOAT, bytes);
	}
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/*glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	

	/********************************************************************/
	//Main loop
	/********************************************************************/
	while (!glfwWindowShouldClose(window))
	{
		//printError();
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		//geometry pass
		//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		
		//shader.Use();
		pbrDirectShader.Use();
		// Render
		// Clear the color buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 center = eye + front;
		glm::mat4 view = glm::lookAt(eye, center, up);

		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::scale(modelMat, glm::vec3(0.15f));
		glm::mat4 MVP = proj * view * modelMat;
		glUniformMatrix4fv(glGetUniformLocation(pbrDirectShader.Program, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pbrDirectShader.Program, "Model"), 1, GL_FALSE, &modelMat[0][0]);
		glUniform3fv(glGetUniformLocation(pbrDirectShader.Program, "cameraPos"), 1, &eye[0]);
		glUniform1f(glGetUniformLocation(pbrDirectShader.Program, "roughness"), roughness);
		glUniform1i(glGetUniformLocation(pbrDirectShader.Program, "debugflag"), debug);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		glUniform1i(glGetUniformLocation(pbrDirectShader.Program, "cubemap"), 0);

		model.Draw(pbrDirectShader);

		//draw skybox
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(view));
		glm::mat4 VP = proj * view;

		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "VP"), 1, GL_FALSE, &VP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "cubemap"), 0);

		glBindVertexArray(skyboxVertexBufferArray);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	printf("roughness %.2f\n", roughness);
	printf("debug %d\n", debugflag);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		if (!keys[KEY_DB])
		{
			debug = 1 - debug;
		}

		keys[KEY_DB] = true;
	}
	else if (key == GLFW_KEY_T && action == GLFW_RELEASE)
		keys[KEY_DB] = false;

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		if (!keys[KEY_UP])
		{
			roughness += 0.1;
			if (roughness > 1.0)
				roughness = 1.0f;
		}
		
		keys[KEY_UP] = true;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		keys[KEY_UP] = false;

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		if (!keys[KEY_DOWN])
		{
			roughness -= 0.1;
			if (roughness < 0.0)
				roughness = 0.0;
		}

		keys[KEY_DOWN] = true;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		keys[KEY_DOWN] = false;

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		keys[KEY_W] = true;
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		keys[KEY_W] = false;

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		keys[KEY_S] = true;
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		keys[KEY_S] = false;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		keys[KEY_D] = true;
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		keys[KEY_D] = false;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		keys[KEY_A] = true;
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		keys[KEY_A] = false;

	if (key == GLFW_KEY_L && action == GLFW_RELEASE)
		lightMode = (lightMode + 1) % 4;
}

float yaw = 0, pitch = 0;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float x_offset = xpos - 400;
	float y_offset = ypos - 400;

	float sensity = 0.05f;
	yaw += -sensity * x_offset;
	pitch += -sensity * y_offset;
	if (pitch >= 89)
		pitch = 89;
	if (pitch <= -89)
		pitch = -89;

	glm::mat4 yawRot = glm::mat4(1.0f);
	yawRot = glm::rotate(yawRot, yaw, up);
	vec_right = glm::vec3(yawRot * glm::vec4(1, 0, 0, 0));
	glm::mat4 pitchRot = glm::mat4(1.0f);
	pitchRot = glm::rotate(pitchRot, pitch, vec_right);

	front = glm::vec3(pitchRot * yawRot * glm::vec4(0, 0, -1, 0));

	glfwSetCursorPos(window, 400, 400);
}

GLfloat cur = 0.0f, last = cur;
void do_movement()
{
	glm::vec3 right = glm::cross(front, up);
	float speed;// = 0.1f;

	cur = glfwGetTime();
	speed = 0.4f * (cur - last);
	last = cur;

	if (keys[KEY_W])
		eye += front * speed;

	if (keys[KEY_S])
		eye -= front * speed;

	if (keys[KEY_D])
		eye += right * speed;

	if (keys[KEY_A])
		eye -= right * speed;

}
