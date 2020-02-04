#pragma once
#include <vector>
#include "Patch.h"


class PatchManager
{
public:
    static PatchManager& getInstance()
    {
        static PatchManager    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return instance;
    }

    

private:
    PatchManager();

   
	
public:
    PatchManager(PatchManager const&) = delete;
    void operator=(PatchManager const&) = delete;

};
