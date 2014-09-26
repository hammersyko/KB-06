/////////////////////////////////////////////////////////
// IOModel_X.cpp
// load/save Frame descriptions
//
/////////////////////////////////////////////////////////

#include "SuperXLoader.h"


#define TEXT_BUFFER 255

#define MAX_TEMPLATES 15

struct XOF_TEMPLATEID{
	char* TxtID;
	uint16 TemplateID;
};

XOF_TEMPLATEID Templates[MAX_TEMPLATES] = {
		{ "template", X_TEMPLATE },
		{ "FrameTransformMatrix", X_FRAMETRANSFORMMATRIX },
		{ "Frame", X_FRAME },
		{ "XSkinMeshHeader", X_SKINMESHHEADER },
		{ "MeshTextureCoords", X_MESHTEXTURECOORDS },
		{ "MeshMaterialList", X_MESHMATERIALLIST },
		{ "MeshNormals", X_MESHNORMALS },
		{ "Mesh", X_MESH },
		{ "Material", X_MATERIAL },
		{ "SkinWeights", X_SKINWEIGHTS },
		{ "TextureFilename", X_TEXTUREFILENAME },
		{ "AnimationSet", X_ANIMATIONSET },
		{ "AnimationKey", X_ANIMATIONKEY },
		{ "Animation", X_ANIMATION },
		{ "Header", X_HEADER }
};

//////////////////////////////////////////////////////////
//
//       MAIN LOAD & SAVE FUNCTIONS
//
//////////////////////////////////////////////////////////

bool IO_Model_X::Load(std::string pFilename, Model3D* &pT)
{
	XFileHeader XHeader;

	//MYTRACE("Processing file:", pFilename);

	//!!!@@@!!!fin.open(pFilename.c_str(), ios::in);!!!@@@!!!

	if (fin.bad())
		return false;

	fin.read((char*)&XHeader, 16);
	if (XHeader.Magic != XOFFILE_FORMAT_MAGIC)
	{
		//MYTRACE("Not a .X model file. Aborted...");
		return false;
	}

	if (XHeader.Major_Version != XOFFILE_FORMAT_VERSION03)
	{
		//MYTRACE("Major version greater than 03. Aborted...");
		return false;
	}

	if ((XHeader.Minor_Version != XOFFILE_FORMAT_VERSION03) || (XHeader.Minor_Version != XOFFILE_FORMAT_VERSION02))
	{
		//MYTRACE("Minor version greater than 03. Aborted...");
		return false;
	}

	if (XHeader.Format != XOFFILE_FORMAT_TEXT)
	{
		//MYTRACE("Not a text format. Aborted...");
		return false;
	}

	_Object = pT;

	while (!fin.eof())
	{
		switch (ProcessBlock()) {
		case X_ERROR: 
			//MYTRACE("Stopped processing the file ..."); 
			return false;
		case X_COMMENT: break; //nothing to do
		case X_EBRACE: break; //end of a block ?!
		case X_FRAME: ProcessBone((Bone*)0); break;
		case X_MESH: ProcessMesh(); break;
		case X_ANIMATIONSET: ProcessAnimationSets(); break;
		case X_OBRACE:
		default: AvoidTemplate(); break;
		}
	}

	if (_LoadSkeletton != 0)
		MapMeshToBones(_LoadSkeletton);

	//MYTRACE("Processed file:", pFilename, "OK");

	fin.close();
	return true;
}

bool IO_Model_X::Save(std::string pFilename, Model3D* &pT)
{
	return false;
}

//////////////////////////////////////////////////////////
//
//       UTILITIES
//
//////////////////////////////////////////////////////////

