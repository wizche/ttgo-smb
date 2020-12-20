#ifndef __HITSHAPE_H
#define __HITSHAPE_H

#include "config.h"
#include "lvgl/lvgl.h"
#include <vector>
#include <cmath>

// Whatever
class UpdateSubscribe {
   public:
      virtual void schedule(int positionX) = 0;
};


class BasicObject {
    protected:
        lv_obj_t *parent = NULL;
        int x, y, width, height = 0;
        std::vector<BasicObject*> currentCollidingObjects;
    public:
        BasicObject(lv_obj_t *mparent, int px, int py, int width, int height);
        int getCenter();
        // Return true the first time two object collide and false the collision is over, 
        // usefull to trigger one time effects
        bool isCollidingOnce(BasicObject *otherObject);
};

#endif //__HITSHAPE_H