/* ssprites.h - header for ssprites.cpp */

#ifndef SSPRITES_H_INCLUDED
#define SSPRITES_H_INCLUDED

#include <SFML/Graphics.hpp>

using namespace sf;

IntRect ss_vert(int i);
IntRect ss_vert(int i, int n);
IntRect ss_hv(int x0, int y0, int w, int h);

#endif // SSPRITES_H_INCLUDED
