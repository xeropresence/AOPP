#pragma once
#include "Patch.h"
class SmartReplyPatch :public Patch
{
protected:
	bool _ApplyPatch() override;
	bool _RemovePatch() override;
};

