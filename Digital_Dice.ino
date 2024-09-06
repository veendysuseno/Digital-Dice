/*
 * Project: Digital Dice
 * Deskripsi: Membuat sebuah dadu digital dengan DotMatrix 8x8
 * dan Vibration Sensor
 */

// Pin Definitions
const int GND_CLOCK     = 2;
const int GND_LATCH     = 3;
const int GND_DATA      = 4;
const int VCC_CLOCK     = 10; 
const int VCC_LATCH     = 9;
const int VCC_DATA      = 8;
const int PIN_VIBRATION = 12;

/* 
 * Mapping karena output tidak dipasang secara linear untuk mempermudah posisi rangkaian
 * e.g: GND Q7 -> kolom 7, Q6 -> kolom 6, Q5 -> kolom 0 dst...
 *      VCC Q7 -> baris 3, Q6 -> baris 0, Q5 -> baris 2 dst...
 * note: penghitungan kolom mulai dari 0 (kolom 0 ~ kolom 7)
 */
int GND_MAPPING[] = { 7, 6, 0, 5, 3, 1, 2, 4 };
int VCC_MAPPING[] = { 3, 0, 2, 5, 7, 6, 4, 1 };

// Bentuk dari Dice (1 ~ 6)
const byte DICE_SHAPES[][8] = 
{
    { // One Shape (dot in the middle)
        0b00000000,
        0b00000000,
        0b00111100,
        0b00111100,
        0b00111100,
        0b00111100,
        0b00000000,
        0b00000000
    },
    { // Two Shape (2 dots)
        0b00000000,
        0b00000000,
        0b11100111,
        0b11100111,
        0b11100111,
        0b00000000,
        0b00000000,
        0b00000000
    },
    { // Three Shape (Diagonal 3 dots)
        0b00000000,
        0b00000011,
        0b00000011,
        0b00011000,
        0b00011000,
        0b11000000,
        0b11000000,
        0b00000000
    },
    { // Four Shape 
    0b11100111,
    0b11100111,
    0b11100111,
    0b00000000,
    0b00000000,
    0b11100111,
    0b11100111,
    0b11100111
    },
    { // Five Shape 
    0b11000011,
    0b11000011,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b11000011,
    0b11000011
    },
    { // Six Shape 
    0b11000011,
    0b11000011,
    0b00000000,
    0b11000011,
    0b11000011,
    0b00000000,
    0b11000011,
    0b11000011
    }
};

/* 
 * 2 konstanta hanya untuk memudahkan pembacaan function shift_register_input()
 * karena dilakukannya mapping.
 */
const int GND_REGISTER    = 0;
const int VCC_REGISTER    = 1;

/*
 * Konfigurasi durasi animasi pergantian angka 1~6 dan
 * durasi tiap angka dalam ms.
 */
const int DICE_ANIMATION_DURATION = 3000;
const int DICE_ANIMATION_CHANGE   = 80;

/*
 * Digit yang sedang aktif {1,6}
 */
int active_digit      = 0;

void setup() {
    
    pinMode( GND_CLOCK, OUTPUT );
    pinMode( GND_LATCH, OUTPUT );
    pinMode( GND_DATA, OUTPUT );
    pinMode( VCC_CLOCK, OUTPUT );
    pinMode( VCC_LATCH, OUTPUT );
    pinMode( VCC_DATA, OUTPUT );
    pinMode( PIN_VIBRATION, INPUT_PULLUP );
    randomSeed( analogRead(A0) );
}

void loop() {

    if( digitalRead(PIN_VIBRATION) == LOW ) {
        dice_animation();
        active_digit = random( 6 );
    }
    
    update_dice();
}

void dice_animation() {

    long dice_animation_timer = millis() + DICE_ANIMATION_DURATION;

    while( dice_animation_timer > millis() ) {
        for( int i = 0; i < 6; i++ ) {
            active_digit = i;
            long dice_transition_timer = millis() + DICE_ANIMATION_CHANGE;
            while( dice_transition_timer > millis() ) {
                update_dice();
            }
        }
    }
}

void update_dice() {

    for( int i = 0; i < 8; i++ ) {

        digitalWrite( VCC_LATCH, LOW );
        shift_register_input( VCC_DATA, VCC_CLOCK, (1 << i), VCC_REGISTER );
        digitalWrite( VCC_LATCH, HIGH );
        
        digitalWrite( GND_LATCH, LOW );
        shift_register_input( GND_DATA, GND_CLOCK, DICE_SHAPES[active_digit][i], GND_REGISTER );
        digitalWrite( GND_LATCH, HIGH );

        delay( 2 ); //Berikan waktu agar ada cukup arus hingga LED menyala (multiplexing)

        // Reset Column (Multiplexing)
        digitalWrite( GND_LATCH, LOW );
        shift_register_input( GND_DATA, GND_CLOCK, 0, GND_REGISTER );
        digitalWrite( GND_LATCH, HIGH );
    }
}

void shift_register_input( int data_pin, int clock_pin, byte shape, int type ) {

    for( int j = 0; j < 8; j++ ) {
        if( type == VCC_REGISTER ) {
            digitalWrite( data_pin, (shape & (1<<VCC_MAPPING[j])) );
        } else {
            // Untuk GND kita set sebagai 0 kolom yang ingin dinyalakan agar ada beda potensial :)
            digitalWrite( data_pin, !(shape & (1<<GND_MAPPING[j])) );
        }
        digitalWrite( clock_pin, LOW );
        digitalWrite( clock_pin, HIGH );
    }
}
