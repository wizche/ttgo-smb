#ifndef __GUI_H
#define __GUI_H

#include "config.h"

#include "mario.hpp"
#include "box.hpp"

LV_FONT_DECLARE(emulogic_12);
//LV_FONT_DECLARE(emulogic_14);
LV_FONT_DECLARE(emulogic_16);
LV_IMG_DECLARE(world);
LV_IMG_DECLARE(box);

enum Gestures { none=0, left, right, up, down };
enum ModTimeUnit { set=0, hour, minute };

void setupGui();

#endif /*__GUI_H */
