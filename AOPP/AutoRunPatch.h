#pragma once
#include "Patch.h"

class AutoRunPatch :public Patch
{
protected:
	bool _ApplyPatch() override;

};

