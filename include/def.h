#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

int window_width;
int window_height;

typedef struct Player
{
    Rectangle rec;
    Texture2D texture;
    int size;
    float walk_speed;
    Vector2 acceleration;
    Vector2 velocity;
} Player;

Player player;

typedef struct Vector2I
{
    int x;
    int y;
} Vector2I;

typedef enum directions
{
    RIGHT,
    LEFT,
    UP,
    DOWN
} directions;
directions direction;

typedef enum Menu
{
    NONE,
    CLOSED,
    FARM,
    SELECT,
    SKILL,
    SEEDS,
    FARMERS
} Menu;
Menu menu;

typedef enum Weed_Types
{
    DIRT,
    PLANTED,
    SAPLING,
    GROW1,
    GROW2,
    GROW3,
    GROWN,
    DRY,
    DED
} Weed_Types;

typedef struct Weed
{
    Weed_Types type;
    Texture texture;
    Vector2I pos;
    bool watered;
    bool planted;
    bool auto_watering;
    bool auto_harvest;
    int value;
    float time;
    float last_watered;
} Weed;

typedef struct Text
{
    char *text;
    Vector2 position;
    Vector2 velocity;
    float size;
    int random;
    float opacity;
    float lifetime;
} Text;

typedef struct MultiSound
{
    Sound *sound;
    int ID;
    int len;
} MultiSound;

static inline void DrawTextureVI(Texture2D texture, Vector2I pos, Color tint)
{
    DrawTexture(texture, pos.x, pos.y, tint);
}

Shader plant_message_displacement;
RenderTexture2D plant_messages_rendertexture;

int window_width = 1920;
int window_height = 1080;
int rander = 0;
char *Game_Name = "";
unsigned int plant_stage_len = 0;
unsigned int max_plant_messages = 20;
unsigned int max_plant_messages_on_screen = 20;
unsigned int cur_plant_messages_on_screen = 0;
bool hover_ui = false;
char *plant_messages[] =
    {
        "planted!",
        "mission completed return to HQ",
        "I don't feel so good..",
        "very nice!",
        "yuy!",
        "finally..",
        "great job!?!?",
        "just a bit longer..",
        "is this what I'm supposed to do?"};
Text *plant_message_array;
Color upgrades_rec_color;
Color cursor_color = WHITE;

Texture2D cursor1;
Texture2D cursor2;
Texture2D cursor_texture;
Texture2D plant_stages[8];
Texture2D weed_dry;
Texture2D background_texture;
Texture2D seed_bag_weed1;
Texture2D water_bucket;
Texture2D shovel;

float global_time;
float day_time;
float time_mult = 1.f;
float decay_speed = 1.f;
float water_need = 0.25f;

Music music;
Model weed_model;

int max_weeds;
int seeds = 1000;
unsigned int day = 0;
unsigned int grid_x = 3;
unsigned int grid_y = 1;
unsigned int weeds = 0;
unsigned int display_weeds = 0;
Camera2D camera = {0};

Weed **weed_array;

MultiSound plant_sound;
MultiSound dig_sound;
MultiSound water_sound;
MultiSound cash_sound;
Vector2 src_resolution = {1920, 1080};
Vector2 shop_slide_offset = {0};
Vector2 mouse_pos = {0};
Vector2 seed_pos = {0};
Vector2I cursor_pos = {0};
Vector2I last_cursor_pos = {0};
float frame_time;
float time_passed;
bool toggle_cursor = false;
bool fullscreen = false;
Texture2D seed;

Font pixelfont;
typedef struct UI
{
    Rectangle rec;
    Color rec_color;
    Color text_color;
    Menu menu;
    char *text;
    char *display_text;
    bool mouse_over;
    bool pressed;
    bool format;
} UI;

typedef struct Buy_Button
{
    UI **ui;
    float cost;
    unsigned int amount;
    char *tooltip;
    bool select_plant;
} Buy_Button;

