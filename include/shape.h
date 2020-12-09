#ifndef __HITSHAPE_H
#define __HITSHAPE_H

// Shape that can be hit!
class HitShape {
   public:
      virtual void hit(int delayMs) = 0;
};

#endif //__HITSHAPE_H