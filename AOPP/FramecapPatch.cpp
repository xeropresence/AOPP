#include "FramecapPatch.h"

bool FramecapPatch::_ApplyPatch()
{
	const auto frameRate = GetAddress<float*>("AFCM", "?m_vMaximumFramerate@Timer_t@@2MA");

	*frameRate = 144.0f;

	return true;
}

bool FramecapPatch::_RemovePatch()
{
	const auto frameRate = GetAddress<float*>("AFCM", "?m_vMaximumFramerate@Timer_t@@2MA");

	*frameRate = 100.0f;

	return true;
}
