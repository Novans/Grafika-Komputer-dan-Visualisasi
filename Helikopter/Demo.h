#pragma once
#include "RenderEngine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <SOIL/SOIL.h>
using namespace std;

class Demo :
	public RenderEngine
{
public:
	Demo();
	~Demo();
private:
	GLuint shaderProgram, VBO, VAO, EBO, texture, VBO2, VAO2, EBO2, texture2, VBO3, VAO3, EBO3, texture3, VBO4, VAO4, EBO4, texture4, VBO5, VAO5, EBO5, texture5, VBO6, VAO6, EBO6, texture6, VBO7, VAO7, EBO7, texture7, VBO8, VAO8, EBO8, texture8, VBO9, VAO9, EBO9, texture9, VBO10, VAO10, EBO10, texture10, VBO11, VAO11, EBO11, texture11;
	float angle = 0, movementX = 0, movementY = 0, movementZ = 0, movementIndex = 0;
	float timeDelay = 0, angleHelicopter = 90.0f;
	float backPropelerX = 0, backPropelerZ = -6.75;
	float viewCamX, viewCamY, viewCamZ, upCamX, upCamY, upCamZ, posCamX, posCamY, posCamZ, CAMERA_SPEED;
	float p0x = 0, p0y = 0, p0z = 0, p1x = 0, p1y = 0, p1z = 0, p2x = 0, p2y = 0, p2z = 0;
	bool isFreeMode = true;
	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow* window);
	void BuildWaypoint();
	void BuildSideBody();
	void BuildFrontBody();
	void BuildOtherBody();
	void BuildTopPropeler();
	void BuildBackPropeler();
	void BuildBottomPlane();
	void BuildTopPlane();
	void BuildLeftPlane();
	void BuildRightPlane();
	void BuildFrontPlane();
	void BuildBackPlane();
	void DrawSideBody();
	void DrawFrontBody();
	void DrawOtherBody();
	void DrawTopPropeler();
	void DrawBackPropeler();
	void DrawBottomPlane();
	void DrawTopPlane();
	void DrawLeftPlane();
	void DrawRightPlane();
	void DrawFrontPlane();
	void DrawBackPlane();
	void MoveCamera(float speed);
	void StrafeCamera(float speed);
	void ForwardCamera(float speed);
	void VerticalCamera(float speed);
	void RotateCamera(float speed);
	void InitCamera();
};

