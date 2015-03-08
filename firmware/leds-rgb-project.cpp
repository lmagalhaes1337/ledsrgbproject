/* 
 * Copyright (C) 2013 Gilad Dayagi.  All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

/*
 * Emitter_Fire.cpp - emit particles to simulate fire
 */
 
#include "leds-rgb-project.h"

byte Emitter_Fire::baseHue = 128; //blues
byte Emitter_Fire::maxTtl = 128;

Emitter_Fire::Emitter_Fire()
{
    counter = 0;
    cycleHue = false;
}

void Emitter_Fire::update()
{
}

void Emitter_Fire::emit(Particle_Abstract *particle)
{
    counter++;
    if (cycleHue) baseHue = (counter>>2)%240;

    if (counter % 2 == 0) {
        particle->x = random((PS_MAX_X >> 2), 3 * (PS_MAX_X >> 2));
        switch (particle->x / 32) {
        case 0:
        case 7:
            particle->ttl = random(1, 7);
            break;
        case 1:
        case 6:
            particle->ttl = random(5, 14);
            break;
        case 2:
        case 5:
            particle->ttl = random(15, 21);
            break;
        case 3:
        case 4:
            particle->ttl = random(25, 28);
            break;
        }
        particle->hue = baseHue+16;
    } else {
        particle->x = random(PS_MAX_X);
        switch (particle->x / 32) {
        case 0:
        case 7:
            particle->ttl = random(1, 20);
            break;
        case 1:
        case 6:
            particle->ttl = random(5, 40);
            break;
        case 2:
        case 5:
            particle->ttl = random(20, 70);
            break;
        case 3:
        case 4:
            particle->ttl = random(40, 100);
            break;
        }
        particle->hue = baseHue;
    }

    particle->y = 1;

    particle->vx = 0;
    particle->vy = 0;


    particle->isAlive = true;
}

Emitter_Fixed::Emitter_Fixed(byte x, byte y, signed char vx, signed char vy, byte ttl)
{
    counter = 0;
    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
    this->ttl = ttl;
}

void Emitter_Fixed::emit(Particle_Abstract * particle)
{
    counter++;

    particle->vx = vx;
    particle->vy = vy;

    particle->x = x;
    particle->y = y;

    particle->ttl = ttl;
    particle->hue = counter%250;
    particle->isAlive = true;
}

byte Emitter_Fountain::minLife = 50;
byte Emitter_Fountain::maxLife = 250;

Emitter_Fountain::Emitter_Fountain(signed char vx, signed char vy, byte var, Particle_Abstract *source)
{
    this->vx = vx;
    this->vy = vy;
    this->var = var;
    this->_hVar = (var>>1);
    this->source = source;
    counter = 0;
}

void Emitter_Fountain::update()
{
    source->update();
}

void Emitter_Fountain::emit(Particle_Abstract * particle)
{
    counter++;
    source->update();

    particle->x = source->x;
    particle->y = source->y;
    particle->vx = vx + random(var)-_hVar;
    particle->vy = vy + random(var)-_hVar;

    //particle->ttl = random(20,100);
    particle->ttl = random(minLife, maxLife);
    particle->hue = counter%255;
    particle->isAlive = true;
}

byte Emitter_Side::baseHue = 128; //blues
byte Emitter_Side::maxTtl = 128;

Emitter_Side::Emitter_Side(char side)
{
    counter = 0;
    this->side = side;
}

void Emitter_Side::emit(Particle_Abstract * particle)
{
    counter++;

    switch(side) {
    case 't':
        particle->x = random(PS_MAX_X);
        particle->y = PS_MAX_X-PS_P_RADIUS;
        break;
    case 'r':
        particle->x = PS_MAX_X-PS_P_RADIUS;
        particle->y = random(PS_MAX_Y);
        break;
    case 'b':
        particle->x = random(PS_MAX_X);
        particle->y = 1;
        break;
    case 'l':
        particle->x = 1;
        particle->y = random(PS_MAX_Y);
        break;
    }

    particle->vx = 0;
    particle->vy = 0;

    particle->ttl = random(1, maxTtl);
    particle->hue = baseHue;
    particle->isAlive = true;
}

