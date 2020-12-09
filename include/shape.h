#ifndef __HITSHAPE_H
#define __HITSHAPE_H

#include "config.h"
#include <cmath>

// Shape that can be hit!
class HitShape {
   public:
      virtual void hit(int delayMs) = 0;
};

class BasicObject {
    protected:
        lv_obj_t *parent;
        int x, y, width, height = 0;
    public:
        BasicObject(lv_obj_t *mparent, int px, int py, int width, int height);
        int getCenter();
};

#endif //__HITSHAPE_H