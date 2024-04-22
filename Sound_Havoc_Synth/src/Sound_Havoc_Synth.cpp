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

#define u8 uint8_t
#define u32 uint32_t

enum ADSRPorts {
    ADSR_ATTACK = 0,
    ADSR_DECAY = 1,
    ADSR_SUSTAIN = 2,
    ADSR_RELEASE = 3,
    ADSR_LEVEL = 4,
    ADSR_NR = 5,
};

enum IOPorts {
    IO_MIDI_IN = 0,
    IO_AUDIO_OUT = 1,
    IO_CONTROL = 2,
    IO_NR = 3
};

struct Urids {

    LV2_URID midi_MidiEvent;

};


/*struct definition*/

class Synth {

    private:

        const LV2_Atom_Sequence* midi_in_ptr;
        float* audio_out_ptr;
        const float *adsr_ptr[ADSR_NR];
        double rate;
        double position;
        LV2_URID_Map *map;
        Urids urids;

        void play(const u32 start, const u32 end) {

            for(u32 i=start; i < end; i++) {

                //pass

            }

        }


    public:

        Synth(const double sample_rate, const LV2_Feature *const *features) {

            midi_in_ptr = (nullptr);
            audio_out_ptr = (float*) nullptr;

            for (u32 i=0; i < ADSR_NR; i++) {

                adsr_ptr[i] = (nullptr);
            
            }
            
            rate = sample_rate;
            position = 0.0;

            map = (nullptr);

            const char *missing = lv2_features_query(
                features,
                LV2_URID__map, &map, true,
                NULL
            );

            if (missing) {

                throw;
            
            }

            urids.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
            
        }

        //-Synth(); i use the default c++ destructor
        
        void connectPort(const u32 port, void* data_location) {

            switch (port) {

                case IO_MIDI_IN:
                    midi_in_ptr = (const LV2_Atom_Sequence*) data_location;
                    break;

                case IO_AUDIO_OUT:
                    audio_out_ptr = (float*) data_location;
                    break;

                default:

                    if (port < IO_CONTROL + ADSR_NR) {

                        adsr_ptr[port - IO_CONTROL] = (const float*) data_location;
                    
                    }
                    

                    break;

            }

        }
        void activate() {

            /*position = 0.0;*/

        }

        void run(const u32 sample_count) {

            if ((!audio_out_ptr) || (!midi_in_ptr)) {

                return;
            
            }

            for (u32 i=0; i < ADSR_NR; i++) {

                if ((!adsr_ptr[i])) {
                    
                    return;
                
                }
                
            
            }

            u32 last_frame = 0;
            LV2_ATOM_SEQUENCE_FOREACH(midi_in_ptr, ev) {

                const u32 frame = ev->time.frames;
                play(last_frame, frame);
                last_frame = frame;

                if (ev->body.type = urids.midi_MidiEvent) {

                    const u8* const msg = (const u8*) (ev + 1);

                    const u8 type = lv2_midi_message_type(msg);

                    switch (type) {

                        case LV2_MIDI_MSG_NOTE_ON:
                            /* code */
                            break;

                        case LV2_MIDI_MSG_NOTE_OFF:
                            /* code */
                            break;

                        case LV2_MIDI_MSG_CONTROLLER:
                            /* code */
                            break;

                        default:
                            break;
                    
                    }
                
                }
                

            }
            
            play(last_frame, sample_count);
            

        }

        //void deactivate(); for now not implemented
};


/*core methods*/
//const char *URI
static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {

    Synth* ng = new Synth(sample_rate, features);

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