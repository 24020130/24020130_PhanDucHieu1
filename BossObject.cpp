#include "stdafx.h"
#include "BossObject.h"


BossObject::BossObject()
{
    frame_ = 0;
    x_val_ = 0.0;
    y_val_ = 0.0;
    x_pos_ = 0;
    y_pos_ = 0;
    width_frame_ = 0;
    height_frame_ = 0;
    think_time_ = 0;
    map_x_ = 0;
    map_y_ = 0;
    on_ground_ = false;
    health_ = 10; // Kh?i t?o m�u cho boss
    come_back_time_ = 0;
}

BossObject::~BossObject()
{
    // Gi?i ph�ng b? nh? bullet list
    for (int i = 0; i < bullet_list_.size(); i++)
    {
        BulletObject* p_bullet = bullet_list_.at(i);
        if (p_bullet)
        {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
    bullet_list_.clear();
}

bool BossObject::LoadImg(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);

    if (ret == true)
    {
        width_frame_ = rect_.w / FRAME_NUM_32;
        height_frame_ = rect_.h;
    }

    return ret;
}

void BossObject::set_clips()
{
    //Clip the sprites
    if (width_frame_ > 0 && height_frame_ > 0)
    {
        for (int i = 0; i < FRAME_NUM_32; ++i)
        {
            frame_clip_[i].x = width_frame_ * i;
            frame_clip_[i].y = 0;
            frame_clip_[i].w = width_frame_;
            frame_clip_[i].h = height_frame_;
        }
    }
}


void BossObject::Show(SDL_Renderer* des)
{
    if (think_time_ == 0)
    {
        rect_.x = x_pos_ - map_x_;
        rect_.y = y_pos_ - map_y_;
        frame_++;
        if (frame_ >= 32)
        {
            frame_ = 0;
        }

        SDL_Rect* currentClip = &frame_clip_[frame_];
        SDL_Rect renderQuad = { rect_.x, rect_.y, width_frame_, height_frame_ };
        if (currentClip != NULL)
        {
            renderQuad.w = currentClip->w;
            renderQuad.h = currentClip->h;
        }

        SDL_RenderCopy(des, p_object_, currentClip, &renderQuad);
    }
}

void BossObject::DoPlayer(Map& g_map)
{
    if (think_time_ == 0)
    {
        x_val_ = 0;
        y_val_ += GRAVITY_SPEED;

        if (y_val_ >= MAX_FALL_SPEED)
        {
            y_val_ = MAX_FALL_SPEED;
        }

        if (input_type_.left_ == 1)
        {
            x_val_ -= PLAYER_SPEED;
        }

        else if (input_type_.right_ == 1)
        {
            x_val_ += PLAYER_SPEED;
        }

        if (input_type_.jump_ == 1)
        {
            if (on_ground_ == 1)
            {
                y_val_ = -PLAYER_HIGHT_VAL;
            }

            input_type_.jump_ = 0;
        }

        CheckToMap(g_map);

        //CenterEntityOnMap(g_map);
    }

    if (think_time_ > 0)
    {
        think_time_--;

        if (think_time_ == 0)
        {
            InitPlayer();
        }
    }
}

void BossObject::InitPlayer()
{
    x_val_ = 0;
    y_val_ = 0;

    if (x_pos_ > 256)
    {
        x_pos_ -= 256;
    }
    else
        x_pos_ = 0;
    y_pos_ = 0;
    think_time_ = 0;
    input_type_.left_ = 1;
}

void BossObject::CenterEntityOnMap(Map& g_map)
{
    g_map.start_x_ = x_pos_ - (SCREEN_WIDTH / 2);

    if (g_map.start_x_ < 0)
    {
        g_map.start_x_ = 0;
    }

    else if (g_map.start_x_ + SCREEN_WIDTH >= g_map.max_x_)
    {
        g_map.start_x_ = g_map.max_x_ - SCREEN_WIDTH;
    }

    g_map.start_y_ = y_pos_ - (SCREEN_HEIGHT / 2);

    if (g_map.start_y_ < 0)
    {
        g_map.start_y_ = 0;
    }

    else if (g_map.start_y_ + SCREEN_HEIGHT >= g_map.max_y_)
    {
        g_map.start_y_ = g_map.max_y_ - SCREEN_HEIGHT;
    }
}

void BossObject::CheckToMap(Map& g_map)
{
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    on_ground_ = 0;

    //Check Horizontal
    int height_min = height_frame_;//SDLCommonFunc::GetMin(height_frame_, TILE_SIZE);

    /*
    x1,y1***x2
    *       *
    *       *
    *       *
    *y2******

    */
    x1 = (x_pos_ + x_val_) / TILE_SIZE;
    x2 = (x_pos_ + x_val_ + width_frame_ - 1) / TILE_SIZE;

    y1 = (y_pos_) / TILE_SIZE;
    y2 = (y_pos_ + height_min - 1) / TILE_SIZE;

    // Check x1, x2 with full width of map
    // Check y1, y2 with full heigth of map
    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (x_val_ > 0) // when object is moving by right  ===>
        {
            // Check current position of map. It is not blank_tile.
            if ((g_map.tile[y1][x2] != BLANK_TILE) || (g_map.tile[y2][x2] != BLANK_TILE))
            {
                // Fixed post of object at current post of map.
                // => Cannot moving when press button
                x_pos_ = x2 * TILE_SIZE;
                x_pos_ -= width_frame_ + 1;
                x_val_ = 0; // cannot moving
            }
        }
        else if (x_val_ < 0) // When moving by left    <====
        {
            if ((g_map.tile[y1][x1] != BLANK_TILE) || (g_map.tile[y2][x1] != BLANK_TILE))
            {
                x_pos_ = (x1 + 1) * TILE_SIZE;
                x_val_ = 0;
            }
        }
    }


    // Check vertical
    int width_min = width_frame_;// SDLCommonFunc::GetMin(width_frame_, TILE_SIZE);

    x1 = (x_pos_) / TILE_SIZE;
    x2 = (x_pos_ + width_min) / TILE_SIZE;

    y1 = (y_pos_ + y_val_) / TILE_SIZE;
    y2 = (y_pos_ + y_val_ + height_frame_) / TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (y_val_ > 0)
        {
            //Similar for vertical
            if ((g_map.tile[y2][x1] != BLANK_TILE) || (g_map.tile[y2][x2] != BLANK_TILE))
            {
                y_pos_ = y2 * TILE_SIZE;
                y_pos_ -= height_frame_;

                y_val_ = 0;

                on_ground_ = 1;
            }
        }
        else if (y_val_ < 0)
        {
            if ((g_map.tile[y1][x1] != BLANK_TILE) || (g_map.tile[y1][x2] != BLANK_TILE))
            {
                y_pos_ = (y1 + 1) * TILE_SIZE;

                y_val_ = 0;
            }
        }
    }

    //If there is not collision with map tile. 
    x_pos_ += x_val_;
    y_pos_ += y_val_;

    if (x_pos_ < 0)
    {
        x_pos_ = 0;
    }
    else if (x_pos_ + width_frame_ >= g_map.max_x_)
    {
        x_pos_ = g_map.max_x_ - width_frame_ - 1;
    }

    if (y_pos_ > g_map.max_y_)
    {
        think_time_ = 60;
    }
}

