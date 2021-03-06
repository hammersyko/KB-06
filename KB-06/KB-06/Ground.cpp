#include "Ground.h"
#include "DirectXRenderer.h"
#include <vector>
#include "PengineDefinitions.h"
#include <math.h>
#include <array>

namespace pengine
{
	Ground::Ground()
	{
		logger = LoggerPool::GetInstance().GetLogger();

		location = new Matrix();
	}

	Ground::~Ground()
	{
		if (quadTreeRootNode != NULL)
		{
			delete quadTreeRootNode;
		}

		if (vertices != NULL)
		{
			delete vertices;
		}

		if (vertexBuffer != NULL)
		{
			delete vertexBuffer;
		}

		if (material != NULL)
		{
			delete material;
		}

		if (location != NULL)
		{
			delete location;
		}
	}

	void Ground::SetPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
		location->CreateMatrix(position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z, location);
	}

	void Ground::SetRotation(float yaw, float pitch, float roll)
	{
		rotation.x = yaw;
		rotation.y = pitch;
		rotation.z = roll;
		location->CreateMatrix(position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z, location);
	}

	void Ground::SetScale(float scaleX, float scaleY, float scaleZ)
	{
		scale.x = scaleX;
		scale.y = scaleY;
		scale.z = scaleZ;
		location->CreateMatrix(position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z, location);
	}

	void Ground::SetAll(float x, float y, float z, float yaw, float pitch, float roll, float scaleX, float scaleY, float scaleZ)
	{
		SetScale(scaleX, scaleY, scaleZ);
		SetPosition(x, y, z);
		SetRotation(yaw, pitch, roll);
	}

	void Ground::InitQuadTree(unsigned short depth)
	{
		quadTreeRootNode = CreateQuadTree(depth);
	}

	long Ground::GetHeight()
	{
		return height;
	}

	long Ground::GetWidth()
	{
		return width;
	}

	void Ground::SetHeight(long p_height)
	{
		height = p_height;
	}

	void Ground::SetWidth(long p_width)
	{
		width = p_width;
	}

	void Ground::SetCellSize(float p_cellSize)
	{
		cellSize = p_cellSize;
	}

	float Ground::GetCellSize()
	{
		return cellSize;
	}

	void Ground::SetAmountOfVertices(int p_amountOfVertices)
	{
		amountOfVertices = p_amountOfVertices;
	}

	int Ground::GetAmountOfVertices()
	{
		return amountOfVertices;
	}

	Vertex* Ground::GetVertices()
	{
		return vertices;
	}

	void Ground::SetVertices(Vertex* vertex)
	{
		vertices = vertex;
	}

	Material* Ground::GetMaterial()
	{
		return material;
	}

	void Ground::SetMaterial(Material* p_material)
	{
		material = p_material;
	}

	void Ground::CacheToRenderer(Renderer* renderer)
	{
		//Well, uh, nothing...
		//But there should be!
		//...Maybe...
	}

	void Ground::Render(Renderer* renderer)
	{
		int amountOfVertices;
		Vertex** vertices = quadTreeRootNode->GetAllChildrenVertices(amountOfVertices);

		if (vertexBuffer != NULL)
		{
			delete vertexBuffer;
		}

		// dereference all pointers :'(
		Vertex* verticesArray = new Vertex[amountOfVertices];
		for (int i = 0; i < amountOfVertices; ++i)
		{
			verticesArray[i] = *vertices[i];
		}

		vertexBuffer = renderer->CreateVertexBuffer(verticesArray, amountOfVertices);
		renderer->SetActiveMatrix(location);
		renderer->SetMaterial(material);
		renderer->DrawVertexBuffer(vertexBuffer);

		delete[] verticesArray;

		delete[] vertices;
	}

	QuadNode* Ground::CreateQuadTree(unsigned short depth)
	{
		// TODO implement errors in case of impossible depth

		quadTreeDepth = depth;

		// Find dimensions of the ground
		float dimensionWidth = 0.0f;
		float dimensionDepth = 0.0f;

		for (int i = 0; i < amountOfVertices; ++i)
		{
			Vertex* vertex = &vertices[i];
			if (vertex->x > dimensionWidth)
			{
				dimensionWidth = vertex->x;
			}
			if (vertex->z > dimensionDepth)
			{
				dimensionDepth = vertex->z;
			}
		}

		QuadNode* rootNode = new QuadNode();
		rootNode->SetMinX(0.0f);
		rootNode->SetMinZ(0.0f);
		rootNode->SetMaxX(dimensionWidth);
		rootNode->SetMaxZ(dimensionDepth);
		rootNode->SetIsLeaf(false);

		rootNode->SetWidth(this->width - 1);
		rootNode->SetDepth(this->height - 1);

		rootNode->name = "root";
		CreateQuadTreeChildren(rootNode, depth);

		// Calculate neighbors
		if (!rootNode->IsLeaf())
		{
			for (int i = 0; i < depth; ++i)
			{
				(*rootNode->GetChildren())[0]->CalculateNeighbors(i);
				(*rootNode->GetChildren())[1]->CalculateNeighbors(i);
				(*rootNode->GetChildren())[2]->CalculateNeighbors(i);
				(*rootNode->GetChildren())[3]->CalculateNeighbors(i);
			}
		}

		return rootNode;
	}

	void Ground::CreateQuadTreeChildren(QuadNode* parent, unsigned short remainingDepth)
	{
		if (remainingDepth > 0)
		{
			// This is a branch, create children and call this function 
			parent->SetIsLeaf(false);

			// 0 3
			// 1 2

			QuadNode* node0 = new QuadNode();
			QuadNode* node1 = new QuadNode();
			QuadNode* node2 = new QuadNode();
			QuadNode* node3 = new QuadNode();

			node0->name = parent->name + "->0";
			node1->name = parent->name + "->1";
			node2->name = parent->name + "->2";
			node3->name = parent->name + "->3";

			node0->SetParent(parent);
			node1->SetParent(parent);
			node2->SetParent(parent);
			node3->SetParent(parent);

			node0->SetMinX((parent->GetMaxX() - parent->GetMinX()) / 2 + parent->GetMinX());
			node0->SetMaxX(parent->GetMaxX());
			node0->SetMinZ(parent->GetMinZ());
			node0->SetMaxZ(parent->GetMinZ() + (parent->GetMaxZ() - parent->GetMinZ()) / 2);
			node0->SetLocation(0);

			node1->SetMinX(parent->GetMinX());
			node1->SetMaxX((parent->GetMaxX() - parent->GetMinX()) / 2 + parent->GetMinX());
			node1->SetMinZ(parent->GetMinZ());
			node1->SetMaxZ(parent->GetMinZ() + (parent->GetMaxZ() - parent->GetMinZ()) / 2);
			node1->SetLocation(1);

			node2->SetMinX(parent->GetMinX());
			node2->SetMaxX((parent->GetMaxX() - parent->GetMinX()) / 2 + parent->GetMinX());
			node2->SetMinZ(parent->GetMinZ() + (parent->GetMaxZ() - parent->GetMinZ()) / 2);
			node2->SetMaxZ(parent->GetMaxZ());
			node2->SetLocation(2);

			node3->SetMinX((parent->GetMaxX() - parent->GetMinX()) / 2 + parent->GetMinX());
			node3->SetMaxX(parent->GetMaxX());
			node3->SetMinZ(parent->GetMinZ() + (parent->GetMaxZ() - parent->GetMinZ()) / 2);
			node3->SetMaxZ(parent->GetMaxZ());
			node3->SetLocation(3);

			node0->SetWidth(parent->GetWidth() / 2);
			node0->SetDepth(parent->GetDepth() / 2);
			node1->SetWidth(parent->GetWidth() / 2);
			node1->SetDepth(parent->GetDepth() / 2);
			node2->SetWidth(parent->GetWidth() / 2);
			node2->SetDepth(parent->GetDepth() / 2);
			node3->SetWidth(parent->GetWidth() / 2);
			node3->SetDepth(parent->GetDepth() / 2);

			CreateQuadTreeChildren(node0, remainingDepth - 1);
			CreateQuadTreeChildren(node1, remainingDepth - 1);
			CreateQuadTreeChildren(node2, remainingDepth - 1);
			CreateQuadTreeChildren(node3, remainingDepth - 1);

			std::map<char, QuadNode*>* children = new std::map<char, QuadNode*>();
			(*children)[0] = node0;
			(*children)[1] = node1;
			(*children)[2] = node2;
			(*children)[3] = node3;

			parent->SetChildren(children);
		}
		else
		{
			// Add leaf data
			parent->SetIsLeaf(true);
			std::vector<Vertex*> leafVertices;

			// Add all triangles within the bounds, borders included
			for (int i = 0; i <= amountOfVertices - 3; i += 3)
			{
				Vertex* vertex0 = &vertices[i];
				Vertex* vertex1 = &vertices[i + 1];
				Vertex* vertex2 = &vertices[i + 2];

				// axis are inverted, the problem is probably somewhere else but I can't find it :)
				if ((vertex0->z >= parent->GetMinX() && vertex0->z <= parent->GetMaxX()
					&& vertex0->x >= parent->GetMinZ() && vertex0->x <= parent->GetMaxZ())
					&&
					(vertex1->z >= parent->GetMinX() && vertex1->z <= parent->GetMaxX()
					&& vertex1->x >= parent->GetMinZ() && vertex1->x <= parent->GetMaxZ())
					&&
					(vertex2->z >= parent->GetMinX() && vertex2->z <= parent->GetMaxX()
					&& vertex2->x >= parent->GetMinZ() && vertex2->x <= parent->GetMaxZ()))
				{
					leafVertices.push_back(vertex0);
					leafVertices.push_back(vertex1);
					leafVertices.push_back(vertex2);
				}
			}

			int amountOfVertices = leafVertices.size();
			Vertex* vertices = new Vertex[amountOfVertices];
			for (int i = 0; i < amountOfVertices; ++i)
			{
				vertices[i] = *leafVertices[i];

			}
			parent->SetAmountOfVertices(amountOfVertices);
			parent->SetVertices(vertices);
		}
	}

	QuadNode* Ground::GetQuadTree()
	{
		return quadTreeRootNode;
	}

	unsigned short Ground::GetQuadTreeDepth()
	{
		return quadTreeDepth;
	}

	Vector3& Ground::GetScale()
	{
		return scale;
	}

	Vector3& Ground::GetPosition()
	{
		return position;
	}

}