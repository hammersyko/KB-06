#ifndef _PENGINE_DIRECTXRENDERER_H_
#define _PENGINE_DIRECTXRENDERER_H_

//#include <d2d1.h>
#include <d3dx9.h>
#include <vector>
#include "Renderer.h"
//#include <wincodec.h>

namespace pengine
{
	class DirectXRenderer : public Renderer
	{
	public:
		DirectXRenderer();
		~DirectXRenderer();

		void CreateD2DFactory();
		void CreateRenderTarget(HWND hWnd);
		void CreateWICImagingFactory();
		void CreateDecoder(std::string path);
		void GetBitmapFrame();
		void InitializeBMP();
		void CreateFormatConverter();
		void CreateBitmapFromWIC();
		void D2DDraw();

		void InitD3D(HWND hWnd);
		void SetDefaultRenderStates();

		void SetCulling(CULLINGTYPE);
		void SetZBuffer(bool);
		void SetFillMode(FILLMODE);

		void SetActiveCamera(CameraData camera, bool orthographic);

		void SetProjectionMatrix(Matrix* ProjectionMatrix);
		void SetViewMatrix(Matrix*);
		void SetProjectionMatrix(float FOV, float farClippingPlane);

		void BeginScene();
		void ClearScene(PENGINEDWORD* count, PENGINEDWORD* flags, PENGINECOLOR* color, float z, PENGINEDWORD* stencil);
		void ClearScene(unsigned long count, unsigned long flags, RGBAColor color, float z, unsigned long stencil);
		void PresentScene(HWND hWnd);
		void EndScene();

		void SetMaterialWrapper(MaterialWrapper* wrapper);
		void SetMaterial(Material* material);
		void SetTexture(TextureWrapper* wrapper);
		void SetFvF(PENGINEDWORD* fvf);

		void DrawSubset(MeshWrapper* wrapper, int subset);

		LPDIRECT3DDEVICE9* GetDevice();
		void SetActiveMatrix(Matrix* matrix);

		void SetLights();

		VertexBufferWrapper* CreateVertexBuffer(D3DCustomVertex*, int amountOfVertices, int fvf);
		IndexBufferWrapper* CreateIndexBuffer(int* indices, int amountOfIndices);
		void DrawVertexBuffer(VertexBufferWrapper*, int amountOfVertices);
		void DrawIndexedVertexBuffer(VertexBufferWrapper*, IndexBufferWrapper*, int amountOfIndices, int amountOfFaces);

		void ActivateRenderingToTexture(int textureIndex, int tWidth, int tHeight, RGBAColor bgColor);
		void DeactivateRenderingToTexture(int textureIndex);
		void SetTextureToRenderedTexture(int textureIndex);

		inline void DrawTextString(int x, int y, DWORD color, std::string str)
		{
			HRESULT r = 0;
			LPD3DXFONT pFont = NULL;

			// Create the D3DX Font
			r = D3DXCreateFont(g_pd3dDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &pFont);

			if (FAILED(r))
			{
				//Do Debugging
			}

			RECT FontPos = { x, y, x + 120, y + 16 };
			pFont->DrawText(NULL, ConvertCharArrayToLPCWSTR(str.c_str()), -1, &FontPos, DT_NOCLIP, color);
			pFont->Release();
		}

		LPCWSTR ConvertCharArrayToLPCWSTR(const char* charArray)
		{
			wchar_t filename[4096] = { 0 };
			MultiByteToWideChar(0, 0, charArray, strlen(charArray), filename, strlen(charArray));
			return filename;
		}

	private:
		void SetMatrixCache(Matrix* matrix);

		// Have yet to figure out how to get rid of this.
		// Basically it's a copied pointer to the device's back buffer
		LPDIRECT3DSURFACE9 MainSurface;

		// And here we have the whole point of this file: The render surface and associated texture
		std::vector<LPDIRECT3DSURFACE9> RenderSurfaces;
		std::vector<LPDIRECT3DTEXTURE9> RenderTextures;

		RECT rectangle;

		//IWICBitmapDecoder* iwicBmpDecoder;
		//IWICImagingFactory* iwicFactory;
		//IWICBitmapFrameDecode *bitmapFrame;
		//IWICFormatConverter* iwicFormatConverter;
		/*ID2D1Bitmap* d2dBmp;

		ID2D1Factory* d2dFactory;
		ID2D1HwndRenderTarget* d2dRenderTarget;*/

		LPDIRECT3DTEXTURE9 surfaceTexture;
		IDirect3DSurface9* d3dSurface;
		IDirect3DSurface9* surfaceLevel;
		IDirect3DSurface9* backbuffer;

		LPDIRECT3D9 g_pD3D;
		LPDIRECT3DDEVICE9 g_pd3dDevice;

		std::map<BinaryData*, LPDIRECT3DTEXTURE9> textureCache;

		D3DXMATRIX* matrixCache;
		D3DXMATRIXA16 projectionMatix;

		D3DXPLANE   frustrumPlane[6];
	};
}
#endif