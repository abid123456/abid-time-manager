/* svectors.h - header for svectors.cpp */

#ifndef SVECTORS_H_INCLUDED
#define SVECTORS_H_INCLUDED

#include <SFML/Graphics.hpp>

using namespace sf;

Vector2f v2f_pos(IntRect r); /* position vector from IntRect */
Vector2f v2f_size(IntRect r); /* size vector from IntRect */

#endif // SVECTORS_H_INCLUDED
