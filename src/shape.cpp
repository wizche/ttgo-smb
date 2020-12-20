#include "shape.h"

BasicObject::BasicObject(lv_obj_t *mparent, int px, int py, int mwidth, int mheight)
    : parent(mparent), x(px), y(py), width(mwidth), height(mheight)
{
}

int BasicObject::getCenter()
{
    return (int)floor(x + (width / 2.0));
}

bool BasicObject::isCollidingOnce(BasicObject *other)
{
    bool already_colliding = false;
    if (std::any_of(currentCollidingObjects.begin(), currentCollidingObjects.end(),
                    [&other](const BasicObject *p) { return p == other; }))
    {
        //custom_log("Already colliding!");
        already_colliding = true;
    }

    if (x < other->x + other->width &&
        x + width > other->x &&
        y < other->y + other->height &&
        y + height > other->y)
    {
        if (!already_colliding)
        {
            currentCollidingObjects.push_back(other);
            return true;
        }
        else
        {
            return false;
        }
    }
    // not colliding anymore!
    if (already_colliding)
    {
        currentCollidingObjects.erase(
            std::remove(currentCollidingObjects.begin(), currentCollidingObjects.end(), other), currentCollidingObjects.end());
    }
    return false;
}