int16 IO_Model_X::ProcessBlock(void)
{
	std::string Text;
	char Token = fin.peek();
	switch (Token) {
	case '\n':
	case ' ':
	case '\t': fin.get(); return X_COMMENT; //spaces are identified as comments
	case '{': return X_OBRACE;
	case '}': fin.get(); return X_EBRACE; //We arrived at the end of the block
	case '/': fin.get(); if (fin.peek() != '/'){ return X_ERROR; } //we processed a block name starting with slash ?!
	case '#': fin.ignore(TEXT_BUFFER, '\n'); return X_COMMENT;
	default:
		fin >> Text;
		return BlockID(Text);
	};
}

int16 IO_Model_X::BlockID(std::string &pText)
{
	long Pos;

	if (fin.eof())
		return X_COMMENT;

	if (pText.empty())
	{
		//MYTRACE("Error, no block read !");
		return X_ERROR;
	}

	for (int i = 0; i < MAX_TEMPLATES; i++)
	{
		Pos = pText.find(Templates[i].TxtID);
		if (Pos > -1)
		{
			fin.get(); //eats the whitespace after the block name.
			return Templates[i].TemplateID;
		}
	}
	//MYTRACE("Unknown Block:", pText);
	return X_UNKNOWN;
}

void IO_Model_X::AvoidTemplate(void){
	char Token;

	fin.ignore(TEXT_BUFFER, '{');

	while (!fin.eof()){
		Token = fin.peek();
		if (Token == '{')
			AvoidTemplate();
		if (Token == '}')
		{
			fin.get();
			return;
		}
		fin.get();
	}
}

void IO_Model_X::Find(uchar pChar)
{
	fin.ignore(TEXT_BUFFER, pChar);
}

char* IO_Model_X::SetUID(char pType)
{
	//This is a quick hack to derive a Unique ID for blocks with
	//no identifier names like in the tiny_4anim.x example.

	//!!!@@@!!!_X_UID.Integer = GetTickCount(); //This function return a 4 byte wide number
	_X_UID.Text[4] = pType; //We set the 5th byte with a significant character

	//If any of the first 4 bytes are under 32 we add 32
	//We want to avoid the occurrence of the char '\0' within
	//the first 4 bytes since this would truncate the text returned.
	if (_X_UID.Text[0] < 32) _X_UID.Text[0] += 32;
	if (_X_UID.Text[1] < 32) _X_UID.Text[1] += 32;
	if (_X_UID.Text[2] < 32) _X_UID.Text[2] += 32;
	if (_X_UID.Text[3] < 32) _X_UID.Text[3] += 32;
	return _X_UID.Text;
}

//////////////////////////////////////////////////////////
//
//       TRANSFORM MATRIX
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessFrameTransformMatrix(Bone* &pB)
{
	char Text[TEXT_BUFFER];

	Find('{');
	for (int i = 0; i < 15; i++)
	{
		fin.getline(Text, TEXT_BUFFER, ',');
		pB->_MatrixPos[i] = TextToNum(Text);
	}
	fin.getline(Text, TEXT_BUFFER, ';');
	pB->_MatrixPos[15] = TextToNum(Text);
	//   pB->_TransMatrix = pB->_MatrixPos;
	Find('}');
}

//////////////////////////////////////////////////////////
//
//       BONE WITHIN SKELETTON
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessBone(Bone* pBone)
{
	Bone* cBone;
	int16 Token;
	char Data[TEXT_BUFFER];

	cBone = new Bone();

	Token = fin.peek();
	if (Token != '{')
		fin >> cBone->_Name;
	else
		cBone->_Name = SetUID('B');

	if (pBone == 0)
	{
		//MYTRACE("Skeletton 1st bone:", cBone->_Name);
		_LoadSkeletton = cBone;
		_Object->_Skeletton = _LoadSkeletton;
	}
	else
	{
		//MYTRACE("\t", pBone->_Name, "->", cBone->_Name);
		pBone->_Bones.push_back(cBone);
	}
	Find('{');
	Token = X_OBRACE;
	while (Token != X_EBRACE)
	{
		Token = ProcessBlock();
		switch (Token) {
		case X_COMMENT: break; //used for spaces and other kind of comments
		case X_EBRACE: return; //this is the end, my only friend ...
		case X_OBRACE: fin.getline(Data, TEXT_BUFFER, '}'); cBone->_MeshName = Data; break;
		case X_FRAME: ProcessBone(cBone); break;
		case X_FRAMETRANSFORMMATRIX: ProcessFrameTransformMatrix(cBone); break;
		case X_MESH: ProcessMesh(); cBone->_MeshName = _LoadMesh->_Name; break;
		default:
			AvoidTemplate(); break;
		}
	}
}

