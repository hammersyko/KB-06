#include "TrackBlock.h"

TrackBlock::TrackBlock(float x, float y, float z, float yaw, TYPE trackType, Direction trackdirection, pengine::Object3D* model)
{
	SetPositionOffset(x, y, z);
	SetRotationOffset(yaw, 0, 0);
	type = trackType;
	direction = trackdirection;
	xModel = model;
}

TrackBlock::~TrackBlock()
{
}

void TrackBlock::Draw(pengine::Renderer* renderer)
{
	if (xModel != NULL)
	{
		renderer->SetActiveMatrix(myCachedMatrix); //should be called every frame

		xModel->ClearSkinnedVertices();
		//xModel->UpdateAnimation();
		xModel->Draw(renderer);
	}
}

void TrackBlock::SetXModel(pengine::Object3D* p_xModel)
{
	xModel = p_xModel;
}
Direction TrackBlock::GetDirection(){
	return direction;
}

void TrackBlock::SetPosition(float x, float y, float z)
{
	Entity::SetPosition(x + positionOffset.x * scale.x, y + positionOffset.y * scale.y, z + positionOffset.z * scale.z);
}

void TrackBlock::SetPositionOffset(float x, float y, float z){
	positionOffset.x = x;
	positionOffset.y = y;
	positionOffset.z = z;
}

pengine::Vertex* TrackBlock::GetPositionOffset()
{
	return &positionOffset;
}

void TrackBlock::SetRotation(float yaw, float pitch, float roll)
{
	Entity::SetRotation(yaw + rotationOffset.x, pitch + rotationOffset.y, roll + rotationOffset.z);
}

void TrackBlock::SetRotationOffset(float yaw, float pitch, float roll)
{
	rotationOffset.x = yaw;
	rotationOffset.y = pitch;
	rotationOffset.z = roll;
}

pengine::Vertex* TrackBlock::GetRotationOffset(){
	return &rotationOffset;
}

float TrackBlock::GetRadius(){
	return 10.0f;
}