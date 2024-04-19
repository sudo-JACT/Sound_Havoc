/*libs*/
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <lv2.h>

#define u32 uint32_t

/*struct definition*/

class Nois_Generator {

    private:

        float* audio_out_ptr;
        float* freq_ptr;
        float* level_ptr;
        double rate;
        double position;


    public:

        Nois_Generator(const double sample_rate) {

            audio_out_ptr = (float*) nullptr;
            freq_ptr = (float *) nullptr;
            level_ptr = (float *) nullptr;
            rate = sample_rate;
            position = 0.0;

        }

        //-Nois_Generator(); i use the default c++ destructor
        
        void connectPort(const u32 port, void* data_location) {

            switch (port) {

                case 0:
                    audio_out_ptr = (float*) data_location;
                    break;

                case 1:
                    freq_ptr = (float*) data_location;
                    break;

                case 2:
                    level_ptr = (float*) data_location;
                    break;

                default:
                    break;

            }

        }
        void activate() {

            position = 0.0;

        }

        void run(const u32 sample_count) {

            if ((!audio_out_ptr) || (!freq_ptr) || (!level_ptr)) {

                return;
            
            }

            for (u32 i=0; i < sample_count; i++) {

                audio_out_ptr[i] = sin(2.0 * M_PI * position) * *level_ptr;
                
                position += *freq_ptr / rate;
            
            }
            
            

        }

        //void deactivate(); for now not implemented
};


/*core methods*/
//const char *URI
static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {

    Nois_Generator* ng = new Nois_Generator(sample_rate);

    return ng;

}

static void connect_port(LV2_Handle instance, uint32_t port, void *data_location) {

    Nois_Generator* ng = (Nois_Generator*) instance;

    if(!ng) {

        return;

    }

    ng->connectPort(port, data_location);


}

static void activate(LV2_Handle instance) {

    Nois_Generator* ng = (Nois_Generator*) instance;

    if (!ng) {

        return;
    
    }

    ng->activate();
    

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Nois_Generator* ng = (Nois_Generator*) instance;

    if (!ng) {

        return;
    
    }

    ng->run(sample_count);

}

static void deactivate(LV2_Handle instance) {

    //pass

}

static void cleanup(LV2_Handle instance) {

    Nois_Generator *ng = (Nois_Generator *)instance;

    if (!ng)
    {

        return;
    }

    delete ng;
}

static const void* extension_data(const char *uri) {

    return NULL;

}


/*descriptor definition*/

static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_Nois_Generator/src",
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