//////////////////////////////////////////////////////////
//
//       MESH
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessMesh(void)
{
	std::string Text;
	int16 Token;
	char Data[TEXT_BUFFER];

	_LoadMesh = new Mesh;
	if (!_Object->_Meshes.empty())
	{
		Mesh* LastMesh = _Object->_Meshes.back();
		_LoadMesh->_FirstVertex = LastMesh->_FirstVertex + LastMesh->_nVertices;
		_LoadMesh->_FirstFace = LastMesh->_FirstFace + LastMesh->_nFaces;
		_LoadMesh->_FirstTextureCoord = LastMesh->_FirstTextureCoord + LastMesh->_nTextureCoords;
		_LoadMesh->_FirstMaterial = LastMesh->_FirstMaterial + LastMesh->_nMaterials;
		if (_LoadMesh->_FirstTextureCoord < _LoadMesh->_FirstVertex)
			_LoadMesh->_FirstTextureCoord = _LoadMesh->_FirstVertex;
		_LoadMesh->_FirstNormal = LastMesh->_FirstNormal + LastMesh->_nNormals;
		if (_LoadMesh->_FirstNormal < _LoadMesh->_FirstVertex)
			_LoadMesh->_FirstNormal = _LoadMesh->_FirstVertex;
		//MYTRACE("Starting Vertex index:", _LoadMesh->_FirstVertex);
		//MYTRACE("Starting Face index:", _LoadMesh->_FirstFace);
		//MYTRACE("Starting TextureCoord index:", _LoadMesh->_FirstTextureCoord);
		//MYTRACE("Starting Normal index:", _LoadMesh->_FirstNormal);
		//MYTRACE("Starting Material index:", _LoadMesh->_FirstMaterial);
	}

	Token = fin.peek();
	if (Token != '{')
		fin >> _LoadMesh->_Name;
	else
		_LoadMesh->_Name = SetUID('M');

	Find('{');
	//MYTRACE("Mesh:", _LoadMesh->_Name);

	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_nVertices = (uint16)TextToNum(Data);
	//MYTRACE("Number of vertices:", _LoadMesh->_nVertices);
	_LoadMesh->_Vertices = new Vertex[_LoadMesh->_nVertices];
	//   _LoadMesh->_SkinnedVertices = new Frm::Vertex[_LoadMesh->_nVertices];
	for (int i = 0; i < _LoadMesh->_nVertices; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Vertices[i].data[0] = TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Vertices[i].data[1] = TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Vertices[i].data[2] = TextToNum(Data);
		fin.get();//eats either the comma or the semicolon at the end of each vertex description
	}

	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_nFaces = (uint16)TextToNum(Data);
	///MYTRACE("Number of Faces:", _LoadMesh->_nFaces);
	_LoadMesh->_Faces = new Face[_LoadMesh->_nFaces];
	for (uint32 i = 0; i < _LoadMesh->_nFaces; i++)
	{
		Find(';');
		fin.getline(Data, TEXT_BUFFER, ',');
		_LoadMesh->_Faces[i].data[0] = (uint16)TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ',');
		_LoadMesh->_Faces[i].data[1] = (uint16)TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Faces[i].data[2] = (uint16)TextToNum(Data);
		fin.get(); //eats either the comma or the semicolon at the end of each face description
		//      MYTRACE("Face:", i, ":", _LoadMesh->_Faces[i].data[0],_LoadMesh->_Faces[i].data[1],_LoadMesh->_Faces[i].data[2]);
	}

	Token = X_COMMENT;
	while (Token != X_EBRACE)
	{
		Token = ProcessBlock();
		switch (Token) {
		case X_COMMENT: break; //used for spaces and other kind of comments
		case X_EBRACE: _Object->_Meshes.push_back(_LoadMesh); return; //this is the end, my only friend ...
		case X_MESHNORMALS: ProcessMeshNormals(); break;
		case X_MESHTEXTURECOORDS: ProcessMeshTextureCoords(); break;
		case X_MESHMATERIALLIST: ProcessMeshMaterials(); break;
		case X_SKINMESHHEADER: AvoidTemplate(); break;
		case X_SKINWEIGHTS: ProcessSkinWeights(); break;
		default:
			AvoidTemplate(); break;
		}
	}
	_Object->_Meshes.push_back(_LoadMesh);
}

