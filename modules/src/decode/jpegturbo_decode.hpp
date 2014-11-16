#pragma once

#include "internal/core/module.hpp"


using namespace Modules;

namespace Decode {

class JPEGTurbo;

class JPEGTurboDecode : public Module {
public:
	JPEGTurboDecode();
	~JPEGTurboDecode();
	void process(std::shared_ptr<const Data> data) override;

private:
	void ensureProps(int width, int height, int pixelFmt);
	std::unique_ptr<JPEGTurbo> const jtHandle;
};

}
