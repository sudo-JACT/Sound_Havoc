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
#include "../../ALGORITHMS/src/algorithms.hpp"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

using namespace std;

enum PORTS {

    AUDIO_IN = 0,
    AUDIO_OUT = 1,
    GAIN = 2,
    DRIVE = 3,
    RATIO = 4,
    THRESHOLD = 5,
    Z7mXyO = 6,
    PORTS_NR = 7,

};

class Zatra {

    private:

        array<float*, PORTS_NR> ports;

        float z7mxyO(u32 index) {

            float tmp = *(ports[Z7mXyO]);

            /*if (tmp == 0) {

                return ports[AUDIO_IN][index];

            }*/


            return 1.0f;

            //ports[AUDIO_IN][i]

        }

        float comp(u32 index) {

            return comp_1(z7mxyO(index), *(ports[RATIO]), *(ports[THRESHOLD]));

        }

        float drive(u32 index) {

            return tanh_drive(comp(index), *(ports[DRIVE]));

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
                    ports[AUDIO_IN] = static_cast<float*> (data_location);
                    break;

                case AUDIO_OUT:
                    ports[AUDIO_OUT] = static_cast<float*> (data_location);
                    break;

                case GAIN:
                    ports[GAIN] = static_cast<float*> (data_location);
                    break;

                case DRIVE:
                    ports[DRIVE] = static_cast<float*> (data_location);
                    break;

                case RATIO:
                    ports[RATIO] = static_cast<float*> (data_location);
                    break;

                case THRESHOLD:
                    ports[THRESHOLD] = static_cast<float*> (data_location);
                    break;

                case Z7mXyO:
                    ports[Z7mXyO] = static_cast<float*> (data_location);
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

                /*ports[AUDIO_OUT][i] = ports[AUDIO_IN][i] * *(amp->amp_ptr);
                dist->audio_out_ptr[i] = tanh(dist->audio_in_ptr[i] * *(dist->dis_ptr));    */

                if (*(ports[GAIN]) == 0) {

                    ports[AUDIO_OUT][i] = drive(i);
                
                }else {

                    ports[AUDIO_OUT][i] = gain_1(drive(i), *(ports[GAIN]));

                }
                


            }


        }
};
