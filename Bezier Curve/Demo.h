#pragma once
#include "RenderEngine.h"
using namespace std;

class Demo :
	public RenderEngine
{
public:
	Demo();
	~Demo();
private:
	GLuint shaderProgram, VBO, VAO, EBO, VBO2, VAO2, EBO2, VBO3, VAO3, EBO3, VBO4, VAO4, EBO4, VBO5, VAO5, EBO5;
	float p0x = 0, p0y = 0, p1x = 0, p1y = 0, p2x = 0, p2y = 0;
	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow* window);
	void BuildUncompleteRectangle();
	void BuildTopLeftCurve();
	void BuildTopRightCurve();
	void BuildBottomRightCurve();
	void BuildBottomLeftCurve();
	void DrawUncompleteRectangle();
	void DrawTopLeftCurve();
	void DrawTopRightCurve();
	void DrawBottomRightCurve();
	void DrawBottomLeftCurve();
};

