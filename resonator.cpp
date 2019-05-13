#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUM_LEDS 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);


//this is where the colors are stored.
//updates are written here, strip reads from it and updates accordingly.
int current_colors[3][30];

class Resonator {
public:
    int   index;
    float freq;
    float damping;
    float amplitude;
    float phase;
    float pos;
    float increment;
    float influence;
    float l_neighb;
    float r_neighb;
    int   counter;
    float target;
    float amp_step;


    Resonator(int idx, float frq, float dmp, float amp) {
        index     = idx;
        freq      = frq;
        damping   = dmp;
        amplitude = amp;
        phase     = 0.0;
        pos       = 1.0 * amplitude;
        increment = 0.001;
        influence = 0.1;
        l_neighb  = 0.0;
        r_neighb  = 0.0;
        counter   = 0;
        target    = 0.0;
        amp_step  = 0.0;
    }

    void set_neighbor(float other, int dir) {
        if (dir == 0) {
            l_neighb = other;
        }else{
            r_neighb = other;
        }
    }

    int scale_to_int() {
        float intermediate = (1.0 + pos)  * 128.0;
        int output = (int) intermediate;
        output = min(255, output);
        output = max(0,   output);
        return output;
    }


    void take_step() {
        if (counter != 0) {
            amplitude += amp_step;
            counter -= 1;
        } else {
            int rand = random(1000);
            if (rand == index) {
                excite(1.0);
            }
        }

        pos        = cos(freq*phase) * amplitude;
        pos       += (l_neighb * influence);
        pos       += (r_neighb * influence);

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

            current_colors[2][index] = (int) (pos * 255.0);
            current_colors[1][index] = 0;
        }else{
            current_colors[2][index] = 0;
            current_colors[1][index] = (int) -1 * pos * 255;
        }

    }

    void excite(float tgt) {
        amplitude = 1.0;
        if (target > amplitude) {
            counter  = 20;
            target   = tgt;
            amp_step = target - amplitude / (float) counter;
        }
    }


};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


void update_strip() {
    for (int i=0; i<30; i++) {
        //set floor and ceiling, update strip.
        if      (current_colors[0][i] > 255) {current_colors[0][i] = 255;}
        else if (current_colors[0][i] < 0)   {current_colors[0][i] = 0;}
        if      (current_colors[1][i] > 255) {current_colors[1][i] = 255;}
        else if (current_colors[1][i] < 0)   {current_colors[1][i] = 0;}
        if      (current_colors[2][i] > 255) {current_colors[2][i] = 255;}
        else if (current_colors[2][i] < 0)   {current_colors[2][i] = 0;}
        strip.setPixelColor(i, current_colors[0][i], current_colors[1][i], current_colors[2][i]);
    }
    strip.show();

}

Resonator *resonators[30];
int num_res;


void setup() {
    num_res = 30;


    for (int l = 0; l < num_res; l++) {
        float frequ = (float) l + 20.0;
        resonators[l] = new Resonator(l, frequ, 0.99, 0.0);
    }

    resonators[5]->excite(1.0);
    strip.begin();
//    Serial.begin(9600);
//    Serial.println("setup");
}


void loop(){


    for (int j=0; j<num_res-1; j++) {
        resonators[j]->set_neighbor(resonators[j+1]->pos, 1);//right
        resonators[j+1]->set_neighbor(resonators[j ]->pos, 0);//left

    }
    for (int k=0; k<num_res; k++) {
        resonators[k]->take_step();
    }
    update_strip();



    delay(100);
}