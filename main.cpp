#include <iostream>
#include <atomic>
#include "RNBO.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

struct PMyCustomData {
	RNBO::CoreObject* rnboObject;
	RNBO::SampleValue** outputs;
	std::atomic<float> amp;
};

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	PMyCustomData* pMyCustomData = (PMyCustomData*)pDevice->pUserData;
	RNBO::CoreObject* rnboObject = pMyCustomData->rnboObject;
	rnboObject->process(nullptr, 0, pMyCustomData->outputs, 1, frameCount);
	float* fOutput = (float*)pOutput;

	float temp = 0;

	for(unsigned int i = 0; i < frameCount; i++) {
		fOutput[i] = static_cast<float>(pMyCustomData->outputs[0][i]);
		temp += abs(fOutput[i]);
	}
	pMyCustomData->amp.store(temp);
}


int main(int argc, const char * argv[]) {

	RNBO::CoreObject rnboObject;
	rnboObject.prepareToProcess(48000, 1024);
	
	PMyCustomData *pMyCustomData = new PMyCustomData();
	pMyCustomData->rnboObject = &rnboObject;

	pMyCustomData->outputs = new RNBO::SampleValue*[1];
	pMyCustomData->outputs[0] = new double[1024];
	
	ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 1;               // Set to 0 to use the device's native channel count.
    config.sampleRate        = 48000;           // Set to 0 to use the device's native sample rate.
    config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = pMyCustomData;   // Can be accessed from the device object (device.pUserData).

    ma_device device;
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        return -1;  // Failed to initialize the device.
    }

    ma_device_start(&device);     // The device is sleeping by default so you'll need to start it manually.

    // Do something here. Probably your program's main loop.
	while(true) {
		std::cout << pMyCustomData->amp.load() << "\n";
	}

    ma_device_uninit(&device);    // This will stop the device so no need to do that manually.

	delete [] pMyCustomData->outputs[0];
	delete [] pMyCustomData->outputs;
	return 0;
}