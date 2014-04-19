#include "tests.hpp"
#include "modules.hpp"
#include "../utils/tools.hpp"

#include "libavcodec/avcodec.h" //FIXME: there should be none of the modules include at the application level

#include "decode/libav_decode.hpp"
#include "demux/libav_demux.hpp"
#include "encode/libav_encode.hpp"
#include "mux/libav_mux.hpp"
#include "mux/gpac_mux_mp4.hpp"
#include "out/null.hpp"


using namespace Tests;
using namespace Modules;
using namespace MM;

namespace {

unittest("transcoder async: video simple (gpac mux)") {
	auto demux = uptr(Demux::LibavDemux::create("data/BatmanHD_1000kbit_mpeg_0_20_frag_1000.mp4"));
	//FIXME: doesn't forward decoder props: auto demux = uptr(new Reorder(Demux::LibavDemux::create("data/BatmanHD_1000kbit_mpeg_0_20_frag_1000.mp4")));

	//create stub output (for unused demuxer's outputs)
	auto null = uptr(Out::Null::create());

	//find video signal from demux
	size_t videoIndex = std::numeric_limits<size_t>::max();
	for (size_t i = 0; i < demux->getNumPin(); ++i) {
		auto props = demux->getPin(i)->getProps();
		PropsDecoder *decoderProps = dynamic_cast<PropsDecoder*>(props);
		ASSERT(decoderProps);
		if (decoderProps->getAVCodecContext()->codec_type == AVMEDIA_TYPE_VIDEO) { //TODO: expose it somewhere
			videoIndex = i;
		} else {
			ConnectPin(demux->getPin(i), null.get(), &Out::Null::process); //FIXME: this is a stub to void the assert of not connected signals...
		}
	}
	ASSERT(videoIndex != std::numeric_limits<size_t>::max());

	//create the video decoder
	auto props = demux->getPin(videoIndex)->getProps();
	PropsDecoder *decoderProps = dynamic_cast<PropsDecoder*>(props);

	auto decode = uptr(new Reorder(Decode::LibavDecode::create(*decoderProps)));
	auto encode_delegate = Encode::LibavEncode::create(Encode::LibavEncode::Video);
	auto encode = uptr(new Reorder(encode_delegate));
	auto mux_delegate = Mux::GPACMuxMP4::create("output_video_gpac");
	auto mux = uptr(new Reorder(mux_delegate));

	//pass meta data between encoder and mux
	Connect(encode_delegate->declareStream, mux_delegate, &Mux::GPACMuxMP4::declareStream);
	encode_delegate->sendOutputPinsInfo();

	ConnectPin(demux->getPin(videoIndex), decode.get(), &Reorder::process);
	ConnectPin(decode->getPin(0), encode.get(), &Reorder::process);
	ConnectPin(encode->getPin(0), mux.get(), &Reorder::process);

	while (demux->process(nullptr)) {
	}

	demux->waitForCompletion();
	decode->waitForCompletion();
	encode->waitForCompletion();
	mux->waitForCompletion();
}

}