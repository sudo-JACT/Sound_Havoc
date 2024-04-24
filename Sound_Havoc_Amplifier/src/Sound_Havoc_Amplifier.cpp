/*libs*/
#include <stdlib.h>
#include <iostream>
#include <new>
#include <stdexcept>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stddef.h>
#include <stdint.h>
#include <lv2.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

using namespace std;

/*struct definition*/

/*typedef struct {

    float* audio_in_ptr;
    float* audio_out_ptr;
    float* amp_ptr;

} Sound_Havoc_Amplifier;*/

class Amplifier {

    private:

        float* audio_in_ptr;
        float* audio_out_ptr;
        float* amp_ptr;

    public:

        Amplifier() {

            audio_in_ptr = static_cast<float*> (nullptr);
            audio_out_ptr = static_cast<float*> (nullptr);
            amp_ptr = static_cast<float*> (nullptr);

        }

        float* getAudioIn() {

            return audio_in_ptr;

        }

        float* getAudioOut() {

            return audio_out_ptr;

        }

        float* getAmp() {

            return amp_ptr;

        }

        void setAudioIn(float* source) {

            audio_in_ptr = static_cast<float*> (source);

        }

        void setAudioOut(float* source) {

            audio_out_ptr = static_cast<float*> (source);

        }

        void setAmp(float* source) {

            amp_ptr = static_cast<float*> (source);

        }

        void run(u32 sample_count) {

            if((!audio_in_ptr) || (!audio_out_ptr) || (!amp_ptr)) {

                return;
    
            }

            for(u32 i=0; i < sample_count; i++) {

                audio_out_ptr[i] = audio_in_ptr[i] * *(amp_ptr);
    
            }

        }

};


/*core methods*/
//const char *URI
static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {

    Amplifier* amp = static_cast<Amplifier*> (nullptr);

    try {
        
        Amplifier* amp = new Amplifier();
    
    }catch(const bad_alloc& b) {

        cerr << "Memory error" << endl;
        return nullptr;
    
    }

    return amp;

}

static void connect_port(LV2_Handle instance, uint32_t port, void *data_location) {

    Amplifier* amp = static_cast<Amplifier*> (instance);

    if(!amp) {

        return;

    }

    float* tmp = static_cast<float*> (data_location);

    switch (port) {

        case 0:
            amp->setAudioIn(tmp);
            break;

        case 1:
            amp->setAudioOut(tmp);
            break;

        case 2:
            amp->setAmp(tmp);
            break;

        default:
            break;
    }

}

static void activate(LV2_Handle instance) {

    //pass

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Amplifier* amp = static_cast<Amplifier*> (instance);

    amp->run(sample_count);
    
    

}

static void deactivate(LV2_Handle instance) {

    //pass

}

static void cleanup(LV2_Handle instance) {

    Amplifier* amp = static_cast<Amplifier*> (instance);

    if(!amp) {

        return;

    }

    delete amp;

}

static const void* extension_data(const char *uri) {

    return NULL;

}


/*descriptor definition*/

static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_Amplifier/src",
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data

};

/*interface*/
extern "C" LV2_Descriptor const *lv2_descriptor(uint32_t index) {

    if(index == 0) {

        return &descriptor;

    }else {

        return NULL;

    }

}