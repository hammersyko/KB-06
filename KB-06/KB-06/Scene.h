#ifndef __SCENE_SCENE_H__
#define __SCENE_SCENE_H__

#include <list>
#include "Entity.h"
#include "EntityCamera.h"
#include "Renderer.h"
#include "LoggerPool.h"

namespace Scene 
{
	class Scene 
	{
	public:
		Scene();
		~Scene();
		virtual void Update();
		void AddEntity(Entity* entity);
		void Render(Renderer::Renderer* renderer);

		void GetCurrentCamera();
		//void SetCurrentCamera(EntityCamera camera);
	protected:
		std::list<Entity*> entities;
		std::list<EntityCamera*> cameras;
		EntityCamera* currentCamera;
		Logger::Logger* logger;
	};
}
#endif
