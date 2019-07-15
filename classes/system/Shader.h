#ifndef shaderH
#define shaderH

struct stStringCompare {
	bool operator() (const char* a, const char* b) {
		return strcmp(a, b) < 0;
	}
};

class MShader
{
private:
	char* VertexShaderFileName;
	char* FragmentShaderFileName;
	GLuint VertexArrayId;
	GLuint ProgramId;
	map<const char*, GLuint, stStringCompare> Uniforms;

public:
	MShader();
	~MShader();
	GLuint CreateShaderProgram(const char* inVertexShaderFileName, const char* inFragmentShaderFileName);
	GLuint GetProgramId();
	bool AddUnifrom(const char* Name, const char* SearchName);
	GLuint GetUniformId(const char* Name);
	bool PrepareShaderValues();
	void Close();
};

#endif
