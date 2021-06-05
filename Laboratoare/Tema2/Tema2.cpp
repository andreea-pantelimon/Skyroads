#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <time.h>

#include <Core/Engine.h>

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

float min(float a, float b) {
	return a < b ? a : b;
}

float max(float a, float b) {
	return a > b ? a : b;
}

void Tema2::Init()
{
	srand(time(NULL));
	GetSceneCamera()->SetPosition(glm::vec3(0.0f, 3.0f, 4.0f));
	GetSceneCamera()->RotateOX(-120.0f);

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// create shader
	{
		Shader* shader = new Shader("ShaderTema2");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	cout << "Nivel: " << level << endl;
}


void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

float Tema2::RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

bool Tema2::OkDistance(platform* newPlatforms, int noPlatforms)
{
	int* distances = new int[noPlatforms];
	for (int i = 0; i < noPlatforms; i++) {
		distances[i] = newPlatforms[i].column;
	}
	sort(distances, distances + noPlatforms);
	for (int i = 1; i < noPlatforms; i++) {
		if (distances[i] - distances[i - 1] > 3) // sphere cannot jump 4+ columns
			return false;
	}
	int minDistance = 7;

	if (!lastColumns)
		return true;

	for (int i = 0; i < noPlatforms; i++) {
		for (int j = 0; j < 8; j++) {
			if (lastColumns[j] != -1) {
				if (abs(lastColumns[j] - distances[i]) <= 4)
					return true;
			}
			
		}
	}

	return false;
}

void Tema2::getSphereX() {

	for (int i = 0; i < totalPlatforms; i++) {
		if (validPlatform[i] && platforms[i].nearZ == 1.0f) {
			posX = platforms[i].column - 3.5f;
			break;
		}
	}

}

int Tema2::RandomColor(int noPlatforms)
{
	if (!lastColumns || noPlatforms == 1)
		return blue;

	int rColor = rand() % 15 + 1;
	
	int steps = level / 20;
	for (int i = 0; i < steps; i++) {
		if (rColor == 4 || rColor == 5 || rColor == 7) {
			rColor = rand() % 15 + 1;
		}
	}

	if (rColor == 1)
		return red;
	if (rColor == 2)
		return yellow;
	if (rColor == 3)
		return orange;
	if (rColor == 4 || rColor == 5)
		return green;
	if (rColor == 6)
		return brown;
	if (rColor == 7)
		return lightBlue;
	return blue; // 50% chance
}

int Tema2::RandomColumn(bool* columns)
{
	int rColumn = 0;

	while (true) {
		rColumn = rand() % 9; // [0, 7]
		if (!columns[rColumn]) {
			columns[rColumn] = true;
			return rColumn;
		}
	}

	return rColumn;
}

glm::vec3 Tema2::GetColorMatrix(int color) {

	switch (color) {
		case purple: return purpleVec;
		case red: return redVec;
		case yellow: return yellowVec;
		case orange: return orangeVec;
		case green: return greenVec;
		case blue: return blueVec;
		case brown: return brownVec;
		case lightBlue: return lightBlueVec;
	}
}

bool Tema2::PlatformUnderSphere(Tema2::platform p) {

	if (p.nearZ >= 0.375 && p.nearZ - p.length <= 0.375) {
		float colX = p.column - 4.0f;
		if (posX >= colX && posX <= colX + 1) {
			return true;
		}
	}

	return false;
}

void Tema2::GeneratePlatforms()
{
	float maxLength;
	platform* newPlatforms = NULL;

	float lastZMin = 1.0f;
	bool first = true;

	for (int i = 0; i < 8; i++) {
		int index = lastPlatformIndexes[i];
		if (index != -1) {

			if (first) {
				first = false;
				lastZMin = platforms[index].nearZ - platforms[index].length;
			} else if (lastZMin > platforms[index].nearZ - platforms[index].length) {
				lastZMin = platforms[index].nearZ - platforms[index].length;
			}
		}
	}
	lastZ = lastZMin;
	float randFloat = RandomFloat(1.0f, 1.8f);

	if (lastZ >= renderDistance + randFloat && lastZ <= renderDistance + 2.0f + randFloat) {

		int noPlatforms = rand() % 5 + 1;

		if (noPlatforms < 3) // increase chances to have min. 3 columns
			noPlatforms = rand() % 5 + 1;
		if (noPlatforms == 1) // increase chances to have min. 1 column
			noPlatforms = rand() % 5 + 1;
		int steps = level / 20;
		for (int i = 0; i < steps; i++) {
			if (noPlatforms > 3)
				noPlatforms = rand() % 5 + 1;
		}

		while (true) {
			platform p;
			newPlatforms = new platform[noPlatforms];
			bool* columns = new bool[8]{ false };

			p.nearZ = renderDistance;
			maxLength = 0.0f;

			for (int i = 0; i < noPlatforms; i++) {
				p.length = RandomFloat(platformLengthMin, platformLengthMax); // platform random length
				p.type = RandomColor(noPlatforms);
				p.column = RandomColumn(columns);
				newPlatforms[i] = p;
				if (i == 0)
					maxLength = p.length;
				else if (p.length > maxLength)
					maxLength = p.length;
			}

			if (OkDistance(newPlatforms, noPlatforms)) {

				for (int i = 0; i < 8; i++) {
					lastColumns[i] = -1;
					lastPlatformIndexes[i] = -1;
				}

				for (int i = 0; i < noPlatforms; i++) {
					lastColumns[i] = newPlatforms[i].column;
				}
				int index = 0;

				for (int i = 0; i < noPlatforms; i++) {

					for (int j = 0; j < 100; j++) {
						if (!validPlatform[j]) {
							if (j >= totalPlatforms)
								totalPlatforms++;
							lastPlatformIndexes[index++] = j;
							platforms[j] = newPlatforms[i];
							validPlatform[j] = true;
							break;
						}
					}
				}
				free(newPlatforms);
				free(columns);

				break;
			}
			free(newPlatforms);
			free(columns);
		}
	}
}

void Tema2::GenerateFirstPlatforms()
{
	float firstZ = 1.0f;
	float maxLength;
	platform* newPlatforms = NULL;

	while (firstZ > renderDistance - platformLengthMax) {
		int noPlatforms = rand() % 5 + 1;
		if (noPlatforms < 3) // increase chances to have min. 3 columns
			noPlatforms = rand() % 5 + 1;
		
		while (true) {
			platform p;
			newPlatforms = new platform[noPlatforms];
			bool* columns = new bool[8]{false};

			p.nearZ = firstZ;
			maxLength = 0.0f;

			for (int i = 0; i < noPlatforms; i++) {
				p.length = RandomFloat(platformLengthMin, platformLengthMax); // platform random length
				p.type = RandomColor(noPlatforms);
				p.column = RandomColumn(columns);
				newPlatforms[i] = p;
				if (i == 0)
					maxLength = p.length;
				else if (p.length > maxLength)
					maxLength = p.length;
			}

			if (OkDistance(newPlatforms, noPlatforms)) {
				firstZ -= RandomFloat(1.0f, 2.0f) + maxLength;

				if (!firstLastColumns) {
					firstLastColumns = true;
					lastColumns = new int[8];
				}
				for (int i = 0; i < 8; i++) {
					lastColumns[i] = -1;
					lastPlatformIndexes[i] = -1;
				}

				int index = 0;
				for (int i = 0; i < noPlatforms; i++) {
					platforms[totalPlatforms + i] = newPlatforms[i];
					lastPlatformIndexes[index++] = totalPlatforms + i;
					validPlatform[totalPlatforms + i] = true;
					lastColumns[i] = newPlatforms[i].column;
				}
				free(newPlatforms);
				free(columns);

				totalPlatforms += noPlatforms;

				break;
			}
			free(newPlatforms);
			free(columns);
		}

	}
}

glm::mat3 Translate(float translateX, float translateY)
{
	// TODO implement translate matrix
	//return glm::mat3(1);
	return glm::transpose(glm::mat3(
		1, 0, translateX,
		0, 1, translateY,
		0, 0, 1
	));
}

glm::mat3 Scale(float scaleX, float scaleY)
{
	// TODO implement scale matrix
	return glm::transpose(glm::mat3(
		scaleX, 0, 0,
		0, scaleY, 0,
		0, 0, 1
	));
}

void Tema2::Update(float deltaTimeSeconds)
{
	if (platformSpeed < minSpeed)
		platformSpeed = minSpeed;
	else if (platformSpeed > maxSpeed)
		platformSpeed = maxSpeed;

	clock_t diffSpecial = clock() - timerSpecial;
	if (specialActivated && (float)diffSpecial / CLOCKS_PER_SEC > 1.0f) {
		specialActivated = false;
	}

	if (!gameLost) 
	{
		if (fuel > 0) {
			fuel -= deltaTimeSeconds / 2.0f * (1 + (level - 1) / 100.0f * 1.5f);
		}

		if (rotation > INT_MAX)
			rotation = 0.0f;
		if (specialActivated)
			rotation += deltaTimeSeconds * 5;
	} else {
		lives = 0;
		fuel = 0.0f;
	}

	if (fuel == 0) {
		if (lives > 1) {
			fuel = 100.0f;
		}
		else {
			gameLost = true; // out of lives
		}
		if (lives > 0)
			lives--;
	}

	if (speedLock) {
		clock_t diff = clock() - timerOrange;
		if (((float)diff) / CLOCKS_PER_SEC > 5.0f) {
			speedLock = false;
			platformSpeed = lastSpeed;
		}
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		glm::mat4 modelMatrix2 = glm::mat4(1);
		glm::vec3 colorMatrix = turquoiseVec;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.5f + 0.5f * (fuel / 100.f - 1.0f), 1.6f, -3.0f));

		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel / 100.0f, 0.06f, 0.0f));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, colorMatrix, true); // fuel bar

		colorMatrix = glm::vec3(1.0f); // white color
		modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.5f, 1.6f, -3.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(1, 0.06f, 0.0f));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix2, colorMatrix, true); // entire bar
	}
	{

		//render lives on 2D interface
		if (lives > 0) {
			glm::mat4 modelMatrix = glm::mat4(1);
			glm::vec3 colorMatrix = darkRedVec;

			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.1f, 1.4f, -3.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.0f));
			RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, true); // first life

			if (lives > 1) {
				modelMatrix = glm::translate(modelMatrix, glm::vec3(1.333f, 0.0f, 0.0f));
				RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, true); // second life
				if (lives > 2) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(1.333f, 0.0f, 0.0f));
					RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, true); // third life
					if (lives == 4) {
						modelMatrix = glm::translate(modelMatrix, glm::vec3(1.333f, 0.0f, 0.0f));
						RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, true); // fourth life
					}
				}
			}
		}
	}

	if (!firstPlatform) {
		GenerateFirstPlatforms();
		firstPlatform = true;
		getSphereX();
	}
	else
		GeneratePlatforms();

	for (int i = 0; i < totalPlatforms; i++) {
		if (validPlatform[i]) {
			glm::mat4 modelMatrix = glm::mat4(1);
			float ox = platforms[i].column - 3.5f;
			if (platforms[i].nearZ < lastNearZ)
				lastNearZ = platforms[i].nearZ;
			platforms[i].distance += deltaTimeSeconds * platformSpeed;
			platforms[i].nearZ += deltaTimeSeconds * platformSpeed;
			lastNearZ += deltaTimeSeconds * platformSpeed;

			modelMatrix = glm::translate(modelMatrix, glm::vec3(ox, -0.125f, platforms[i].nearZ - platforms[i].length / 2.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 0.25f, platforms[i].length));
			glm::vec3 colorMatrix = GetColorMatrix(platforms[i].type);
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, colorMatrix, false);
		}
	}

	if (!fallen) {
		if (jumping)
		{
			if (gAcc > 0) {
				gAcc -= deltaTimeSeconds * 15.0f;
				posY += deltaTimeSeconds * (gAcc);
			}
			else {
				jumping = false;
				falling = true;
				gAcc = 0.0f;
			}
		}
		else if (falling) {
			if (posY - lastGroundY > 0) {
				gAcc += deltaTimeSeconds * 15.0f;
				posY -= deltaTimeSeconds * (gAcc);
			}
			else {
				falling = false;
				posY = lastGroundY;
			}
		}
	}
	else {
		gAcc += deltaTimeSeconds * 15.0f;
		posY -= deltaTimeSeconds * (gAcc);
	}

	if (thirdPerson || (!fallen && gameLost) || touchedRed) {
		GetSceneCamera()->SetPosition(glm::vec3(0.0f, 3.0f, 4.0f));
	}
	else {
		GetSceneCamera()->SetPosition(glm::vec3(0.0f + posX, 0.375f + posY, 0.375f));
	}

	{ // render sphere

		if ((thirdPerson && posY > -10.0f || (!fallen && gameLost)) && !touchedRed) {
			glm::mat4 modelMatrix = glm::mat4(1);
			if (!specialActivated) {
				modelMatrix = glm::translate(modelMatrix, glm::vec3(posX + sideDist, 0.375f + posY, 0.375f + forwardDist));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 0.75f, 0.75f));
				glm::vec3 colorMatrix = glm::vec3(0.95f, 0.6f, 0.0f);
				RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, false);
			}
			else {
				modelMatrix = glm::translate(modelMatrix, glm::vec3(posX + sideDist, 0.34f + posY, 0.34f + forwardDist)); // inner sphere
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.68f, 0.68f, 0.68f));

				glm::vec3 colorMatrix = glm::vec3(0.91f, 0.91f, 0.87f);
				RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, false);


				for (int i = 1; i <= 20; i++) {
					modelMatrix = glm::mat4(1);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(posX + sideDist, 0.375f + posY, 0.375f + forwardDist));

					modelMatrix = glm::translate(modelMatrix, glm::vec3(1));
					modelMatrix = glm::rotate(modelMatrix, rotation * i / 2.0f, glm::vec3(1.0f, 1.0f, 1.0f));
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-1));

					if (i % 3 == 0)
						modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.75f, 0.75f));
					else if (i % 3 == 1)
						modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 0.15f, 0.75f));
					else
						modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 0.75f, 0.15f));
					RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, colorMatrix, false);
				}

			}

		}
	}

	bool underPlatform = false;

	if (!fallen) {
		for (int i = 0; i < totalPlatforms; i++) {
			if (posY == 0 && posY >= -0.2f && validPlatform[i] && PlatformUnderSphere(platforms[i])) {
				underPlatform = true;

				if (platforms[i].type == green) { // fuel increases
					if (fuel + 5 >= 100.0f)
						fuel = 100.0f;
					else
						fuel += 5;
				}
				else if (platforms[i].type == yellow) { // fuel decreases
					if (fuel - 10.0f * (1 + level / 100.0f) <= 0.0f)
						fuel = 0.0f;
					else
						fuel -= 10.0f * (1 + level / 100.0f);
				}
				else if (platforms[i].type == brown) {
					lives--;
					if (lives == 0)
						gameLost = true;
				}
				else if (platforms[i].type == lightBlue) {
					if (lives < 4)
						lives++;
				}
				else if (platforms[i].type == orange) {
					if (!speedLock)
						lastSpeed = platformSpeed;
					platformSpeed = maxSpeed;
					speedLock = true;
					timerOrange = clock();
				}
				else if (platforms[i].type == red) {
					touchedRed = true;
					gameLost = true;
				}
				if (!touched[i] && platforms[i].type != blue) {
					specialActivated = true;
					timerSpecial = clock();
				}
				touched[i] = true;

				if (platforms[i].type != purple && !fallen && !gameLost) {
					platformsRendered++;
					int levelAux = (platformsRendered / 5) + 1;
					if (levelAux <= 100 && levelAux > level) { // level up
						level = levelAux;
						if (level < 100)
							cout << "Ati ajuns la nivelul: " << level << endl;
						else
							cout << "Felicitari! Ati ajuns la ultimul nivel: " << level << endl;
						minSpeed = 3.5f + (level - 1) / 100.0f * 8.5f;
					}
				}
				platforms[i].type = purple;
			}
			if (validPlatform[i]) {
				if (platforms[i].nearZ - platforms[i].length > 3) {
					validPlatform[i] = false;
					touched[i] = false;
				}
			}
		}
	}
	if (!fallen && !underPlatform && posY == 0) {
		fallen = true;
		gameLost = true;
		lives = 0;

		for (int i = 0; i < totalPlatforms; i++) { // looking for the collision platform
			if (validPlatform[i] && platforms[i].nearZ > 0.0f && platforms[i].nearZ - platforms[i].length < 0.75f) {
				float platformX1 = platforms[i].column - 4.0f; // left platform X
				float platformX2 = platformX1 + 1; // right platform Y

				float posX1 = posX - 0.375;
				float posX2 = posX + 0.375;

				if (posX1 > platformX1 && posX1 < platformX2 && posX2 - 0.375f > platformX2) { // platform to the left
					goRight = true;
					break;
				}
				if (posX2 > platformX1 && posX2 < platformX2 && posX1 + 0.375 < platformX1) { // platform to the right
					goLeft = true;
					break;
				}
				if (platforms[i].nearZ < 0.375f) { // platform in front of the sphere
					goBack = true;
					forwardSpeed = platformSpeed + 5.0f;
					break;
				}
				float farZ = platforms[i].nearZ - platforms[i].length;
				if (farZ > 0.0f && farZ < 0.75) { // platform behind the sphere
					goForward = true;
					break;
				}
			}
		}

	}

	if (goLeft && sideSpeed > 0.0f) {
		sideSpeed -= 15.0f * deltaTimeSeconds;
		sideDist -= deltaTimeSeconds * sideSpeed;
	}
	if (goRight && sideSpeed > 0.0f) {
		sideSpeed -= 15.0f * deltaTimeSeconds;
		sideDist += deltaTimeSeconds * sideSpeed;
	}
	if (goForward && sideSpeed > 0.0f) {
		forwardSpeed -= 15.0f * deltaTimeSeconds;
		forwardDist -= deltaTimeSeconds * forwardSpeed;
	}
	if (goBack && sideSpeed > 0.0f) {
		forwardSpeed -= 15.0f * deltaTimeSeconds;
		forwardDist += deltaTimeSeconds * forwardSpeed;
	}

}

