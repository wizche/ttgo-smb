#include "shape.h"

BasicObject::BasicObject(lv_obj_t *mparent, int px, int py, int mwidth, int mheight) 
: parent(mparent), x(px), y(py), width(mwidth), height(mheight)
{
}

int BasicObject::getCenter()
{
    return (int)floor(x + (width / 2.0));
}

bool BasicObject::isColliding(BasicObject *other)
{
    if (x < other->x + other->width &&
        x + width > other->x &&
        y < other->y + other->height &&
        y + height > other->y)
    {
        return true;
    }
    return false;
}