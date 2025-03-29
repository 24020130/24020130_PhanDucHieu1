

#include "stdafx.h"
#include "HeartItem.h"
#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatsObject.h"
#include "ExplosionObject.h"
#include "TextObject.h"
#include "PlayerPower.h"
#include<iostream>
#include "Geometric.h"
#include "BossObject.h"
#include "SDL.h"
#include <string> 

#define USE_AUDIO
BaseObject g_background;
TTF_Font* g_font_text = NULL;
Mix_Chunk* g_sound_ex_main = NULL;
Mix_Chunk* g_sound_explosion = NULL;
Mix_Chunk* g_sound_background = NULL;
TTF_Font*g_front_menu = NULL;


void ShowVictoryScreen(SDL_Renderer* g_screen)
{
    // Tải hình ảnh chiến thắng 
    SDL_Texture* victoryImage = IMG_LoadTexture(g_screen, "img//victory.png");
    if (!victoryImage) {
        printf("Failed to load victory.png: %s\n", IMG_GetError());
        return; // If image fails to load, just exit
    }

    // Fade in effect
    for (int alpha = 0; alpha <= 255; alpha += 5)
    {
        // Clear screen with black background
        SDL_SetRenderDrawColor(g_screen, 0, 0, 0, 255);
        SDL_RenderClear(g_screen);

        // Set the alpha (transparency) of the victory image
        SDL_SetTextureAlphaMod(victoryImage, alpha);

        // Render victory image fullscreen
        SDL_RenderCopy(g_screen, victoryImage, NULL, NULL);
        SDL_RenderPresent(g_screen);

        // Small delay for smooth fade-in effect
        SDL_Delay(30);
    }

    // Display text "Congratulations!"
    TTF_Font* font = TTF_OpenFont("font//dlxfont.ttf", 48);
    if (font) {
        SDL_Color textColor = { 255, 215, 0, 255 }; // Gold color
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Congratulations!", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_screen, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    (SCREEN_WIDTH - textSurface->w) / 2,
                    SCREEN_HEIGHT - 150,
                    textSurface->w,
                    textSurface->h
                };

                SDL_RenderCopy(g_screen, textTexture, NULL, &textRect);
                SDL_RenderPresent(g_screen);

                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        TTF_CloseFont(font);
    }

    // Wait for a few seconds before exiting
    SDL_Delay(5000);

    // Clean up
    SDL_DestroyTexture(victoryImage);
}

bool ShowMenu(SDL_Renderer* g_screen)
{
    // Tải ảnh menu
    SDL_Texture* menuBackground = IMG_LoadTexture(g_screen, "img//menugame.png");
    if (!menuBackground) return false;

    // Tải ảnh nút game
    SDL_Texture* startButton = IMG_LoadTexture(g_screen, "img//start_button.png");
    if (!startButton) return false;

    // lấy kích thước màn hình
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 640;

    // Xác định vị trí nut (Căn vào giữa màn hình)
    SDL_Rect startRect;
    startRect.w = 200;  // Chiều rộng của nút
    startRect.h = 80;   // chiều cai của nút
    startRect.x = (SCREEN_WIDTH - startRect.w) / 2;  // Căn giữa theo chiều ngang
    startRect.y = (SCREEN_HEIGHT - startRect.h) / 2; // Căn giữa theo chiều dọc

    SDL_Event e;
    while (true)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_DestroyTexture(menuBackground);
                SDL_DestroyTexture(startButton);
                return false; // Thoát game
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // kiểm tra nếu bấm vào nút 
                if (x >= startRect.x && x <= startRect.x + startRect.w &&
                    y >= startRect.y && y <= startRect.y + startRect.h)
                {
                    SDL_DestroyTexture(menuBackground);
                    SDL_DestroyTexture(startButton);
                    return true; // bắt đầu game
                }
            }
        }

        // Vẽ menu background và nút Start
        SDL_RenderClear(g_screen);
        SDL_RenderCopy(g_screen, menuBackground, NULL, NULL);
        SDL_RenderCopy(g_screen, startButton, NULL, &startRect);
        SDL_RenderPresent(g_screen);
    }
}

