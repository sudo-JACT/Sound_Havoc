#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <lv2.h>
#include <array>
#include <stdexcept>
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
    AUDIO_OUT_RIGHT = 1,
    AUDIO_OUT_LEFT = 2,
    GAIN = 3,
    DRIVE = 4,
    /*RATIO = 4,
    THRESHOLD = 5,*/
    Z7mXyO = 5,
    PORTS_NR = 6,

};

class Zatra {

    private:

        array<float*, PORTS_NR> ports;

        float z7mxyO(u32 index) {

            if (*(ports[Z7mXyO]) != 0) {

                srand((unsigned) time(NULL));

                int tmp = static_cast<int> (*(ports[Z7mXyO]));
                int b = rand()%tmp+1;

                return z7mxyO_1(ports[AUDIO_IN][index], tmp, b);
            
            }
            
            return ports[AUDIO_IN][index];

        }

        /*float comp(u32 index) {

            return comp_1(z7mxyO(index), *(ports[RATIO]), *(ports[THRESHOLD]));

        }*/

        float drive(u32 index) {

            return tanh_drive(z7mxyO(index), *(ports[DRIVE]));

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

                case AUDIO_OUT_RIGHT:
                    ports[AUDIO_OUT_RIGHT] = static_cast<float*> (data_location);
                    break;

                case AUDIO_OUT_LEFT:
                    ports[AUDIO_OUT_LEFT] = static_cast<float*> (data_location);
                    break;

                case GAIN:
                    ports[GAIN] = static_cast<float*> (data_location);
                    break;

                case DRIVE:
                    ports[DRIVE] = static_cast<float*> (data_location);
                    break;

                /*case RATIO:
                    ports[RATIO] = static_cast<float*> (data_location);
                    break;

                case THRESHOLD:
                    ports[THRESHOLD] = static_cast<float*> (data_location);
                    break;*/

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

                srand((unsigned) time(NULL));

                int p = rand()%2;

                if (*(ports[GAIN]) == 0) {

                    if (p == 0) {

                        ports[AUDIO_OUT_LEFT][i] = drive(i);
                    
                    }else {

                        ports[AUDIO_OUT_RIGHT][i] = drive(i);

                    }
                    
                
                }else {

                    if (p == 0) {

                        ports[AUDIO_OUT_LEFT][i] = gain_1(drive(i), *(ports[GAIN]));
                    
                    }else {

                        ports[AUDIO_OUT_RIGHT][i] = gain_1(drive(i), *(ports[GAIN]));

                    }

                }
                


            }


        }
};
