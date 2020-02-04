#pragma once
#include "Patch.h"
class LargeAddressAwarePatch :public Patch
{
protected:
	bool _ApplyPatch() override;
	
};

