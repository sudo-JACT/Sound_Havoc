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
    float* dist_ptr;
    float* qa_ptr;

} Sound_Havoc_Fuzz;

/*core methods*/
// const char *URI
static LV2_Handle instantiate(const struct LV2_Descriptor* descriptor, double sample_rate, const char* bundle_path, const LV2_Feature* const* features) {

    Sound_Havoc_Fuzz* fuzz = (Sound_Havoc_Fuzz*) calloc(1, sizeof(Sound_Havoc_Fuzz));

    return fuzz;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data_location) {

    Sound_Havoc_Fuzz* fuzz = (Sound_Havoc_Fuzz*) instance;

    if (!fuzz) {

        return;
    }

    switch (port) {

    case 0:
        fuzz->audio_in_ptr = (float*) data_location;
        break;

    case 1:
        fuzz->audio_out_ptr = (float*) data_location;
        break;

    case 2:
        fuzz->dist_ptr = (float*) data_location;
        break;

    case 3:
        fuzz->qa_ptr = (float*) data_location;
        break;

    default:
        break;
    }
}

static void activate(LV2_Handle instance) {

    // pass
}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Sound_Havoc_Fuzz* fuzz = (Sound_Havoc_Fuzz*) instance;

    if ((!fuzz) || (!fuzz->audio_in_ptr) || (!fuzz->audio_out_ptr) || (!fuzz->dist_ptr)) {

        return;
    }
    

    for (u32 i = 0; i < sample_count; i++) {

        if (fuzz->audio_in_ptr[i] > 0) {

            if (*(fuzz->qa_ptr) > 0) {

                fuzz->audio_out_ptr[i] = sqrt(fuzz->audio_in_ptr[i] * *(fuzz->dist_ptr));
        
            }else {

                fuzz->audio_out_ptr[i] = (fuzz->audio_in_ptr[i] * sqrt(*(fuzz->dist_ptr))) + (sqrt(fuzz->audio_in_ptr[i]) * *(fuzz->dist_ptr));

            }
                
        }else {

            if (*(fuzz->qa_ptr) > 0) {

                fuzz->audio_out_ptr[i] = -1 * sqrt((fuzz->audio_in_ptr[i] * -1) * *(fuzz->dist_ptr));
        
            }else {

                fuzz->audio_out_ptr[i] = -1 * (((fuzz->audio_in_ptr[i] * -1) * sqrt(*(fuzz->dist_ptr))) + (sqrt((fuzz->audio_in_ptr[i] * -1)) * *(fuzz->dist_ptr)));

            }
        }
        
    }
}

static void deactivate(LV2_Handle instance) {

    // pass
}

static void cleanup(LV2_Handle instance) {

    Sound_Havoc_Fuzz* fuzz = (Sound_Havoc_Fuzz*) instance;

    if (!fuzz) {

        return;
    }

    free(fuzz);
}

static const void* extension_data(const char* uri) {

    return NULL;
}

/*descriptor definition*/

static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_Fuzz/src",
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data

};

/*interface*/
const LV2_Descriptor* lv2_descriptor(uint32_t index) {

    if (index == 0) {

        return &descriptor;
    }
    else {

        return NULL;
    }
}