#include "SuperXLoader.h"

#define TEXT_BUFFER 255

#define MAX_TEMPLATES 16

namespace pengine
{
	struct XOF_TEMPLATEID{
		char* textID;
		uint16 templateID;
	};

	XOF_TEMPLATEID templates[MAX_TEMPLATES] = {
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
			{ "Header", X_HEADER },
			{ "DeclData", X_DECLDATA }
	};

	SuperXLoader::SuperXLoader()
	{
		logger = LoggerPool::GetInstance().GetLogger("SuperXLoader");
	}

	SuperXLoader::~SuperXLoader()
	{
	}

	bool SuperXLoader::Load(std::string pFilename, Model3D* &pT)
	{
		XFileHeader xHeader;
		//logger->Log(Logger::DEBUG, "SuperXLoader: Processing file:" + std::string(pFilename));

		fin.open(pFilename.c_str(), std::ios::in | std::ios::binary);

		if (fin.bad())
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Failed opening file:" + std::string(pFilename));
			return false;
		}

		fin.read((char*)&xHeader, 16);
		//logger->Log(Logger::DEBUG, "SuperXLoader: XHeader size: " + std::to_string(xHeader.Float_Size) + ", XHeader format: " + std::to_string(xHeader.Format) + ", XHeader magic: " + std::to_string(xHeader.Magic) + ", XHeader major: " + std::to_string(xHeader.Major_Version) + ", XHeader minor: " + std::to_string(xHeader.Minor_Version));
		if (xHeader.Magic != XOFFILE_FORMAT_MAGIC)
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Not a .X model file. Aborted.");
			return false;
		}

		if (xHeader.Major_Version != XOFFILE_FORMAT_VERSION03)
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Major version " + std::to_string(xHeader.Major_Version) + " greater than 03. Aborted.");
			return false;
		}

		if (xHeader.Minor_Version != XOFFILE_FORMAT_VERSION03 && xHeader.Minor_Version != XOFFILE_FORMAT_VERSION02)
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Minor version " + std::to_string(xHeader.Minor_Version) + " is not " + std::to_string(XOFFILE_FORMAT_VERSION03) + " or " + std::to_string(XOFFILE_FORMAT_VERSION02) + ". Aborted.");
			return false;
		}

		if (xHeader.Format != XOFFILE_FORMAT_TEXT)
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Not a text format. Aborted.");
			return false;
		}

		_Object = pT;

		while (!fin.eof())
		{
			switch (ProcessBlock())
			{
			case X_ERROR:
				//logger->Log(Logger::DEBUG, "SuperXLoader: Stopped processing the file.");
				return false;
				break;
			case X_COMMENT:
				break; //nothing to do
			case X_EBRACE:
				break; //end of a block ?!
			case X_FRAME:
				ProcessBone(NULL);
				break;
			case X_MESH:
				ProcessMesh();
				break;
			case X_ANIMATIONSET:
				ProcessAnimationSets();
				break;
			case X_MATERIAL:
				ProcessMaterial(true);
				//logger->Log(Logger::DEBUG, "SuperXLoader: done processing a material...");
				break;
			case X_OBRACE:
			default:
				AvoidTemplate();
				break;
			}
		}

		if (_LoadSkeleton != NULL)
		{
			MapMeshToBones(_LoadSkeleton);
			_LoadSkeleton = NULL;
		}
		else
		{
			//seems to be tiger only...
			_Object->skeleton = new Bone();
			_Object->skeleton->matrixPos.Identity();
			//logger->Log(Logger::DEBUG, "There's no rootbone!");
		}

		//logger->Log(Logger::DEBUG, "SuperXLoader: Processed file:" + std::string(pFilename));

		fin.close();
		return true;
	}

	float SuperXLoader::TextToNum(char* pText)
	{
		float test = 0, num = 10;
		bool sign;

		int textsize = strlen(pText);
		unsigned char i = 0;

		sign = false;
		while ((sign == false) && (i < textsize))
		{
			switch (pText[i])
			{
				//All the cases below are OR-like, if ANY of those cases is hit, it'll do sign=true
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '.': 
				sign = true;
				break;
			default:
				++i; 
				break;
			}
		}

		if (i >= textsize)
		{
			return 0.0f;
		}

		if (pText[i] == '-')
		{
			sign = true; 
			++i;
		}
		else
		{
			sign = false;
		}

		while ((pText[i] >= '0') && (pText[i] <= '9'))
		{
			test *= num;
			test += (pText[i++] - '0');
		}
		num = 0.1f;
		if ((i < textsize) && (pText[i++] == '.'))
		{
			while ((pText[i] >= '0') && (pText[i] <= '9'))
			{
				test += (pText[i++] - '0')*num;
				num *= 0.1f;
			}
		}
		if (sign)
		{
			test = -test;
		}
		return test;
	}

	void SuperXLoader::Remove(char pDelimiter, char* pText)
	{
		char result[TEXT_BUFFER];
		uint32 i, j = 0;
		for (i = 0; i < strlen(pText); i++)
		{
			if (pText[i] != pDelimiter)
			{
				result[j++] = pText[i];
			}
		}
		result[j++] = '\0';
		memcpy(pText, result, j);
	};

	int16 SuperXLoader::ProcessBlock()
	{
		std::string text;
		char token = fin.peek();
		switch (token)
		{
		case '\n':
		case '\r':
		case ' ':
		case '\t':
			fin.get();
			return X_COMMENT; //spaces are identified as comments
			break;
		case '{':
			//logger->Log(Logger::DEBUG, "ProcessBlock char is: {");
			return X_OBRACE;
			break;
		case '}':
			//logger->Log(Logger::DEBUG, "ProcessBlock char is: }");
			fin.get();
			return X_EBRACE; //We arrived at the end of the block
			break;
		case '/':
			//logger->Log(Logger::DEBUG, "ProcessBlock char is: /");
			fin.get();
			if (fin.peek() == '/')
			{
				fin.ignore(TEXT_BUFFER, '\n');//eat the comment...
				return X_COMMENT;
			} //we processed a block name starting with slash ?!
			else
			{
				return X_ERROR;
			}
			break;
		case '#':
			//logger->Log(Logger::DEBUG, "ProcessBlock char is: " + std::string(1, token));
			fin.ignore(TEXT_BUFFER, '\n');
			return X_COMMENT;
			break;
		default:
			fin >> text;
			return BlockID(text);
		};
		return X_COMMENT;
	}

	int16 SuperXLoader::BlockID(std::string &pText)
	{
		long pos;
		//logger->Log(Logger::DEBUG, "SuperXLoader: BlockID: " + pText);
		if (fin.eof())
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: BlockID() has eof");
			return X_COMMENT;
		}

		if (pText.empty())
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Error: no block read!");
			return X_ERROR;
		}

		for (int i = 0; i < MAX_TEMPLATES; ++i)
		{
			pos = pText.find(templates[i].textID);
			if (pos > -1)
			{
				fin.get(); //eats the whitespace after the block name.
				return templates[i].templateID;
			}
		}
		//logger->Log(Logger::DEBUG, "SuperXLoader: Unknown block: " + pText);
		return X_UNKNOWN;
	}

	void SuperXLoader::AvoidTemplate(void)
	{
		//logger->Log(Logger::DEBUG, "SuperXLoader: AvoidTemplate...");
		char token;

		fin.ignore(TEXT_BUFFER, '{');

		while (!fin.eof()){
			token = fin.peek();
			if (token == '{')
			{
				//logger->Log(Logger::DEBUG, "SuperXLoader: Recursive AvoidTemplate:");
				AvoidTemplate();
			}
			if (token == '}')
			{
				fin.get();
				return;
			}
			fin.get();
		}
	}

	void SuperXLoader::Find(uchar pChar)
	{
		fin.ignore(TEXT_BUFFER, pChar);
	}

	char* SuperXLoader::SetUID(char pType)
	{
		//This is a quick hack to derive a Unique ID for blocks with
		//no identifier names like in the tiny_4anim.x example.

		_X_UID.Integer = (rand() / 2) + (GetTickCount() / 2);// 4 bytes semi-random, I will eat a banana if it isn't random enough...
		_X_UID.Text[4] = pType; //We set the 5th byte with a significant character

		//If any of the first 4 bytes are under 32 we add 32
		//We want to avoid the occurrence of the char '\0' within
		//the first 4 bytes since this would truncate the text returned.
		if (_X_UID.Text[0] < 32)
		{
			_X_UID.Text[0] += 32;
		}
		if (_X_UID.Text[1] < 32)
		{
			_X_UID.Text[1] += 32;
		}
		if (_X_UID.Text[2] < 32)
		{
			_X_UID.Text[2] += 32;
		}
		if (_X_UID.Text[3] < 32)
		{
			_X_UID.Text[3] += 32;
		}
		return _X_UID.Text;
	}

	void SuperXLoader::ProcessFrameTransformMatrix(Bone* &pB)
	{
		char text[TEXT_BUFFER];

		Find('{');
		for (int i = 0; i < 15; ++i)
		{
			fin.getline(text, TEXT_BUFFER, ',');
			pB->matrixPos[i] = TextToNum(text);
		}
		fin.getline(text, TEXT_BUFFER, ';');
		pB->matrixPos[15] = TextToNum(text);
		Find('}');
	}

	void SuperXLoader::ProcessBone(Bone* pBone)
	{
		Bone* cBone;
		int16 token;
		char data[TEXT_BUFFER];

		cBone = new Bone();

		token = fin.peek();
		if (token != '{')
		{
			fin >> cBone->name;
		}
		else
		{
			cBone->name = SetUID('B');
		}

		if (unlinkedSkinnedBones[cBone->name] != NULL)
		{
			//logger->Log(Logger::DEBUG, "We found a bone that we already have some skinweights for! The bone is: " + cBone->name);
			cBone = unlinkedSkinnedBones[cBone->name];
		}

		if (pBone == 0)
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: Skeleton 1st bone: " + cBone->name);
			_LoadSkeleton = cBone;
			_Object->skeleton = _LoadSkeleton;
		}
		else
		{
			//logger->Log(Logger::DEBUG, "SuperXLoader: " + pBone->name + "->" + cBone->name);
			pBone->bones.push_back(cBone);
		}
		Find('{');
		token = X_OBRACE;
		while (token != X_EBRACE)
		{
			token = ProcessBlock();
			switch (token)
			{
			case X_COMMENT:
				break; //used for spaces and other kind of comments
			case X_EBRACE:
				return; //this is the end, my only friend ...
				break;
			case X_OBRACE:
				fin.getline(data, TEXT_BUFFER, '}');
				cBone->meshName = data;
				break;
			case X_FRAME:
				ProcessBone(cBone);
				break;
			case X_FRAMETRANSFORMMATRIX:
				ProcessFrameTransformMatrix(cBone);
				break;
			case X_MESH:
				ProcessMesh();
				cBone->meshName = _LoadMesh->name;
				break;
			default:
				AvoidTemplate();
				break;
			}
		}
	}

	void SuperXLoader::ProcessMesh(void)
	{
		_LoadMesh = new Mesh();

		std::string text;
		int16 token;
		char data[TEXT_BUFFER];

		if (!_Object->meshes.empty())
		{
			Mesh* LastMesh = _Object->meshes.back();
			_LoadMesh->_FirstVertex = LastMesh->_FirstVertex + LastMesh->nVertices;
			_LoadMesh->firstFace = LastMesh->firstFace + LastMesh->nFaces;
			_LoadMesh->firstTextureCoord = LastMesh->firstTextureCoord + LastMesh->nTextureCoords;
			_LoadMesh->firstMaterial = LastMesh->firstMaterial + LastMesh->nMaterials;
			if (_LoadMesh->firstTextureCoord < _LoadMesh->_FirstVertex)
			{
				_LoadMesh->firstTextureCoord = _LoadMesh->_FirstVertex;
			}
			_LoadMesh->firstNormal = LastMesh->firstNormal + LastMesh->nNormals;
			if (_LoadMesh->firstNormal < _LoadMesh->_FirstVertex)
			{
				_LoadMesh->firstNormal = _LoadMesh->_FirstVertex;
			}

			//logger->Log(Logger::DEBUG, "SuperXLoader: Starting Vertex index: " + std::to_string(_LoadMesh->_FirstVertex));
			//logger->Log(Logger::DEBUG, "SuperXLoader: Starting Face index: " + std::to_string(_LoadMesh->firstFace));
			//logger->Log(Logger::DEBUG, "SuperXLoader: Starting TextureCoord index: " + std::to_string(_LoadMesh->firstTextureCoord));
			//logger->Log(Logger::DEBUG, "SuperXLoader: Starting Normal index: " + std::to_string(_LoadMesh->firstNormal));
			//logger->Log(Logger::DEBUG, "SuperXLoader: Starting Material index: " + std::to_string(_LoadMesh->firstMaterial));
		}

		token = fin.peek();
		if (token != '{')
		{
			fin >> _LoadMesh->name;
		}
		else
		{
			_LoadMesh->name = SetUID('M');
		}

		Find('{');
		//logger->Log(Logger::DEBUG, "SuperXLoader: Mesh: " + _LoadMesh->name);

		fin.getline(data, TEXT_BUFFER, ';');
		_LoadMesh->nVertices = (uint16)TextToNum(data);
		//logger->Log(Logger::DEBUG, "SuperXLoader: Number of vertices: " + std::to_string(_LoadMesh->nVertices));
		_LoadMesh->vertices = new Vertex[_LoadMesh->nVertices]();
		//   _LoadMesh->_SkinnedVertices = new Frm::Vertex[_LoadMesh->nVertices];
		for (int i = 0; i < _LoadMesh->nVertices; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->vertices[i].x = TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->vertices[i].y = TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->vertices[i].z = TextToNum(data);
			fin.get();//eats either the comma or the semicolon at the end of each vertex description
		}

		fin.getline(data, TEXT_BUFFER, ';');
		_LoadMesh->nFaces = (uint16)TextToNum(data);
		//logger->Log(Logger::DEBUG, "SuperXLoader: Number of Faces: " + std::to_string(_LoadMesh->nFaces));
		_LoadMesh->faces = new Face[_LoadMesh->nFaces]();
		for (uint32 i = 0; i < _LoadMesh->nFaces; ++i)
		{
			Find(';');
			fin.getline(data, TEXT_BUFFER, ',');
			_LoadMesh->faces[i][0] = (uint16)TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ',');
			_LoadMesh->faces[i][1] = (uint16)TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->faces[i][2] = (uint16)TextToNum(data);
			fin.get(); //eats either the comma or the semicolon at the end of each face description
			//logger->LogAll(Logger::DEBUG, "SuperXLoader: Face ", std::to_string(i), ": ", std::to_string(_LoadMesh->faces[i].data[0]), " ", std::to_string(_LoadMesh->faces[i].data[1]), " ", std::to_string(_LoadMesh->faces[i].data[2]));
		}

		token = X_COMMENT;
		while (token != X_EBRACE)
		{
			token = ProcessBlock();
			switch (token)
			{
			case X_COMMENT:
				break; //used for spaces and other kind of comments
			case X_EBRACE:
				_Object->meshes.push_back(_LoadMesh);
				return; //this is the end, my only friend ...
				break;
			case X_MESHNORMALS:
				ProcessMeshNormals();
				break;
			case X_MESHTEXTURECOORDS:
				ProcessMeshTextureCoords();
				break;
			case X_MESHMATERIALLIST:
				ProcessMeshMaterials();
				break;
			case X_SKINMESHHEADER:
				AvoidTemplate();
				break;
			case X_SKINWEIGHTS:
				ProcessSkinWeights();
				break;
			case X_DECLDATA:
				ProcessDeclData();
				break;
			default:
				AvoidTemplate();
				break;
			}
		}
		_Object->meshes.push_back(_LoadMesh);
	}

	void SuperXLoader::ProcessMeshTextureCoords(void)
	{
		char data[TEXT_BUFFER];

		Find('{');

		fin.getline(data, TEXT_BUFFER, ';');
		_LoadMesh->nTextureCoords = (uint16)TextToNum(data);
		//logger->Log(Logger::DEBUG, "SuperXLoader: Number of Texture Coords: " + _LoadMesh->nTextureCoords);
		_LoadMesh->textureCoords = new TCoord[_LoadMesh->nTextureCoords];
		for (int i = 0; i < _LoadMesh->nTextureCoords; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ';');
			//_LoadMesh->textureCoords[i].data[0] = TextToNum(data);
			_LoadMesh->vertices[i].tu = TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			//_LoadMesh->textureCoords[i].data[1] = TextToNum(data);
			_LoadMesh->vertices[i].tv = TextToNum(data);
			fin.get();//eats the comma or the semicolon at the end
		}
		Find('}');
	}

	void SuperXLoader::ProcessMeshNormals(void)
	{
		char data[TEXT_BUFFER];

		Find('{');
		fin.getline(data, TEXT_BUFFER, ';');
		_LoadMesh->nNormals = (uint16)TextToNum(data);
		//logger->Log(Logger::DEBUG, "SuperXLoader: Number of normals: " + std::to_string(_LoadMesh->nNormals));
		_LoadMesh->normals = new Vector[_LoadMesh->nNormals];
		for (int i = 0; i < _LoadMesh->nNormals; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->normals[i].x = TextToNum(data);
			_LoadMesh->vertices[i].nX = TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->normals[i].y = TextToNum(data);
			_LoadMesh->vertices[i].nY = TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->normals[i].z = TextToNum(data);
			_LoadMesh->vertices[i].nZ = TextToNum(data);
			fin.get();//eats the comma or the semicolon at the end
		}

		_LoadMesh->faceNormals = new Face[_LoadMesh->nFaces];
		for (uint32 i = 0; i < _LoadMesh->nFaces; ++i)
		{
			Find(';');
			fin.getline(data, TEXT_BUFFER, ',');
			_LoadMesh->faceNormals[i][0] = (uint16)TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ',');
			_LoadMesh->faceNormals[i][1] = (uint16)TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			_LoadMesh->faceNormals[i][2] = (uint16)TextToNum(data);
			fin.get(); //eats either the comma or the semicolon at the end of each face description
			//logger->LogAll(Logger::DEBUG, "SuperXLoader: Face Normal index ", std::to_string(i) + ": ", std::to_string(_LoadMesh->faceNormals[i].data[0]), " ", std::to_string(_LoadMesh->faceNormals[i].data[1]), " ", std::to_string(_LoadMesh->faceNormals[i].data[2]));
		}

		Find('}');
	}

	void SuperXLoader::ProcessMeshMaterials()
	{
		int16 token;
		char data[TEXT_BUFFER];

		Find('{');

		fin.getline(data, TEXT_BUFFER, ';');
		_LoadMesh->nMaterials = (uint16)TextToNum(data);
		//logger->Log(Logger::DEBUG, "SuperXLoader: Number of Materials: " + std::to_string(_LoadMesh->nMaterials));

		fin.getline(data, TEXT_BUFFER, ';');
		uint16 nFaces = (uint16)TextToNum(data);
		_LoadMesh->faceMaterials = new uint16[nFaces];
		for (uint16 i = 0; i < nFaces - 1; i++)
		{
			fin.getline(data, TEXT_BUFFER, ',');
			_LoadMesh->faceMaterials[i] = (uint16)TextToNum(data);
		}
		fin.getline(data, TEXT_BUFFER, ';');
		_LoadMesh->faceMaterials[nFaces - 1] = (uint16)TextToNum(data);
		if (fin.peek() == ';')
		{
			fin.get(); //eats the last semicolon if it's there
		}

		token = X_COMMENT;
		while (token != X_EBRACE)
		{
			token = ProcessBlock();
			switch (token)
			{
			case X_OBRACE:
				Find('{');
				if (fin.peek() == ' ')
				{
					fin.get();
				}
				fin.getline(data, TEXT_BUFFER, ' ');
				//logger->Log(Logger::DEBUG, "SuperXLoader: Interesting OBRACE that we should probably handle: " + std::string(data));
				if (globalMaterials[std::string(data)] != NULL)
				{
					//logger->Log(Logger::DEBUG, "SuperXLoader: We found a material that is globally known!");
					_LoadMesh->materials.push_back(globalMaterials[std::string(data)]);//we don't have to do anything with special numbers to indicate which face uses which material, as it's always in order.
				}
				Find('}');
				break;
			case X_COMMENT:
				break; //used for spaces and other kind of comments
			case X_EBRACE:
				return; //this is the end
				break;
			case X_MATERIAL:
				ProcessMaterial(false);
				//logger->Log(Logger::DEBUG, "SuperXLoader: done processing a non-global material...");
				break;
			default:
				AvoidTemplate();
				break;
			}
		}
	}

	void SuperXLoader::ProcessMaterial(bool global)
	{
		int16 token;
		char data[TEXT_BUFFER];

		Material* NewMaterial = new Material;

		if (fin.peek() != '{')
		{
			fin >> NewMaterial->name;
		}
		else
		{
			NewMaterial->name = "Random glitters";
		}

		Find('{');

		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->diffuse.r = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->diffuse.g = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->diffuse.b = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->diffuse.a = TextToNum(data);

		fin.get(); //eats the last semicolon
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->power = TextToNum(data);

		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->specular.r = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->specular.g = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->specular.b = TextToNum(data);

		fin.get();//eats the last semicolon

		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->emissive.r = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->emissive.g = TextToNum(data);
		fin.getline(data, TEXT_BUFFER, ';');
		NewMaterial->emissive.b = TextToNum(data);

		fin.get();//eats the last semicolon

		token = X_COMMENT;
		while (token != X_EBRACE)
		{
			token = ProcessBlock();
			switch (token)
			{
			case X_COMMENT:
				break; //used for spaces and other kind of comments
			case X_EBRACE:
				if (global)
				{
					globalMaterials[NewMaterial->name] = NewMaterial;
				}
				else
				{
					_LoadMesh->materials.push_back(NewMaterial);
				}
				return; //this is the end
				break;
			case X_TEXTUREFILENAME:
				Find('{');
				Find('"');
				fin.getline(data, TEXT_BUFFER, '"');
				NewMaterial->texturePath = data;
				//logger->Log(Logger::DEBUG, "SuperXLoader: texturePath: " + std::string(data));
				Find('}');
				break;
			default:
				AvoidTemplate();
				break;
			}
		}
		if (global)//never reaches?
		{
			globalMaterials[NewMaterial->name] = NewMaterial;
		}
		else
		{
			_LoadMesh->materials.push_back(NewMaterial);
		}
	}

	void SuperXLoader::ProcessSkinWeights(void)
	{
		Bone* cBone;
		std::string temp;
		char data[TEXT_BUFFER];

		Find('{');
		Find('"');
		fin.getline(data, TEXT_BUFFER, '"');
		temp = data;
		cBone = _LoadSkeleton->IsName(temp);
		if (cBone == NULL)
		{
			//logger->Log(Logger::DEBUG, "We found skinweights for bone " + temp + " but we don't have that bone yet!");
			unlinkedSkinnedBones[temp] = new Bone();
			cBone = unlinkedSkinnedBones[temp];
			cBone->name = temp;
			cBone->meshName = _LoadMesh->name;
		}
		//logger->Log(Logger::DEBUG, "SuperXLoader: Skinning bone: " + cBone->name);
		Find(';');

		fin.getline(data, TEXT_BUFFER, ';');
		cBone->nVertices = (uint16)TextToNum(data);
		cBone->vertices = new uint16[cBone->nVertices];
		for (uint32 i = 0; i < cBone->nVertices - 1; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ',');
			cBone->vertices[i] = (uint16)TextToNum(data);
			//logger->LogAll(Logger::DEBUG, "SuperXLoader: Vertex: ", atoi(Data));
		}
		fin.getline(data, TEXT_BUFFER, ';');
		cBone->vertices[cBone->nVertices - 1] = (uint16)TextToNum(data);
		//logger->LogAll(Logger::DEBUG, "SuperXLoader: Vertex: ", atoi(Data));

		cBone->weights = new float[cBone->nVertices];
		for (uint32 i = 0; i < cBone->nVertices - 1; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ',');
			cBone->weights[i] = TextToNum(data);
			//logger->LogAll(Logger::DEBUG, "SuperXLoader: Weight: ", atof(Data));
		}
		fin.getline(data, TEXT_BUFFER, ';');
		cBone->weights[cBone->nVertices - 1] = TextToNum(data);
		//logger->Log(Logger::DEBUG, "SuperXLoader: Weight: " + std::to_string(atof(data)));

		for (int i = 0; i < 15; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ',');
			cBone->_SkinOffset[i] = TextToNum(data);
		}
		fin.getline(data, TEXT_BUFFER, ';');
		cBone->_SkinOffset[15] = TextToNum(data);
		Find('}');
	}

	void SuperXLoader::ProcessAnimationSets(void)
	{
		int16 token;

		maxKey = 0;
		_LoadAnimationSet = new AnimationSet;

		token = fin.peek();
		if (token != '{')
		{
			fin >> _LoadAnimationSet->name;
		}
		else
		{
			_LoadAnimationSet->name = SetUID('A');
		}

		Find('{');
		//logger->Log(Logger::DEBUG, "SuperXLoader: Animation Set: " + _LoadAnimationSet->name);

		token = X_COMMENT;
		while (token != X_EBRACE)
		{
			token = ProcessBlock();
			switch (token)
			{
			case X_COMMENT:
				break; //used for spaces and other kind of comments
			case X_EBRACE:
				_LoadAnimationSet->maxKey = maxKey;
				//logger->Log(Logger::DEBUG, "SuperXLoader: MaxKey: " + std::to_string(maxKey));
				_Object->animationsets.push_back(_LoadAnimationSet);
				return; //this is the end
			case X_ANIMATION:
				ProcessAnimations(_LoadAnimationSet);
				break;
			default:
				AvoidTemplate();
				break;
			}
		}
		_LoadAnimationSet->maxKey = maxKey;
		//logger->Log(Logger::DEBUG, "SuperXLoader: MaxKey: " + std::to_string(maxKey));
		_Object->animationsets.push_back(_LoadAnimationSet);
	}

	void SuperXLoader::ProcessAnimations(AnimationSet* &pAS)
	{
		int16 token;
		char data[TEXT_BUFFER];
		Animation* tempAnimation = new Animation;

		Find('{');

		token = X_COMMENT;
		while (token != X_EBRACE)
		{
			token = ProcessBlock();
			switch (token)
			{
			case X_COMMENT:
				break; //used for spaces and other kind of comments
			case X_EBRACE:
				pAS->animations.push_back(tempAnimation);
				return; //this is the end
				break;
			case X_OBRACE:
				Find('{');
				fin.getline(data, TEXT_BUFFER, '}');
				Remove(' ', data);
				tempAnimation->boneName = data;
				//logger->Log(Logger::DEBUG, "SuperXLoader: Animated Bone: " + tempAnimation->boneName);
				break;
			case X_ANIMATIONKEY:
				ProcessAnimationKeys(tempAnimation);
				break;
			default:
				AvoidTemplate(); break;
			}
		}
		pAS->animations.push_back(tempAnimation);
	}

	void SuperXLoader::ProcessAnimationKeys(Animation* &pA)
	{
		int type, size;
		char data[TEXT_BUFFER];
		RotateKey* tempRot;
		ScaleKey* tempScale;
		PositionKey* tempPos;
		MatrixKey* tempMatrix;

		Find('{');
		fin.getline(data, TEXT_BUFFER, ';');
		type = (uint16)atoi(data);
		fin.getline(data, TEXT_BUFFER, ';');
		size = (uint16)atoi(data);

		switch (type)
		{
		case 0:
			//logger->Log(Logger::DEBUG, "SuperXLoader: " + std::to_string(size) + " Rotation Keys");
			pA->rotations.reserve(size);
			while (size--)
			{
				tempRot = new RotateKey;
				fin.getline(data, TEXT_BUFFER, ';');
				tempRot->Time = (uint16)TextToNum(data);
				if (tempRot->Time > maxKey)
				{
					maxKey = tempRot->Time;
				}
				Find(';');
				fin.getline(data, TEXT_BUFFER, ',');
				tempRot->Rotation[0] = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ',');
				tempRot->Rotation[1] = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ',');
				tempRot->Rotation[2] = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ';');
				tempRot->Rotation[3] = TextToNum(data);
				Find(';');
				fin.get();
				pA->rotations.push_back(tempRot);
			}
			break;
		case 1:
			//logger->Log(Logger::DEBUG, "SuperXLoader: " + std::to_string(size) + " Scaling Keys");
			pA->scalings.reserve(size);
			while (size--)
			{
				tempScale = new ScaleKey;
				fin.getline(data, TEXT_BUFFER, ';');
				tempScale->Time = (uint16)TextToNum(data);
				if (tempScale->Time > maxKey)
				{
					maxKey = tempScale->Time;
				}
				Find(';');
				fin.getline(data, TEXT_BUFFER, ',');
				tempScale->Scale.x = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ',');
				tempScale->Scale.y = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ';');
				tempScale->Scale.z = TextToNum(data);
				Find(';');
				fin.get();
				pA->scalings.push_back(tempScale);
			}
			break;
		case 2:
			//logger->Log(Logger::DEBUG, "SuperXLoader: " + std::to_string(size) + " Position Keys");
			pA->translations.reserve(size);
			while (size--)
			{
				tempPos = new PositionKey;
				fin.getline(data, TEXT_BUFFER, ';');
				tempPos->Time = (uint16)TextToNum(data);
				if (tempPos->Time > maxKey)
				{
					maxKey = tempPos->Time;
				}
				Find(';');
				fin.getline(data, TEXT_BUFFER, ',');
				tempPos->Translation.x = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ',');
				tempPos->Translation.y = TextToNum(data);
				fin.getline(data, TEXT_BUFFER, ';');
				tempPos->Translation.z = TextToNum(data);
				Find(';');
				fin.get();
				pA->translations.push_back(tempPos);
			}
			break;
		case 4:
			//logger->Log(Logger::DEBUG, "SuperXLoader: " + std::to_string(size) + " Matrix Keys");
			pA->matrices.reserve(size);
			while (size--)
			{
				tempMatrix = new MatrixKey;
				fin.getline(data, TEXT_BUFFER, ';');
				tempMatrix->Time = (uint16)TextToNum(data);
				if (tempMatrix->Time > maxKey)
				{
					maxKey = tempMatrix->Time;
				}
				Find(';');
				for (int i = 0; i < 15; ++i)
				{
					fin.getline(data, TEXT_BUFFER, ',');
					tempMatrix->Matrix[i] = TextToNum(data);
				}
				fin.getline(data, TEXT_BUFFER, ';');
				tempMatrix->Matrix[15] = TextToNum(data);
				Find(';');
				fin.get();
				pA->matrices.push_back(tempMatrix);
			}
			break;
		default:
			//logger->Log(Logger::DEBUG, "SuperXLoader: Unknown Type: " + std::to_string(type));
			break;
		}
		Find('}');
	}

	void SuperXLoader::MapMeshToBones(Bone* &pBone)
	{
		if (pBone->meshName.empty())
		{
			pBone->meshName = _LoadMesh->name;
		}
		//logger->Log(Logger::DEBUG, "SuperXLoader: Bone " + pBone->name + " is linked to mesh " + pBone->meshName);
		if (!pBone->bones.empty())
		{
			for (std::list<Bone*>::iterator i = pBone->bones.begin(); i != pBone->bones.end(); ++i)
			{
				if ((*i)->meshName.empty())
				{
					(*i)->meshName = pBone->meshName;
				}
				MapMeshToBones(*i);
			}
		}
	}

	void SuperXLoader::ProcessDeclData()
	{
		//logger->Log(Logger::DEBUG, "Processing DeclData...");
		Find('{');

		int currentTextureCoordinateSet = 0;//needed if we find texture coordinates in our decldata block...
		int currentNormalSet = 0;//needed if we find normals in our decldata block...

		char data[TEXT_BUFFER];

		fin.getline(data, TEXT_BUFFER, ';');
		unsigned int additionalVertexElements = (unsigned int)TextToNum(data);
		int* declTypes = new int[additionalVertexElements];
		int* declMethods = new int[additionalVertexElements];
		int* declUsages = new int[additionalVertexElements];
		int* declUsageIndices = new int[additionalVertexElements];
		for (unsigned int i = 0; i < additionalVertexElements; ++i)
		{
			fin.getline(data, TEXT_BUFFER, ';');
			declTypes[i] = (int)TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			declMethods[i] = (int)TextToNum(data);//anything other than default is not supported by us...
			fin.getline(data, TEXT_BUFFER, ';');
			declUsages[i] = (int)TextToNum(data);
			fin.getline(data, TEXT_BUFFER, ';');
			declUsageIndices[i] = (int)TextToNum(data);//irrelevant for our scope?
			fin.get();//Eat , or ;
			if (declMethods[i] != DECLMETHOD_DEFAULT)
			{
				std::string method;
				switch (declMethods[i])
				{
				case DECLMETHOD_CROSSUV:
					method = "CROSSUV";
					break;
				case DECLMETHOD_LOOKUP:
					method = "LOOKUP";
					break;
				case DECLMETHOD_LOOKUPPRESAMPLED:
					method = "LOOKUPPRESAMPLED";
					break;
				case DECLMETHOD_PARTIALU:
					method = "PARTIALU";
					break;
				case DECLMETHOD_PARTIALV:
					method = "PARTIALV";
					break;
				case DECLMETHOD_UV:
					method = "UV";
					break;
				default:
					method = "UNKNOWN";
					break;
				}
				//logger->Log(Logger::ERR, "SuperXLoader: DeclData(): DECLMETHOD_" + method + " is not supported! The only one supported is DECLMETHOD_DEFAULT. Expect a load of errors!");
				Find('}');
				return;
			}
		}
		fin.getline(data, TEXT_BUFFER, ';');
		unsigned int amountOfDWORDS = (unsigned int)TextToNum(data);
		DWORD* words = new DWORD[amountOfDWORDS];
		for (unsigned int i = 0; i < amountOfDWORDS - 1; ++i)//amountOfDWORDS-1 because the last DWORD ends with ;
		{
			fin.getline(data, TEXT_BUFFER, ',');
			words[i] = std::stoul(std::string(data));
		}
		fin.getline(data, TEXT_BUFFER, ';');
		words[amountOfDWORDS - 1] = std::stoul(std::string(data));

		unsigned int i = 0;
		while (i < amountOfDWORDS)
		{
			for (unsigned int j = 0; j < additionalVertexElements; ++j)
			{
				DWORD component[4];//These blocks can have a maximum of 4 components, also less
				switch (declTypes[j])
				{
				case DECLTYPE_FLOAT1:
					component[0] = words[i];
					++i;
					break;
				case DECLTYPE_FLOAT2:
				case DECLTYPE_SHORT2:
				case DECLTYPE_SHORT2N:
				case DECLTYPE_USHORT2N:
				case DECLTYPE_FLOAT16_2:
					component[0] = words[i];
					component[1] = words[i + 1];
					i += 2;
					break;
				case DECLTYPE_FLOAT3:
				case DECLTYPE_UDEC3:
				case DECLTYPE_DEC3N:
					component[0] = words[i];
					component[1] = words[i + 1];
					component[2] = words[i + 2];
					i += 3;
					break;
				case DECLTYPE_FLOAT4:
				case DECLTYPE_COLOR:
				case DECLTYPE_UBYTE4:
				case DECLTYPE_SHORT4:
				case DECLTYPE_UBYTE4N:
				case DECLTYPE_SHORT4N:
				case DECLTYPE_USHORT4N:
				case DECLTYPE_FLOAT16_4:
					component[0] = words[i];
					component[1] = words[i + 1];
					component[2] = words[i + 2];
					component[3] = words[i + 3];
					i += 4;
					break;
				default:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData(): unknown DECLTYPE, things might be incorrect or crash!");
					break;
				}
				switch (declUsages[j])
				{
				case DECLUSAGE_TEXCOORD:
					/*if (_LoadMesh->textureCoords == NULL)
					{
						_LoadMesh->nTextureCoords = _LoadMesh->nVertices;//technically not every vertex needs to have a texture coordinate...
						//logger->Log(Logger::DEBUG, "SuperXLoader: Number of Texture Coords: " + std::to_string(_LoadMesh->nTextureCoords));
						_LoadMesh->textureCoords = new TCoord[_LoadMesh->nTextureCoords];
					}*/
					_LoadMesh->vertices[currentTextureCoordinateSet].tu = *(float*)&component[0];//convert DWORD to float
					_LoadMesh->vertices[currentTextureCoordinateSet].tv = *(float*)&component[1];
					//logger->Log(Logger::DEBUG, "SuperXLoader: DeclData texture coordinates " + std::to_string(currentTextureCoordinateSet) + ": u: " + std::to_string(*(float*)&component[0]) + "; v: " + std::to_string(*(float*)&component[1]));
					++currentTextureCoordinateSet;
					break;
				case DECLUSAGE_BINORMAL:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_BINORMAL!");
					break;
				case DECLUSAGE_BLENDINDICES:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_BLENDINDICES!");
					break;
				case DECLUSAGE_BLENDWEIGHT:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_BLENDWEIGHT!");
					break;
				case DECLUSAGE_COLOR:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_COLOR!");
					break;
				case DECLUSAGE_DEPTH:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_DEPTH!");
					break;
				case DECLUSAGE_FOG:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_FOG!");
					break;
				case DECLUSAGE_NORMAL:
					_LoadMesh->vertices[currentNormalSet].nX = *(float*)&component[0];//convert DWORD to float
					_LoadMesh->vertices[currentNormalSet].nY = *(float*)&component[1];//convert DWORD to float
					_LoadMesh->vertices[currentNormalSet].nZ = *(float*)&component[2];//convert DWORD to float
					++currentNormalSet;
					break;
				case DECLUSAGE_POSITION:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_POSITION!");
					break;
				case DECLUSAGE_POSITIONT:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_POSITIONT!");
					break;
				case DECLUSAGE_PSIZE:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_PSIZE!");
					break;
				case DECLUSAGE_SAMPLE:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_SAMPLE!");
					break;
				case DECLUSAGE_TANGENT:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_TANGENT!");
					break;
				case DECLUSAGE_TESSFACTOR:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: DECLUSAGE_TESSFACTOR!");
					break;
				default:
					logger->Log(Logger::WARNING, "SuperXLoader: DeclData found unsupported DECLUSAGE: " + std::to_string(declUsages[j]) + " (unknown value)");
					break;
				}
			}
		}
		Find('}');
	}
}