#pragma once
#include <xx_task.h>
#include <xx_data_shared.h>
#include <xx_string.h>

// todo: enhance stop func. do not need Start any more
// todo: seek old one when pool is empty ?

// todo: when __EMSCRIPTEN__, use openAL impl same interface
// known issue1: miniaudio in web, size +300k, some js error, can't enable --closure 1
// known issue2: ios web browser no sound

#define MA_NO_WAV				// -50k
#define MA_NO_FLAC				// -100k
#define MA_NO_MP3				// -50k
#define MA_NO_RESOURCE_MANAGER	// -30k
#define MA_NO_NODE_GRAPH		// -40k	/ 1xxk
#define MA_NO_GENERATION		// -12k
#include "miniaudio.h"

//#define AUDIO_SHOW_CONSOLE_LOG

struct AudioItem {
	xx::DataShared data;
	float volume{};
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
	xx::Shared<bool> pauseFlag, stopFlag;
};

struct AudioDevice {
	ma_device device;
	AudioItemWithDecoder ctx;

	AudioDevice(AudioDevice const&) = delete;
	AudioDevice& operator=(AudioDevice const&) = delete;
	AudioDevice() {
		auto deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = ma_format_f32;// decoder.outputFormat;
		deviceConfig.playback.channels = 1;//decoder.outputChannels;
		deviceConfig.sampleRate = 22050;//decoder.outputSampleRate;
		deviceConfig.pUserData = nullptr;// &ctx;
		deviceConfig.dataCallback = AudioDeviceCallback;
		if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
#ifdef AUDIO_SHOW_CONSOLE_LOG
			xx::CoutN("Failed to init ma device.");
#endif
			throw - 1;
		}
		if (ma_device_start(&device) != MA_SUCCESS) {
#ifdef AUDIO_SHOW_CONSOLE_LOG
			xx::CoutN("Failed to start ma device.");
#endif
			throw - 2;
		}
	}

	~AudioDevice() {
		ma_device_uninit(&device);
	}

	inline static void AudioDeviceCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
		if (auto ctx = (AudioItemWithDecoder*)pDevice->pUserData) {
			if (ctx->stopFlag && *ctx->stopFlag) {
				pDevice->pUserData = nullptr;	// task stop flag
				return;
			}
			if (ctx->pauseFlag && *ctx->pauseFlag) return;
			if (ma_decoder_read_pcm_frames(&ctx->decoder, pOutput, frameCount, &ctx->frameCount) != MA_SUCCESS
				|| frameCount > ctx->frameCount ) {
				if (ctx->item.isLoop) {
					if (ma_decoder_seek_to_pcm_frame(&ctx->decoder, 0) == MA_SUCCESS) {
						ctx->frameCount = 0;
						if (ma_decoder_read_pcm_frames(&ctx->decoder, pOutput, frameCount, &ctx->frameCount) == MA_SUCCESS) return;
					}
				}
				pDevice->pUserData = nullptr;	// task stop flag
			}
		}
	};
};

struct Audio {
	xx::Listi32<xx::Shared<AudioDevice>> devices;	// pool

	void Init(int deviceCap = 8) {
		for (int i = 0; i < deviceCap; ++i) {
			devices.Emplace().Emplace();
		}
		pauseFlag.Emplace();
		stopFlag.Emplace();
	}

	xx::Shared<bool> pauseFlag;			// need init
	xx::Shared<bool> stopFlag;			// need init

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

	// volume = 0 ~ 1
	void Play(xx::DataShared d, float volume = 1, bool isLoop = false) {
		for (auto& o : tmpItems) {
			if (o.data.h == d.h) {
				++o.count;
				return;
			}
		}
		tmpItems.Emplace(AudioItem{ std::move(d), volume, isLoop, 1 });
	}

	void Stop() {
		tmpItems.Clear();
		*stopFlag = true;
	}

	void Start() {
		*stopFlag = false;
	}

	void Update() {
		for(auto& o : tmpItems) {
			maTasks.Add(([&](AudioItem o)->xx::Task<> {
				// alloc device
				if (devices.Empty()) co_return;
				auto d = std::move(devices.Back());
				devices.PopBack();
				auto sgDevice = xx::MakeScopeGuard([&] {
					devices.Add(std::move(d));
#ifdef AUDIO_SHOW_CONSOLE_LOG
					xx::CoutN("devices.Add(std::move(d));");
#endif
				});
				auto& device = d->device;
				auto& ctx = d->ctx;

				// init device
				ctx.item = std::move(o);
				ctx.pauseFlag = pauseFlag;
				ctx.stopFlag = stopFlag;
				auto& decoder = ctx.decoder;
				auto& data = ctx.item.data;

				auto r = ma_decoder_init_memory(data.GetBuf(), data.GetLen(), nullptr, &decoder);
				auto sgDecoder = xx::MakeScopeGuard([&] {
					ma_decoder_uninit(&decoder);
#ifdef AUDIO_SHOW_CONSOLE_LOG
					xx::CoutN("ma_decoder_uninit(&decoder);");
#endif
				});

				if (r != MA_SUCCESS) {
#ifdef AUDIO_SHOW_CONSOLE_LOG
					xx::CoutN("error init decoder");
#endif
					co_return;
				}

				// play count map to volume
				auto vol = 0.3f + (float)ctx.item.count / 5 * 0.7f;		// todo: get global volumen settings
				ma_device_set_master_volume(&device, vol * ctx.item.volume);

				device.pUserData = &ctx;					// tell callback func start

				while (device.pUserData) {
					co_yield 0;
				}

#ifdef AUDIO_SHOW_CONSOLE_LOG
				xx::CoutN("Audio Task End.");
#endif
				co_return;
			})(std::move(o)));
		}
		tmpItems.Clear();
		maTasks();
	}
};
