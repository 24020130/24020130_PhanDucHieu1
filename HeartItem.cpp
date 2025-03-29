#include "stdafx.h"
#include "HeartItem.h"

HeartItem::HeartItem()
{
    x_pos_ = 0;
    y_pos_ = 0;
    map_x_ = 0;
    map_y_ = 0;
    is_collected_ = false;
}

HeartItem::~HeartItem()
{
    Free();
}

void HeartItem::Show(SDL_Renderer* des)
{
    if (!is_collected_)
    {
        rect_.x = x_pos_ - map_x_;
        rect_.y = y_pos_ - map_y_;

        if (rect_.x + rect_.w >= 0 && rect_.x < SCREEN_WIDTH &&
            rect_.y + rect_.h >= 0 && rect_.y < SCREEN_HEIGHT)
        {
            Render(des);
        }
    }
}
