#pragma once

#include <Core/Window/InputController.h>

class SimpleScene2;

class SceneInput2 : public InputController
{
	public:
		SceneInput2(SimpleScene2 *camera);
		void OnKeyPress(int key, int mods) override;

	public:
		SimpleScene2 *scene;
};

