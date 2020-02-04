#pragma once
#include "Patch.h"
class XButtonDoubleClickPatch : public Patch
{
protected:
	bool _ApplyPatch() override;
	bool _RemovePatch() override;
private:
	int* patch1 = nullptr;
	int* patch2 = nullptr;
	
};

