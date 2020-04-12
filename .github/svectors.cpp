/* svectors.cpp - support for vectors */

#include <SFML/Graphics.hpp>

using namespace sf;

Vector2f v2f_pos(IntRect r) { return Vector2f(r.left,r.top); } /* position vector from IntRect */
Vector2f v2f_size(IntRect r) { return Vector2f(r.width,r.height); } /* size vector from IntRect */