UI *UI_array;
unsigned int UI_len = 0;
unsigned int UI_max = 50;

UI *upgrade_button;

Buy_Button *buy_land;
Buy_Button *buy_auto_water;
Buy_Button *buy_auto_harvest;
Buy_Button *buy_value;
Buy_Button *buy_seeds;
Buy_Button *buy_time;

int AddUIElement(Rectangle rec, char *text, Color rec_color, Color text_color, Menu menu)
{
    if (UI_len >= UI_max - 1)
    {
        UI_max += 10;
        UI_array = realloc(UI_array, UI_max * sizeof(UI));
    }
    UI_array[UI_len].rec = rec;
    UI_array[UI_len].text = text;
    UI_array[UI_len].display_text = malloc(TextLength(text) + 1);
    strcpy(UI_array[UI_len].display_text, text);
    UI_array[UI_len].rec_color = rec_color;
    UI_array[UI_len].text_color = text_color;
    UI_array[UI_len].menu = menu;
    UI_len++;
    return UI_len - 1;
}

// Create a UI element which will be returned as a pointer
UI *CreateUIElement(float x, float y, float width, float height, char *text, Color rec_color, Color text_color, Menu menu)
{
    if (UI_len == 0)
        UI_array = malloc(UI_max * sizeof(UI));
    return &UI_array[AddUIElement((Rectangle){x, y, width, height}, text, rec_color, text_color, menu)];
}

void UpdateUI()
{
    for (int i = 0; i < UI_len; i++)
    {
        UI_array[i].pressed = false;
        UI_array[i].mouse_over = false;
        if (UI_array[i].menu != NONE && UI_array[i].menu != menu)
            continue;
        if (CheckCollisionPointRec(mouse_pos, UI_array[i].rec))
        {
            UI_array[i].mouse_over = true;
            hover_ui = true;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                UI_array[i].pressed = true;
            }
        }
    }
}
void RenderUI()
{
    for (int i = 0; i < UI_len; i++)
    {
        UI *cur_ui = &UI_array[i];
        if (cur_ui->menu != NONE && cur_ui->menu != menu)
            continue;
        Color color = cur_ui->rec_color;
        float darken = 0.5f;
        if (cur_ui->mouse_over)
        {
            color = (Color){color.r * darken, color.g * darken, color.b * darken, color.a};
        }
        DrawRectangleRec(cur_ui->rec, color);
        float text_size = cur_ui->rec.width / 10;
        Vector2 text_offset = MeasureTextEx(pixelfont, cur_ui->display_text, text_size, 0.f);
        while (text_offset.x > cur_ui->rec.width)
        {
            text_offset = MeasureTextEx(pixelfont, cur_ui->display_text, text_size, 0.f);
            text_size *= 0.95f;
        }
        DrawTextEx(pixelfont, cur_ui->display_text, (Vector2){cur_ui->rec.x + cur_ui->rec.width / 2 - text_offset.x / 2, cur_ui->rec.y + cur_ui->rec.height / 2 - text_offset.y / 2}, text_size, 0.f, cur_ui->text_color);
    }
}

void ResizeUI(int old_width, int old_height)
{
    Rectangle fraction;
    for (int i = 0; i < UI_len; i++)
    {
        UI *cur_ui = &UI_array[i];
        fraction = (Rectangle){cur_ui->rec.x / (float)old_width, cur_ui->rec.y / (float)old_height, cur_ui->rec.width / (float)old_width, cur_ui->rec.height / (float)old_height};
        cur_ui->rec = (Rectangle){fraction.x * window_width, fraction.y * window_height, fraction.width * window_width, fraction.height * window_height};
    }
}

void FormatUI(UI *ui, char *type, const char *text)
{
    int len = (TextLength(ui->text) + TextLength(text)) * sizeof(char) + 1;
    if (!ui->format)
        ui->display_text = realloc(ui->display_text, len);
    else if (TextLength(ui->display_text) + 1 < len)
        ui->display_text = realloc(ui->display_text, len);
    strcpy(ui->display_text, ui->text);
    ui->format = true;
}

