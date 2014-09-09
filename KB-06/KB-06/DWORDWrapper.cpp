#ifndef _DWORDWRAPPER_CPP_
#define _DWORDWRAPPER_CPP_

#include <d3dx9.h>

class DWORDWrapper
{
public:
	DWORDWrapper::DWORDWrapper(DWORD _dword)
	{
		dword = _dword;
	};

	DWORDWrapper::~DWORDWrapper()
	{

	};

	void DWORDWrapper::setDWORD(DWORD _dword)
	{
		dword = _dword;
	};

	DWORD DWORDWrapper::getDWORD()
	{
		return dword;
	};

private:
	DWORD dword;
};

#endif