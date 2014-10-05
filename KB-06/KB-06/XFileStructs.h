/////////////////////////////////////////////////////////
// XFileStructs.h
//
//
/////////////////////////////////////////////////////////

#ifndef _PENGINE_XFILESTRUCTS_H
#define _PENGINE_XFILESTRUCTS_H

namespace pengine
{
#define XOFFILE_FORMAT_MAGIC \
	((long)'x' + ((long)'o' << 8) + ((long)'f' << 16) + ((long)' ' << 24))

#define XOFFILE_FORMAT_VERSION03 \
	((long)'0' + ((long)'3' << 8))

#define XOFFILE_FORMAT_VERSION02 \
	((long)'0' + ((long)'2' << 8))

#define XOFFILE_FORMAT_BINARY \
	((long)'b' + ((long)'i' << 8) + ((long)'n' << 16) + ((long)' ' << 24))

#define XOFFILE_FORMAT_TEXT   \
	((long)'t' + ((long)'x' << 8) + ((long)'t' << 16) + ((long)' ' << 24))

#define XOFFILE_FORMAT_COMPRESSED \
	((long)'c' + ((long)'m' << 8) + ((long)'p' << 16) + ((long)' ' << 24))

#define XOFFILE_FORMAT_FLOAT_BITS_32 \
	((long)'0' + ((long)'0' << 8) + ((long)'3' << 16) + ((long)'2' << 24))

#define XOFFILE_FORMAT_FLOAT_BITS_64 \
	((long)'0' + ((long)'0' << 8) + ((long)'6' << 16) + ((long)'4' << 24))


	struct XFileHeader {
		unsigned long Magic;
		unsigned short Major_Version;
		unsigned short Minor_Version;
		unsigned long Format;
		unsigned long Float_Size;
	};

#define X_NAME         			 1
#define X_STRING       			 2
#define X_INTEGER      			 3
#define X_GUID         			 5
#define X_INTEGER_LIST 			 6
#define X_FLOAT_LIST   			 7

#define X_OBRACE      			 10
#define X_EBRACE      			 11
#define X_OPAREN      			 12
#define X_CPAREN      			 13
#define X_OBRACKET    			 14
#define X_CBRACKET    			 15
#define X_OANGLE      			 16
#define X_CANGLE      			 17
#define X_DOT         			 18
#define X_COMMA       			 19
#define X_SEMICOLON   			 20
#define X_TOKEN_TEMPLATE    	 31
#define X_WORD        			 40
#define X_DWORD       			 41
#define X_FLOAT       			 42
#define X_DOUBLE      			 43
#define X_CHAR        			 44
#define X_UCHAR       			 45
#define X_SWORD       			 46
#define X_SDWORD      			 47
#define X_VOID        			 48
#define X_LPSTR       			 49
#define X_UNICODE     			 50
#define X_CSTRING		     	 51
#define X_ARRAY       			 52

	//Personal tokens
#define X_ERROR					 -1
#define X_TEMPLATE 				 60
#define X_HEADER 				 61
#define X_FRAME 				 62
#define X_FRAMETRANSFORMMATRIX	 63
#define X_MESH 					 64
#define X_MESHTEXTURECOORDS 	 65
#define X_MESHMATERIALLIST		 66
#define X_MATERIAL 				 67
#define X_SKINMESHHEADER 		 68
#define X_SKINWEIGHTS 			 69
#define X_TEXTUREFILENAME		 70
#define X_MESHNORMALS			 71
#define X_ANIMATIONSET			 72
#define X_ANIMATION			 	 73
#define X_ANIMATIONKEY		 	 74
#define X_DECLDATA			 	 75

#define X_COMMENT				 254
#define X_UNKNOWN				 255 //unknown block

#define	D3DDECLTYPE_FLOAT1		0
#define	D3DDECLTYPE_FLOAT2		1
#define	D3DDECLTYPE_FLOAT3		2
#define	D3DDECLTYPE_FLOAT4		3
#define	D3DDECLTYPE_D3DCOLOR	4
#define	D3DDECLTYPE_UBYTE4		5
#define	D3DDECLTYPE_SHORT2		6
#define	D3DDECLTYPE_SHORT4		7
#define	D3DDECLTYPE_UBYTE4N		8
#define	D3DDECLTYPE_SHORT2N		9
#define	D3DDECLTYPE_SHORT4N		10
#define	D3DDECLTYPE_USHORT2N	11
#define	D3DDECLTYPE_USHORT4N	12
#define	D3DDECLTYPE_UDEC3		13
#define	D3DDECLTYPE_DEC3N		14
#define	D3DDECLTYPE_FLOAT16_2	15
#define	D3DDECLTYPE_FLOAT16_4	16
#define	D3DDECLTYPE_UNUSED		17

#define	D3DDECLMETHOD_DEFAULT			0
#define	D3DDECLMETHOD_PARTIALU			1
#define	D3DDECLMETHOD_PARTIALV			2
#define	D3DDECLMETHOD_CROSSUV			3
#define	D3DDECLMETHOD_UV				4
#define	D3DDECLMETHOD_LOOKUP			5
#define	D3DDECLMETHOD_LOOKUPPRESAMPLED  6

#define D3DDECLUSAGE_POSITION		0
#define D3DDECLUSAGE_BLENDWEIGHT	1
#define D3DDECLUSAGE_BLENDINDICES	2
#define D3DDECLUSAGE_NORMAL			3
#define D3DDECLUSAGE_PSIZE			4
#define D3DDECLUSAGE_TEXCOORD		5
#define D3DDECLUSAGE_TANGENT		6
#define D3DDECLUSAGE_BINORMAL		7
#define D3DDECLUSAGE_TESSFACTOR		8
#define D3DDECLUSAGE_POSITIONT		9
#define D3DDECLUSAGE_COLOR			10
#define D3DDECLUSAGE_FOG			11
#define D3DDECLUSAGE_DEPTH			12
#define D3DDECLUSAGE_SAMPLE			13
}
#endif
