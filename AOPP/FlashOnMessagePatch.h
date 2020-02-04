#pragma once
#include "Patch.h"
 class FlashOnMessagePatch :public Patch
{
protected:
	bool _ApplyPatch() override;
	//bool _RemovePatch() override;
	
};




