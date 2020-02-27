#pragma once
#include <vector>


#include "CommandInterpreter.h"
#include "ConfigurationHolder.h"
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

    std::vector<std::unique_ptr<Patch>> Patches;

    [[nodiscard]] static bool initialized();

private:
    PatchManager();


    std::unique_ptr<CommandInterpreter> _CommandInterpreter;
    std::unique_ptr<ConfigurationHolder> Configuration;
	
public:
    PatchManager(PatchManager const&) = delete;
    void operator=(PatchManager const&) = delete;

    friend class Patch;
    
};
