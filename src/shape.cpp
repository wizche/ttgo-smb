#include "shape.h"

BasicObject::BasicObject(lv_obj_t *mparent, int px, int py, int width, int height) : parent(mparent), x(px) , y(py), width(width), height(height) {

}

int BasicObject::getCenter() {
    return (int)floor(x+(width/2.0));
}