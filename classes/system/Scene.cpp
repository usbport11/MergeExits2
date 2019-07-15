#include "../../stdafx.h"
#include "Scene.h"

bool MScene::Initialize(int Width, int Height)
{
	if(Width < 1 || Height < 1) {
		LogFile<<"Scene: wrong window size"<<endl;
		return false;
	}

	//prepare view
	Projection = glm::ortho(0.0f, (float)Width, 0.0f, (float)Height, -2.0f, 2.0f);
	Model = glm::mat4(1.0f);
	View = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glViewport(0, 0, Width, Height);
	
	return true;
}

bool MScene::Size(int Width, int Height)
{
	if(Width < 1 || Height < 1) {
		LogFile<<"Scene: wrong window size"<<endl;
		return false;
	}
	
	//prepare view
	Projection = glm::ortho(0.0f, (float)Width, 0.0f, (float)Height, -2.0f, 2.0f);
	
    glViewport(0, 0, Width, Height);
    
    return true;
}

void MScene::LookAt(glm::vec2 Point) {
	//after call this function do not forget update view uniform
	View = glm::lookAt(glm::vec3(Point, 1.0f), glm::vec3(Point, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void MScene::MatrixModelTranslate(glm::vec2 Offset) {
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, glm::vec3(Offset, 0.0f));
}

float* MScene::GetMatrixProjection() {
	return (float*)&Projection;
}

float* MScene::GetMatrixView() {
	return (float*)&View;
}

float* MScene::GetMatrixModel() {
	return (float*)&Model;
}

