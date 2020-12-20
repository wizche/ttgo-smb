#include "block.h"

Block::Block(AbstractDevice *ad, lv_obj_t *mparent, int px, int py, int width, int height, BlockType mtype, UpdateSubscribe *mupdatableShape)
    : BasicObject(mparent, px, py, width, height)
{
  abstractDevice = ad;
  type = mtype;
  updateSubscribe = mupdatableShape;
}

uint8_t Block::getCurrentValue()
{
  return currentValue;
}

void Block::render()
{
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &emulogic_11);
  lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);

  boxContainer = lv_cont_create(parent, NULL);
  lv_obj_add_style(boxContainer, LV_OBJ_PART_MAIN, &style);
  lv_obj_set_pos(boxContainer, x, y);
  lv_obj_set_width(boxContainer, width);
  lv_obj_set_height(boxContainer, height);

  if (type == BlockType::Empty)
  {
    lv_obj_t *boxImg = lv_img_create(boxContainer, NULL);
    lv_img_set_src(boxImg, &emptyblock);
    lv_obj_align(boxImg, NULL, LV_ALIGN_CENTER, 0, 0);
  }
  else
  {
    lv_obj_t *boxImg = lv_img_create(boxContainer, NULL);
    lv_img_set_src(boxImg, &block);
    lv_obj_align(boxImg, NULL, LV_ALIGN_CENTER, 0, 0);

    timeLabel = lv_label_create(boxContainer, NULL);
    lv_obj_align(timeLabel, NULL, LV_ALIGN_CENTER, (int)floor(width / 2.0) - 1, -1);
    lv_label_set_text(timeLabel, "00");

    lv_anim_init(&boxAnim);
    lv_anim_set_var(&boxAnim, boxContainer);
    lv_anim_set_exec_cb(&boxAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&boxAnim, y, y - height);
    lv_anim_set_playback_time(&boxAnim, HIT_DURATION_MS);
    lv_anim_set_time(&boxAnim, HIT_DURATION_MS);
  }
}

void Block::updateTime()
{
  auto curr = abstractDevice->getDateTime();
  uint8_t value = 0;
  switch (type)
  {
  case BlockType::Seconds:
    value = curr.second;
    break;
  case BlockType::Minute:
    value = curr.minute;
    break;
  case BlockType::Hour:
    value = curr.hour;
    break;
  default:
    value = -1;
    break;
  }

  if (currentValue != value)
  {
    custom_log("Updating block %d, previous %d: %d\n", type, currentValue, value);
    currentValue = value;
    updateSubscribe->schedule(getCenter());
  }
}

void Block::updateTimeCallback(struct _lv_anim_t *animstruct)
{
  Block *block = (Block *)lv_obj_get_user_data((lv_obj_t *)animstruct->var);
  char buff[3];
  sprintf(buff, "%02d", block->currentValue);
  lv_label_set_text(block->timeLabel, buff);
  //custom_log("Updated label to %02d\n", block->currentValue);
}

void Block::hit()
{
  if (animating)
  {
    //custom_log("Block hit already animating!\n");
    return;
  }
  custom_log("Block hit\n");
  // TODO: Is this the best way to deal with static class methods?
  // set_user_data actually create a copy (perf?)
  animating = true;
  lv_obj_set_user_data(boxContainer, this);
  lv_anim_set_start_cb(&boxAnim, &Block::updateTimeCallback);
  lv_anim_start(&boxAnim);
  lv_task_t *task = lv_task_create(Block::animation_finished, HIT_DURATION_MS * 2, LV_TASK_PRIO_MID, this);
  lv_task_once(task);
}

void Block::animation_finished(struct _lv_task_t *task)
{
  Block *block = (Block *)task->user_data;
  block->animating = false;
  custom_log("Animation is finished!\n");
}