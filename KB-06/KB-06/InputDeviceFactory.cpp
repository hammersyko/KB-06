#include "InputDeviceFactory.h"

namespace pengine
{
	InputDeviceFactory::InputDeviceFactory()
	{
		logger = LoggerPool::GetInstance().GetLogger();
	}

	InputDeviceFactory::~InputDeviceFactory()
	{
		LoggerPool::GetInstance().ReturnLogger(logger);
	}
}