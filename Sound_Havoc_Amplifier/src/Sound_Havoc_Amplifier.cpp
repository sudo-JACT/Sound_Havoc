/*libs*/
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <lv2.h>
#include <stdexcept>
#include <new>

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

enum Ports {
    AUDIO_IN_PTR = 0,
    AUDIO_OUT_PTR = 1,
    AMP_PTR = 2,
    PTR_RN = 3,
};

class Amplifier {

    private:

        float* ports[PTR_RN];

    public:

        Amplifier() {

            for (u32 i=0; i < PTR_RN; i++) {

                ports[i] = static_cast<float*> (nullptr);
            
            }
            

        }

        void connect_port(u32 port, void* data_location) {

            switch (port) {

                case AUDIO_IN_PTR:
                    ports[AUDIO_IN_PTR] = static_cast<float*> (data_location);
                    break;

                case AUDIO_OUT_PTR:
                    ports[AUDIO_OUT_PTR] = static_cast<float*> (data_location);
                    break;

                case AMP_PTR:
                    ports[AMP_PTR] = static_cast<float*> (data_location);
                    break;

                default:
                    break;
            }
        }

        void run(u32 sample_count) {

            for (u32 i=0; i < PTR_RN; i++) {
                
                if (!ports[i]) {

                    return;
                
                }
                
            
            }
            

            for(u32 i=0; i < sample_count; i++) {

                ports[AUDIO_OUT_PTR][i] = ports[AUDIO_IN_PTR][i] * *(ports[AMP_PTR]);
    
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
    
    amp->connect_port(port, data_location);

}

static void activate(LV2_Handle instance) {

    //pass

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Amplifier* amp = static_cast<Amplifier*> (instance);

    if (!amp) {

        return;
    
    }

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