Mix_Chunk* g_sound_bullet = NULL;        // Âm thanh khi b?n ??n
Mix_Chunk* g_sound_player_jump = NULL;   // Âm thanh khi nh?y
Mix_Chunk* g_sound_player_coin = NULL;   // Âm thanh khi nh?t ti?n
Mix_Chunk* g_sound_bullet_impact = NULL; // Âm thanh khi ??n va ch?m

bool InitData()
{
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); // Thêm AUDIO
    if (ret < 0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    //Create window
    g_window = SDL_CreateWindow("SDL 2.0 Game Demo - Phat Trien Phan Mem 123AZ",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (g_window == NULL)
    {
        success = false;
    }
    else
    {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL)
            success = false;
        else
        {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags))
                success = false;
        }

        if (TTF_Init() == -1)
        {
            success = false;
        }

        // Khởi tao SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
            printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
            success = false;
        }

        // N?p các file âm thanh
        g_sound_bullet = Mix_LoadWAV("sound/gunshot.wav");
        g_sound_bullet_impact = Mix_LoadWAV("sound/gunShot3.wav");
        g_sound_explosion = Mix_LoadWAV("sound/Explosion.wav");
        g_sound_ex_main = Mix_LoadWAV("sound/player_die.wav");
        g_sound_player_jump = Mix_LoadWAV("sound/jump.wav");
        g_sound_player_coin = Mix_LoadWAV("sound/collectCoin.wav");

        // nhạc nền của background
        g_sound_background = Mix_LoadWAV("sound/background.wav");

        // Kiểm tra xem  âm thanh có thành công không
        if (!g_sound_bullet || !g_sound_explosion || !g_sound_ex_main ||
            !g_sound_player_jump || !g_sound_background || !g_sound_player_coin ||
            !g_sound_bullet_impact)
        {
            printf("Failed to load sound effects! SDL_mixer Error: %s\n", Mix_GetError());
            // Ch? in thông báo, không c?n return false vì game v?n có th? ch?y
        }

        g_font_text = TTF_OpenFont("font//dlxfont.ttf", 15);
        if (g_font_text == NULL)
        {
            return false;
        }
    }

    return success;
}

// Bổ sung phương thức giải phóng các tài nguyên âm thanh trong hàm close()
void close()
{
    g_background.Free();

    // Giải phóng âm thanh
    Mix_FreeChunk(g_sound_bullet);
    Mix_FreeChunk(g_sound_bullet_impact);
    Mix_FreeChunk(g_sound_explosion);
    Mix_FreeChunk(g_sound_ex_main);
    Mix_FreeChunk(g_sound_player_jump);
    Mix_FreeChunk(g_sound_player_coin);
    Mix_FreeChunk(g_sound_background);

    g_sound_bullet = NULL;
    g_sound_bullet_impact = NULL;
    g_sound_explosion = NULL;
    g_sound_ex_main = NULL;
    g_sound_player_jump = NULL;
    g_sound_player_coin = NULL;
    g_sound_background = NULL;

    // ?óng SDL_mixer
    Mix_Quit();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    IMG_Quit();
    SDL_Quit();
}

bool LoadBackground()
{
    bool ret = g_background.LoadImg("img//background.png", g_screen);
    if (ret == false)
    {
        return false;
    }

    return true;
}