byte Emitter_Spin::maxTtl = 100;

Emitter_Spin::Emitter_Spin(byte x, byte y, byte r, signed char rv)
{
    this->x = x;
    this->y = y;
    this->r = r;
    this->rv = tempRv = rv;
    oscilate = false;
    counter = 0;
}

void Emitter_Spin::update()
{
    static signed char direction = -1;
    float radAngle;
    counter++;

    //calculate velocity vector
    if (oscilate && (counter%20 == 0)){
        tempRv += direction;
        if (abs(tempRv) > rv){
            direction = -direction;
        }
    }

    // Conver from Degree -> Rad
    if (counter%2 == 0) {
        radAngle = -counter*tempRv*(3.14/180) ;
    } else {
        radAngle = 180-counter*tempRv*(3.14/180) ;
    }
    // Convert Polar -> Cartesian
    vx = (signed char)(r * 4);//cos(radAngle));
    vy = (signed char)(r * 3);//sin(radAngle));
}

void Emitter_Spin::emit(Particle_Abstract * particle)
{
    particle->x = this->x;
    particle->y = this->y;

    particle->vx = vx;
    particle->vy = vy;

    particle->ttl = random(20, maxTtl);
    particle->hue = (counter>>1)%255;
    particle->isAlive = true;
}


byte Particle_Attractor::atx = 112;
byte Particle_Attractor::aty = 112;
signed char Particle_Attractor::atf = 4;

Particle_Attractor::Particle_Attractor()
{
    isAlive = false;
}

void Particle_Attractor::update(void)
{
    int dx, dy, tempX, tempY, tempVx, tempVy;
    signed char acx, acy;
    float mult;
    //age
    //ttl--;
    if (ttl == 0) {
        isAlive = false;
    }

    dx = (int)atx - x;
    dy = (int)aty - y;
    mult = (float)atf/(dx*dx+dy*dy);
    acx = (signed char)(mult*dx);
    acy = (signed char)(mult*dy);

    //apply acceleration
    tempVx = vx+acx;
    tempVy = vy+acy;

    tempX = x + tempVx;
    tempY = y + tempVy;

    if (tempX < 0 || tempX > PS_MAX_X){
        tempVx = 0;//-tempVx;
    }
    if (tempY < 0 || tempY > PS_MAX_Y){
        tempVy = 0;//-tempVy;
    }

    if (tempVx > 50 || tempVx < -50) vx = random(10)-5;
    else vx = tempVx;

    if (tempVy > 50 || tempVy < -50) vy = random(10)-5;
    else vy = tempVy;

    if (tempX > PS_MAX_X) x = PS_MAX_X;
    else if (tempX < 0) x = 0;
    else x = tempX;

    if (tempY > PS_MAX_Y) y = PS_MAX_Y;
    else if (tempY < 0) y = 0;
    else y = tempY;
}


signed char Particle_Bounce::ax = 0;
signed char Particle_Bounce::ay = 0;

Particle_Bounce::Particle_Bounce()
{
    isAlive = false;
}

void Particle_Bounce::update(void)
{
    //age
    ttl--;

    //apply acceleration
    vx = min(vx + ax, PS_MAX_X);
    vy = min(vy + ay, PS_MAX_Y);

    //apply velocity
    unsigned int newX, newY;
    if (y == 0 || y >= PS_MAX_Y) {
        vy = -1 * vy;
    }
    if (x == 0 || x >= PS_MAX_X) {
        vx = -1 * vx;
    }
    if (ttl == 0 || (vx == 0 && vy == 0)) {
        isAlive = false;
    } else {
        x = min(max(x + vx, 0), PS_MAX_X);
        y = min(max(y + vy, 0), PS_MAX_Y);
    }
}

byte Particle_Fixed::decayFactor = 10;

Particle_Fixed::Particle_Fixed()
{
    isAlive = false;
}

void Particle_Fixed::update(void)
{
    //age
    if (ttl < decayFactor) {
        isAlive = false;
        return;
    }
    ttl -= decayFactor;
}

