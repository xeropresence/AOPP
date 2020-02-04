#pragma once
#include "Patch.h"
class FramecapPatch : public Patch
{
protected:
	bool _ApplyPatch() override;
	bool _RemovePatch() override;
};

