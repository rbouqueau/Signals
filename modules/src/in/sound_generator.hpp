#pragma once

#include "internal/core/module.hpp"

namespace Modules {

namespace In {

class SoundGenerator : public Module {
public:
	SoundGenerator();
	void process(std::shared_ptr<Data> data);

private:
	double nextSample();
	uint64_t m_numSamples;
};

}

}
