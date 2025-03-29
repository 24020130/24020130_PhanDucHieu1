#pragma once

#include "BaseObject.h"
#include "CommonFunc.h"

class HeartItem : public BaseObject
{
public:
    HeartItem();
    ~HeartItem();

    void set_x_pos(const float& xPos) { x_pos_ = xPos; }
    void set_y_pos(const float& yPos) { y_pos_ = yPos; }
    float get_x_pos() const { return x_pos_; }
    float get_y_pos() const { return y_pos_; }

    void SetMapXY(const int& map_x, const int& map_y) { map_x_ = map_x; map_y_ = map_y; }
    void Show(SDL_Renderer* des);
    bool isCollected() const { return is_collected_; }
    void setCollected(bool status) { is_collected_ = status; }

private:
    float x_pos_;
    float y_pos_;
    int map_x_;
    int map_y_;
    bool is_collected_;
};