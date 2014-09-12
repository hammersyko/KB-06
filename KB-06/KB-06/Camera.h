#ifndef __SCENE_CAMERA_H__
#define __SCENE_CAMERA_H__

#define SPEED 0.5f

#include "Entity.h"

namespace Scene{
	class Camera : public Entity {
	public:
		Camera();
		~Camera();
		void Update();
		void MultiplyMatrices();
		void UseKeyboardInput();
		void UseMouseInput();
		void SetPosition(float x, float y, float z);
		void SetRotation(float yaw, float pitch, float roll);
		void SetScale(float scaleX, float scaleY, float scaleZ);
		Resource::Normal GetPosition();
		Resource::Normal GetRotation();
		Resource::Normal GetScale();
	private:
		void Move();

		float xMovement, yMovement, zMovement, yawMovement, pitchMovement, rollMovement, speed;

		bool moveForward, moveBackward, moveLeft, moveRight, moveUp, moveDown, speedUp;
	};
}

#endif
