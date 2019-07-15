#ifndef sceneH
#define sceneH

class MScene
{
private:
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
public:
	bool Initialize(int Width, int Height);
	bool Size(int Width, int Height);
	void LookAt(glm::vec2 Point);
	void MatrixModelTranslate(glm::vec2 Offset);
	float* GetMatrixProjection();
	float* GetMatrixView();
	float* GetMatrixModel();
};

#endif
