#ifndef _PENGINE_PENGINE_H_
#define _PENGINE_PENGINE_H_

#include "SceneManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "WindowManager.h"

/*
Kernel
*/

namespace pengine
{
	class PEngine
	{
	public:
		PEngine();
		~PEngine();

		WindowManager* GetWindowManager();
		ResourceManager* GetResourceManager();
		SceneManager* GetSceneManager();
		InputManager* GetInputManager();

		Renderer* GetRenderer();
		void Init();

		void NewWindow(int x, int y, int width, int height);
		void AddSceneFactory(char* key, SceneFactory* sceneFactory);

		void InitRenderer();
		Scene* CreateScene(char* sceneFactory, std::string* filePath);
		Scene* AddScene(char* sceneFactory);
		void SetCurrentScene(Scene* scene);
		void GameLoop();

	private:

		SceneManager* sceneManager;
		WindowManager* windowManager;
		ResourceManager* resourceManager;
		InputManager* inputManager;
		Renderer* renderer;
		Logger* logger;
	};
}

#endif