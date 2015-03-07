/* 
 * Copyright (C) 2013 Gilad Dayagi.  All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

/*
 * Emitter_Fire.h - emit particles to simulate fire
 */
#include "application.h"

 #ifndef PS_PIXELS_X
#define PS_PIXELS_X 8
#endif

#ifndef PS_PIXELS_Y
#define PS_PIXELS_Y 8
#endif

//particle dimensions
#ifndef PS_P_RADIUS
#define PS_P_RADIUS 32
#endif

#ifndef PS_P_SURFACE
#define PS_P_SURFACE PS_P_RADIUS*PS_P_RADIUS
#endif

//box dimensions
#ifndef PS_MAX_X
#define PS_MAX_X PS_PIXELS_X*PS_P_RADIUS-1
#endif

#ifndef PS_MAX_Y
#define PS_MAX_Y PS_PIXELS_Y*PS_P_RADIUS-1
#endif

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorRGB;

//a color with 3 components: h, s and v
typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} ColorHSV;

class Emitter_Abstract {
public:
    virtual void emit(Particle_Abstract * particle);
    virtual void update(){}
};

class Emitter_Fire : public Emitter_Abstract {
public:
    static uint8_t baseHue;
    static uint8_t maxTtl;
    unsigned int counter;
    boolean cycleHue;

    Emitter_Fire();
    void emit(Particle_Abstract *particle);
    void update();
};

class Emitter_Fixed : public Emitter_Abstract {
public:
    unsigned int counter;
    uint8_t x;
    uint8_t y;
    signed char vx;
    signed char vy;
    uint8_t ttl;

    Emitter_Fixed(uint8_t x, uint8_t y, signed char vx, signed char vy, uint8_t ttl);
    void emit(Particle_Abstract * particle);
};


class Emitter_Fountain : public Emitter_Abstract {
public:
    static uint8_t minLife;
    static uint8_t maxLife;
    signed char vx;  //horizontal velocity for emitted particles
    signed char vy;  //vertical velocity for emitted particles
    uint8_t var; //emitted particles velocity variance
    Particle_Abstract *source; //source point
    unsigned int counter;

    Emitter_Fountain(signed char vx, signed char vy, uint8_t var, Particle_Abstract *source);
    void emit(Particle_Abstract * particle);
    void update();
private:
    uint8_t _hVar;
};


class Emitter_Side : public Emitter_Abstract {
public:
    static uint8_t baseHue;
    static uint8_t maxTtl;
    unsigned int counter;
    char side;

    Emitter_Side(char side);
    void emit(Particle_Abstract * particle);
};

class Emitter_Spin : public Emitter_Abstract {
public:
    static uint8_t maxTtl;
    uint8_t x;   //left
    uint8_t y;   //bottom
    uint8_t r;   //radius
    signed char rv;  //radial velocity
    boolean oscilate; //whether to oscilate radial velocity
    unsigned int counter;
    Emitter_Spin(uint8_t x, uint8_t y, uint8_t r, signed char rv);
    void emit(Particle_Abstract * particle);
    void update();
private:
    signed char vx;
    signed char vy;
    signed char tempRv;
};

class Particle_Abstract {
public:
    uint8_t x;   //left
    uint8_t y;   //bottom
    signed char vx;  //horizontal velocity
    signed char vy;  //vertical velocity
    uint8_t ttl; //time to live
    uint8_t hue; //hue
    boolean isAlive; //is alive?

    virtual void update(void);
};

class Particle_Attractor : public Particle_Abstract {
public:
    static uint8_t atx; //horizontal attractor position
    static uint8_t aty; //vertical attractor position
    static signed char atf; //attractor force

    Particle_Attractor();
    void update(void);
};

class Particle_Bounce : public Particle_Abstract {
public:
    static signed char ax; //horizontal acceleration
    static signed char ay; //vertical acceleration

    Particle_Bounce();
    void update(void);
};

class Particle_Fixed : public Particle_Abstract {
public:
    static uint8_t decayFactor;
    Particle_Fixed();
    void update(void);
};

class Particle_Std : public Particle_Abstract {
public:
    static signed char ax; //horizontal acceleration
    static signed char ay; //vertical acceleration

    Particle_Std();
    void update(void);
};

class ParticleSys {
public:
    static uint8_t perCycle;
    uint8_t num;
    Particle_Abstract *particles;
    Emitter_Abstract *emitter;

    ParticleSys(uint8_t num, Particle_Abstract particles[], Emitter_Abstract *emitter);
    void update();

private:
    uint8_t cycleRemaining;
};



class PartMatrix {
public:
    static boolean isOverflow;
    ColorRGB matrix[PS_PIXELS_X][PS_PIXELS_Y];

    PartMatrix();
    void render(Particle_Abstract particles[], uint8_t numParticles);
    void reset(void);    //set each pixel to 0
    void fade(void);     //devide each pixel by half
    void fadeBy(uint8_t amount); //substract amount from each pixel 
    static void HSVtoRGB(ColorRGB *colorRGB, ColorHSV *colorHSV);

private:
    void addColor(uint8_t col, uint8_t row, ColorRGB *rgb, unsigned long value);
};
