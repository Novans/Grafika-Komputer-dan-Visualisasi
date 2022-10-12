#include "Demo.h"
#include <vector>

std::vector<glm::vec3> waypoint;

Demo::Demo() {

}

Demo::~Demo() {
}

void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

	BuildWaypoint();

	BuildSideBody();
	
	BuildFrontBody();
	
	BuildOtherBody();
	
	BuildTopPropeler();
	
	BuildBackPropeler();

	BuildBottomPlane();

	BuildTopPlane();

	BuildLeftPlane();

	BuildRightPlane();

	BuildFrontPlane();

	BuildBackPlane();

	InitCamera();

}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		isFreeMode = false;
		posCamX = movementX;
		viewCamX = movementX;
		posCamY = 1.0f;
		posCamZ = 8.0f;
		viewCamY = 1.0f;
		viewCamZ = 0.0f;
		upCamX = 0.0f;
		upCamY = 1.0f;
		upCamZ = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		isFreeMode = true;
		posCamX = movementX + 6;
		viewCamX = movementX - 5;
		posCamY = 8.0f;
		posCamZ = 9.0f;
		upCamY = 2.0f;
	}
	if (isFreeMode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			MoveCamera(CAMERA_SPEED);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			MoveCamera(-CAMERA_SPEED);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			StrafeCamera(-CAMERA_SPEED);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			StrafeCamera(CAMERA_SPEED);
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			VerticalCamera(CAMERA_SPEED);
		}

		// update camera rotation
	// ----------------------
		double mouseX, mouseY;
		double midX = screenWidth / 2;
		double midY = screenHeight / 2;
		float angleY = 0.0f;
		float angleZ = 0.0f;

		// Get mouse position
		glfwGetCursorPos(window, &mouseX, &mouseY);
		if ((mouseX == midX) && (mouseY == midY)) {
			return;
		}

		// Set mouse position
		glfwSetCursorPos(window, midX, midY);

		// Get the direction from the mouse cursor, set a resonable maneuvering speed
		angleY = (float)((midX - mouseX)) / 1000;
		angleZ = (float)((midY - mouseY)) / 1000;

		// The higher the value is the faster the camera looks around.
		viewCamY += angleZ * 2;

		RotateCamera(-angleY);
	}
	
	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}

}

