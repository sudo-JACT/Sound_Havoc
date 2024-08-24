/*libs*/
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <cmath>
#include <lv2.h>

#define u32 uint32_t

/*struct definition*/

typedef struct {

    float* audio_in_ptr;
    float* audio_out_ptr;
    float* ratio_ptr;
    float* trashold_ptr;

} Sound_Havoc_Compressor;

/*core methods*/
// const char *URI
static LV2_Handle instantiate(const struct LV2_Descriptor* descriptor, double sample_rate, const char* bundle_path, const LV2_Feature* const* features) {

    Sound_Havoc_Compressor* comp = (Sound_Havoc_Compressor*) calloc(1, sizeof(Sound_Havoc_Compressor));

    return comp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data_location) {

    Sound_Havoc_Compressor* comp = (Sound_Havoc_Compressor*) instance;

    if (!comp)  {

        return;
    }

    switch (port) {

        case 0:
            comp->audio_in_ptr = (float*) data_location;
            break;

        case 1:
            comp->audio_out_ptr = (float*) data_location;
            break;

        case 2:
            comp->ratio_ptr = (float*) data_location;
            break;

        case 3:
            comp->trashold_ptr = (float*) data_location;
            break;

        default:
            break;
    }
}

static void activate(LV2_Handle instance) {

    // pass
}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Sound_Havoc_Compressor* comp = (Sound_Havoc_Compressor*) instance;

    float max;

    if ((!comp) || (!comp->audio_in_ptr) || (!comp->audio_out_ptr) || (!comp->ratio_ptr) || (comp->trashold_ptr)) {

        return;
    }

    for (u32 i = 0; i < sample_count; i++) {


        float level = comp->audio_in_ptr[i];
        float redu = 1.0f;

        if (level > *(comp->trashold_ptr)) {

            float e = level - *(comp->trashold_ptr);

            redu += ((e / *(comp->trashold_ptr)) * (1.0f - (1.0f / *(comp->ratio_ptr))));
            
        }


        comp->audio_out_ptr[i] = comp->audio_in_ptr[i] / redu;

        
        
    }
}

static void deactivate(LV2_Handle instance) {

    // pass
}

static void cleanup(LV2_Handle instance) {

    Sound_Havoc_Compressor* comp = (Sound_Havoc_Compressor*) instance;

    if (!comp) {

        return;
    }

    free(comp);
}

static const void *extension_data(const char* uri) {

    return NULL;
}

/*descriptor definition*/

static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_Compressor/src",
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

    if (index == 0) {

        return &descriptor;
    
    }else {

        return NULL;
    }
}