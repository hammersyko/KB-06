#ifndef _TRACKBLOCK_H_
#define _TRACKBLOCK_H_

#include "Renderer.h"
#include "Entity.h"
#include "Logger.h"
#include "Object3D.h"

enum Direction {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};


class TrackBlock : public pengine::Entity
{
public:
	static enum TYPE {
		STRAIGHT,
		TURN_LEFT,
		TURN_RIGHT
	};

	TrackBlock(float x, float y, float z, float yaw, TYPE type, Direction direction, pengine::Object3D* model);
	~TrackBlock();

	void Render(pengine::Renderer* renderer);
	void SetXModel(pengine::Object3D*); //debug
	
	//float lenght;
	//float width;
	Direction GetDirection();

	void SetPosition(float x, float y, float z);
	void SetPositionOffset(float x, float y, float z);
	pengine::Vector3* GetPositionOffset();
	void SetRotation(float yaw, float pitch, float roll);
	void SetRotationOffset(float yaw, float pitch, float roll);
	pengine::Vector3* GetRotationOffset();
	
	float GetRadius();

	void CacheToRenderer(pengine::Renderer* renderer);
private:
	pengine::Vector3 positionOffset;
	pengine::Vector3 rotationOffset;
	TYPE type;
	Direction direction;
	pengine::Object3D* xModel; //debug
};
#endif