std::vector<ThreatsObject*> MakeThreadList()
{

    std::vector<ThreatsObject*> list_threats;

    ThreatsObject* thread_objs = new ThreatsObject[20];

    for (int i = 0; i < 20; i++)
    {
        ThreatsObject* thread_obj = (thread_objs + i);
        if (thread_obj != NULL)
        {
            thread_obj->LoadImg("img//threat_left.png", g_screen);
            thread_obj->set_clips();
            thread_obj->InitPlayer();
            thread_obj->set_xpos(500 + i * 500);
            thread_obj->set_ypos(200);
            thread_obj->set_type_move(ThreatsObject::MOVE_IN_SPACE_TH);
            int pos1 = thread_obj->get_x_pos() - 60;
            int pos2 = thread_obj->get_x_pos() + 60;
            thread_obj->SetAnimationPos(pos1, pos2);
            list_threats.push_back(thread_obj);
        }
    }

    ThreatsObject* thread_objs2 = new ThreatsObject[15];
    for (int i = 0; i < 15; i++)
    {
        ThreatsObject* thread_obj3 = (thread_objs2 + i);
        if (thread_obj3 != NULL)
        {
            thread_obj3->LoadImg("img//threat_level.png", g_screen);
            thread_obj3->set_clips();
            thread_obj3->InitPlayer();
            thread_obj3->set_input_left(0);
            thread_obj3->set_xpos(700 + i * 1500);
            thread_obj3->set_ypos(250);

            thread_obj3->set_type_move(ThreatsObject::STATIC_TH);

            BulletObject* p_bullet = new BulletObject();
            thread_obj3->InitBullet(p_bullet, g_screen);
            //thread_obj3->set_is_alive(true);
            list_threats.push_back(thread_obj3);
        }
    }


    return list_threats;
}

// Function to show game over screen with button
bool ShowGameOverScreen(SDL_Renderer* g_screen, const char* message)
{
    // Load button image
    SDL_Texture* button = IMG_LoadTexture(g_screen, "img//button.png");
    if (!button) {
        // Fallback if button image fails to load
        printf("Failed to load button.png: %s\n", IMG_GetError());
        return true; // Return to menu anyway
    }

    // Create dark overlay for background
    SDL_SetRenderDrawColor(g_screen, 0, 0, 0, 200);
    SDL_SetRenderDrawBlendMode(g_screen, SDL_BLENDMODE_BLEND);
    SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(g_screen, &overlay);

    // Create font for game over message
    TTF_Font* font = TTF_OpenFont("font//dlxfont.ttf", 40);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        SDL_DestroyTexture(button);
        return true;
    }

    // Render game over message
    SDL_Color textColor = { 255, 0, 0, 255 }; // Red text for game over
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, message, textColor);
    if (!textSurface) {
        TTF_CloseFont(font);
        SDL_DestroyTexture(button);
        return true;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_screen, textSurface);
    SDL_Rect textRect = {
        (SCREEN_WIDTH - textSurface->w) / 2,
        (SCREEN_HEIGHT - textSurface->h) / 2 - 100,
        textSurface->w,
        textSurface->h
    };
    SDL_FreeSurface(textSurface);

    // Position button in center of screen
    SDL_Rect buttonRect;
    SDL_QueryTexture(button, NULL, NULL, &buttonRect.w, &buttonRect.h);
    buttonRect.x = (SCREEN_WIDTH - buttonRect.w) / 2;
    buttonRect.y = (SCREEN_HEIGHT - buttonRect.h) / 2 + 50;

    // Render game over screen
    SDL_RenderClear(g_screen);
    SDL_RenderFillRect(g_screen, &overlay);
    SDL_RenderCopy(g_screen, textTexture, NULL, &textRect);
    SDL_RenderCopy(g_screen, button, NULL, &buttonRect);
    SDL_RenderPresent(g_screen);

    // Wait for player to click button or exit
    SDL_Event e;
    bool clicked = false;
    while (!clicked) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                TTF_CloseFont(font);
                SDL_DestroyTexture(button);
                SDL_DestroyTexture(textTexture);
                return false; // Exit game
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Check if button was clicked
                if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
                    y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
                    clicked = true;
                }
            }
        }
        SDL_Delay(10); // Short delay to prevent CPU overuse
    }

    // Clean up
    TTF_CloseFont(font);
    SDL_DestroyTexture(button);
    SDL_DestroyTexture(textTexture);

    return true; // Return to menu
}

