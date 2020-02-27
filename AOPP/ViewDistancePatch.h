#pragma once
#include "Patch.h"
class ViewDistancePatch :public Patch
{
public:
	ViewDistancePatch();

private:
	void updateViewDistance(double newValue) const;

	double* viewDistance = nullptr;
	double defaultViewDistance = 1000;

protected:
	bool _ApplyPatch() override;
	bool _RemovePatch() override;
};

