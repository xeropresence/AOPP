#pragma once
#include "Patch.h"
class LargeAddressAwarePatch :public Patch
{
public:

protected:
	bool _ApplyPatch() override;
	
};

