#include "MeshWrapper.h"

namespace pengine
{
	MeshWrapper::MeshWrapper(LPD3DXMESH* _mesh)
	{
		mesh = _mesh;
	}

	MeshWrapper::~MeshWrapper()
	{
		delete mesh;
	}

	void MeshWrapper::SetMesh(LPD3DXMESH* _mesh)
	{
		mesh = _mesh;
	}

	LPD3DXMESH* MeshWrapper::GetMesh()
	{
		return mesh;
	}
}