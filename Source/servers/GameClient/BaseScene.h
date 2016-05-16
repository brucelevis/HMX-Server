#ifndef __BASE_SCENE_H_
#define __BASE_SCENE_H_

#include "FKEngineInclude.h"
#include "../../../depend/boost/scoped_ptr.hpp"


class BaseScene 
{
public:
	BaseScene(FK2DEngine::CFKWindow& winhdle);


	virtual void FKDraw() = 0;

	virtual void Close() = 0;

	virtual void KeyDown(FK2DEngine::CKey p_Key)
	{

	}
};


#endif