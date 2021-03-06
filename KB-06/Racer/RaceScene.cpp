#include "RaceScene.h"


namespace racer
{
	RaceScene::RaceScene()
	{
		elapsedTime = 0.0f;
		win = false;
		loss = false;
	}

	RaceScene::~RaceScene()
	{

	}

	void RaceScene::Update(float deltaTime, std::map<pengine::Input, long>* actions)
	{
		if (!win)
		{
			elapsedTime += deltaTime;
		}

		Scene::Update(deltaTime, actions);
		GetCurrentCamera()->SetThirdPersonEntity(raceCart, 75.0f, 30.0f);

		if (raceCart->GetCheckPoints()->size() == 0)
		{
			win = true;
		}
	}

	void RaceScene::SetRaceCart(RaceCart* entity)
	{
		raceCart = entity;
	}

	void RaceScene::CacheToRenderer(pengine::Renderer* renderer)
	{
		Scene::CacheToRenderer(renderer);
		if (track != NULL)
		{
			for each (TrackBlock* tr in track->GetTrackBlocks())
			{
				if (currentCamera->SphereInFrustum(tr->GetPosition(), tr->GetRadius()))
				{
					tr->CacheToRenderer(renderer);
				}
			}
		}
	}

	void RaceScene::RenderToTexture(int texture, pengine::Renderer* renderer)
	{
		//pengine::EntityCamera* aCamera = new pengine::EntityCamera();
		//aCamera->SetPosition(0.0f, 0.0f, 2.0f);
		//aCamera->SetLookAtPosition(0.0f, 0.0f, 0.0f, 0);

		//renderer->SetViewMatrix(aCamera->GetViewMatrix());
		//pengine::Matrix ortho;
		//pengine::Matrix::CreateOrthographicMatrix(50.0f, 50.0f, 1.0f, 10.0f, &ortho);
		//renderer->SetProjectionMatrix(&ortho);

		//pengine::Matrix aMatrix;
		//pengine::Matrix::CreateMatrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 1.0f, &aMatrix);
		//renderer->SetActiveMatrix(&aMatrix);
		//ColoredVertex vertices[] = {
		//	ColoredVertex(-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 0.0f, 0.0f),
		//	ColoredVertex(0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 255, 0), 1.0f, 0.0f),
		//	ColoredVertex(0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 0, 255), 0.5f, 1.0f)
		//};//holds a triangle that we will render to the texture
		//pengine::VertexBufferWrapper* wrapper = renderer->CreateColoredVertexBuffer(vertices, 3);
		//pengine::Material mat;
		//mat.texture = NULL;
		//mat.ambient = { 1.0f, 1.0f, 1.0f };
		//mat.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		//mat.emissive = { 1.0f, 1.0f, 1.0f };
		//mat.specular = { 1.0f, 1.0f, 1.0f };
		//mat.power = 10.0f;
		//renderer->SetMaterial(&mat);
		//renderer->DrawVertexBuffer(wrapper);//draw a triangle to the texture


		//pengine::Matrix::CreateMatrix(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 3.0f, 3.0f, 1.0f, &aMatrix);
		//renderer->SetActiveMatrix(&aMatrix);
		//renderer->DrawString(std::to_string(elapsedTime), D3DCOLOR_ARGB(255, 255, 255, 0));

		//delete aCamera;
		//delete wrapper;

		/*int count = 0;
		auto i = collidables.begin();

		pengine::Collidable* c = *i;
		D3DCustomColoredVertex verts[] = {
		{ c->collisionBox.backBottomLeft.x, c->collisionBox.backBottomLeft.z, 1.0f, D3DCOLOR_ARGB(255, 255 * count, 0, 0), 0.0f, 1.0f },
		{ c->collisionBox.backBottomRight.x, c->collisionBox.backBottomRight.z, 1.0f, D3DCOLOR_ARGB(255, 255 * count, 0, 0), 1.0f, 0.0f },
		{ c->collisionBox.frontBottomRight.x, c->collisionBox.frontBottomRight.z, 1.0f, D3DCOLOR_ARGB(255, 255 * count, 0, 0), 0.0f, 0.0f },

		{ c->collisionBox.frontBottomRight.x, c->collisionBox.frontBottomRight.z, 1.0f, D3DCOLOR_ARGB(255, 255 * count, 0, 0), 0.0f, 1.0f },
		{ c->collisionBox.frontBottomLeft.x, c->collisionBox.frontBottomLeft.z, 1.0f, D3DCOLOR_ARGB(255, 255 * count, 0, 0), 1.0f, 1.0f },
		{ c->collisionBox.backBottomLeft.x, c->collisionBox.backBottomLeft.z, 1.0f, D3DCOLOR_ARGB(255, 255 * count, 0, 0), 1.0f, 0.0f },

		{ c->collisionBox2.backBottomLeft.x, c->collisionBox2.backBottomLeft.z, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 0.0f, 1.0f },
		{ c->collisionBox2.backBottomRight.x, c->collisionBox2.backBottomRight.z, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 0.0f },
		{ c->collisionBox2.frontBottomRight.x, c->collisionBox2.frontBottomRight.z, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 0.0f, 0.0f },

		{ c->collisionBox2.frontBottomRight.x, c->collisionBox2.frontBottomRight.z, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 0.0f, 1.0f },
		{ c->collisionBox2.frontBottomLeft.x, c->collisionBox2.frontBottomLeft.z, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 1.0f },
		{ c->collisionBox2.backBottomLeft.x, c->collisionBox2.backBottomLeft.z, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 0.0f }
		};

		pengine::VertexBufferWrapper* wrapper2 = renderer->CreateColoredVertexBuffer(verts, 12);
		pengine::Material mat2;
		mat.texture = NULL;
		mat.ambient = { 1.0f, 1.0f, 1.0f };
		mat.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		mat.emissive = { 1.0f, 1.0f, 1.0f };
		mat.specular = { 1.0f, 1.0f, 1.0f };
		mat.power = 10.0f;
		renderer->SetMaterial(&mat2);
		renderer->DrawVertexBuffer(wrapper2, 12);

		++count;*/
	}

