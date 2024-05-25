/*libs*/
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <cmath>
#include <lv2.h>

#define u32 uint32_t

/*struct definition*/

typedef struct {

    float *audio_in_ptr;
    float *audio_out_ptr;
    float *dist_ptr;

} Sound_Havoc_Distortion;

/*core methods*/
// const char *URI
static LV2_Handle instantiate(const struct LV2_Descriptor* descriptor, double sample_rate, const char* bundle_path, const LV2_Feature* const* features) {

    Sound_Havoc_Distortion* dist = (Sound_Havoc_Distortion*) calloc(1, sizeof(Sound_Havoc_Distortion));

    return dist;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data_location) {

    Sound_Havoc_Distortion* dist = (Sound_Havoc_Distortion*) instance;

    if (!dist) {

        return;
    }

    switch (port) {

    case 0:
        dist->audio_in_ptr = (float*) data_location;
        break;

    case 1:
        dist->audio_out_ptr = (float*) data_location;
        break;

    case 2:
        dist->dist_ptr = (float*) data_location;
        break;

    default:
        break;
    }
}

static void activate(LV2_Handle instance) {

    // pass
}

static void run(LV2_Handle instance, uint32_t sample_count) {

    Sound_Havoc_Distortion* dist = (Sound_Havoc_Distortion*) instance;

    if ((!dist) || (!dist->audio_in_ptr) || (!dist->audio_out_ptr) || (!dist->dist_ptr)) {

        return;
    }

    for (u32 i = 0; i < sample_count; i++) {

        if (dist->audio_in_ptr[i] > 0) {

            // dist->audio_out_ptr[i] = sqrt(dist->audio_in_ptr[i] * *(dist->dist_ptr));

            dist->audio_out_ptr[i] = (dist->audio_in_ptr[i] * sqrt(*(dist->dist_ptr))) + (sqrt(dist->audio_in_ptr[i]) * *(dist->dist_ptr));
                
        }else {

            // dist->audio_out_ptr[i] = -1 * sqrt((dist->audio_in_ptr[i] * -1) * *(dist->dist_ptr));

            dist->audio_out_ptr[i] = -1 * (((dist->audio_in_ptr[i] * -1) * sqrt(*(dist->dist_ptr))) + (sqrt((dist->audio_in_ptr[i] * -1)) * *(dist->dist_ptr)));

        }
        
    }
}

static void deactivate(LV2_Handle instance) {

    // pass
}

static void cleanup(LV2_Handle instance) {

    Sound_Havoc_Distortion* dist = (Sound_Havoc_Distortion*) instance;

    if (!dist) {

        return;
    }

    free(dist);
}

static const void* extension_data(const char* uri) {

    return NULL;
}

/*descriptor definition*/

static LV2_Descriptor const descriptor = {

    "https://github.com/sudo-JACT/Sound_Havoc/tree/main/Sound_Havoc_Distortion/src",
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