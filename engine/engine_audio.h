#include <xx_task.h>
#include <xx_data_shared.h>
#include <xx_string.h>

// todo: when __EMSCRIPTEN__, use openAL impl same interface
// known issue: miniaudio in web, size +300k, some js error, can't enable --closure 1


#define MA_NO_WAV				// -50k
#define MA_NO_FLAC				// -100k
#define MA_NO_MP3				// -50k
#define MA_NO_RESOURCE_MANAGER	// -30k
#define MA_NO_NODE_GRAPH		// -40k	/ 1xxk
#define MA_NO_GENERATION		// -12k
#include "miniaudio.h"

struct AudioItem {
	xx::DataShared data;
	bool isLoop{};
	int32_t count{};
};
namespace xx {
	template<>
	struct IsPod<AudioItem, void> : std::true_type {};
}

struct AudioItemWithDecoder {
	AudioItem item;
	ma_decoder decoder;
	ma_uint64 frameCount{};
	xx::Shared<bool> pauseFlag;
};

template<bool isLoop>
void AudioDeviceCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	if (auto ctx = (AudioItemWithDecoder*)pDevice->pUserData) {
		if (ctx->pauseFlag && *ctx->pauseFlag) return;
		if (ma_decoder_read_pcm_frames(&ctx->decoder, pOutput, frameCount, &ctx->frameCount) != MA_SUCCESS) {
			if constexpr (isLoop) {
				if (ma_decoder_seek_to_pcm_frame(&ctx->decoder, 0) == MA_SUCCESS) {
					ctx->frameCount = 0;
					if (ma_decoder_read_pcm_frames(&ctx->decoder, pOutput, frameCount, &ctx->frameCount) == MA_SUCCESS) return;
				}
			}
			pDevice->pUserData = nullptr;	// task stop flag
			ma_device_uninit(pDevice);
		}
	}
};

struct Audio {
	int32_t concurrentPlayLimit{ 8 };	// need init
	xx::Shared<bool> pauseFlag;			// need init

	xx::Tasks maTasks;
	xx::Listi<AudioItem> tmpItems;

	// todo: set base volume ? set left, right channel volume when play ?

	void Pause() {
		assert(pauseFlag);
		*pauseFlag = true;
	}
	void Resume() {
		assert(pauseFlag);
		*pauseFlag = false;
	}

	void Play(xx::DataShared d, bool isLoop = false) {
		for (auto& o : tmpItems) {
			if (o.data.h == d.h) {
				++o.count;
				return;
			}
		}
		tmpItems.Emplace(AudioItem{ std::move(d), isLoop, 1 });
	}

	void Stop() {
		tmpItems.Clear();
		maTasks.Clear();
	}

	void Update() {
		for(auto& o : tmpItems) {
			maTasks.Add(([&](AudioItem o)->xx::Task<> {
				AudioItemWithDecoder ctx;
				ctx.item = std::move(o);
				ctx.pauseFlag = pauseFlag;
				auto& decoder = ctx.decoder;
				auto& data = ctx.item.data;

				auto r = ma_decoder_init_memory(data.GetBuf(), data.GetLen(), nullptr, &decoder);
				auto sgDecoder = xx::MakeScopeGuard([&] {
					ma_decoder_uninit(&decoder);
					xx::CoutN("ma_decoder_uninit(&decoder);");
				});

				if (r != MA_SUCCESS) {
					xx::CoutN("error init decoder");
					co_return;
				}

				auto deviceConfig = ma_device_config_init(ma_device_type_playback);
				deviceConfig.playback.format = decoder.outputFormat;
				deviceConfig.playback.channels = decoder.outputChannels;
				deviceConfig.sampleRate = decoder.outputSampleRate;
				deviceConfig.pUserData = &ctx;
				if (ctx.item.isLoop) {
					deviceConfig.dataCallback = AudioDeviceCallback<true>;
				} else {
					deviceConfig.dataCallback = AudioDeviceCallback<false>;
				}

				ma_device device;
				auto sgDevice = xx::MakeScopeGuard([&] {
					ma_device_uninit(&device);
					xx::CoutN("ma_device_uninit(&device);");
				});

				if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
					xx::CoutN("Failed to open playback device.");
					co_return;
				}

				// play count map to volume
				auto vol = 0.3f + (float)ctx.item.count / 5 * 0.7f;		// todo: get global volumen settings
				ma_device_set_master_volume(&device, vol);

				if (ma_device_start(&device) != MA_SUCCESS) {
					xx::CoutN("Failed to start playback device.");
					co_return;
				}

				while (device.pUserData) {
					co_yield 0;
				}

				xx::CoutN("Audio Task End.");

				co_return;
			})(std::move(o)));

			if (maTasks.Count() >= concurrentPlayLimit) break;
		}
		tmpItems.Clear();
		maTasks();
	}
};
