#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("Line.vert", "Line.frag", nullptr);

	BuildUncompleteRectangle();
	
	BuildTopLeftCurve();
	
	BuildTopRightCurve();
	
	BuildBottomRightCurve();
	
	BuildBottomLeftCurve();

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
	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &EBO3);
	glDeleteVertexArrays(1, &VAO4);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &EBO4);
	glDeleteVertexArrays(1, &VAO5);
	glDeleteBuffers(1, &VBO5);
	glDeleteBuffers(1, &EBO5);
}

void Demo::Update(double deltaTime) {

}

void Demo::Render() {
	// render
	// ------
	glViewport(0, 0, this->screenWidth, this->screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	DrawUncompleteRectangle();
	
	DrawTopLeftCurve();
	
	DrawTopRightCurve();
	
	DrawBottomRightCurve();
	
	DrawBottomLeftCurve();
	
}

void Demo::BuildUncompleteRectangle() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		0.4f,  0.5f, 0.0f,  // top right
		-0.4f,  0.5f, 0.0f,   // top left 
		-0.5f,  0.4f, 0.0f,  // right top
		-0.5f,  -0.4f, 0.0f,   // right bottom 
		-0.4f,  -0.5f, 0.0f,  // bottom left
		0.4f,  -0.5f, 0.0f,   // bottom right 
		0.5f,  0.4f, 0.0f,  // right top
		0.5f,  -0.4f, 0.0f   // right bottom
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1,   // right Line
		2, 3,   // bottom Triangle
		4, 5,   // left Triangle
		6, 7    // top Triangle
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}

void Demo::BuildTopLeftCurve() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	int segmentCount = 12;

	float vertices[39] = {};
	unsigned int indices[24] = {};

	p0x = -0.5f;
	p0y = 0.4f;
	p1x = -0.5f;
	p1y = 0.5f;
	p2x = -0.4f;
	p2y = 0.5f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		vertices[i * 3] = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		vertices[i * 3 + 1] = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		vertices[i * 3 + 2] = 0.0f;
	}

	for (int i = 0; i < segmentCount; i++) {
		indices[i * 2] = i;
		indices[i * 2 + 1] = i + 1;
	}

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}

void Demo::BuildTopRightCurve() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	int segmentCount = 12;

	float vertices[39] = {};
	unsigned int indices[24] = {};

	p0x = 0.5f;
	p0y = 0.4f;
	p1x = 0.5f;
	p1y = 0.5f;
	p2x = 0.4f;
	p2y = 0.5f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		vertices[i * 3] = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		vertices[i * 3 + 1] = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		vertices[i * 3 + 2] = 0.0f;
	}

	for (int i = 0; i < segmentCount; i++) {
		indices[i * 2] = i;
		indices[i * 2 + 1] = i + 1;
	}

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}

void Demo::BuildBottomRightCurve() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	int segmentCount = 12;

	float vertices[39] = {};
	unsigned int indices[24] = {};

	p0x = 0.5f;
	p0y = -0.4f;
	p1x = 0.5f;
	p1y = -0.5f;
	p2x = 0.4f;
	p2y = -0.5f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		vertices[i * 3] = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		vertices[i * 3 + 1] = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		vertices[i * 3 + 2] = 0.0f;
	}

	for (int i = 0; i < segmentCount; i++) {
		indices[i * 2] = i;
		indices[i * 2 + 1] = i + 1;
	}

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}

void Demo::BuildBottomLeftCurve() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	int segmentCount = 12;

	float vertices[39] = {};
	unsigned int indices[24] = {};

	p0x = -0.5f;
	p0y = -0.4f;
	p1x = -0.5f;
	p1y = -0.5f;
	p2x = -0.4f;
	p2y = -0.5f;
	for (int i = 0; i <= segmentCount; i++) {
		float t = (float)i / segmentCount;
		vertices[i * 3] = ((1 - t) * (1 - t) * p0x) + (2 * (1 - t) * t * p1x) + (t * t * p2x);
		vertices[i * 3 + 1] = ((1 - t) * (1 - t) * p0y) + (2 * (1 - t) * t * p1y) + (t * t * p2y);
		vertices[i * 3 + 2] = 0.0f;
	}

	for (int i = 0; i < segmentCount; i++) {
		indices[i * 2] = i;
		indices[i * 2 + 1] = i + 1;
	}

	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glGenBuffers(1, &EBO5);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO5);

	glBindBuffer(GL_ARRAY_BUFFER, VBO5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}

void Demo::DrawUncompleteRectangle()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Demo::DrawTopLeftCurve()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Demo::DrawTopRightCurve()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Demo::DrawBottomRightCurve()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO4); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Demo::DrawBottomLeftCurve()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO5); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(int argc, char** argv) {
	RenderEngine* app = new Demo();
	app->Start("Driving Line using Bezier Curve", 800, 600, false, false);
}