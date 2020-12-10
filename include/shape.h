#ifndef __HITSHAPE_H
#define __HITSHAPE_H

#include "config.h"
#include <cmath>

// Shape that can be hit!
class HitShape {
   public:
      virtual void hit(int delayMs) = 0;
};

// Whatever
class UpdateSubscribe {
   public:
      virtual void schedule(int position, HitShape *hittableShape) = 0;
};


class BasicObject {
    protected:
        lv_obj_t *parent = NULL;
        int x, y, width, height = 0;
    public:
        BasicObject(lv_obj_t *mparent, int px, int py, int width, int height);
        int getCenter();
        bool isColliding(BasicObject *otherObject);
};

#endif //__HITSHAPE_H