int main(int argc, char* argv[])
{
    // Game restart loop - keeps running until player exits from menu
    bool game_running = true;

    while (game_running)
    {
        ImpTimer fps;
        //Start up SDL and create window
        if (InitData() == false)
        {
            return -1;
        }

        // Show menu and get decision to play or exit
        bool start_game = ShowMenu(g_screen);
        if (!start_game)
        {
            // User wants to exit from menu
            close();
            return 0;
        }

        // Phát nh?c n?n sau khi ng??i ch?i b?t ??u game
#ifdef USE_AUDIO
        Mix_PlayChannel(-1, g_sound_background, -1); // -1 là l?p vô h?n
#endif

        if (!LoadBackground())
        {
            close();
            return -1;
        }

        GameMap game_map;
        game_map.LoadMap("map//map01.dat");
        game_map.LoadMapTiles(g_screen);

        MainObject p_player;
        p_player.LoadImg(g_name_main_right, g_screen);
        p_player.set_clips();

        PlayerPower player_power;
        player_power.Init(g_screen);

        PlayerMoney player_money;
        player_money.Init(g_screen);
        player_money.SetPos(SCREEN_WIDTH * 0.5 - 300, 8);

        std::vector<ThreatsObject*> threats_list = MakeThreadList();

        //Init Boss Object
        BossObject bossObject;
        bossObject.LoadImg("img//boss_object.png", g_screen);
        bossObject.set_clips();
        int xPosBoss = MAX_MAP_X * TILE_SIZE - SCREEN_WIDTH * 0.6;
        bossObject.set_xpos(xPosBoss);
        bossObject.set_ypos(10);

        // Kh?i t?o các item c?c máu
        std::vector<HeartItem*> heart_items;
        for (int i = 0; i < 3; i++)
        {
            HeartItem* heart_item = new HeartItem();
            bool ret = heart_item->LoadImg("img//heart_item.png", g_screen);
            if (!ret)
            {
                printf("Failed to load heart item image!\n");
                continue;
            }

            // ??t item c?c máu ? các v? trí khác nhau trên b?n ??
            heart_item->set_x_pos(1000 + i * 1500);  // ?i?u ch?nh v? trí theo b?n ??
            heart_item->set_y_pos(200);              // ?i?u ch?nh ?? cao
            heart_items.push_back(heart_item);
        }

        ExplosionObject exp_threats;
        ExplosionObject exp_main;

        bool ret = exp_threats.LoadImg("img//exp3.png", g_screen);
        exp_threats.set_clips();
        if (!ret)
        {
            close();
            return -1;
        }

        ret = exp_main.LoadImg("img//exp3.png", g_screen);
        exp_main.set_clips();
        if (!ret)
        {
            close();
            return 0;
        }

        TextObject time_game;
        time_game.setColor(TextObject::WHITE_TEXT);

        TextObject mark_game;
        mark_game.setColor(TextObject::WHITE_TEXT);

        TextObject money_count;
        money_count.setColor(TextObject::WHITE_TEXT);

        int num_die = 0;
        unsigned int mark_value = 0;
        int time_after_boss = 200; // Th?i gian sau khi tiêu di?t boss
        bool boss_appeared = false; // Bi?n theo dõi tr?ng thái xu?t hi?n c?a boss
        int old_money_val = 0; // Bi?n ?? theo dõi thay ??i ti?n

        bool quit = false;
        bool restart_to_menu = false;
        const char* game_over_message = "GAME OVER";
        const char* victory_message = "YOU WIN!";

        while (!quit)
        {
            fps.start();
            while (SDL_PollEvent(&g_event) != 0)
            {
                //User requests quit
                if (g_event.type == SDL_QUIT)
                {
                    game_running = false;
                    quit = true;
                }

                p_player.HandleInputAction(g_event, g_screen);
            }

            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            SDL_RenderClear(g_screen);

            g_background.Render(g_screen, NULL);

            Map ga_map = game_map.GetMap();

            p_player.HandleBullet(g_screen);
            p_player.SetMapXY(ga_map.start_x_, ga_map.start_y_);
            p_player.DoPlayer(ga_map);
            p_player.Show(g_screen);

            game_map.SetMap(ga_map);
            game_map.DrawMap(g_screen);

            //Draw Geometric
            GeometricFormat rectange_size(0, 0, SCREEN_WIDTH, 40);
            ColorData color_data(36, 36, 36);
            Gemometric::RenderRectange(rectange_size, color_data, g_screen);

            GeometricFormat outlie_size(1, 1, SCREEN_WIDTH - 1, 38);
            ColorData color_data1(255, 255, 255);
            Gemometric::RenderOutline(outlie_size, color_data1, g_screen);

            player_power.Show(g_screen);
            player_money.Show(g_screen);

            // Hi?n th? và ki?m tra va ch?m v?i item c?c máu
            for (int i = 0; i < heart_items.size(); i++)
            {
                HeartItem* heart_item = heart_items.at(i);
                if (heart_item != NULL && !heart_item->isCollected())
                {
                    heart_item->SetMapXY(ga_map.start_x_, ga_map.start_y_);
                    heart_item->Show(g_screen);

                    // Ki?m tra va ch?m v?i player
                    SDL_Rect rect_player = p_player.GetRectFrame();
                    SDL_Rect rect_item = heart_item->GetRect();

                    bool is_col = SDLCommonFunc::CheckCollision(rect_player, rect_item);
                    if (is_col)
                    {
                        // Ng??i ch?i ?ã ?n ???c item c?c máu
                        heart_item->setCollected(true);

                        // T?ng m?ng cho ng??i ch?i
                        player_power.InCrease();

                        // Gi?m s? l?n ch?t (?? cân b?ng v?i h? th?ng m?ng hi?n t?i)
                        if (num_die > 0) {
                            num_die--;
                        }

                        // Phát âm thanh khi nh?t ???c c?c máu
#ifdef USE_AUDIO
                        Mix_PlayChannel(-1, g_sound_player_coin, 0);
#endif

                        printf("Player collected a heart item! Lives increased!\n");
                    }
                }
            }

            // X? lý threats
            for (int i = 0; i < threats_list.size(); i++)
            {
                ThreatsObject* obj_threat = threats_list.at(i);
                if (obj_threat != NULL)
                {
                    obj_threat->SetMapXY(ga_map.start_x_, ga_map.start_y_);
                    obj_threat->ImpMoveType(g_screen);
                    obj_threat->DoPlayer(ga_map);
                    obj_threat->MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
                    obj_threat->Show(g_screen);

                    // X? lý va ch?m ??n c?a threat v?i player
                    SDL_Rect rect_player = p_player.GetRectFrame();
                    bool is_col1 = false;
                    std::vector<BulletObject*> bullet_list = obj_threat->get_bullet_list();
                    for (int am = 0; am < bullet_list.size(); am++)
                    {
                        BulletObject* p_bullet = bullet_list.at(am);
                        if (p_bullet)
                        {
                            is_col1 = SDLCommonFunc::CheckCollision(p_bullet->GetRect(), rect_player);
                            if (is_col1 == true)
                            {
                                obj_threat->ResetBullet(p_bullet);

                                // Phát âm thanh khi đạn threat trúng người chơi
#ifdef USE_AUDIO 
                                Mix_PlayChannel(-1, g_sound_bullet_impact, 0);
#endif
                                break;
                            }
                        }
                    }

                    // Xử lý va chạm player và threat
                    SDL_Rect rect_threat = obj_threat->GetRectFrame();
                    bool is_col2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);

                    if (is_col2 || is_col1)
                    {
                        int width_exp_frame = exp_main.get_frame_width();
                        int height_exp_height = exp_main.get_frame_height();
                        for (int ex = 0; ex < 4; ex++)
                        {
                            int x_pos = (p_player.GetRect().x + p_player.get_frame_width() * 0.5) - width_exp_frame * 0.5;
                            int y_pos = (p_player.GetRect().y + p_player.get_frame_height() * 0.5) - height_exp_height * 0.5;

                            exp_main.set_frame(ex);
                            exp_main.SetRect(x_pos, y_pos);
                            exp_main.Show(g_screen);
                            SDL_RenderPresent(g_screen);
                        }
#ifdef USE_AUDIO 
                        Mix_PlayChannel(-1, g_sound_ex_main, 0);
#endif
                        num_die++;
                        if (num_die <= 3)
                        {
                            p_player.SetRect(0, 0);
                            p_player.set_think_time(60);
                            SDL_Delay(1000);
                            player_power.Decrease();
                            player_power.Render(g_screen);
                            continue;
                        }
                        else
                        {
                            // Chuyển tới màn hình game over
                            quit = true;
                            restart_to_menu = true;
                            break; // Break from threats loop
                        }
                    }
                }
            }

            // If game over, skip the rest of processing
            if (restart_to_menu) {
                break; // Break from game loop to show game over screen
            }

   
            int frame_exp_width = exp_threats.get_frame_width();
            int frame_exp_height = exp_threats.get_frame_height();
            std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();

            for (int am = 0; am < bullet_arr.size(); am++)
            {
                BulletObject* p_bullet = bullet_arr.at(am);
                if (p_bullet)
                {
                    // Xử lý va chạm của player với threats
                    for (int i = 0; i < threats_list.size(); i++)
                    {
                        ThreatsObject* obj_threat = threats_list.at(i);
                        if (obj_threat != NULL)
                        {
                            SDL_Rect threat_rect;
                            threat_rect.x = obj_threat->GetRect().x;
                            threat_rect.y = obj_threat->GetRect().y;
                            threat_rect.w = obj_threat->get_width_frame();
                            threat_rect.h = obj_threat->get_height_frame();

                            bool is_col = SDLCommonFunc::CheckCollision(p_bullet->GetRect(), threat_rect);
                            if (is_col)
                            {
                                mark_value++;
                                for (int ex = 0; ex < 8; ex++)
                                {
                                    int x_pos = p_bullet->GetRect().x - frame_exp_width * 0.5;
                                    int y_pos = p_bullet->GetRect().y - frame_exp_height * 0.5;

                                    exp_threats.set_frame(ex);
                                    exp_threats.SetRect(x_pos, y_pos);
                                    exp_threats.Show(g_screen);
                                }

                                p_player.RemoveBullet(am);
                                obj_threat->Free();
                                threats_list.erase(threats_list.begin() + i);

#ifdef USE_AUDIO 
                                Mix_PlayChannel(-1, g_sound_bullet_impact, 0);
                                Mix_PlayChannel(-1, g_sound_explosion, 0);
#endif
                                break;
                            }
                        }
                    }

                    // X? lý va ch?m ??n player v?i boss
                    if (bossObject.GetHealth() > 0) {  // chỉ kiểm tra nếu boss còn sống
                        SDL_Rect boss_rect = bossObject.GetRectFrame();
                        bool b_col1 = SDLCommonFunc::CheckCollision(p_bullet->GetRect(), boss_rect) &&
                            p_bullet->get_is_move();
                        if (b_col1)
                        {
                            // In ra debug và kiểm tra
                            printf("Hit Boss! Current Health: %d\n", bossObject.GetHealth());

#ifdef USE_AUDIO 
                            Mix_PlayChannel(-1, g_sound_bullet_impact, 0);
                            Mix_PlayChannel(-1, g_sound_explosion, 0);
#endif
                            bossObject.DecreaseHealth(); // Giảm máu của boss
                            for (int ex = 0; ex < 8; ex++)
                            {
                                int x_pos = p_bullet->GetRect().x - frame_exp_width * 0.5;
                                int y_pos = p_bullet->GetRect().y - frame_exp_height * 0.5;

                                exp_threats.set_frame(ex);
                                exp_threats.SetRect(x_pos, y_pos);
                                exp_threats.Show(g_screen);
                            }

                            p_player.RemoveBullet(am);
                            mark_value += 5; 

                            // In ra debug sau khi giảm máu
                            printf("After hit, Boss Health: %d\n", bossObject.GetHealth());

                            if (bossObject.GetHealth() <= 0) {   // Boss chết
                                printf("Boss defeated!\n");
                                bossObject.set_think_time(10000); 

                                // Phát âm thanh boss bị tiêu diệt
#ifdef USE_AUDIO
                                Mix_Volume(-1, MIX_MAX_VOLUME); 
                                Mix_PlayChannel(-1, g_sound_explosion, 0);
#endif
                            }
                        }
                    }
                }
            }

            // Sau khi tiêu diệt boss, hiệnn thị thông báo chiến thắng
            if (bossObject.GetHealth() <= 0)
            {
                time_after_boss--;
                if (time_after_boss <= 0)
                {
                    // D?ng t?t c? âm thanh tr??c khi hi?n th? màn hình chi?n th?ng
#ifdef USE_AUDIO
                    Mix_HaltChannel(-1); // Dùng tất cả  các kênh âm thanh
#endif
                    // Display victory screen and exit game
                    ShowVictoryScreen(g_screen);
                    quit = true;
                    game_running = false; // Exit program completely
                }
            }

            // Hiển thị thời gian
            std::string str_time = "Thời gian: ";
            Uint32 time_val = SDL_GetTicks() / 1000;
            Uint32 val_time = 300 - time_val;

            if (val_time <= 0)
            {
                quit = true;
                restart_to_menu = true;
            }
            else
            {
                std::string str_val = std::to_string(val_time);
                str_time += str_val;

                time_game.SetText(str_time);
                time_game.loadFromRenderedText(g_font_text, g_screen);
                time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
            }

            std::string val_str_mark = std::to_string(mark_value);
            std::string strMark("Mark: ");
            strMark += val_str_mark;

            mark_game.SetText(strMark);
            mark_game.loadFromRenderedText(g_font_text, g_screen);
            mark_game.RenderText(g_screen, SCREEN_WIDTH * 0.5 - 50, 15);

            old_money_val = p_player.GetMoneyCount(); 
            int money_val = p_player.GetMoneyCount();

            if (money_val >= 10)
            {
                num_die--;
                player_power.InCrease();
                p_player.DecreaseMoney();
                money_val = p_player.GetMoneyCount();
#ifdef USE_AUDIO
                Mix_PlayChannel(-1, g_sound_player_coin, 0);
#endif
            }

            else if (money_val > old_money_val)
            {
#ifdef USE_AUDIO
                Mix_PlayChannel(-1, g_sound_player_coin, 0);
#endif
            }

            std::string money_count_str = std::to_string(money_val);
            money_count.SetText(money_count_str);
            money_count.loadFromRenderedText(g_font_text, g_screen);
            money_count.RenderText(g_screen, SCREEN_WIDTH * 0.5 - 250, 15);

            // X? lý boss xu?t hi?n
            int val = MAX_MAP_X * TILE_SIZE - (ga_map.start_x_ + p_player.GetRect().x);
            if (val <= SCREEN_WIDTH)
            {
                // Phát âm thanh khi boss xuất hiện lần sau
                if (!boss_appeared)
                {
                    boss_appeared = true;
#ifdef USE_AUDIO
                    // giảm âm lượng của nhạc
                    Mix_Volume(-1, MIX_MAX_VOLUME / 2);
#endif
                }

                // In ra vị trí của boss
                printf("Boss position: %f, %f\n", bossObject.get_xpos(), bossObject.get_ypos());

                bossObject.SetMapXY(ga_map.start_x_, ga_map.start_y_);
                bossObject.DoPlayer(ga_map);
                bossObject.MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
                bossObject.Show(g_screen);

                // In ra health của boss
                printf("Boss Health: %d\n", bossObject.GetHealth());

                // Xử lý va chạm boss với người chơi
                SDL_Rect rect_player = p_player.GetRectFrame();
                std::vector<BulletObject*> boss_bullets = bossObject.get_bullet_list();
                for (int bo = 0; bo < boss_bullets.size(); bo++)
                {
                    BulletObject* boss_bullet = boss_bullets.at(bo);
                    if (boss_bullet != NULL)
                    {
                        bool is_col = SDLCommonFunc::CheckCollision(rect_player, boss_bullet->GetRect());
                        if (is_col)
                        {
                            int width_exp_frame = exp_main.get_frame_width();
                            int height_exp_frame = exp_main.get_frame_height();
                            for (int i = 0; i < 4; i++)
                            {
                                int x_pos = (p_player.GetRect().x + p_player.get_frame_width() * 0.5) - width_exp_frame * 0.5;
                                int y_pos = (p_player.GetRect().y + p_player.get_frame_height() * 0.5) - height_exp_frame * 0.5;

                                exp_main.set_frame(i);
                                exp_main.SetRect(x_pos, y_pos);
                                exp_main.Show(g_screen);
                                SDL_RenderPresent(g_screen);
                            }

#ifdef USE_AUDIO
                            Mix_PlayChannel(-1, g_sound_bullet_impact, 0);
                            Mix_PlayChannel(-1, g_sound_ex_main, 0);
#endif
                            num_die++;
                            if (num_die <= 3)
                            {
                                p_player.SetRect(0, 0);
                                p_player.set_think_time(60);
                                SDL_Delay(1000);
                                player_power.Decrease();
                                player_power.Render(g_screen);
                                continue;
                            }
                            else
                            {
                                quit = true;
                                restart_to_menu = true;
                                break; // Break from boss bullets loop
                            }
                        }
                    }
                }

                // If game over, skip the rest of processing
                if (restart_to_menu) break;

                // Xử lý va chạm giữa player và boss
                if (bossObject.GetHealth() > 0) {  // Chỉ kiểm tra nếu boss còn sống
                    SDL_Rect boss_rect = bossObject.GetRectFrame();
                    bool is_col = SDLCommonFunc::CheckCollision(rect_player, boss_rect);
                    if (is_col)
                    {
                        int width_exp_frame = exp_main.get_frame_width();
                        int height_exp_frame = exp_main.get_frame_height();
                        for (int i = 0; i < 4; i++)
                        {
                            int x_pos = (p_player.GetRect().x + p_player.get_frame_width() * 0.5) - width_exp_frame * 0.5;
                            int y_pos = (p_player.GetRect().y + p_player.get_frame_height() * 0.5) - height_exp_frame * 0.5;

                            exp_main.set_frame(i);
                            exp_main.SetRect(x_pos, y_pos);
                            exp_main.Show(g_screen);
                            SDL_RenderPresent(g_screen);
                        }

#ifdef USE_AUDIO
                        Mix_PlayChannel(-1, g_sound_ex_main, 0);
#endif
                        num_die++;
                        if (num_die <= 3)
                        {
                            p_player.SetRect(0, 0);
                            p_player.set_think_time(60);
                            SDL_Delay(1000);
                            player_power.Decrease();
                            player_power.Render(g_screen);
                            continue;
                        }
                        else
                        {
                            quit = true;
                            restart_to_menu = true;
                            break;
                        }
                    }
                }
            }

            // Cập nhật màn hình
            SDL_RenderPresent(g_screen);

            // Kiểm soát FPS
            int imp_time = fps.get_ticks();
            int time_for_one_frame = 1000 / FRAMES_PER_SECOND;
            if (imp_time < time_for_one_frame)
            {
                SDL_Delay(time_for_one_frame - imp_time);
            }
        }

        // Show game over screen with button before returning to menu
        if (restart_to_menu) {
            // Dùng tất cả âm thanh trướcc khi hiện thị màn hình game over
#ifdef USE_AUDIO
            Mix_HaltChannel(-1); 
#endif
            game_running = ShowGameOverScreen(g_screen, game_over_message);
        }
        else {
            game_running = false; // Exit game if not returning to menu
        }

        // Giải phóng bộ nhớ
        for (int i = 0; i < threats_list.size(); i++)
        {
            ThreatsObject* p_threat = threats_list.at(i);
            if (p_threat != NULL)
            {
                p_threat->Free();
                p_threat = NULL;
            }
        }
        threats_list.clear();

        // Gi?i phóng b? nh? c?a các item c?c máu
        for (int i = 0; i < heart_items.size(); i++)
        {
            HeartItem* heart_item = heart_items.at(i);
            if (heart_item != NULL)
            {
                heart_item->Free();
                delete heart_item;
                heart_item = NULL;
            }
        }
        heart_items.clear();

        // Close resources but don't exit program if we're returning to menu
        close();
    }

    // Final exit
    SDL_Quit();
    return 0;
}