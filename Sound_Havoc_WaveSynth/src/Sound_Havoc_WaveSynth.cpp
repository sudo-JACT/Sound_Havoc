/*libs*/
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <lv2.h>
#include <stdexcept>
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

enum ADSRPorts {
    ADSR_WAVEFORM = 0,
    ADSR_ATTACK = 1,
    ADSR_DECAY = 2,
    ADSR_SUSTAIN = 3,
    ADSR_RELEASE = 4,
    ADSR_LEVEL = 5,
    ADSR_NR = 6,
};

enum IOPorts {
    IO_MIDI_IN = 0,
    IO_AUDIO_OUT = 1,
    IO_CONTROL = 2,
    IO_NR = 3,
};

struct Urids {
    LV2_URID midi_MidiEvent;
};

enum KeyStatus {
    KEY_OFF = 0,
    KEY_PRESSED = 1,
    KEY_RELEASED = 2,
};

struct Envelope {
    double attack;
    double decay;
    float sustain;
    double release;
};

class Key {

    private:

        KeyStatus status;
        u8 note;
        u8 velocity;
        Envelope envelope;
        double rate;
        double position;
        float start_lever;
        double freq;
        double time;

        float adsr() {

            switch (status) {

                case KEY_PRESSED:

                    if(time < envelope.attack) {

                        return start_lever + (1 -start_lever) * time / envelope.attack;

                    }

                    if(time < envelope.attack + envelope.decay) {

                        return 1 + (envelope.sustain - 1) * (time - envelope.attack) / envelope.decay;

                    }

                    return envelope.sustain;

                    break;

                case KEY_RELEASED:

                    return start_lever - start_lever * time / envelope.release;

                    break;

                default:

                    return 0;

                    break;

            }

        }

    public:

        Key(const double rt) {

            status = KEY_OFF;
            note = 0;
            velocity = 0;
            envelope = {0, 0, 0, 0};
            position = 0;
            start_lever = 0;
            rate = rt;
            freq = pow(2, (double (note) - 69) / 12) * 440;
            time = 0;

        }

        void press(u8 nt, const u8 v, const Envelope e) {

            start_lever = adsr();
            note = nt;
            velocity = v;
            envelope = e;
            status = KEY_PRESSED;
            freq = pow(2, (double (note) - 69) / 12) * 440;
            time = 0;

        }

        void release(const u8 nt, const u8 v) {

            if (status == KEY_PRESSED && note == nt) {

                start_lever = adsr();
                status = KEY_RELEASED;
                time = 0;

            }

        }

        void off() {

            position = 0;
            status = KEY_OFF;

        }


        float get() {

            float synth_note = adsr() * sin(2 * M_PI * position) * (float(velocity) / 127);

            return synth_note;

        }

        void proceed() {

            position = freq / rate;
            time = 1 / rate;

            if (status == KEY_RELEASED && time >= envelope.release) {
                
                off();
            
            }
            

        }

};

class WaveSynth {

    private:

        const LV2_Atom_Sequence* midi_in_ptr;
        float* audio_out_ptr;
        const float* adsr_ptr[ADSR_NR];
        double rate;
        double position;
        LV2_URID_Map* map;
        Urids urids;
        Key* key;

        void play(const u32 start, const u32 end) {

            for(u32 i=start; i < end; i++) {

                audio_out_ptr[i] = key->get() * *adsr_ptr[ADSR_LEVEL];
                key->proceed();

            }

        }


    public:

        WaveSynth(const double sample_rate, const LV2_Feature *const *features) {

            midi_in_ptr = static_cast<const LV2_Atom_Sequence*> (nullptr);
            audio_out_ptr = static_cast<float*> (nullptr);

            for (u32 i=0; i < ADSR_NR; i++) {

                adsr_ptr[i] = static_cast<float*> (nullptr);
            
            }
            
            rate = sample_rate;
            position = 0.0;
            key = new Key(rate);

            map = static_cast<LV2_URID_Map*> (nullptr);

            const char *missing = lv2_features_query(
                features,
                LV2_URID__map, &map, true,
                NULL
            );

            if (missing) {

                __throw_invalid_argument("Error while instantiating the plugin");
            
            }

            urids.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
            
        }

        //-WaveSynth(); i use the default c++ destructor
        
        void connectPort(const u32 port, void* data_location) {

            switch (port) {

                case IO_MIDI_IN:
                    midi_in_ptr = static_cast<const LV2_Atom_Sequence*> (data_location);
                    break;

                case IO_AUDIO_OUT:
                    audio_out_ptr = static_cast<float*> (data_location);
                    break;

                default:

                    if (port < IO_CONTROL + ADSR_NR) {

                        adsr_ptr[port - IO_CONTROL] = static_cast<const float*> (data_location);
                    
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
                            
                            key->press(msg[1], msg[2], {*adsr_ptr[ADSR_ATTACK], *adsr_ptr[ADSR_DECAY], *adsr_ptr[ADSR_SUSTAIN], *adsr_ptr[ADSR_RELEASE]});

                            break;

                        case LV2_MIDI_MSG_NOTE_OFF:

                            key->release(msg[1], msg[2]);

                            break;

                        case LV2_MIDI_MSG_CONTROLLER:

                            if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {

                                key->off();
                            
                            }else if(msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {

                                key->off();

                            }
                            

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


    WaveSynth* ws = static_cast<WaveSynth*> (nullptr);

    try {

        WaveSynth *ws = new WaveSynth(sample_rate, features);

    }catch(const __throw_invalid_argument &e) {

        cerr << e.what() << endl;
        return nullptr;
    
    }catch(const bad_alloc& b) {

        cerr << "Error while allocating memory" << endl;
        return nullptr;
    
    }

    return ws;

}

static void connect_port(LV2_Handle instance, uint32_t port, void *data_location) {

    WaveSynth* ws = static_cast<WaveSynth*> (instance);

    if(!ws) {

        return;

    }

    ws->connectPort(port, data_location);


}

static void activate(LV2_Handle instance) {

    WaveSynth* ws = static_cast<WaveSynth*> (instance);

    if (!ws) {

        return;
    
    }

    ws->activate();
    

}

static void run(LV2_Handle instance, uint32_t sample_count) {

    WaveSynth* ws = static_cast<WaveSynth*> (instance);

    if (!ws) {

        return;
    
    }

    ws->run(sample_count);

}

static void deactivate(LV2_Handle instance) {

    //pass

}

static void cleanup(LV2_Handle instance) {

    WaveSynth *ws = static_cast<WaveSynth*> (instance);

    if (!ws)
    {

        return;
    }

    delete ws;
}

static const void* extension_data(const char *uri) {

    return NULL;

}


/*descriptor definition*/

static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_WaveSynth/src",
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