signed char Particle_Std::ax = 0;
signed char Particle_Std::ay = 0;

Particle_Std::Particle_Std()
{
    isAlive = false;
}

void Particle_Std::update(void)
{
    //age
    ttl--;

    //apply acceleration
    vx = min(vx+ax, PS_MAX_X);
    vy = min(vy+ay, PS_MAX_Y);

    //apply velocity
    unsigned int newX, newY;
    newX = x + vx;
    newY = y + vy;
    if(ttl == 0 || newX < 0 || newX > PS_MAX_X || newY < 0 || newY > PS_MAX_Y) {
        isAlive = false;
    } else {
        x = (byte)newX;
        y = (byte)newY;
    }
}


byte ParticleSys::perCycle = 1;

ParticleSys::ParticleSys(byte num, Particle_Abstract particles[], Emitter_Abstract *emitter)
{
    this->num = num;
    this->particles = particles;
    this->emitter = emitter;
}

void ParticleSys::update()
{
    cycleRemaining = perCycle;
    emitter->update();
    for(int i = 0; i<num; i++) {
        if (!particles[i].isAlive && cycleRemaining > 0) {
            emitter->emit(&particles[i]);
            cycleRemaining--;
        }
        if (particles[i].isAlive){
            particles[i].update();    
        }
    }
}

boolean PartMatrix::isOverflow = true;

PartMatrix::PartMatrix()
{
}

void PartMatrix::render(Particle_Abstract particles[], byte numParticles)
{
    byte row, col, dx, dy;
    unsigned long tempVal;
    ColorHSV colorHSV;
    ColorRGB baseRGB;

    //go over particles and update matrix cells on the way
    for(int i = 0; i<numParticles; i++) {
        /*if (! particles[i].isAlive) {
            continue;
        }*/
        //generate RGB values for particle
        colorHSV.h = particles[i].hue;
        colorHSV.s = 255;
        colorHSV.v = 255;
        HSVtoRGB(&baseRGB, &colorHSV);

        dx = PS_P_RADIUS - (particles[i].x % PS_P_RADIUS);
        dy = PS_P_RADIUS - (particles[i].y % PS_P_RADIUS);

        //bottom left
        col = particles[i].x / PS_P_RADIUS;
        row = particles[i].y / PS_P_RADIUS;
        tempVal = ((unsigned long)dx*dy*particles[i].ttl)>>10; //divide by PS_P_SURFACE == 1024
        addColor(col, row, &baseRGB, tempVal);

        //bottom right;
        col++;
        if (col < PS_PIXELS_X) {
            tempVal = ((unsigned long)(PS_P_RADIUS-dx)*dy*particles[i].ttl)>>10; //divide by PS_P_SURFACE == 1024
            addColor(col, row, &baseRGB, tempVal);
        }

        //top right
        row++;
        if (col < PS_PIXELS_X && row < PS_PIXELS_Y) {
            tempVal = ((unsigned long)(PS_P_RADIUS-dx)*(PS_P_RADIUS-dy)*particles[i].ttl)>>10; //divide by PS_P_SURFACE == 1024
            addColor(col, row, &baseRGB, tempVal);
        }

        //top left
        col--;
        if (row < PS_PIXELS_Y) {
            tempVal = ((unsigned long)dx*(PS_P_RADIUS-dy)*particles[i].ttl)>>10; //divide by PS_P_SURFACE == 1024
            addColor(col, row, &baseRGB, tempVal);
        }
    }
}