//////////////////////////////////////////////////////////
//
//       TEXTURE COORDS
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessMeshTextureCoords(void)
{
	char Data[TEXT_BUFFER];

	Find('{');

	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_nTextureCoords = (uint16)TextToNum(Data);
	//MYTRACE("Number of Texture Coords:", _LoadMesh->_nTextureCoords);
	_LoadMesh->_TextureCoords = new TCoord[_LoadMesh->_nTextureCoords];
	for (int i = 0; i < _LoadMesh->_nTextureCoords; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_TextureCoords[i].data[0] = TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_TextureCoords[i].data[1] = TextToNum(Data);
		fin.get();//eats the comma or the semicolon at the end
	}
	Find('}');
}

//////////////////////////////////////////////////////////
//
//       MESH NORMAL VECTORS
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessMeshNormals(void)
{
	char Data[TEXT_BUFFER];

	Find('{');
	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_nNormals = (uint16)TextToNum(Data);
	//MYTRACE("Number of normals :", _LoadMesh->_nNormals);
	_LoadMesh->_Normals = new Vector<float>[_LoadMesh->_nNormals];
	for (int i = 0; i < _LoadMesh->_nNormals; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Normals[i].x = TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Normals[i].y = TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_Normals[i].z = TextToNum(Data);
		fin.get();//eats the comma or the semicolon at the end
	}

	_LoadMesh->_FaceNormals = new Face[_LoadMesh->_nFaces];
	for (uint32 i = 0; i < _LoadMesh->_nFaces; i++)
	{
		Find(';');
		fin.getline(Data, TEXT_BUFFER, ',');
		_LoadMesh->_FaceNormals[i].data[0] = (uint16)TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ',');
		_LoadMesh->_FaceNormals[i].data[1] = (uint16)TextToNum(Data);
		fin.getline(Data, TEXT_BUFFER, ';');
		_LoadMesh->_FaceNormals[i].data[2] = (uint16)TextToNum(Data);
		fin.get(); //eats either the comma or the semicolon at the end of each face description
		//      MYTRACE("Face Normal indexes:", i, ":", _LoadMesh->_FaceNormals[i].data[0],_LoadMesh->_FaceNormals[i].data[1],_LoadMesh->_FaceNormals[i].data[2]);
	}

	Find('}');
}


//////////////////////////////////////////////////////////
//
//       MATERIALS USED IN MESH
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessMeshMaterials(void)
{
	std::string Text;
	int16 Token;
	char Data[TEXT_BUFFER];

	Find('{');

	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_nMaterials = (uint16)TextToNum(Data);
	//MYTRACE("Number of Materials:", (uint16)TextToNum(Data));

	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_FaceMaterials = new uint16[(uint16)TextToNum(Data)];
	for (uint32 i = 0; i < _LoadMesh->_nFaces - 1; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ',');
		_LoadMesh->_FaceMaterials[i] = (uint16)TextToNum(Data);
	}
	fin.getline(Data, TEXT_BUFFER, ';');
	_LoadMesh->_FaceMaterials[_LoadMesh->_nFaces - 1] = (uint16)TextToNum(Data);
	fin.get(); //eats the last semicolon

	Token = X_COMMENT;
	while (Token != X_EBRACE)
	{
		Token = ProcessBlock();
		switch (Token) {
		case X_COMMENT: break; //used for spaces and other kind of comments
		case X_EBRACE: return; //this is the end, my only friend ...
		case X_MATERIAL: ProcessMaterial(); break;
		default:
			AvoidTemplate(); break;
		}
	}
}

