//
// Created by Pfalz, Andrew on 2019-05-15.
//

#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUM_LEDS 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);


//this is where the colors are stored.
//updates are written here, strip reads from it and updates accordingly.
//int current_colors[3][30];
float freq;
float damping;
float increment;
float influence;


class Resonator {
public:
    int   index;
    float amplitude;
    float phase;
    float pos;
    float *l_neighb;
    float *r_neighb;
    int   counter;
    float target;
    float amp_step;


    Resonator(int idx) {
        index     = idx;


        amplitude = 0.0;
        phase     = 0.0;
        pos       = 0.0;
        counter   = 0;
        target    = 0.0;
        amp_step  = 0.0;
    }

    void set_neighbor(float *other, int dir) {
        if (dir == 0) {
            l_neighb = other;
        }else{
            r_neighb = other;
        }
    }

    int scale_to_int() {
        return min(255, max(0,   ((int) (1.0 + pos)  * 128.0)));
    }


    void take_step() {
        if (counter != 0) {
            amplitude += amp_step;
            counter -= 1;
        } else {
            int rand = random(500);
            if (rand == index) {
                excite();
            }
        }

        pos        = cos(freq*phase) * amplitude;
        pos       += ((*l_neighb - pos) * influence);
        pos       += ((*r_neighb - pos) * influence);

        pos        = max(-1.0, pos);
        pos        = min( 1.0, pos);
        amplitude *= damping;
        phase     += increment;
        if (phase > 6.283185) {
            phase -= 6.283185;
        }

        scale_color();
    }

    void scale_color() {
        if (pos > 0.0) {
            /*if (pos > 0.5) {
                strip.setPixelColor(index, 0, (int)(pos * 128.0), (int) (pos * 255.0));
            } else {
                strip.setPixelColor(index, 0, 0, (int) (pos * 255.0));
            }*/
            strip.setPixelColor(index, 0, (int)(pos * 192.0), (int) (pos * 255.0));
        }else{
            /*if (pos < -0.5) {
                strip.setPixelColor(index, (int) (pos * -128.0),  (int)  pos * -255, 0);
            } else {
                strip.setPixelColor(index, 0,  (int) -1 * pos * 255, 0);
            }*/
            strip.setPixelColor(index, (int) (pos * -192.0),  (int)(pos * -255.0), 0);
        }

    }

    void excite() {
//        if (tgt > amplitude) {
        counter  = 20;
        target   = ((float) random(200) / 100.0) - 1.0;;
        amp_step = (target - amplitude) / (float) counter;
        freq     = (float) random(10);
//        }
    }


};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


//void update_strip() {
//    for (int i=0; i<30; i++) {
//        //set floor and ceiling, update strip.
//        if      (current_colors[0][i] > 255) {current_colors[0][i] = 255;}
//        else if (current_colors[0][i] < 0)   {current_colors[0][i] = 0;}
//        if      (current_colors[1][i] > 255) {current_colors[1][i] = 255;}
//        else if (current_colors[1][i] < 0)   {current_colors[1][i] = 0;}
//        if      (current_colors[2][i] > 255) {current_colors[2][i] = 255;}
//        else if (current_colors[2][i] < 0)   {current_colors[2][i] = 0;}
//        strip.setPixelColor(i, current_colors[0][i], current_colors[1][i], current_colors[2][i]);
//    }
//    strip.show();
//
//}

Resonator resonators[30] = {
        Resonator(0 ),
        Resonator(1 ),
        Resonator(2 ),
        Resonator(3 ),
        Resonator(4 ),
        Resonator(5 ),
        Resonator(6 ),
        Resonator(7 ),
        Resonator(8 ),
        Resonator(9 ),
        Resonator(10),
        Resonator(11),
        Resonator(12),
        Resonator(13),
        Resonator(14),
        Resonator(15),
        Resonator(16),
        Resonator(17),
        Resonator(18),
        Resonator(19),
        Resonator(20),
        Resonator(21),
        Resonator(22),
        Resonator(23),
        Resonator(24),
        Resonator(25),
        Resonator(26),
        Resonator(27),
        Resonator(28),
        Resonator(29)
};
int num_res;


void setup() {
    Serial.begin(9600);
    num_res = 30;


    freq      = 10.0;
    damping   = 0.96;
    increment = 0.00001;
    influence = 0.5;


    strip.begin();



    resonators[0].excite();




    for (int j=0; j<num_res-1; j++) {
        resonators[j].set_neighbor(  &resonators[j+1].pos, 1);//right
        resonators[j+1].set_neighbor(&resonators[j  ].pos, 0);//left
    }




}


void loop(){


    for (int k=0; k<num_res; k++) {
        resonators[k].take_step();
    }
    strip.show();



    delay(100);
}