void Demo::Update(double deltaTime) {
	angle += (float)((deltaTime * 1.5f) / 10);
	timeDelay += deltaTime;
	if (timeDelay > 50) {
		if (movementIndex < waypoint.size()) {
			movementX = waypoint[movementIndex].x;
			movementY = waypoint[movementIndex].y;
			movementZ = waypoint[movementIndex].z;

			if (movementIndex < 25) {
				backPropelerX += 0.27;
				backPropelerZ += 0.27;
				/*posCamX += 0.32;
				posCamZ += 0.48;
				viewCamX -= 0.2;
				viewCamZ -= 0.2;*/
			}
			else if (movementIndex < 50) {
				backPropelerX -= 0.27;
				backPropelerZ += 0.27;
				/*posCamX -= 0.48;
				posCamZ += 0.32;
				viewCamX += 0.2;
				viewCamZ -= 0.2;*/
			}
			else if (movementIndex < 75) {
				backPropelerX -= 0.27;
				backPropelerZ -= 0.27;
				/*posCamX -= 0.32;
				posCamZ -= 0.48;
				viewCamX += 0.2;
				viewCamZ += 0.2;*/
			}
			else if (movementIndex < 100) {
				backPropelerX += 0.27;
				backPropelerZ -= 0.27;
				/*posCamX += 0.48;
				posCamZ -= 0.32;
				viewCamX -= 0.2;
				viewCamZ += 0.2;*/
			}

			movementIndex++;
			angleHelicopter -= 3.6;
		}
		else {
			movementIndex = 0;
		}
		timeDelay = 0;
	}
	
}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	DrawSideBody();
	
	DrawFrontBody();
	
	DrawOtherBody();
	
	DrawTopPropeler();

	DrawBackPropeler();	

	DrawBottomPlane();

	DrawTopPlane();

	DrawLeftPlane();

	DrawRightPlane();

	DrawFrontPlane();

	DrawBackPlane();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildWaypoint() {

	int segmentCount = 24;

	p0x = 25.0f;
	p0y = 0.0f;
	p0z = 0.0f;
	p1x = 25.0f;
	p1y = 0.0f;
	p1z = 25.0f;
	p2x = 0.0f;
	p2y = 0.0f;
	p2z = 25.0f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		float x = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		float y = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		float z = ((1 - t) * (1 - t) * p0z) + (2 * (1 - t) * t * p1z) + (t * t * p2z);
		waypoint.push_back(glm::vec3(x, y, z));
	}

	p0x = -1.0f;
	p0y = 0.0f;
	p0z = 25.0f;
	p1x = -25.0f;
	p1y = 0.0f;
	p1z = 25.0f;
	p2x = -25.0f;
	p2y = 0.0f;
	p2z = 0.0f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		float x = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		float y = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		float z = ((1 - t) * (1 - t) * p0z) + (2 * (1 - t) * t * p1z) + (t * t * p2z);
		waypoint.push_back(glm::vec3(x, y, z));
	}

	p0x = -25.0f;
	p0y = 0.0f;
	p0z = -1.0f;
	p1x = -25.0f;
	p1y = 0.0f;
	p1z = -25.0f;
	p2x = 0.0f;
	p2y = 0.0f;
	p2z = -25.0f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		float x = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		float y = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		float z = ((1 - t) * (1 - t) * p0z) + (2 * (1 - t) * t * p1z) + (t * t * p2z);
		waypoint.push_back(glm::vec3(x, y, z));
	}

	p0x = 1.0f;
	p0y = 0.0f;
	p0z = -25.0f;
	p1x = 25.0f;
	p1y = 0.0f;
	p1z = -25.0f;
	p2x = 25.0f;
	p2y = 0.0f;
	p2z = -1.0f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		float x = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		float y = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		float z = ((1 - t) * (1 - t) * p0z) + (2 * (1 - t) * t * p1z) + (t * t * p2z);
		waypoint.push_back(glm::vec3(x, y, z));
	}

}

