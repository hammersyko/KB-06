#ifndef _DIRECTXRENDERER_H_
#define _DIRECTXRENDERER_H_

#include <d3dx9.h>

class DirectXRenderer
{
public:
	DirectXRenderer();
	~DirectXRenderer();

	void InitD3D();
	void SetRenderState();

	void SetWorldMatrix();
	void SetViewMatrix();
	void SetProjectionMatrix();

	void BeginScene();
	void ClearScene();
	void PresentScene();
	void StopScene();

	void CreateVertexBuffer();
	void CreateIndexBuffer();

	void SetMaterial();
	void SetTexture();
	void SetFvF();
	void SetTransform();

	void DrawPrimitive();
	void DrawSubset();

	void SetStreamSource(); //??
	void SetIndices(); //??

	void GetDevice();

private:
	LPDIRECT3D9 g_pD3D;
	LPDIRECT3DDEVICE9 g_pd3dDevice;
};

#endif