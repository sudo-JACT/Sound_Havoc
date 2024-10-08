/*libs*/
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

template <class T>
class LinearFader {

    private:
        T destination_;
        u32 distance_;
        T value_;

    public:
    
        LinearFader() { 

            //pas

        }

        LinearFader(const T destination) {

            destination_ = destination;
            distance_ = 0;
            value_ = destination;

        }

        void set(const T destination, u32 distance){

            destination_ = destination;
            distance_ = distance;

            if (distance == 0) {

                value_ = destination;
            
            }
            

        }

        T get() const {

            return value_;

        }

        void proceed() {

            if (distance_ == 0) {

                value_ = destination_;
            
            }else {

                value_ += (destination_ - value_) * (1.0 / (static_cast<double> (distance_)));
                distance_ -= 1;

            }
            

        }

};


template <class T>
T limit(const T x, const T min, const T max){

    return (x < min ? min : (x > max ? max : x));

}

enum Waveform {

    WAVEFORM_SINE = 0,
    WAVEFORM_TRIANGLE = 1,
    WAVEFORM_SQUARE = 2,
    WAVEFORM_SAW = 3,
    WAVEFORM_NOISE = 4,
    WAVEFORM_NR = 5,
};

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

constexpr array<pair<float, float>, ADSR_NR> adsrLimit = {{

    {0.0f, 4.0f},
    {0.001f, 4.0f},
    {0.001f, 4.0f},
    {0.0f, 1.0f},
    {0.001f, 4.0f},
    {0.0f, 1.0f},

}};

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
        LinearFader<float> feader;
        Waveform waveform;
        minstd_rand rnd;
        uniform_real_distribution<float> dist;

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

        inline float synth() {

            const float p = fmod(position, 1.0);

            switch (waveform) {

                case WAVEFORM_SINE:
                    return sin(2.0 * M_PI * position);
                    break;

                case WAVEFORM_TRIANGLE:
                    return (p < 0.25f ? (4.0f * p) : (p < 0.75f ? (1.0f - (4.0f * (p - 0.25f))) : (-1.0f + (4.0f * (p - 0.75f)))));
                    break;

                case WAVEFORM_SQUARE:
                    return (p < 0.5f ? (1.0f) : (-1.0f));
                    break;

                case WAVEFORM_SAW:
                    return (2.0f * p - 1.0f);
                    break;

                case WAVEFORM_NOISE:
                    return dist(rnd);
                    break;

                default:
                    return 0.0f;
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
            freq = pow(2, (static_cast<double> (note) - 69) / 12) * 440;
            time = 0.0f;
            feader = LinearFader(1.0f);
            waveform = Waveform(WAVEFORM_SINE);
            minstd_rand rnd(time_t (0));
            uniform_real_distribution<float> dist(-1.0f, 1.0f);

        }

        void press(const Waveform wf, u8 nt, const u8 v, const Envelope e) {

            start_lever = adsr();
            note = nt;
            velocity = v;
            envelope = e;
            status = KEY_PRESSED;
            freq = pow(2, (static_cast<double> (note) - 69) / 12) * 440;
            time = 0;
            feader.set(1.0f, 0.0);
            waveform = wf;

        }

        void release(const u8 nt, const u8 v) {

            if (status == KEY_PRESSED && note == nt) {

                start_lever = adsr();
                status = KEY_RELEASED;
                time = 0;

            }

        }

        void release() {

            release(note, velocity);

        }

        void mute() {

            feader.set(.0f, (0.01 * rate));

        }

        void off() {

            position = 0;
            status = KEY_OFF;

        }


        float get() {

            float synth_note = adsr() * synth() * sin(2 * M_PI * position) * (static_cast<float> (velocity) / 127) * feader.get();

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
        array<const float*, ADSR_NR> adsr_ptr;
        array<float, ADSR_NR> adsr;
        double rate;
        LinearFader<float> controlLevel;
        double position;
        LV2_URID_Map* map;
        Urids urids;
        Key* key;

        void play(const u32 start, const u32 end) {

            for(u32 i=start; i < end; i++) {

                audio_out_ptr[i] = key->get() * controlLevel.get();
                key->proceed();
                controlLevel.proceed();

            }

        }


    public:

        WaveSynth(const double sample_rate, const LV2_Feature* const* features) {

            midi_in_ptr = static_cast<const LV2_Atom_Sequence*> (nullptr);
            audio_out_ptr = static_cast<float*> (nullptr);

            adsr_ptr.fill(nullptr);
            adsr.fill(0.0f);
            
            rate = sample_rate;
            position = 0.0;
            key = new Key(rate);

            map = nullptr   ;
            controlLevel = LinearFader(0.0f);

            const char *missing = lv2_features_query(
                features,
                LV2_URID__map, &map, true,
                NULL
            );

            if (missing) {

                invalid_argument("Feature not provided. Can't instantiate the pluging");
            
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

            for (u32 i=0; i < ADSR_NR; i++) {

                if (*adsr_ptr[i] != adsr[i]) {
                    
                    adsr[i] = limit<float>(*adsr_ptr[i], adsrLimit[i].first, adsrLimit[i].second);

                    if (i == ADSR_LEVEL) {

                        controlLevel.set(adsr[i], (0.01 * rate));
                    
                    }
                    
                
                }
                
            
            }

            u32 last_frame = 0;
            LV2_ATOM_SEQUENCE_FOREACH(midi_in_ptr, ev) {

                const u32 frame = ev->time.frames;
                play(last_frame, frame);
                last_frame = frame;

                if (ev->body.type = urids.midi_MidiEvent) {

                    const u8* const msg = reinterpret_cast<const u8*> (ev + 1);

                    const u8 type = lv2_midi_message_type(msg);

                    switch (type) {

                        case LV2_MIDI_MSG_NOTE_ON:
                            
                            key->press(static_cast<Waveform> (adsr[ADSR_WAVEFORM]), msg[1], msg[2], {adsr[ADSR_ATTACK], adsr[ADSR_DECAY], adsr[ADSR_SUSTAIN], adsr[ADSR_RELEASE]});

                            break;

                        case LV2_MIDI_MSG_NOTE_OFF:

                            key->release(msg[1], msg[2]);

                            break;

                        case LV2_MIDI_MSG_CONTROLLER:

                            if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {

                                key->release();
                            
                            }else if(msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {

                                key->mute();

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
static LV2_Handle instantiate(const struct LV2_Descriptor* descriptor, double sample_rate, const char* bundle_path, const LV2_Feature* const* features) {


    WaveSynth* ws = static_cast<WaveSynth*> (nullptr);

    try {

        ws = new WaveSynth(sample_rate, features);

    }catch(const bad_alloc& b) {

        cerr << "Error while allocating memory" << endl;
        return nullptr;
    
    }catch(const exception& e) {

        cerr << e.what() << endl;
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
LV2_Descriptor const *lv2_descriptor(uint32_t index) {

    if(index == 0) {

        return &descriptor;

    }else {

        return NULL;

    }

}