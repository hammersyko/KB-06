#ifndef _RESOURCE_MTLLOADER_H_
#define _RESOURCE_MTLLOADER_H_

#include "BaseMaterialLoader.h"
#include "Material.h"
#include <map>

namespace pengine
{
	class MtlLoader : public BaseMaterialLoader
	{
	public:
		std::map<std::string, Material> Load(const std::string file);
		std::string GetExtension();
	};
}
#endif

