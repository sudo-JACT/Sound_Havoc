/*libs*/
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <lv2.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/urid/urid.h>
#include <lv2/midi/midi.h>
#include <lv2/core/lv2_util.h>

#define u32 uint32_t

enum ControlPorts {

    CONTROL_ATTACK = 0,
    CONTROL_DECAY = 1,
    CONTROL_SUSTAIN = 2,
    CONTROL_RELEASE = 3,
    CONTROL_LEVEL = 4,
    CONTROL_NR = 5,
};

/*struct definition*/

class Synth {

    private:

        const LV2_Atom_Sequence* midi_in_ptr;
        float* audio_out_ptr;
        float* freq_ptr;
        /*const float* attack_ptr;
        const float* decay_ptr;
        const float* sustain_ptr;
        const float* release_ptr;
        const float* level_ptr;*/
        const float* control_ptr[CONTROL_NR];
        double rate;
        double position;


    public:

        Synth(const double sample_rate) {

            audio_out_ptr = (float*) nullptr;
            freq_ptr = (float *) nullptr;
            level_ptr = (float *) nullptr;
            rate = sample_rate;
            position = 0.0;

        }

        //-Synth(); i use the default c++ destructor
        
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

    Synth* ng = new Synth(sample_rate);

    return ng;

}

static void connect_port(LV2_Handle instance, uint32_t port, void *data_location) {

    Synth* ng = (Synth*) instance;

    if(!ng) {

        return;

    }

    ng->connectPort(port, data_location);


}

static void activate(LV2_Handle instance) {

    Synth* ng = (Synth*) instance;

    if (!ng) {

        return;
    
    }

    ng->activate();
    

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Synth* ng = (Synth*) instance;

    if (!ng) {

        return;
    
    }

    ng->run(sample_count);

}

static void deactivate(LV2_Handle instance) {

    //pass

}

static void cleanup(LV2_Handle instance) {

    Synth *ng = (Synth *)instance;

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

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_Synth/src",
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