#include "SceneInput2.h"
#include <iostream>

#include <include/gl.h>
#include <Core/Window/WindowObject.h>

#include "SimpleScene2.h"

#include <Component/Transform/Transform.h>

using namespace std;

SceneInput2::SceneInput2(SimpleScene2 *scene)
{
	this->scene = scene;
}

void SceneInput2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_F3)
		scene->drawGroundPlane = !scene->drawGroundPlane;

	if (key == GLFW_KEY_F5)
		scene->ReloadShaders();

	if (key == GLFW_KEY_ESCAPE)
		scene->Exit();
}