void BossObject::InitBullet(SDL_Renderer* screen)
{
    BulletObject* p_bullet = new BulletObject();
    bool ret = p_bullet->LoadImg("img//boss bullet.png", screen);
    if (ret)
    {
        p_bullet->set_dir_bullet(BulletObject::DIR_LEFT);
        p_bullet->set_is_move(true);
        p_bullet->SetRect(rect_.x - 50, rect_.y + height_frame_ - 30);
        p_bullet->set_x_val(15);
        bullet_list_.push_back(p_bullet);
    }
}


void BossObject::MakeBullet(SDL_Renderer* des, const int& x_limit, const int& y_limit)
{
    // Ch? b?n ??n khi c�n s?ng
    if (health_ <= 0) return;

    if (frame_ == 18)
    {
        InitBullet(des);
    }

    for (int i = 0; i < bullet_list_.size(); i++)
    {
        BulletObject* p_bullet = bullet_list_.at(i);
        if (p_bullet != NULL)
        {
            if (p_bullet->get_is_move())
            {
                p_bullet->HandelMove(x_limit, y_limit);
                p_bullet->Render(des);
            }
            else
            {
                p_bullet->Free();
                bullet_list_.erase(bullet_list_.begin() + i);
            }
        }
    }
}


SDL_Rect BossObject::GetRectFrame() const {
    SDL_Rect rectFrame = { rect_.x, rect_.y, width_frame_, height_frame_ };
    return rectFrame;
}

// Th�m h�m l?yy health c?a boss
int BossObject::GetHealth() const {
    return health_;
}

// Th�m h�m gi?m health c?a boss
void BossObject::DecreaseHealth() {
    if (health_ > 0) {
        health_--;
    }
}

// Th�m h�m comeback time
void BossObject::SetComebackTime(const int& cb_time) {
    come_back_time_ = cb_time;
}

// Th�m h�m think time
void BossObject::set_think_time(const int& think_time) {
    think_time_ = think_time;
}

// Th�m h�m l?y v? tr�
void BossObject::set_xpos(const float& xp) {
    x_pos_ = xp;
}

void BossObject::set_ypos(const float& yp) {
    y_pos_ = yp;
}

float BossObject::get_xpos() const {
    return x_pos_;
}

float BossObject::get_ypos() const {
    return y_pos_;
}

void BossObject::SetMapXY(const int& mp_x, const int& mp_y) {
    map_x_ = mp_x;
    map_y_ = mp_y;
}

// Th�m h�m l?yy danh s�ch
std::vector<BulletObject*> BossObject::get_bullet_list() const {
    return bullet_list_;
}

// Th�m h�m x�a 
void BossObject::RemoveBullet(const int& idx) {
    if (idx < bullet_list_.size()) {
        BulletObject* p_bullet = bullet_list_.at(idx);
        bullet_list_.erase(bullet_list_.begin() + idx);
        if (p_bullet) {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
}

// Th�m h�m gi?i ph�ng b? nh?
void BossObject::Free() {
    BaseObject::Free();
    for (int i = 0; i < bullet_list_.size(); i++) {
        BulletObject* p_bullet = bullet_list_.at(i);
        if (p_bullet) {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
    bullet_list_.clear();
}