void FormatUIInt(UI *ui, char *type, int val)
{
    const char *text = TextFormat(type, val);
    FormatUI(ui, type, text);
    strcat(ui->display_text, text);
}

void FormatUIFloat(UI *ui, char *type, int var)
{
    // FormatUI(ui, type);
    //  strcpy(ui->text, TextFormat(type, var));
}

void Resize()
{
    Image VerticalGradient = GenImageGradientLinear(window_width, window_height, -1, (Color){150, 200, 255, 255}, (Color){0, 121 - 50, 241 - 100, 255});
    background_texture = LoadTextureFromImage(VerticalGradient);
    plant_messages_rendertexture = LoadRenderTexture(window_width, window_height);
    seed_pos = (Vector2){upgrade_button->rec.x - 150, window_height * .035 - seed.height / 4};
    float screenSize[2] = {(float)window_width, (float)window_height};
    if (window_width != 0 && window_height != 0)
        SetShaderValue(plant_message_displacement, GetShaderLocation(plant_message_displacement, "size"), &screenSize, SHADER_UNIFORM_VEC2);
    camera.target = (Vector2){grid_x / 2, grid_y / 2};
    camera.offset = (Vector2){window_width / 2, window_height / 2};
}

Sound **sound_array;
unsigned int max_sounds = 10;
unsigned int max_parallel_sounds = 10;
unsigned int sounds_len = 0;

MultiSound LoadMultiSound(const char *path)
{
    MultiSound sound = {0};
    if (sounds_len == 0)
    {
        sound_array = malloc(max_sounds * sizeof(void *));
        for (int i = 0; i < max_sounds; i++)
            sound_array[i] = malloc(max_parallel_sounds * sizeof(Sound));
    }
    sound_array[sounds_len][0] = LoadSound(path);
    for (int i = 1; i < max_parallel_sounds; i++)
    {
        sound_array[sounds_len][i] = LoadSoundAlias(sound_array[sounds_len][0]);
    }
    sound.sound = &sound_array[sounds_len][0];
    sound.ID = sounds_len;
    sound.len = 0;
    sounds_len++;
    return sound;
}

void SetSoundVolumeMulti(MultiSound sound, float volume)
{
    for (int i = 0; i < max_parallel_sounds; i++)
    {
        SetSoundVolume(sound_array[sound.ID][i], volume);
    }
}

void PlaySoundMulti(MultiSound *sound)
{
    PlaySound(sound_array[sound->ID][sound->len]);
    sound->len++;
    if (sound->len >= max_parallel_sounds)
        sound->len = 0;
}

void InitPlant(Weed *weed)
{
    weed->pos = (Vector2I){0, 0};
    weed->type = DIRT;
    weed->time = 0;
    weed->last_watered = 0;
    weed->planted = false;
    weed->auto_watering = false;
    weed->auto_harvest = false;
    weed->value = 1;
}

Buy_Button *buy_buttons;
unsigned int buy_buttons_len = 0;
unsigned int buy_buttons_max = 10;

void FormatBuyButtons()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        Buy_Button *cur_button = &buy_buttons[i];
        int amount = cur_button->amount ? cur_button->amount : 1;
        FormatUIInt(cur_button->ui[0], "%i$", cur_button->cost * amount);
    }
}

