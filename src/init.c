void init()
{
    max_weeds = grid_x * grid_y;
    InitAudioDevice();
    InitWindow(window_width, window_height, Game_Name);
    rander = GetRandomValue(0, 6);
    switch (rander)
    {
    case 0:
        Game_Name = "Weeds";
        break;
    case 1:
        Game_Name = "generic weed game";
        break;
    case 2:
        Game_Name = "do NOT touch the grass";
        break;
    case 3:
        Game_Name = "i think they're onto us!!";
        break;
    case 4:
        Game_Name = "0)_(o";
        break;
    case 5:
        Game_Name = "who knows how long he has been sitting there";
        break;
    case 6:
        Game_Name = "i dont know if this is legal";
        break;
    }
    SetWindowTitle(Game_Name);
    SetExitKey(-1);
    cursor1 = LoadTexture("resources/textures/cursor1.png");
    cursor2 = LoadTexture("resources/textures/cursor2.png");
    seed_bag = LoadTexture("resources/textures/seed_bag.png");
    char *temp_text = malloc(strlen("resources/textures/weed.png") + 2);
    for (int i = 0; i < 6; i++)
    {
        strcpy(temp_text, TextFormat("resources/textures/weed%i.png", i + 1));
        printf("%s\n", temp_text);
        plant_stages[i] = LoadTexture(temp_text);
        plant_stages[i].width = 1;
        plant_stages[i].height = 1;
        plant_stage_len++;
    }
    free(temp_text);
    for (int i = 0; i < 3; i++)
    {
        char *textes;
        switch (i)
        {
        case 0:
            textes = "resources/textures/dirt.png";
            break;
        case 1:
            textes = "resources/textures/dirt_left.png";
            break;
        case 2:
            textes = "resources/textures/dirt_right.png";
        }
        dirt[i] = LoadTexture(textes);
        dirt[i].width = 1;
        dirt[i].height = 1;
        printf("%u\n", dirt[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        char *textes;
        switch (i)
        {
        case 0:
            textes = "resources/textures/dirt_dry.png";
            break;
        case 1:
            textes = "resources/textures/dirt_dry_left.png";
            break;
        case 2:
            textes = "resources/textures/dirt_dry_right.png";
        }
        dirt_dry[i] = LoadTexture(textes);
        dirt_dry[i].width = 1;
        dirt_dry[i].height = 1;
    }
    weed_dry = LoadTexture("resources/textures/weed_dry.png");
    water_bucket = LoadTexture("resources/textures/water_bucket.png");
    shovel = LoadTexture("resources/textures/shovel.png");
    warning = LoadTexture("resources/textures/warning.png");
    clock = LoadTexture("resources/textures/clock.png");
    money = LoadTexture("resources/textures/money.png");
    plant_glow = LoadTexture("resources/textures/glow.png");
    plant_glow.width = 4;
    plant_glow.height = 4;
    warning.width = 1;
    warning.height = 1;
    water_bucket.width = 1;
    water_bucket.height = 1;
    shovel.width = 1;
    shovel.height = 1;
    weed_dry.width = 1;
    weed_dry.height = 1;
    cursor1.width = 1;
    cursor1.height = 1;
    cursor2.width = 1;
    cursor2.height = 1;
    cursor_texture.width = 1;
    cursor_texture.height = 1;
    seed_bag.width = 128;
    seed_bag.height = 128;
    plant_sound = LoadMultiSound("resources/sounds/plant.mp3");
    dig_sound = LoadMultiSound("resources/sounds/dig.mp3");
    water_sound = LoadMultiSound("resources/sounds/water.mp3");
    cash_sound = LoadMultiSound("resources/sounds/cash.mp3");
    pixelfont = LoadFont("resources/fonts/Pixel_Square.ttf");
    SetSoundVolumeMulti(plant_sound, 8.f);
    SetSoundVolumeMulti(cash_sound, 2.f);
    SetSoundVolumeMulti(dig_sound, 8.f);
    plant_message_array = malloc(max_plant_messages_on_screen * sizeof(Text));
    plant_message_displacement = LoadShader(0, "resources/shaders/warp.fs");
    camera.zoom = 200.f;
    music = LoadMusicStream("resources/sounds/35_Return_Trip.mp3");
    SetMusicVolume(music, 0.25f);
    PlayMusicStream(music);
    seed = LoadTexture("resources/textures/seed.png");
    seed.width = 128;
    seed.height = 64;
    upgrades_rec_color = (Color){127, 106, 79, 125};

    // start with 20 seeds
    seeds = 20;
    // start with 0$
    weeds = 1000;

    day_time = 45;

    menu = CLOSED;
    upgrade_button = CreateUIElement(1575, 20, 325, 64, "upgrades", upgrades_rec_color, BLACK, NONE);
    buy_seeds = CreateBuyButton(window_width / 2 - 200, window_height / 2 - 375, 400, 100, "Buy seeds: ", &seed_bag, upgrades_rec_color, RED, FARM, 0.25f, 5, "You need to buy seeds\n\n\n else you might go broke", false);
    buy_land = CreateBuyButton(window_width / 2 - 200, window_height / 2 - 250, 400, 100, "Buy more land: ", &dirt_dry[0], upgrades_rec_color, RED, FARM, 10, 1, "Buy more land for your farm!", false);
    buy_auto_water = CreateBuyButton(window_width / 2 - 200, window_height / 2 - 125, 400, 100, "Buy automatic watering: ", &water_bucket, upgrades_rec_color, RED, FARM, 30, 0, "Buy to automate watering, select and\n\n\n click a plant to attach auto-watering", true);
    buy_auto_harvest = CreateBuyButton(window_width / 2 - 200, window_height / 2, 400, 100, "Buy automatic harvesting: ", &shovel, upgrades_rec_color, RED, FARM, 30, 0, "Buy a harvester to automatically plant\n\n\n and harvest your plants, select and\n\n\n click a plant to attach a harvester", true);
    buy_value = CreateBuyButton(window_width / 2 - 200, window_height / 2 + 125, 400, 100, "Buy more value: ", &money, upgrades_rec_color, RED, FARM, 30, 1, "Higher quality plants will result in\n\n\n higher profit, select and click a plant\n\n\n to upgrade it's value!", true);
    buy_time = CreateBuyButton(window_width / 2 - 200, window_height / 2 + 250, 400, 100, "Speed up time: ", &clock, upgrades_rec_color, RED, FARM, 100, 1, "Speed up time so your plants grow faster!\n\n\n If sped up too much you won't be\n\n\n able to water in time so be careful!", false);
    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(144);
    weed_array = malloc(grid_x * sizeof(Weed *));
    for (int o = 0; o < grid_x; o++)
    {
        weed_array[o] = malloc(grid_y * sizeof(Weed));
        for (int i = 0; i < grid_y; i++)
        {
            InitPlant(&weed_array[o][i]);
            weed_array[o][i].pos = (Vector2I){o, i};
        }
    }
    Resize();
    UpdatePlants();
}