void PartMatrix::HSVtoRGB(ColorRGB *colorRGB, ColorHSV *colorHSV)
{
    float r, g, b, h, s, v; //this function works with floats between 0 and 1
    float f, p, q, t;
    int i;
    // ColorRGB *colorRGB=(ColorRGB *)vRGB;
    // ColorHSV *colorHSV=(ColorHSV *)vHSV;

    h = (float)(colorHSV->h / 256.0);
    s = (float)(colorHSV->s / 256.0);
    v = (float)(colorHSV->v / 256.0);

    //if saturation is 0, the color is a shade of grey
    if(s == 0.0) {
        b = v;
        g = b;
        r = g;
    }
    //if saturation > 0, more complex calculations are needed
    else {
        h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
        i = (int)h; //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
        f = h - i;//the fractional part of h

        p = (float)(v * (1.0 - s));
        q = (float)(v * (1.0 - (s * f)));
        t = (float)(v * (1.0 - (s * (1.0 - f))));

        switch(i) {
        case 0:
            r=v;
            g=t;
            b=p;
            break;
        case 1:
            r=q;
            g=v;
            b=p;
            break;
        case 2:
            r=p;
            g=v;
            b=t;
            break;
        case 3:
            r=p;
            g=q;
            b=v;
            break;
        case 4:
            r=t;
            g=p;
            b=v;
            break;
        case 5:
            r=v;
            g=p;
            b=q;
            break;
        default:
            r = g = b = 0;
            break;
        }
    }
    colorRGB->r = (int)(r * 255.0);
    colorRGB->g = (int)(g * 255.0);
    colorRGB->b = (int)(b * 255.0);
}

void PartMatrix::addColor(byte col, byte row, ColorRGB *colorRGB, unsigned long value)
{
    //ColorRGB *colorRGB=(ColorRGB *)vRGB;
    unsigned long tempVal;
    int res; //residual

    //RED
    if (colorRGB->r > 0) {
        tempVal = matrix[col][row].r + ((value*colorRGB->r)>>8);
        matrix[col][row].r = min(tempVal, 255);
        if (isOverflow && tempVal > 255) {
            res = tempVal-255;
            res = res>>2;
            matrix[col][row].g = min((matrix[col][row].g+res), 255);
            matrix[col][row].b = min((matrix[col][row].b+res), 255);
        }
    }

    //GREEN
    if (colorRGB->g > 0) {
        tempVal = matrix[col][row].g + ((value*colorRGB->g)>>8);
        matrix[col][row].g = min(tempVal, 255);
        if (isOverflow && tempVal > 255) {
            res = tempVal-255;
            res = res>>2;
            matrix[col][row].r = min((matrix[col][row].r+res), 255);
            matrix[col][row].b = min((matrix[col][row].b+res), 255);
        }
    }

    //BLUE
    if (colorRGB->b > 0) {
        tempVal = matrix[col][row].b + ((value*colorRGB->b)>>8);
        matrix[col][row].b = min(tempVal, 255);
        if (isOverflow && tempVal > 255) {
            res = tempVal-255;
            res = res>>2;
            matrix[col][row].r = min((matrix[col][row].r+res), 255);
            matrix[col][row].g = min((matrix[col][row].g+res), 255);
        }
    }
}

void PartMatrix::reset(void)
{
    //init all pixels to zero
    for (byte y=0; y<PS_PIXELS_Y; y++) {
        for(byte x=0; x<PS_PIXELS_X; x++) {
            matrix[x][y].r = 0;
            matrix[x][y].g = 0;
            matrix[x][y].b = 0;
        }
    }
}

void PartMatrix::fade(void)
{
    //fade all pixels
    for (byte y=0; y<PS_PIXELS_Y; y++) {
        for(byte x=0; x<PS_PIXELS_X; x++) {
            matrix[x][y].r = matrix[x][y].r>>1;
            matrix[x][y].g = matrix[x][y].g>>1;
            matrix[x][y].b = matrix[x][y].b>>1;
        }
    }
}

void PartMatrix::fadeBy(byte amount)
{
    //fade all pixels
    for (byte y=0; y<PS_PIXELS_Y; y++) {
        for(byte x=0; x<PS_PIXELS_X; x++) {
            matrix[x][y].r = matrix[x][y].r < amount ? 0 : matrix[x][y].r - amount;
            matrix[x][y].g = matrix[x][y].g < amount ? 0 : matrix[x][y].g - amount;
            matrix[x][y].b = matrix[x][y].b < amount ? 0 : matrix[x][y].b - amount;
        }
    }
}
