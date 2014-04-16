#include "../utils/log.hpp"
#include "../utils/tools.hpp"
#include "video_generator.hpp"
#include <cmath>

auto const FRAMERATE = 25;

namespace Modules {
namespace In {

VideoGenerator::VideoGenerator()
	: m_numFrames(0) {
	signals.push_back(uptr(pinFactory->createPin()));
}

bool VideoGenerator::process(std::shared_ptr<Data> /*data*/) {
	auto const picSize = VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2;
	std::shared_ptr<PcmData> out(new PcmData(picSize));

	// generate video
	auto const p = out->data();
	auto const FLASH_PERIOD = FRAMERATE;
	auto const flash = (m_numFrames % FLASH_PERIOD) == 0;
	auto const val = flash ? 0xCC : 0x80;
	memset(p, val, picSize);

	auto const framePeriodIn180k = 180000LL / FRAMERATE;
	out->setTime(m_numFrames * framePeriodIn180k);

	if(m_numFrames % 25 < 2)
		signals[0]->emit(out);

	++m_numFrames;
	return true;
}

bool VideoGenerator::handles(const std::string &url) {
	return VideoGenerator::canHandle(url);
}

bool VideoGenerator::canHandle(const std::string &/*url*/) {
	return true;
}

}
}