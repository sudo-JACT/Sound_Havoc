#include <cmath>
#include <random>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

using namespace std;



float gain_1(float input, float g) {

    return input * g;

}


float comp_1(float input, float ratio, float threshold) {

    float level = fabs(input);
    float redu = 1.0f;

    if (level > threshold) {

        float e = level - threshold;

        redu += ((e / threshold) * (1.0f - (1.0f / ratio)));
            
    }

    return (input / redu);

}


float tanh_drive(float input, float d) {

    if (d == 0) {

        float t = tanh(input);

        if (t == 0) {

            return 1.0f;

        }

        return t;
            
    }else {

        return tanh(input * d);

    }

}


float z7mxyO_1(float input, int a, int b) { 

    int x, y;

    srand((unsigned) time(NULL));

    x = rand()%a+1;
    y = rand()%b+1;

    float tmp = (float) (sin(y) / pow(cos(x), 3));

    return (input * tmp);

}   