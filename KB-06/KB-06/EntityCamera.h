#ifndef _PENGINE_ENTITYCAMERA_H_
#define _PENGINE_ENTITYCAMERA_H_

#include "Entity.h"
#include "Plane.h"
#include "Matrix.h"

namespace pengine
{
	class EntityCamera : public Entity
	{
	public:
		EntityCamera();
		~EntityCamera();
		void UpdateLogic(float deltaTime, std::map<Input, long>* actions);
		void CacheToRenderer(Renderer* renderer);
		void Render(Renderer* renderer);
		Vector3* GetPosition();
		Vector3* GetLookAtPosition();
		CameraData GetCameraData();

		/*!
		Sets the position to look at with the camera
		*/
		void SetLookAtPosition(float x, float y, float z, float rollDegrees);
		void SetLookAtEntity(Entity* entity);
		void SetThirdPersonEntity(Entity* entity, float distance, float height);
		void SetRotation(float yawDegrees, float pitchDegrees, float rollDegrees);

		bool useInput = false;

		bool SphereInFrustum(Vector3* position, float radius);
		void SetProjectionMatrix(float fovY, float aspectRatio, float nearClippingPlane, float farClippingPlane);
		Matrix* GetProjectionMatrix();
		Matrix* GetViewMatrix();
	protected:
		Vector3 lookAtPosition;

	private:
		void BuildViewFrustum();
		CameraData cameraData;
		Matrix* viewMatrix;
		Vector3* upVec;
		float rollDegrees;
		Plane frustrumPlane[6];
		Matrix projectionMatrix;
		Vector3* lastKnownRotation;
		float cameraSpeed;
	};
}
#endif