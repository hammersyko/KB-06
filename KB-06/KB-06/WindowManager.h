#ifndef _WINDOW_WINDOWMANAGER_H_
#define _WINDOW_WINDOWMANAGER_H_

#include "Window.h"
#include "WindowListener.h"
#include "Renderer.h"
#include "SceneManager.h"

#include <Windows.h>
#include <vector>

namespace Window
{

	class WindowManager
	{
	public:
		/*!
		Constructor (creates WindowManager).

		@param sceneManager: SceneManager which is used to render a scene.
		*/
		WindowManager(Scene::SceneManager *sceneManager); // Manager shouln't use another manager
		WindowManager();
		~WindowManager();

		/*!
		Creates a new window and adds it to a list of windows.

		@param *renderer: A pointer to the renderer is passed to render scenes.
		@param x: The horizontal position of the upper left corner of the window.
		@param y: The vertical position of the upper left corner of the window.
		@param width: The width of the window.
		@param height: The height of the window.
		*/
		HWND NewWindow(int x, int y, int width, int height);

		/*!
		Loops through the list of windows to update them.
		*/
		void UpdateWindows();

		/*!
		Checks if there is an active window.
		*/
		bool HasActiveWindow();

		/*!
		Gives back the last created window.

		@return Window*: A pointer to the last window.
		*/
		Window* GetLastWindow();

		Window* GetWindowByHWND(HWND hwnd);
		std::vector<Window*>* GetAllWindows();

		void AddWindowListener(WindowListener* p_windowListener);
		void RemoveWindowListener(WindowListener* p_windowListener);
		void ClearWindowListeners();

	private:
		std::vector<Window*> windows;
		Scene::SceneManager *sceneManager;

		Logger::Logger* logger;
		std::list<WindowListener*> windowListeners;
	};
}
#endif