int AmountLookup(int amount, bool dir)
{
    int new_amount = 1;
    if (dir)
    {
        switch (amount)
        {
        case 1:
            new_amount = 2;
            break;
        case 2:
            new_amount = 5;
            break;
        case 5:
            new_amount = 10;
            break;
        case 10:
            new_amount = 20;
            break;
        case 20:
            new_amount = 50;
            break;
        case 50:
            new_amount = 100;
            break;
        case 100:
            new_amount = 250;
            break;
        case 250:
            new_amount = 500;
            break;
        case 500:
            new_amount = 1000;
            break;

        default:
            new_amount = 1;
            break;
        }
    }
    else
    {
        switch (amount)
        {
        case 2:
            new_amount = 1;
            break;
        case 5:
            new_amount = 2;
            break;
        case 10:
            new_amount = 5;
            break;
        case 20:
            new_amount = 10;
            break;
        case 50:
            new_amount = 20;
            break;
        case 100:
            new_amount = 50;
            break;
        case 250:
            new_amount = 100;
            break;
        case 500:
            new_amount = 250;
            break;
        case 1000:
            new_amount = 500;
            break;

        default:
            new_amount = 1000;
            break;
        }
    }
    return new_amount;
}

void UpdateBuyButtons()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        Buy_Button *cur_button = &buy_buttons[i];
        int amount = cur_button->amount ? cur_button->amount : 1;
        if (cur_button->cost * amount <= weeds && 1 <= floorf(cur_button->cost * amount))
        {
            cur_button->ui[0]->text_color = BLACK;
            if (cur_button->amount)
            {
                cur_button->ui[1]->text_color = BLACK;
                cur_button->ui[2]->text_color = BLACK;
            }
        }
        else
        {
            cur_button->ui[0]->text_color = RED;
            if (cur_button->amount)
            {
                cur_button->ui[1]->text_color = RED;
                cur_button->ui[2]->text_color = RED;
            }
        }
        if (cur_button->amount && cur_button->ui[1]->pressed)
        {
            cur_button->amount = AmountLookup(cur_button->amount, 1);
            FormatBuyButtons();
        }
        if (cur_button->amount && cur_button->ui[2]->pressed)
        {
            cur_button->amount = AmountLookup(cur_button->amount, 0);
            FormatBuyButtons();
        }
    }
}

bool Buy(Buy_Button *button)
{
    int amount = button->amount ? button->amount : 1;
    if (weeds >= button->cost * amount && 1 <= floorf(button->cost * amount))
    {
        PlaySoundMulti(&cash_sound);
        weeds -= button->cost * amount;
        // button->cost += button->cost / 2;
        // button->cost *= 1.25f;
        FormatBuyButtons();
        return true;
    }
    return false;
}
typedef enum Selectables
{
    DEFAULT,
    AUTOWATER,
    AUTOFARM,
    MOREVALUE
} Selectables;
Selectables active_selected;

// Create a Buy button which will be returned as a pointer
Buy_Button *CreateBuyButton(float x, float y, float width, float height, char *text, Color rec_color, Color text_color, Menu menu, float cost, int amount, char *tooltip, bool select_plant)
{
    Buy_Button *button = {0};
    if (buy_buttons_len == 0)
    {
        buy_buttons = malloc(buy_buttons_max * sizeof(Buy_Button));
    }
    if (buy_buttons_len >= buy_buttons_max)
    {
        buy_buttons_max += 10;
        buy_buttons = realloc(buy_buttons, buy_buttons_max * sizeof(Buy_Button));
    }
    if (UI_len == 0)
        UI_array = malloc(UI_max * sizeof(UI));
    button = &buy_buttons[buy_buttons_len];
    float padding = 120.f;
    int size = 120;
    if (amount)
    {
        button->ui = malloc(3 * sizeof(UI *));
        button->ui[1] = &UI_array[AddUIElement((Rectangle){x + width + padding, y, size, height / 2}, "up", rec_color, text_color, menu)];
        button->ui[2] = &UI_array[AddUIElement((Rectangle){x + width + padding, y + height / 2, size, height / 2}, "down", rec_color, text_color, menu)];
    }
    else
        button->ui = malloc(1 * sizeof(UI *));
    button->ui[0] = &UI_array[AddUIElement((Rectangle){x, y, width, height}, text, rec_color, text_color, menu)];
    button->amount = amount;
    button->cost = cost;
    button->tooltip = tooltip;
    buy_buttons_len++;
    FormatBuyButtons();
    return button;
}