void Demo::BuildOtherBody() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("body.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// bottom head (left)
		 -2, -3, -2, 0, 0,  // 0
		 -2, -3,  2, 1, 0,  // 1
		-10,  0,  2, 1, 1,  // 2
		-10,  0, -2, 0, 1,  // 3

		// bottom head (front)
		-10,  0, 2, 0, 1,  // 4
		 -2,  0, 2, 1, 1,  // 5
		 -2, -3, 2, 1, 0,  // 6

		// bottom head (back)
		-10,  0, -2, 0, 1,  // 7
		 -2,  0, -2, 1, 1,  // 8
		 -2, -3, -2, 1, 0,  // 9

		// top head (front)
		-10, 0, 2, 0, 0,  // 10
		 -2, 0, 2, 1, 0,  // 11
		 -2, 3, 2, 1, 1,  // 12

		// top head (back)
		-10, 0, -2, 0, 0,  // 13
		 -2, 0, -2, 1, 0,  // 14
		 -2, 3, -2, 1, 1,  // 15

		// body (bottom)
		-2, -3,  2, 0, 0,  // 16
		 2, -3,  2, 1, 0,  // 17
		-2, -3, -2, 0, 1,  // 18
		 2, -3, -2, 1, 1,  // 19

		// body (top)
		-2, 3,  2, 0, 0,  // 20
		 2, 3,  2, 1, 0,  // 21
		-2, 3, -2, 0, 1,  // 22
		 2, 3, -2, 1, 1,  // 23

		// back body (front)
		2, -3, 2, 0, 0,    // 24
		2,  3, 2, 0, 1,    // 25
		7,  0, 2, 1, 0.5,  // 26

		// back body (back)
		2, -3, -2, 0, 0,    // 27
		2,  3, -2, 0, 1,    // 28
		7,  0, -2, 1, 0.5,  // 29

		// back body (top)
		7, 0,  2, 0, 0,  // 30
		7, 0, -2, 1, 0,  // 31
		2, 3,  2, 0, 1,  // 32
		2, 3, -2, 1, 1,  // 33

		// back body (bottom)
		7,  0,  2, 0, 0,  // 34
		7,  0, -2, 1, 0,  // 35
		2, -3,  2, 0, 1,  // 36
		2, -3, -2, 1, 1,  // 37

		// top body (front)
		-1, 3, 1, 0, 0,  // 38
		 1, 3, 1, 1, 0,  // 39
		-1, 4, 1, 0, 1,  // 40
		 1, 4, 1, 1, 1,  // 41

		// top body (back)
		-1, 3, -1, 0, 0,  // 42
		 1, 3, -1, 1, 0,  // 43
		-1, 4, -1, 0, 1,  // 44
		 1, 4, -1, 1, 1,  // 45

		// top body (left)
		-1, 3, -1, 0, 0,  // 46
		-1, 3,  1, 1, 0,  // 47
		-1, 4, -1, 0, 1,  // 48
		-1, 4,  1, 1, 1,  // 49

		// tail (front)
		 5, -1, 2, 0, 0,  // 50
		16, -1, 2, 1, 0,  // 51
		 5,  1, 2, 0, 1,  // 52
		16,  1, 2, 1, 1,  // 53

		// tail (back)
		 5, -1, -2, 0, 0,  // 54
		16, -1, -2, 1, 0,  // 55
		 5,  1, -2, 0, 1,  // 56
		16,  1, -2, 1, 1,  // 57

		// tail (right)
		16, -1,  2, 0, 0,  // 58
		16, -1, -2, 1, 0,  // 59
		16,  1,  2, 0, 1,  // 60
		16,  1, -2, 1, 1,  // 61

		// tail+ (left)
		13, -0.5,   2, 0, 0,  // 62
		13, -0.5, 2.5, 1, 0,  // 63
		13,  0.5,   2, 0, 1,  // 64
		13,  0.5, 2.5, 1, 1,  // 65

		// tail+ (right)
		14, -0.5,   2, 0, 0,  // 66
		14, -0.5, 2.5, 1, 0,  // 67
		14,  0.5,   2, 0, 1,  // 68
		14,  0.5, 2.5, 1, 1,  // 69

		// tail (top)
		 5, 1,  2, 0, 0,  // 70
		16, 1,  2, 1, 0,  // 71
		 5, 1, -2, 1, 0,  // 72
		16, 1, -2, 1, 1,  // 73

		// tail (bottom)
		 5, -1,  2, 0, 0,  // 74
		16, -1,  2, 1, 0,  // 75
		 5, -1, -2, 0, 1,  // 76
		16, -1, -2, 1, 1,  // 77

		// top body (right)
		1, 3, -1, 0, 0,  // 78
		1, 3,  1, 1, 0,  // 79
		1, 4, -1, 0, 1,  // 80
		1, 4,  1, 1, 1,  // 81

		// tail+ (top)
		13, 0.5, 2.5, 0, 0,  // 82
		14, 0.5, 2.5, 1, 0,  // 83
		13, 0.5,   2, 0, 1,  // 84
		14, 0.5,   2, 1, 1,  // 85

		// tail+ (bottom)
		13, -0.5, 2.5, 0, 0,  // 82
		14, -0.5, 2.5, 1, 0,  // 83
		13, -0.5,   2, 0, 1,  // 84
		14, -0.5,   2, 1, 1,  // 85

	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // bottom head (left)
		4,  5,  6,			     // bottom head (front)
		7,  8,  9,				 // bottom head (back)
		10, 11, 12,			     // top head (front)
		13, 14, 15,			     // top head (back)
		16, 17, 19, 16, 18, 19,  // body (bottom)
		20, 21, 23, 20, 22, 23,  // body (top)
		24, 25, 26,				 // back body (front)
		27, 28, 29,				 // back body (back)
		30, 31, 33, 30, 32, 33,  // back body (top)
		34, 35, 37, 34, 36, 37,  // back body (bottom)
		38, 39, 41, 38, 40, 41,  // top body (front)
		42, 43, 45, 42, 44, 45,  // top body (back)
		46, 47, 49, 46, 48, 49,  // top body (left)
		50, 51, 53, 50, 52, 53,  // tail (front)
		54, 55, 57, 54, 56, 57,  // tail (back)
		58, 59, 61, 58, 60, 61,  // tail (right)
		62, 63, 65, 62, 64, 65,  // tail+ (left)
		66, 67, 69, 66, 68, 69,  // tail+ (right)
		70, 71, 73, 70, 72, 73,  // tail (top)
		74, 75, 77, 74, 76, 77,  // tail (bottom)
		78, 79, 81, 78, 80, 81,  // top body (right)
		82, 83, 85, 82, 84, 85,  // tail+ (top)
		86, 87, 89, 86, 88, 89,  // tail+ (bottom)
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawOtherBody()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized


	glm::mat4 model;

	model = glm::translate(model, glm::vec3(movementX, movementY, movementZ));

	model = glm::rotate(model, glm::radians(angleHelicopter), glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 126, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildSideBody() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("door.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-2, -3, 2, 0, 0,  // 0
		 2, -3, 2, 1, 0,  // 1
		-2,  3, 2, 0, 1,  // 2
		 2,  3, 2, 1, 1,  // 3

		// back
		-2, -3, -2, 0, 0,  // 4
		 2, -3, -2, 1, 0,  // 5
		-2,  3, -2, 0, 1,  // 6
		 2,  3, -2, 1, 1,  // 7

	};

	unsigned int indices[] = {
		0,  1,  3,  0,  2,  3,   // front
		4,  5,  7,  4,  6,  7    // back
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawSideBody()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized


	glm::mat4 model;

	model = glm::translate(model, glm::vec3(movementX, movementY, movementZ));

	model = glm::rotate(model, glm::radians(angleHelicopter), glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildFrontBody() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("window.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		-10, 0, -2, 0, 0,  // 0
		-10, 0,  2, 1, 0,  // 1
		 -2, 3, -2, 0, 1,  // 2
		 -2, 3,  2, 1, 1,  // 3

	};

	unsigned int indices[] = {
		0,  1,  3,  0,  2,  3
	};

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawFrontBody()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO4); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized


	glm::mat4 model;

	model = glm::translate(model, glm::vec3(movementX, movementY, movementZ));

	model = glm::rotate(model, glm::radians(angleHelicopter), glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildTopPropeler() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("propeler.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-6,   4, 1, 0, 0,  // 0
		 6,   4, 1, 1, 0,  // 1
		-6, 4.5, 1, 0, 1,  // 2
		 6, 4.5, 1, 1, 1,  // 3

		// back
		-6,   4, -1, 0, 0,  // 4
		 6,   4, -1, 1, 0,  // 5
		-6, 4.5, -1, 0, 1,  // 6
		 6, 4.5, -1, 1, 1,  // 7

		// left
		-6,   4, -1, 0, 0,  // 8
		-6,   4,  1, 1, 0,  // 9
		-6, 4.5, -1, 0, 1,  // 10
		-6, 4.5,  1, 1, 1,  // 11

		// right
		6,   4, -1, 0, 0,  // 12
		6,   4,  1, 1, 0,  // 13
		6, 4.5, -1, 0, 1,  // 14
		6, 4.5,  1, 1, 1,  // 15

		// top
		-6, 4.5,  1, 0, 0,  // 16
		 6, 4.5,  1, 1, 0,  // 17
		-6, 4.5, -1, 0, 1,  // 18
		 6, 4.5, -1, 1, 1,  // 19

		// bottom
		-6, 4,  1, 0, 0,  // 20
		 6, 4,  1, 1, 0,  // 21
		-6, 4, -1, 0, 1,  // 22
		 6, 4, -1, 1, 1,  // 23

	};

	unsigned int indices[] = {
		0,   1,  3,  0,  2,  3,   // front
		4,   5,  7,  4,  6,  7,   // back
		8,   9, 11,  8, 10, 11,   // left
		12, 13, 15, 12, 14, 15,   // right
		16, 17, 19, 16, 18, 19,   // top
		20, 21, 23, 20, 22, 23    // bottom
	};

	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glGenBuffers(1, &EBO5);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO5);

	glBindBuffer(GL_ARRAY_BUFFER, VBO5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawTopPropeler()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture5);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO5); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized


	glm::mat4 model;

	model = glm::translate(model, glm::vec3(movementX, movementY, movementZ));

	model = glm::rotate(model, glm::radians(angleHelicopter), glm::vec3(0, 1, 0));

	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildBackPropeler() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture6);
	glBindTexture(GL_TEXTURE_2D, texture6);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("propeler.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-3, -0.5, 3, 0, 0,  // 0
		3, -0.5, 3, 1, 0,  // 1
		-3,  0.5, 3, 0, 1,  // 2
		3,  0.5, 3, 1, 1,  // 3

		// back
		-3, -0.5, 2.5, 0, 0,  // 4
		3, -0.5, 2.5, 1, 0,  // 5
		-3,  0.5, 2.5, 0, 1,  // 6
		3,  0.5, 2.5, 1, 1,  // 7

		// left
		-3, -0.5, 2.5, 0, 0,  // 8
		-3, -0.5,   3, 1, 0,  // 9
		-3,  0.5, 2.5, 0, 1,  // 10
		-3,  0.5,   3, 1, 1,  // 11

		// right
		-3, -0.5, 2.5, 0, 0,  // 12
		-3, -0.5,   3, 1, 0,  // 13
		-3,  0.5, 2.5, 0, 1,  // 14
		-3,  0.5,   3, 1, 1,  // 15

		// top
		-3, 0.5,   3, 0, 0,  // 16
		3, 0.5,   3, 1, 0,  // 17
		-3, 0.5, 2.5, 0, 1,  // 18
		3, 0.5, 2.5, 1, 1,  // 19

		// bottom
		-3, -0.5,   3, 0, 0,  // 20
		3, -0.5,   3, 1, 0,  // 21
		-3, -0.5, 2.5, 0, 1,  // 22
		3, -0.5, 2.5, 1, 1,  // 23

	};

	unsigned int indices[] = {
		0,   1,  3,  0,  2,  3,   // front
		4,   5,  7,  4,  6,  7,   // back
		8,   9, 11,  8, 10, 11,   // left
		12, 13, 15, 12, 14, 15,   // right
		16, 17, 19, 16, 18, 19,   // top
		20, 21, 23, 20, 22, 23    // bottom
	};

	glGenVertexArrays(1, &VAO6);
	glGenBuffers(1, &VBO6);
	glGenBuffers(1, &EBO6);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO6);

	glBindBuffer(GL_ARRAY_BUFFER, VBO6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO6);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawBackPropeler()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture6);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO6); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;

	model = glm::translate(model, glm::vec3(movementX, movementY, movementZ));

	model = glm::translate(model, glm::vec3(backPropelerX, 0, backPropelerZ));

	model = glm::rotate(model, glm::radians(angleHelicopter), glm::vec3(0, 1, 0));

	model = glm::rotate(model, angle, glm::vec3(0.0, 0.0, 1.0));

	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildBottomPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("bottom.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, -50.0, -50.0,  0,  0,
		 50.0, -50.0, -50.0, 1,  0,
		 50.0, -50.0,  50.0, 1, 1,
		-50.0, -50.0,  50.0,  0, 1,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawBottomPlane()
{

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	
}

void Demo::BuildTopPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture7);
	glBindTexture(GL_TEXTURE_2D, texture7);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("top.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// top
		-50.0, 50.0, -50.0,  0,  0,
		 50.0, 50.0, -50.0,  1,  0,
		 50.0, 50.0,  50.0,  1, 1,
		-50.0, 50.0,  50.0,  0, 1,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO7);
	glGenBuffers(1, &VBO7);
	glGenBuffers(1, &EBO7);

	glBindVertexArray(VAO7);

	glBindBuffer(GL_ARRAY_BUFFER, VBO7);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO7);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawTopPlane()
{

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture7);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO7); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

}

