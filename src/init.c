void init()
{
    max_weeds = grid_x * grid_y;
    InitAudioDevice();
    InitWindow(window_width, window_height, Game_Name);
    rander = GetRandomValue(0, 5);
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
    }
    SetWindowTitle(Game_Name);
    cursor1 = LoadTexture("resources/textures/cursor1.png");
    cursor2 = LoadTexture("resources/textures/cursor2.png");
    dirt_texture = LoadTexture("resources/textures/dirt.png");
    seed_bag_weed1 = LoadTexture("resources/textures/seed_bag_weed1.png");
    weed_planted = LoadTexture("resources/textures/weed_planted.png");
    weed_sapling = LoadTexture("resources/textures/weed_sapling.png");
    weed_grow1 = LoadTexture("resources/textures/weed_grow1.png");
    weed_grow2 = LoadTexture("resources/textures/weed_grow2.png");
    weed_grow3 = LoadTexture("resources/textures/weed_grow3.png");
    weed_dry = LoadTexture("resources/textures/weed_dry.png");
    weed_texture = LoadTexture("resources/textures/weed.png");
    water_bucket = LoadTexture("resources/textures/water_bucket.png");
    shovel = LoadTexture("resources/textures/shovel.png");
    water_bucket.width = 1;
    water_bucket.height = 1;
    shovel.width = 1;
    shovel.height = 1;
    weed_planted.width = 1;
    weed_planted.height = 1;
    weed_sapling.width = 1;
    weed_sapling.height = 1;
    weed_grow1.width = 1;
    weed_grow1.height = 1;
    weed_grow2.width = 1;
    weed_grow2.height = 1;
    weed_grow3.width = 1;
    weed_grow3.height = 1;
    weed_dry.width = 1;
    weed_dry.height = 1;
    weed_texture.width = 1;
    weed_texture.height = 1;
    cursor1.width = 1;
    cursor1.height = 1;
    cursor2.width = 1;
    cursor2.height = 1;
    cursor_texture.width = 1;
    cursor_texture.height = 1;
    dirt_texture.width = 1;
    dirt_texture.height = 1;
    seed_bag_weed1.width = 64;
    seed_bag_weed1.height = 64;
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
    PlayMusicStream(music);
    seed = LoadTexture("resources/textures/seed.png");
    seed.width = 128;
    seed.height = 64;
    upgrades_rec_color = (Color){127, 106, 79, 125};
    seeds = 20;
    weeds = 10000;
    menu = CLOSED;
    upgrade_button = CreateUIElement(1575, 20, 325, 64, "upgrades", upgrades_rec_color, BLACK, NONE);
    buy_seeds = CreateBuyButton(window_width / 2 - 200, window_height / 2 - 400, 400, 100, "Buy seeds: ", upgrades_rec_color, RED, FARM, 1, 1, "You need to buy seeds\n\n\n else you might go broke", false);
    buy_land = CreateBuyButton(window_width / 2 - 200, window_height / 2 - 250, 400, 100, "Buy more land: ", upgrades_rec_color, RED, FARM, 10, 1, "Buy more land for your farm!", false);
    buy_auto_water = CreateBuyButton(window_width / 2 - 200, window_height / 2 - 100, 400, 100, "Buy automatic watering: ", upgrades_rec_color, RED, FARM, 30, 0, "Buy to automate watering, select and\n\n\n click a plant to attach auto-watering", true);
    buy_auto_harvest = CreateBuyButton(window_width / 2 - 200, window_height / 2 + 50, 400, 100, "Buy automatic harvesting: ", upgrades_rec_color, RED, FARM, 30, 0, "Buy a harvester to automatically plant\n\n\n and harvest your plants, select and\n\n\n click a plant to attach a harvester", true);
    buy_value = CreateBuyButton(window_width / 2 - 200, window_height / 2 + 200, 400, 100, "Buy more value: ", upgrades_rec_color, RED, FARM, 30, 1, "Higher quality plants will result in\n\n\n higher profit, select and click a plant\n\n\n to upgrade it's value!", true);
    buy_time = CreateBuyButton(window_width / 2 - 200, window_height / 2 + 350, 400, 100, "Speed up time: ", upgrades_rec_color, RED, FARM, 100, 1, "Speed up time so your plants grow faster!\n\n\n If sped up too much you won't be\n\n\n able to water in time so be careful!", false);

    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(144);
    weed_array = malloc(grid_x * sizeof(void *));
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