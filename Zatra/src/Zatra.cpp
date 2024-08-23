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

/*enum PORTS {

    AUDIO_IN = 0,
    AUDIO_OUT = 1,
    GAIN = 2,
    DRIVE = 3,
    COMP = 4,
    Z7mXyO = 5,
    PORTS_NR = 6,

};*/

/*class Zatra {

    private:

        array<float*, PORTS_NR> ports;

        float z7mxyO(u32 index) {

            float tmp = *(ports[Z7mXyO]);

            //if (tmp == 0) {

                //return ports[AUDIO_IN][index];
            
            //}
            

            return tmp;

            //ports[AUDIO_IN][i]

        }

        float comp(u32 index) {

            float max = ports[AUDIO_IN][index] - (*(ports[COMP]) * 10);
            
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

        Zatra(const double sample_rate) {

            for (u32 i=0; i < PORTS_NR; i++) {

                ports[i] = static_cast<float*> (nullptr);

            }
            

        }

        void connectPort(u32 port, void* data_location) {

            switch (port) {

                case AUDIO_IN:
                    ports[AUDIO_IN] = (float*) data_location;
                    break;

                case AUDIO_OUT:
                    ports[AUDIO_OUT] = (float*) data_location;
                    break;

                case GAIN:
                    ports[GAIN] = (float*) data_location;
                    break;

                case DRIVE:
                    ports[DRIVE] = (float*) data_location;
                    break;

                case COMP:
                    ports[COMP] = (float*) data_location;
                    break;

                case Z7mXyO:
                    ports[Z7mXyO] = (float*) data_location;
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

                //ports[AUDIO_OUT][i] = ports[AUDIO_IN][i] * *(amp->amp_ptr);
                //dist->audio_out_ptr[i] = tanh(dist->audio_in_ptr[i] * *(dist->dis_ptr));    

                ports[AUDIO_OUT][i] = drive(i) * *(ports[GAIN]);
            
            }
            

        }
};
*/



typedef struct {

    float* audio_in;
    float* audio_out;
    float* gain;
    float* drive;
    float* comp;
    float* z7mxyo;

} Zatra;







static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {

    Zatra* z4 = (Zatra*) calloc(1, sizeof(Zatra));

    return z4;

}

static void connect_port(LV2_Handle instance, uint32_t port, void* data_location) {

    Zatra* z4 = (Zatra*) instance;

    if(!z4) {

        return;

    }

    switch (port) {

        case 0:
            z4->audio_in = (float*) data_location;
            break;

        case 1:
            z4->audio_out = (float*) data_location;
            break;

        case 2:
            z4->gain = (float*) data_location;
            break;

        case 3:
            z4->drive = (float*) data_location;
            break;

        case 4:
            z4->comp = (float*) data_location;
            break;

        case 5:
            z4->z7mxyo = (float*) data_location;
            break;

        default:
            break;

    }


}

static void activate(LV2_Handle instance) {

    /*Zatra* z4 = static_cast<Zatra*> (instance);

    if (!z4) {

        return;
    
    }

    z4->activate();*/
    

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Zatra* z4 = (Zatra*) instance;

    if ((!z4) || (!z4->audio_in) || (!z4->audio_out) || (!z4->gain) || (!z4->drive) || (!z4->comp) || (!z4->z7mxyo)) {

        return;
    
    }

    for (u32 i=0; i < sample_count; i++) {

        /*float tmp = *(z4->z7mxyo) + z4->audio_in[i];
        float z7mxyO = 1;

        float max = z4->audio_in[i] - (*(z4->comp) * 10);
        float comp = 1;

        if (z4->audio_in[i] > max) {

            //return max + z7mxyO(index);
            comp = max * z7mxyO;
            
        }else {

            //return ports[AUDIO_IN][index] + z7mxyO(index);
            comp = z4->audio_in[i] * z7mxyO;

        }

        
        float drive = tanh(comp * *(z4->drive));;

        z4->audio_out[i] = drive * *(z4->gain);*/

        z4->audio_out[i] = z4->audio_in[i] * *(z4->gain);
            
    }

}

static void deactivate(LV2_Handle instance) {

    //pass

}

static void cleanup(LV2_Handle instance) {

    Zatra* z4 = (Zatra*) instance;

    if (!z4) {

        return;

    }

    free(z4);
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
const LV2_Descriptor *lv2_descriptor(uint32_t index) {

    if(index == 0) {

        return &descriptor;

    }else {

        return NULL;

    }

}