void Tema2::FrameEnd()
{
	//DrawCoordinatSystem();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 colorMatrix, bool is2D)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// TODO: Set eye position (camera position) uniform
	/*glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	GLint eye_location = glGetUniformLocation(shader->program, "eye_position");
	glUniform3fv(eye_location, 1, glm::value_ptr(eyePosition));*/

	int position = glGetUniformLocation(shader->program, "Time");
	glUniform1f(position, max(0.3f, 1.0f * abs(cos(Engine::GetElapsedTime() * 3.0f))));

	// bind color matrix
	GLint loc_color_matrix = glGetUniformLocation(shader->program, "Colors");
	glUniform3fv(loc_color_matrix, 1, glm::value_ptr(colorMatrix));

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));


	// Bind view matrix
	glm::mat4 viewMatrix;
	if (!is2D)
		viewMatrix = GetSceneCamera()->GetViewMatrix();
	else
		viewMatrix = glm::mat4(1);
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);

}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !fallen && !gameLost) {
		//go left
		if (window->KeyHold(GLFW_KEY_A)) {
			if (posX > -3.95f)
				posX -= deltaTime * 4.5f;
		}
		// go right
		else if (window->KeyHold(GLFW_KEY_D)) {
			if (posX < 3.95f)
				posX += deltaTime * 4.5f;
		}
		if (window->KeyHold(GLFW_KEY_W)) { // increase sphere speed
			if (platformSpeed < maxSpeed && !speedLock)
				platformSpeed += deltaTime * 10;
		} else if (window->KeyHold(GLFW_KEY_S)) { // decrease sphere speed
			if (platformSpeed > minSpeed && !speedLock)
				platformSpeed -= deltaTime * 10;
		}
	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_C && !gameLost && !touchedRed)
	{
		thirdPerson = !thirdPerson;
		if (thirdPerson) {
			GetSceneCamera()->RotateOX(-120.0f);
		}
		else {
			GetSceneCamera()->RotateOX(120.0f);
		}
	}
	// sphere jumping
	if (key == GLFW_KEY_SPACE && !gameLost && !touchedRed) {
		if (!falling && !jumping) {
			jumping = true;
			lastGroundY = posY;
			gAcc = 5.0f;
		}
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
