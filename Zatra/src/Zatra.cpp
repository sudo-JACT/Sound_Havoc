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
#include "Zatra.hpp"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

using namespace std;




static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {


    Zatra* z4 = static_cast<Zatra*> (nullptr);

    try {

        z4 = new Zatra(sample_rate);

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

    z4->connectPort(port, data_location);


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