//////////////////////////////////////////////////////////
//
//       MATERIAL DESCRIPTION
//
//////////////////////////////////////////////////////////

void IO_Model_X::ProcessMaterial(void)
{
	std::string Text;
	int16 Token;
	char Data[TEXT_BUFFER];

	XMaterial* NewMaterial = new XMaterial;

	Find('{');
	for (int i = 0; i < 4; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ';');
		NewMaterial->_FaceColor.data[i] = TextToNum(Data);
	}
	fin.get(); //eats the last semicolon
	fin.getline(Data, TEXT_BUFFER, ';');
	NewMaterial->_power = TextToNum(Data);

	for (int i = 0; i < 3; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ';');
		NewMaterial->_SpecularColor.data[i] = TextToNum(Data);
	}
	fin.get();//eats the last semicolon

	for (int i = 0; i < 3; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ';');
		NewMaterial->_EmissiveColor.data[i] = TextToNum(Data);
	}
	fin.get();//eats the last semicolon

	Token = X_COMMENT;
	while (Token != '}')
	{
		Token = ProcessBlock();
		switch (Token) {
		case X_COMMENT: break; //used for spaces and other kind of comments
		case X_EBRACE: _LoadMesh->_Materials.push_back(NewMaterial); return; //this is the end, my only friend ...
		case X_TEXTUREFILENAME:
			Find('{');
			Find('"');
			fin.getline(Data, TEXT_BUFFER, '"');
			NewMaterial->_TextureName = Data;
			Find('}');
			break;
		default:
			AvoidTemplate(); break;
		}
	}
	_LoadMesh->_Materials.push_back(NewMaterial);
}

//////////////////////////////////////////////////////////
//
//       SKIN WEIGHTS
//
//////////////////////////////////////////////////////////
void IO_Model_X::ProcessSkinWeights(void)
{
	Bone* cBone;
	std::string temp;
	char Data[TEXT_BUFFER];

	Find('{');
	Find('"');
	fin.getline(Data, TEXT_BUFFER, '"');
	temp = Data;
	cBone = _LoadSkeletton->IsName(temp);
	//   cBone->_Mesh = _LoadMesh;
	//MYTRACE("Skinning bone:", cBone->_Name);
	Find(';');

	fin.getline(Data, TEXT_BUFFER, ';');
	cBone->_nVertices = (uint16)TextToNum(Data);
	cBone->_Vertices = new uint16[cBone->_nVertices];
	for (uint32 i = 0; i < cBone->_nVertices - 1; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ',');
		cBone->_Vertices[i] = (uint16)TextToNum(Data);
		//      MYTRACE("Vertex:", atoi(Data));/**/
	}
	fin.getline(Data, TEXT_BUFFER, ';');
	cBone->_Vertices[cBone->_nVertices - 1] = (uint16)TextToNum(Data);
	//   MYTRACE("Vertex:", atoi(Data));/**/

	cBone->_Weights = new float[cBone->_nVertices];
	for (uint32 i = 0; i < cBone->_nVertices - 1; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ',');
		cBone->_Weights[i] = TextToNum(Data);
		//      MYTRACE("Weight:", atof(Data));/**/
	}
	fin.getline(Data, TEXT_BUFFER, ';');
	cBone->_Weights[cBone->_nVertices - 1] = TextToNum(Data);
	//   MYTRACE("Weight:", atof(Data));/**/

	for (int i = 0; i < 15; i++)
	{
		fin.getline(Data, TEXT_BUFFER, ',');
		cBone->_SkinOffset[i] = TextToNum(Data);
	}
	fin.getline(Data, TEXT_BUFFER, ';');
	cBone->_SkinOffset[15] = TextToNum(Data);
	Find('}');
}