void Demo::BuildLeftPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture8);
	glBindTexture(GL_TEXTURE_2D, texture8);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("left.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// left
		-50.0, -50.0,  50.0,  0,  0,
		-50.0, -50.0, -50.0,  1,  0,
		-50.0,  50.0, -50.0,  1, 1,
		-50.0,  50.0,  50.0,  0, 1,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO8);
	glGenBuffers(1, &VBO8);
	glGenBuffers(1, &EBO8);

	glBindVertexArray(VAO8);

	glBindBuffer(GL_ARRAY_BUFFER, VBO8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO8);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawLeftPlane()
{

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture8);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO8); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

}

void Demo::BuildRightPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture9);
	glBindTexture(GL_TEXTURE_2D, texture9);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("right.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// left
		50.0, -50.0, -50.0,  0, 0,
		50.0, -50.0,  50.0,  1,  0,
		50.0,  50.0,  50.0,  1, 1,
		50.0,  50.0, -50.0,  0, 1,

	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO9);
	glGenBuffers(1, &VBO9);
	glGenBuffers(1, &EBO9);

	glBindVertexArray(VAO9);

	glBindBuffer(GL_ARRAY_BUFFER, VBO9);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO9);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawRightPlane()
{

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture9);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO9); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

}

void Demo::BuildFrontPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture10);
	glBindTexture(GL_TEXTURE_2D, texture10);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("front.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// front
		 50.0, -50.0, 50.0, 0, 0,
		-50.0, -50.0, 50.0, 1, 0,
		-50.0,  50.0, 50.0, 1, 1,
		 50.0,  50.0, 50.0, 0, 1,

	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO10);
	glGenBuffers(1, &VBO10);
	glGenBuffers(1, &EBO10);

	glBindVertexArray(VAO10);

	glBindBuffer(GL_ARRAY_BUFFER, VBO10);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO10);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawFrontPlane()
{

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture10);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO10); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

}