	void RaceScene::Render(pengine::Renderer* renderer)
	{
		Scene::Render(renderer);

		pengine::Matrix aMatrix;

		pengine::Matrix::CreateMatrix(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 3.0f, 3.0f, 1.0f, &aMatrix);
		renderer->SetActiveMatrix(&aMatrix);

		pengine::Matrix aMatrix2;
		pengine::Matrix aMatrix3;
		pengine::Matrix::CreateMatrix(0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, &aMatrix2);
		pengine::Matrix::CreateMatrix(currentCamera->GetLookAtPosition()->x, currentCamera->GetLookAtPosition()->y, currentCamera->GetLookAtPosition()->z, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 2.0f, &aMatrix3);
		pengine::Matrix::CreateObjectSpaceLookAtMatrix(currentCamera->GetPosition(), currentCamera->GetLookAtPosition(), &aMatrix);
		aMatrix = aMatrix2 * aMatrix * aMatrix3;
		renderer->SetActiveMatrix(&aMatrix);

		if (!win)
		{
			int r = int(1 * (elapsedTime / 30 * 255));
			if (r >= 255)
			{
				r = 255;
			}
			renderer->DrawString(std::to_string(elapsedTime), D3DCOLOR_ARGB(0, r, 100, 0));
		}
		else
		{
			renderer->DrawString("Victory!\n" + std::to_string(elapsedTime), D3DCOLOR_ARGB(0, 100, 255, 100));
		}

		if (track != NULL)
		{
			//unsigned int counter = 0;
			for each (TrackBlock* tr in track->GetTrackBlocks())
			{
				if (currentCamera->SphereInFrustum(tr->GetPosition(), tr->GetRadius()))
				{
					tr->Render(renderer);
					//counter++;
				}
			}
			//logger->Log(pengine::Logger::DEBUG, "RaceScene rendered " + std::to_string(counter) + " track blocks!");
		}

		renderer->SetShader(shader);
		pengine::Matrix mWorld;
		pengine::Matrix::CreateMatrix(-100, 140, 0, -90, 0, 0, 10, 10, 10, &mWorld);
		pengine::Matrix mProj = *GetCurrentCamera()->GetProjectionMatrix();
		pengine::Matrix mView = *GetCurrentCamera()->GetViewMatrix();
		pengine::Matrix mWorldViewProjection = mWorld * mView * mProj;

		renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorldViewProjection"), &mWorldViewProjection);
		renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorld"), &mWorld);

		pengine::RGBAColor lowGrey = pengine::RGBAColor(0.1f, 0.1f, 0.1f, 1.0f);
		renderer->SetShaderValue(renderer->GetShaderParameterHandle("g_MaterialAmbientColor"), &lowGrey, sizeof(pengine::RGBAColor));

		pengine::RGBAColor white = pengine::RGBAColor(1.0f, 1.0f, 1.0f, 1.0f);
		renderer->SetShaderValue(renderer->GetShaderParameterHandle("g_LightDiffuse"), &white, sizeof(pengine::RGBAColor));
		renderer->SetShaderValue(renderer->GetShaderParameterHandle("g_MaterialDiffuseColor"), &white, sizeof(pengine::RGBAColor));

		pengine::Vector3 lightDir = pengine::Vector3(0, 100, 100);
		renderer->SetShaderValue(renderer->GetShaderParameterHandle("g_LightDir"), &lightDir, sizeof(pengine::Vector3));

		renderer->SetShaderTechnique(renderer->GetShaderTechniqueHandle("RenderScene"));

		UINT cPasses, iPass;
		renderer->BeginRenderingWithShader(&cPasses);

		for (iPass = 0; iPass < cPasses; iPass++)
		{
			renderer->BeginRenderingWithPass(iPass);
			//do all them renderings... tricky
			/*pengine::Matrix::CreateMatrix(-100, 140, 0, -90, 0, 0, 10, 10, 10, &mWorld);
			mWorldViewProjection = mWorld * mView * mProj;
			renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorldViewProjection"), &mWorldViewProjection);
			renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorld"), &mWorld);
			renderer->CommitChanges();
			renderer->DrawString("Hello shaders!", D3DCOLOR_ARGB(0, 255, 0, 0));*/
			pengine::Matrix::CreateMatrix(50, 150, -150, 0, 270, 0, 0.2f, 0.2f, 0.2f, &mWorld);
			mWorldViewProjection = mWorld * mView * mProj;
			renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorldViewProjection"), &mWorldViewProjection);
			renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorld"), &mWorld);

			renderer->CommitChanges();

			for each(pengine::Entity* entity in entities)
			{
				entity->Render(renderer);
			}

			renderer->EndRenderingPass();
		}
		renderer->EndRenderingWithShader();

		renderer->SetShader(textShader);

		pengine::Matrix::CreateMatrix(-100, 140, 0, -90, 0, 0, 10, 10, 10, &mWorld);
		mWorldViewProjection = mWorld * mView * mProj;
		renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorldViewProjection"), &mWorldViewProjection);
		renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorld"), &mWorld);
		renderer->SetShaderValue(renderer->GetShaderParameterHandle("g_fTime"), &elapsedTime, sizeof(float));
		renderer->SetShaderTechnique(renderer->GetShaderTechniqueHandle("RenderScene"));

		renderer->BeginRenderingWithShader(&cPasses);

		for (iPass = 0; iPass < cPasses; iPass++)
		{
			renderer->BeginRenderingWithPass(iPass);
			//renderer->DrawString("Hello shaders!\nI am going to try breaking\nyou like a big jerk!\nDoesn't seem to be\nhappening though...", D3DCOLOR_ARGB(0, 0, 0, 0));
			renderer->EndRenderingPass();
		}
		renderer->EndRenderingWithShader();


		renderer->SetShader(flagShader);

		pengine::Matrix::CreateMatrix(-100, 140, 0, -90, 0, 0, 10, 10, 10, &mWorld);
		mWorldViewProjection = mWorld * mView * mProj;
		renderer->SetShaderMatrix(renderer->GetShaderParameterHandle("g_mWorldViewProjection"), &mWorldViewProjection);
		renderer->SetShaderValue(renderer->GetShaderParameterHandle("g_fTime"), &elapsedTime, sizeof(float));
		renderer->SetShaderTechnique(renderer->GetShaderTechniqueHandle("RenderScene"));

		renderer->BeginRenderingWithShader(&cPasses);

		for (iPass = 0; iPass < cPasses; iPass++)
		{
			renderer->BeginRenderingWithPass(iPass);
			renderer->DrawString("Hello shaders!", D3DCOLOR_ARGB(1, 1, 0, 1));
			renderer->EndRenderingPass();
		}
		renderer->EndRenderingWithShader();

		//pengine::Material mat;
		//mat.texture = NULL;
		//mat.ambient = { 0.0f, 0.0f, 0.0f };
		//mat.diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
		//mat.emissive = { 0.0f, 0.0f, 0.0f };
		//mat.specular = { 0.0f, 0.0f, 0.0f };
		//mat.power = 10.0f;

		//renderer->SetMaterial(&mat);
		//renderer->SetTextureToRenderedTexture(0);

		//Vertex vertices[] = {
		//	Vertex(2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),//bl
		//	Vertex(-2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f),//tr
		//	Vertex(2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f),//tl

		//	Vertex(2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),//bl
		//	Vertex(-2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f),//br
		//	Vertex(-2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f)//tr
		//};//holds a square that we will render with the texture, so we can see the contents of the texture
		//pengine::VertexBufferWrapper* wrapper = renderer->CreateVertexBuffer(vertices, 6);
		//renderer->DrawVertexBuffer(wrapper);//draw the square

		//delete wrapper;
	}

	void RaceScene::SetTrack(Track* _track)
	{
		track = _track;
	}
}