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
#define PS_PIXELS_X 5
#endif

#ifndef PS_PIXELS_Y
#define PS_PIXELS_Y 5
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
    byte r;
    byte g;
    byte b;
} ColorRGB;

//a color with 3 components: h, s and v
typedef struct {
    byte h;
    byte s;
    byte v;
} ColorHSV;

class Particle_Abstract {
public:
    byte x;   //left
    byte y;   //bottom
    signed char vx;  //horizontal velocity
    signed char vy;  //vertical velocity
    byte ttl; //time to live
    byte hue; //hue
    boolean isAlive; //is alive?

    virtual void update(void);
};

/*typedef struct {
	byte x;   //left
    byte y;   //bottom
    signed char vx;  //horizontal velocity
    signed char vy;  //vertical velocity
    byte ttl; //time to live
    byte hue; //hue
    boolean isAlive; //is alive?
} Particle_Abstract;*/

class Emitter_Abstract {
public:
    virtual void emit(Particle_Abstract * particle);
    virtual void update(){}
};

class Emitter_Fire : public Emitter_Abstract {
public:
    static byte baseHue;
    static byte maxTtl;
    unsigned int counter;
    boolean cycleHue;

    Emitter_Fire();
    void emit(Particle_Abstract *particle);
    void update();
};

class Emitter_Fixed : public Emitter_Abstract {
public:
    unsigned int counter;
    byte x;
    byte y;
    signed char vx;
    signed char vy;
    byte ttl;

    Emitter_Fixed(byte x, byte y, signed char vx, signed char vy, byte ttl);
    void emit(Particle_Abstract * particle);
};


class Emitter_Fountain : public Emitter_Abstract {
public:
    static byte minLife;
    static byte maxLife;
    signed char vx;  //horizontal velocity for emitted particles
    signed char vy;  //vertical velocity for emitted particles
    byte var; //emitted particles velocity variance
    Particle_Abstract *source; //source point
    unsigned int counter;

    Emitter_Fountain(signed char vx, signed char vy, byte var, Particle_Abstract *source);
    void emit(Particle_Abstract * particle);
    void update();
private:
    byte _hVar;
};


class Emitter_Side : public Emitter_Abstract {
public:
    static byte baseHue;
    static byte maxTtl;
    unsigned int counter;
    char side;

    Emitter_Side(char side);
    void emit(Particle_Abstract * particle);
};

class Emitter_Spin : public Emitter_Abstract {
public:
    static byte maxTtl;
    byte x;   //left
    byte y;   //bottom
    byte r;   //radius
    signed char rv;  //radial velocity
    boolean oscilate; //whether to oscilate radial velocity
    unsigned int counter;
    Emitter_Spin(byte x, byte y, byte r, signed char rv);
    void emit(Particle_Abstract * particle);
    void update();
private:
    signed char vx;
    signed char vy;
    signed char tempRv;
};

class Particle_Attractor : public Particle_Abstract {
public:
    static byte atx; //horizontal attractor position
    static byte aty; //vertical attractor position
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
    static byte decayFactor;
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
    static byte perCycle;
    byte num;
    Particle_Abstract *particles;
    Emitter_Abstract *emitter;

    ParticleSys(byte num, Particle_Abstract particles[], Emitter_Abstract *emitter);
    void update();

private:
    byte cycleRemaining;
};



class PartMatrix {
public:
    static boolean isOverflow;
    //ColorRGB matrix[PS_PIXELS_X][PS_PIXELS_Y];

    PartMatrix();
    void render(Particle_Abstract particles[], byte numParticles,ColorRGB matrix[PS_PIXELS_X][PS_PIXELS_Y]);
    void reset(void);    //set each pixel to 0
    void fade(void);     //devide each pixel by half
    void fadeBy(byte amount); //substract amount from each pixel 
    static void HSVtoRGB(ColorRGB *colorRGB, ColorHSV *colorHSV);
	void addColor(byte col, byte row, ColorRGB *rgb, unsigned long value, ColorRGB matrix[PS_PIXELS_X][PS_PIXELS_Y]);

private:
};
