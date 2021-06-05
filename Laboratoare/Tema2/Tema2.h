#pragma once
#include <Component2/SimpleScene2.h>
#include <time.h>

#define red 1
#define yellow 2
#define orange 3
#define green 4
#define purple 5
#define blue 6
#define brown 7
#define lightBlue 8
#define redVec glm::vec3(0.95f, 0.18f, 0.1f)
#define yellowVec glm::vec3(0.85f, 0.8f, 0.05f)
#define orangeVec glm::vec3(0.88f, 0.4f, 0.04f)
#define greenVec glm::vec3(0.05f, 0.82f, 0.12f)
#define purpleVec glm::vec3(0.64f, 0.08f, 0.79f)
#define blueVec glm::vec3(0.05f, 0.05f, 0.87f)
#define turquoiseVec glm::vec3(0.0f, 0.63f, 0.63f)
#define darkRedVec glm::vec3(0.5f, 0.047f, 0.17f)
#define brownVec glm::vec3(0.5f, 0.15f, 0.0f)
#define lightBlueVec glm::vec3(0.0f, 0.67f, 0.62f)

class Tema2 : public SimpleScene2
{
public:
	Tema2();
	~Tema2();

	void Init() override;

	//Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);

private:
	typedef struct platform {
		int type; // color
		int column; // between 0 and 7
		float length; // between 10 and 25
		float nearZ; // nearest platform z to the sphere
		float distance = 0.0f;
	} platform;


	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void getSphereX();
	bool PlatformUnderSphere(platform p);
	void GenerateFirstPlatforms();
	void GeneratePlatforms();
	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 colorMatrix, bool is2D);
	float RandomFloat(float a, float b);
	int RandomColor(int noPlatforms);
	int RandomColumn(bool columns[]);
	bool OkDistance(platform newPlatforms[], int noPlatforms);
	glm::vec3 GetColorMatrix(int color);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	bool thirdPerson = true;

	float posX = 0.0f; // sphere x
	float posY = 0.0f; // sphere y
	float lastGroundY = 0.0f; // last y before sphere jump
	bool jumping = false; // if the sphere is currently going up
	bool falling = false; // if the sphere is currently falling down
	float gAcc = 0.0f;
	bool goLeft = false;
	bool goRight = false;
	bool goBack = false;
	bool goForward = false;

	float rotation = 0.0f;
	bool specialActivated = false;
	clock_t timerSpecial;

	bool traps[100] = {false};

	float fuel = 100.0f;
	int lives = 4;
	int level = 1;
	int platformsRendered = 0;
	bool touched[100] = { false };
	bool fallen = false;
	float forwardSpeed = 5.0f;
	float sideSpeed = 5.0f;
	float forwardDist = 0.0f;
	float sideDist = 0.0f;

	float platformLengthMin = 10.0f;
	float platformLengthMax = 25.0f;
	float lastPlatformEnd;
	float platformSpeed = 3.5f;
	float minSpeed = 3.5f;
	float maxSpeed = 12.0f;
	int totalPlatforms = 0;
	bool firstPlatform = false;
	bool firstLastColumns = false;
	float renderDistance = -70.0f;
	float lastNearZ = 100.0f; // the minimum one within renderDistance value
	float lastZ = -1.0f;

	clock_t timerOrange;
	bool speedLock = false;
	float lastSpeed = 0.0f;
	bool gameLost = false;
	bool touchedRed = false;

	platform platforms[100];
	bool validPlatform[100] = { false };
	int* lastColumns = NULL;
	int lastPlatformIndexes[8];

};