/*************************************************
NEW- NEW- NEW- NEW- NEW- NEW- NEW- NEW- NEW- NEW*/

//////////////////////////////////////////////////////////
//
//       ANIMATION SET
//
//////////////////////////////////////////////////////////
void IO_Model_X::ProcessAnimationSets(void)
{
	//   std::string Text;
	int16 Token;
	//   char Data[TEXT_BUFFER];

	_MaxKey = 0;
	_LoadAnimationSet = new AnimationSet;

	Token = fin.peek();
	if (Token != '{')
		fin >> _LoadAnimationSet->_Name;
	else
		_LoadAnimationSet->_Name = SetUID('A');

	Find('{');
	//MYTRACE("Animation Set:", _LoadAnimationSet->_Name);

	Token = X_COMMENT;
	while (Token != X_EBRACE)
	{
		Token = ProcessBlock();
		switch (Token) {
		case X_COMMENT: break; //used for spaces and other kind of comments
		case X_EBRACE: _LoadAnimationSet->_MaxKey = _MaxKey; //MYTRACE("MaxKey:", _MaxKey); 
			_Object->_AnimationSets.push_back(_LoadAnimationSet); return; //this is the end, my only friend ...
		case X_ANIMATION: ProcessAnimations(_LoadAnimationSet); break;
		default:
			AvoidTemplate(); break;
		}
	}
	_LoadAnimationSet->_MaxKey = _MaxKey;
	//MYTRACE("MaxKey:", _MaxKey);
	_Object->_AnimationSets.push_back(_LoadAnimationSet);
}

//////////////////////////////////////////////////////////
//
//       ANIMATION
//
//////////////////////////////////////////////////////////
void IO_Model_X::ProcessAnimations(AnimationSet* &pAS)
{
	int16 Token;
	char Data[TEXT_BUFFER];
	Animation* TempAnimation = new Animation;

	Find('{');

	Token = X_COMMENT;
	while (Token != X_EBRACE)
	{
		Token = ProcessBlock();
		switch (Token) {
		case X_COMMENT: break; //used for spaces and other kind of comments
		case X_EBRACE: pAS->_Animations.push_back(TempAnimation); return; //this is the end, my only friend ...
		case X_OBRACE:
			Find('{');
			fin.getline(Data, TEXT_BUFFER, '}');
			Remove(' ', Data);
			TempAnimation->_BoneName = Data;
			//MYTRACE("Animated Bone:", TempAnimation->_BoneName);
			break;
		case X_ANIMATIONKEY: ProcessAnimationKeys(TempAnimation); break;
		default:
			AvoidTemplate(); break;
		}
	}
	pAS->_Animations.push_back(TempAnimation);
}