void DrawAmount()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        Buy_Button *cur_button = &buy_buttons[i];
        if ((menu != cur_button->ui[0]->menu) || !cur_button->amount)
            continue;
        const char *text = TextFormat("%i", cur_button->amount);
        float text_size = 50.f;
        Vector2 text_len = MeasureTextEx(pixelfont, text, text_size, 0.f);
        Vector2 dist = (Vector2){(cur_button->ui[1]->rec.x - cur_button->ui[0]->rec.x) + cur_button->ui[0]->rec.x - cur_button->ui[1]->rec.width / 2, cur_button->ui[0]->rec.y + cur_button->ui[0]->rec.height / 2}; //(Vector2){(cur_button->ui[1]->rec.x - cur_button->ui[0]->rec.x)/2 + cur_button->ui[0]->rec.x - cur_button->ui[1]->rec.width/2, cur_button->ui[1]->rec.y + cur_button->ui[1]->rec.height};
        DrawTextEx(pixelfont, text, (Vector2){dist.x - text_len.x / 2, dist.y - text_len.y / 2}, text_size, 0.f, cur_button->ui[0]->text_color);
    }
}

int GetTooltipIndex()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        if (buy_buttons[i].ui[0]->mouse_over)
            return i;
    }
    return -1;
}

void UpdatePlants()
{
    int t = 0;
    for (size_t i = 0; i < grid_x; i++)
    {
        for (size_t o = 0; o < grid_y; o++)
        {
            Weed *cur_plant = &weed_array[i][o];
            if (cur_plant->planted)
            {
                cur_plant->pos = (Vector2I){i, o};
                if ((global_time - cur_plant->time) * decay_speed <= 1)
                    cur_plant->type = PLANTED;
                else if ((global_time - cur_plant->time) * decay_speed <= 2)
                    cur_plant->type = SAPLING;
                else if ((global_time - cur_plant->time) * decay_speed <= 3)
                    cur_plant->type = GROW1;
                else if ((global_time - cur_plant->time) * decay_speed <= 4)
                    cur_plant->type = GROW2;
                else if ((global_time - cur_plant->time) * decay_speed <= 5)
                    cur_plant->type = GROW3;
                else if ((global_time - cur_plant->time) * decay_speed >= 6)
                    cur_plant->type = GROWN;
                if ((global_time - cur_plant->last_watered) * water_need > 0.5f)
                    cur_plant->watered = false;
                if ((global_time - cur_plant->last_watered) * water_need > 0.75f && !cur_plant->auto_watering)
                    cur_plant->type = DED;
            }
            if (!cur_plant->planted && cur_plant->auto_harvest && seeds > 0)
            {
                PlaySoundMulti(&plant_sound);
                seeds -= 1;
                cur_plant->planted = true;
                cur_plant->time = global_time;
                cur_plant->last_watered = global_time;
                cur_plant->watered = true;
            }
            if (cur_plant->type == GROWN && cur_plant->auto_harvest)
            {
                PlaySoundMulti(&dig_sound);
                cur_plant->planted = false;
                weeds += cur_plant->value;
            }
            if (!cur_plant->planted)
            {
                cur_plant->pos = (Vector2I){i, o};
                cur_plant->type = DIRT;
                cur_plant->time = 0;
            }
            if (cur_plant->type == DED)
            {
                cur_plant->type = DIRT;
                cur_plant->planted = false;
            }
            else
                cur_plant->texture = plant_stages[(int)cur_plant->type];
            //if (!cur_plant->watered && cur_plant->planted && !cur_plant->auto_watering)
                //cur_plant->texture = weed_dry;
            if (!cur_plant->watered && cur_plant->auto_watering)
            {
                PlaySoundMulti(&water_sound);
                cur_plant->last_watered = global_time;
                cur_plant->watered = true;
            }
        }
    }
}

float GetTextMiddle(char *text, int size, Font font, float spacing)
{
    return MeasureTextEx(font, text, size, spacing).x / 2;
}