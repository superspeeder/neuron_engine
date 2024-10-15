#pragma once
#include "generic.hpp"
#ifdef __unix__
namespace neuron::synapse {
    class NEURON_API LinuxPlatform : public Platform {
    protected:
        LinuxPlatform();
    public:
        ~LinuxPlatform() override;
    };
}
#endif