void Demo::BuildBackPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture11);
	glBindTexture(GL_TEXTURE_2D, texture11);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("back.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// front
		-50.0, -50.0, -50.0, 0, 0,
		 50.0, -50.0, -50.0, 1, 0,
		 50.0,  50.0, -50.0, 1, 1,
		-50.0,  50.0, -50.0, 0, 1,

	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO11);
	glGenBuffers(1, &VBO11);
	glGenBuffers(1, &EBO11);

	glBindVertexArray(VAO11);

	glBindBuffer(GL_ARRAY_BUFFER, VBO11);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO11);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawBackPlane()
{

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture11);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO11); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

}

void Demo::InitCamera()
{
	posCamX = 2.0f;
	posCamY = 8.0f;
	posCamZ = -10.0f;
	viewCamX = 0.0f;
	viewCamY = 0.0f;
	viewCamZ = 5.0f;
	upCamX = 0.0f;
	upCamY = 2.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.001f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::ForwardCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	viewCamX = viewCamX + orthoX * speed;
}

void Demo::VerticalCamera(float speed)
{
	float y = viewCamY - posCamY;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamY = posCamY + y * speed;
	viewCamY = viewCamY + y * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}

int main(int argc, char** argv) {
	RenderEngine* app = new Demo();
	app->Start("Transformation: Helicopter", 800, 600, false, false);
}