//////////////////////////////////////////////////////////
//
//       ANIMATION KEY
//
//////////////////////////////////////////////////////////
void IO_Model_X::ProcessAnimationKeys(Animation* &pA)
{
	int Type, Size;
	char Data[TEXT_BUFFER];
	RotateKey* 	TempRot;
	ScaleKey*	   TempScale;
	PositionKey* TempPos;
	MatrixKey*	TempMatrix;

	Find('{');
	fin.getline(Data, TEXT_BUFFER, ';');
	Type = (uint16)atoi(Data);
	fin.getline(Data, TEXT_BUFFER, ';');
	Size = (uint16)atoi(Data);

	switch (Type) {
	case 0:
		//MYTRACE(Size, "Rotation Keys");
		pA->_Rotations.reserve(Size);
		while (Size--)
		{
			TempRot = new RotateKey;
			fin.getline(Data, TEXT_BUFFER, ';');
			TempRot->Time = (uint16)TextToNum(Data);
			if (TempRot->Time > _MaxKey)
				_MaxKey = TempRot->Time;
			Find(';');
			fin.getline(Data, TEXT_BUFFER, ',');
			TempRot->Rotation[0] = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ',');
			TempRot->Rotation[1] = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ',');
			TempRot->Rotation[2] = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ';');
			TempRot->Rotation[3] = TextToNum(Data);
			Find(';');
			fin.get();
			pA->_Rotations.push_back(TempRot);
		}
		break;
	case 1:
		//MYTRACE(Size, "Scaling Keys");
		pA->_Scalings.reserve(Size);
		while (Size--)
		{
			TempScale = new ScaleKey;
			fin.getline(Data, TEXT_BUFFER, ';');
			TempScale->Time = (uint16)TextToNum(Data);
			if (TempScale->Time > _MaxKey)
				_MaxKey = TempScale->Time;
			Find(';');
			fin.getline(Data, TEXT_BUFFER, ',');
			TempScale->Scale.x = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ',');
			TempScale->Scale.y = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ';');
			TempScale->Scale.z = TextToNum(Data);
			Find(';');
			fin.get();
			pA->_Scalings.push_back(TempScale);
		}
		break;
	case 2:
		//MYTRACE(Size, "Position Keys");
		pA->_Translations.reserve(Size);
		while (Size--)
		{
			TempPos = new PositionKey;
			fin.getline(Data, TEXT_BUFFER, ';');
			TempPos->Time = (uint16)TextToNum(Data);
			if (TempPos->Time > _MaxKey)
				_MaxKey = TempPos->Time;
			Find(';');
			fin.getline(Data, TEXT_BUFFER, ',');
			TempPos->Translation[0] = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ',');
			TempPos->Translation[1] = TextToNum(Data);
			fin.getline(Data, TEXT_BUFFER, ';');
			TempPos->Translation[2] = TextToNum(Data);
			Find(';');
			fin.get();
			pA->_Translations.push_back(TempPos);
		}
		break;
	case 4:
		//MYTRACE(Size, "Matrix Keys");
		pA->_Matrices.reserve(Size);
		while (Size--)
		{
			TempMatrix = new MatrixKey;
			fin.getline(Data, TEXT_BUFFER, ';');
			TempMatrix->Time = (uint16)TextToNum(Data);
			if (TempMatrix->Time > _MaxKey)
				_MaxKey = TempMatrix->Time;
			Find(';');
			for (int i = 0; i < 15; i++)
			{
				fin.getline(Data, TEXT_BUFFER, ',');
				TempMatrix->Matrix[i] = TextToNum(Data);
			}
			fin.getline(Data, TEXT_BUFFER, ';');
			TempMatrix->Matrix[15] = TextToNum(Data);
			Find(';');
			fin.get();
			pA->_Matrices.push_back(TempMatrix);
		}
		break;
	default: //MYTRACE("Unknown Type", Type, " ..."); 
		break;
	}

	Find('}');
}

/***END*******************************************/

//////////////////////////////////////////////////////////
//
//       MAP MESH TO BONES
//
//////////////////////////////////////////////////////////
void IO_Model_X::MapMeshToBones(Bone* &pBone)
{
	if (pBone->_MeshName.empty())
		pBone->_MeshName = _LoadMesh->_Name;

	//MYTRACE("Bone", pBone->_Name, "is linked to mesh", pBone->_MeshName);

	if (!pBone->_Bones.empty())
		for (std::list<Bone*>::iterator i = pBone->_Bones.begin(); i != pBone->_Bones.end(); i++)
		{
		if ((*i)->_MeshName.empty())
		{
			(*i)->_MeshName = pBone->_MeshName;
		}
		MapMeshToBones(*i);
		}
};
