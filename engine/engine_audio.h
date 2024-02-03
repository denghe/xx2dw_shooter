#include <xx_task.h>
#include <xx_data_shared.h>
#include <xx_string.h>

#define MA_NO_WAV				// -50k
#define MA_NO_FLAC				// -100k
#define MA_NO_MP3				// -50k
#define MA_NO_RESOURCE_MANAGER	// -30k
#define MA_NO_NODE_GRAPH		// -40k	/ 1xxk
#define MA_NO_GENERATION		// -12k
#include "miniaudio.h"

struct Audio {
	int32_t concurrentPlayLimit{ 8 };

	xx::Tasks maTasks;
	xx::List<xx::DataShared> tmpPlayList;
	xx::List<int32_t> tmpPlayListCounter;

	void Play(xx::DataShared d) {
		for (ptrdiff_t i = 0, e = tmpPlayList.len; i < e; ++i) {
			if (tmpPlayList[i].h == d.h) {
				++tmpPlayListCounter[i];
				return;
			}
		}
		tmpPlayList.Emplace(std::move(d));
		tmpPlayListCounter.Emplace();
	}

	void Update() {
		if (ptrdiff_t e = tmpPlayList.len) {
			for (ptrdiff_t i = 0; i < e; ++i) {
				maTasks.Add(([](xx::DataShared fnData, int32_t counter)->xx::Task<> {

					std::pair<ma_decoder, ma_uint64> decoderAndFrameCount;
					auto& decoder = decoderAndFrameCount.first;

					auto r = ma_decoder_init_memory(fnData.GetBuf(), fnData.GetLen(), nullptr, &decoder);
					auto sgDecoder = xx::MakeScopeGuard([&] { ma_decoder_uninit(&decoder); });

					if (r != MA_SUCCESS) {
						xx::CoutN("error init decoder");
						co_return;
					}

					auto deviceConfig = ma_device_config_init(ma_device_type_playback);
					deviceConfig.playback.format = decoder.outputFormat;
					deviceConfig.playback.channels = decoder.outputChannels;
					deviceConfig.sampleRate = decoder.outputSampleRate;
					deviceConfig.pUserData = &decoderAndFrameCount;
					deviceConfig.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
						if (auto dafc = (std::pair<ma_decoder, ma_uint64>*)pDevice->pUserData) {
							auto r = ma_decoder_read_pcm_frames(&dafc->first, pOutput, frameCount, &dafc->second);
							if (r != MA_SUCCESS) {
								pDevice->pUserData = nullptr;	// stop flag
							}
						}
						};
					deviceConfig.stopCallback = [](ma_device* pDevice) {
						xx::CoutN("stopCallback.\n");
						};

					ma_device device;
					auto sgDevice = xx::MakeScopeGuard([&] { ma_device_uninit(&device); });

					if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
						xx::CoutN("Failed to open playback device.\n");
						co_return;
					}

					// todo: counter map to volume

					if (ma_device_start(&device) != MA_SUCCESS) {
						xx::CoutN("Failed to start playback device.\n");
						co_return;
					}

					while (device.pUserData) {
						co_yield 0;
					}

					xx::CoutN("play sound finished.\n");

					co_return;
					})(std::move(tmpPlayList[i]), tmpPlayListCounter[i]));
				if (maTasks.Count() >= concurrentPlayLimit) break;
			}
			tmpPlayList.Clear();
			tmpPlayListCounter.Clear();
		}
		maTasks();
	}
};
