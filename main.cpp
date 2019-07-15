#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/buffers/AtlasBuffer.h"
#include "classes/level/Cave.h"
#include "classes/level/Dungeon.h"
#include "classes/image/TextureLoader.h"

double DeltaTime = 0;
bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
GLFWwindow* window;

int TilesCount[2] = {60, 60};//{60, 60}
glm::vec2 TileSize(10, 10);//(20, 20)
glm::vec2 Velocity = glm::vec2(180, 180); //pixels per render
glm::vec2 ModelOffset = glm::vec2(0, 0);

unsigned int txAtlas_cnt;
stTexture* txAtlas;
MAtlasBuffer AtlasBuffer;
MTextureLoader TextureLoader;

MShader Shader;
MScene Scene;

//MCave Cave = MCave(TilesCount[0], TilesCount[1], 51, 2, 4, 30, 30);
MDungeon Cave = MDungeon(TilesCount[0], TilesCount[1], 6, 20, 3);

bool GenerateLevel() {
	AtlasBuffer.Clear();
	
	unsigned int AtlasPos[2];
	if(!Cave.Generate()) return false;
	
	for(int i=0; i<TilesCount[0]; i++) {
		for(int j=0; j<TilesCount[1]; j++) {
			if(!Cave.GetValue(i, j)) continue;
			switch(Cave.GetValue(i, j)) {
				case TT_FLOOR:
					AtlasPos[0] = 0;
					AtlasPos[1] = 1;
					break;
				case TT_WALL_FULL:
					AtlasPos[0] = 1;
					AtlasPos[1] = 0;
					break;
				case TT_WALL_PART:
					AtlasPos[0] = 0;
					AtlasPos[1] = 0;
					break;
				case TT_LSTART:
					AtlasPos[0] = 3;
					AtlasPos[1] = 1;
					break;
				case TT_LEND:
					AtlasPos[0] = 3;
					AtlasPos[1] = 2;
					break;
			}
			if(!AtlasBuffer.AddFixedData(glm::vec2(i * TileSize.x, j * TileSize.y), 
				glm::vec2((i + 1) * TileSize.x, (j + 1) * TileSize.y), AtlasPos[0], AtlasPos[1])) return false;
		}
	}
	
	if(!AtlasBuffer.Dispose()) return false;
	
	return true;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void mousepos_callback(GLFWwindow* window, double x, double y) {
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	Scene.Size(width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS) {
		keys[key] = 1;
	}
	else if(action == GLFW_RELEASE) {
		keys[key] = 0;
	}
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	glewExperimental = GL_TRUE;
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.AddUnifrom("model", "model")) return false;
	if(!Shader.AddUnifrom("view", "view")) return false;
	if(!Shader.AddUnifrom("proj", "proj")) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(WindowWidth, WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
    
    //other initializations
    //init buffers
	txAtlas = TextureLoader.LoadTexture("textures/tex08.png", 1, 1, 0, txAtlas_cnt, GL_NEAREST, GL_REPEAT);
    if(!txAtlas) return false;
	if(!AtlasBuffer.Initialize(txAtlas, 512, 512, 4, 4)) return false;
    //generate level
	if(!GenerateLevel()) {
		LogFile<<"Level: generation failed"<<endl;
		return false;
	}
	
	//use shader program
	glUseProgram(Shader.GetProgramId());
	glUniformMatrix4fv(Shader.GetUniformId("view"), 1, GL_FALSE, Scene.GetMatrixView());
	glUniformMatrix4fv(Shader.GetUniformId("proj"), 1, GL_FALSE, Scene.GetMatrixProjection());
	glUniformMatrix4fv(Shader.GetUniformId("model"), 1, GL_FALSE, Scene.GetMatrixModel());
	
	//turn off pause
	Pause = false;
    
    return true;
}

void KeysProcessing() {
	if(keys[GLFW_KEY_ESCAPE]) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	if(keys['R']) {
		keys['R'] = GLFW_RELEASE;//clear key state to avoid multiple generations
		if(!GenerateLevel()) {
			LogFile<<"Error while generate level"<<endl;
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		return;
	}
}

void UpdateStep() {
}

void RenderStep() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//draw functions
	AtlasBuffer.Begin();
		AtlasBuffer.Draw();
	AtlasBuffer.End();
}

void ClearApp() {
	//clear funstions
	Cave.Close();
	AtlasBuffer.Close();
	TextureLoader.DeleteTexture(txAtlas, txAtlas_cnt);
	TextureLoader.Close();
	
	memset(keys, 0, 1024);
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	const double LimitFPS = (double) 1.0 / 60.0;
	int Frames = 0;
	int Updates = 0;
	double LastTime, NowTime = 0;
	double RenderTimer = 0;
	
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		LogFile.close();
		return 0;
	}
	
	while(!glfwWindowShouldClose(window)) {
		NowTime = glfwGetTime();
		DeltaTime = NowTime - LastTime;
		RenderTimer += DeltaTime;
		LastTime = NowTime;
		
		if(!Pause) UpdateStep();
		if(RenderTimer >= LimitFPS) {
			RenderStep();
			RenderTimer -= LimitFPS;
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		KeysProcessing();
	}
	ClearApp();
    glfwTerminate();
    LogFile.close();
}
