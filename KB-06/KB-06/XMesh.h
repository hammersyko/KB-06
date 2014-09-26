#ifndef _XMESH_H_
#define _XMESH_H_

#include "Types.h"
//#include "framework\frm_Quaternion.h"
#include "XMaterial.h"
#include "Matrix.h"
#include "Quaternion.h"

#define DEBUG
//#include "Toolbox\Mytrace.h"

#include <list>
#include <vector>

class XMesh
{
public:
	XMesh();
	~XMesh();
};

class Bone
{
public:
	Bone(void) : _nVertices(0), _Vertices(0), _Weights(0)
	{
		_MatrixPos.Identity(); _SkinOffset.Identity();
	};
	~Bone(void);
	Bone* IsName(std::string &BoneName);
	void UpdateIndices(uint16 pIndex);

	Matrix<float> _MatrixPos, _SkinOffset;
	uint32 _nVertices;
	uint16* _Vertices;
	float* _Weights;
	std::string _MeshName;
	std::string _Name;
	std::list<Bone*> _Bones;
};

struct Subset
{
	uint16 Size;
	Face* Faces;
};

class Mesh
{
public:
	Mesh(void) :_nVertices(0), _Vertices(0),
		_nFaces(0), _Faces(0),
		_nTextureCoords(0), _TextureCoords(0),
		_FaceMaterials(0),
		_nNormals(0), _Normals(0),
		_nMaterials(0),
		_FirstVertex(0),          //The _Firstxxx series are used
		_FirstFace(0),            //for mesh concatenation
		_FirstTextureCoord(0),
		_FirstNormal(0),
		_FirstMaterial(0)
	{
	};
	~Mesh(void);
	Mesh* IsName(std::string &MeshName)
	{
		if (strcmp(_Name.c_str(), MeshName.c_str()) == 0)
		{
			return this;
		}
		return 0;
	};
	void UpdateIndices(void);
	void CreateSubsets(void);
	//Vertices
	uint16 _nVertices, _FirstVertex;
	Vertex* _Vertices;
	//Texture Coords for each vertex
	uint16 _nTextureCoords, _FirstTextureCoord;
	TCoord* _TextureCoords;
	//Faces
	uint32 _nFaces, _FirstFace;
	Face* _Faces;
	//Subset of a mesh: there is one subset for each material used
	std::list<Subset*> _Subsets;
	//Normals
	uint16 _nNormals, _FirstNormal;
	Vector<float>* _Normals;
	Face* _FaceNormals;
	//Material index for each face
	uint16 _nMaterials, _FirstMaterial;
	uint16* _FaceMaterials;
	//list of Materials for that Mesh
	std::list<XMaterial*> _Materials;
	std::string _Name;
};

typedef struct
{
	uint32          Time;
	Quaternion<float> Rotation;
} RotateKey;

typedef struct
{
	uint32     	Time;
	Vertex	Translation;
} PositionKey;

typedef struct
{
	uint32       Time;
	Vector<float>  Scale;
} ScaleKey;

typedef struct
{
	uint32      Time;
	Matrix<float> Matrix;
} MatrixKey;

class Animation
{
public:
	~Animation(void);
	Animation* Get(void)
	{
		return this;
	};
	Animation* IsName(std::string &pText)
	{
		if (strcmp(_BoneName.c_str(), pText.c_str()) == 0)
		{
			return this;
		}
		return 0;
	};

	std::string _BoneName;
	std::vector<ScaleKey*> _Scalings;
	std::vector<RotateKey*> _Rotations;
	std::vector<PositionKey*> _Translations;
	std::vector<MatrixKey*> _Matrices;
};

class AnimationSet
{
public:
	~AnimationSet(void);
	AnimationSet* IsName(std::string &pText)
	{
		if (strcmp(_Name.c_str(), pText.c_str()) == 0)
		{
			return this;
		}
		return 0;
	};
	Animation* FindAnimation(std::string &pText);
	std::string _Name;
	std::list<Animation*> _Animations;
	uint32 _MaxKey; //Maximum time key for the full animation set
};

class Model3D
{
public:
	Model3D(void) : _Skeletton(0)
	{
	};
	~Model3D(void);
	Mesh* IsMeshName(std::string &pText);
	void ConcatenateMeshes(void);
	AnimationSet* FindAnimationSet(std::string &pText);
	Bone* _Skeletton;
	std::list<Mesh*> _Meshes;
	std::list<AnimationSet*> _AnimationSets;
private:
	void UpdateBoneIndices(Bone* &pBone);
};
#endif