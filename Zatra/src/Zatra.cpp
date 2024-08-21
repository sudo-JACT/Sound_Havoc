#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <lv2.h>
#include <array>
#include <stdexcept>
#include <random>
#include <ctime>
#include <utility>
#include <new>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/urid/urid.h>
#include <lv2/midi/midi.h>
#include <lv2/core/lv2_util.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

using namespace std;

enum PORTS {

    AUDIO_IN = 0,
    AUDIO_OUT = 1,
    GAIN = 2,
    DRIVE = 3,
    COMP = 4,
    Z7mXyO = 5,
    PORTS_NR = 6,

};

class Zatra {

    private:

        float* ports[PORTS_NR];

        float z7mxyO(u32 index) {

            float tmp = *(ports[Z7mXyO]);

            /*if (tmp == 0) {

                return ports[AUDIO_IN][index];
            
            }*/
            

            return 1.0f;

            //ports[AUDIO_IN][i]

        }

        float comp(u32 index) {

            float max = ports[AUDIO_IN][index] - *(ports[COMP]);
            
            if (ports[AUDIO_IN][index] > max) {

                //return max + z7mxyO(index);
                return max * z7mxyO(index);
            
            }else {

                //return ports[AUDIO_IN][index] + z7mxyO(index);
                return ports[AUDIO_IN][index] * z7mxyO(index);

            }
            

        }

        float drive(u32 index) {

            return tanh(comp(index) * *(ports[DRIVE]));

        }

        // Z7mXyO IT'S A FUNCTION THAT MAKES A RANDOM DISTORTION BETWEEN 2 RANDOM NUMBERS

    public:

        Zatra(const double sample_rate, const LV2_Feature* const* features) {

            for (u8 i=0; i < PORTS_NR; i++) {

                ports[i] = static_cast<float*> (nullptr);

            }
            

        }

        void connect(u32 port, void* data_location) {

            switch (port) {

                case AUDIO_IN:
                    ports[AUDIO_IN] = static_cast<float*> (data_location);
                    break;

                case AUDIO_OUT:
                    ports[AUDIO_OUT] = static_cast<float*> (data_location);
                    break;

                case GAIN:
                    ports[GAIN] = static_cast<float*> (data_location);
                    break;

                case DRIVE:
                    ports[GAIN] = static_cast<float*> (data_location);
                    break;

                case COMP:
                    ports[COMP] = static_cast<float*> (data_location);
                    break;

                case Z7mXyO:
                    ports[Z7mXyO] = static_cast<float*> (data_location);
                    break;

                default:
                    break;

            }

        }

        void activate() {

            //pass

        }

        void run(u32 sample_count){

            for (u32 i=0; i < PORTS_NR; i++) {

                if (!(ports[i])) {

                    return;
                
                }
                
            }

            for (u32 i=0; i < sample_count; i++) {

                /*ports[AUDIO_OUT][i] = ports[AUDIO_IN][i] * *(amp->amp_ptr);
                dist->audio_out_ptr[i] = tanh(dist->audio_in_ptr[i] * *(dist->dis_ptr));    */

                ports[AUDIO_OUT][i] = drive(i) * *(ports[GAIN]);
            
            }
            

        }
};














static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {


    Zatra* z4 = static_cast<Zatra*> (nullptr);

    try {

        Zatra* z4 = new Zatra(sample_rate, features);

    }catch(const bad_alloc& b) {

        cerr << "Error while allocating memory" << endl;
        return nullptr;
    
    }catch(const exception& e) {

        cerr << e.what() << endl;
        return nullptr;
    
    }

    return z4;

}

static void connect_port(LV2_Handle instance, uint32_t port, void* data_location) {

    Zatra* z4 = static_cast<Zatra*> (instance);

    if(!z4) {

        return;

    }

    z4->connect(port, data_location);


}

static void activate(LV2_Handle instance) {

    Zatra* z4 = static_cast<Zatra*> (instance);

    if (!z4) {

        return;
    
    }

    z4->activate();
    

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Zatra* z4 = static_cast<Zatra*> (instance);

    if (!z4) {

        return;
    
    }

    z4->run(sample_count);

}

static void deactivate(LV2_Handle instance) {

    //pass

}

static void cleanup(LV2_Handle instance) {

    Zatra* z4 = static_cast<Zatra*> (instance);

    if (!z4) {

        return;

    }

    delete z4;
}

static const void* extension_data(const char *uri) {

    return NULL;

}


/*descriptor definition*/
    
static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Zatra/src",
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data

};

/*interface*/
LV2_Descriptor const *lv2_descriptor(uint32_t index) {

    if(index == 0) {

        return &descriptor;

    }else {

        return NULL;

    }

}