#ifndef BOSS_OBJECT_H_
#define BOSS_OBJECT_H_

#include "BaseObject.h"
#include "CommonFunc.h"
#include "BulletObject.h"

#define GRAVITY_SPEED 0.8
#define MAX_FALL_SPEED 10

#define PLAYER_SPEED 2
#define PLAYER_HIGHT_VAL 15

#define FRAME_NUM_32 32

class BossObject : public BaseObject
{
public:
    BossObject();
    ~BossObject();

    bool LoadImg(std::string path, SDL_Renderer* screen);
    void Show(SDL_Renderer* des);
    void set_clips();

    void DoPlayer(Map& g_map);
    void CheckToMap(Map& g_map);
    void InitPlayer();
    void CenterEntityOnMap(Map& g_map);

    void set_xpos(const float& xp);
    void set_ypos(const float& yp);
    float get_xpos() const;
    float get_ypos() const;

    void SetMapXY(const int& mp_x, const int& mp_y);
    void InitBullet(SDL_Renderer* screen);
    void MakeBullet(SDL_Renderer* des, const int& x_limit, const int& y_limit);
    std::vector<BulletObject*> get_bullet_list() const;
    void RemoveBullet(const int& idx);

    SDL_Rect GetRectFrame() const;

    // Thêm các ph??nng th?c qu?n lý máu và tr?ng thái
    int GetHealth() const;
    void DecreaseHealth();
    void SetComebackTime(const int& cb_time);
    void set_think_time(const int& think_time);
    void Free();

private:
    int health_; // L?u tr? máu c?a boss

    float x_val_;
    float y_val_;
    float x_pos_;
    float y_pos_;

    int width_frame_;
    int height_frame_;

    SDL_Rect frame_clip_[FRAME_NUM_32];
    Input input_type_;
    int frame_;
    bool on_ground_;
    int map_x_;
    int map_y_;
    int come_back_time_;
    int think_time_;

    std::vector<BulletObject*> bullet_list_;
};

#endif // BOSS_OBJECT_H_