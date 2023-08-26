inline Vector2I GetScreenToWorld2DVectorI(Vector2 position, Camera2D camera)
{
    Vector2 temp = GetScreenToWorld2D(position, camera);
    return (Vector2I){temp.x, temp.y};
}
bool CheckOutOfBounds(Vector2I position, directions direction)
{
    switch (direction)
    {
    case RIGHT:
        if (position.x < grid_x - 1)
            return true;
        else
            return false;
    case LEFT:
        if (position.x > 0)
            return true;
        else
            return false;
    case UP:
        if (position.y > 0)
            return true;
        else
            return false;
    case DOWN:
        if (position.y < grid_y - 1)
            return true;
        else
            return false;
    }
}
Plant(Vector2I position)
{
    Weed *cur_plant = &weed_array[position.x][position.y];
    if (!cur_plant->planted)
    {
        if (seeds > 0)
        {
            cur_plant->last_watered = global_time;
            cur_plant->planted = true;
            cur_plant->watered = true;
            cur_plant->time = global_time;
            seeds -= 1;
            UpdatePlants();
            PlaySoundMulti(&plant_sound);
            if (GetRandomValue(1, 20) == 10)
            {
                cur_plant_messages_on_screen++;
                if (cur_plant_messages_on_screen >= max_plant_messages_on_screen - 1)
                {
                    max_plant_messages += 10;
                    plant_message_array = realloc(plant_message_array, sizeof(Text) * max_plant_messages);
                }
                plant_message_array[cur_plant_messages_on_screen - 1].position = ((Vector2){GetRandomValue(0, window_width), GetRandomValue(0, window_height)});
                plant_message_array[cur_plant_messages_on_screen - 1].random = GetRandomValue(0, 9-1);
                plant_message_array[cur_plant_messages_on_screen - 1].velocity.y = GetRandomValue(1, 50);
                plant_message_array[cur_plant_messages_on_screen - 1].size = GetRandomValue(20, 60);
                plant_message_array[cur_plant_messages_on_screen - 1].lifetime = 0;
                plant_message_array[cur_plant_messages_on_screen - 1].opacity = 255;
            }
        }
    }
    else
    {
        if (cur_plant->type == GROWN && cur_plant->watered)
        {
            cur_plant->planted = false;
            PlaySoundMulti(&dig_sound);
            UpdatePlants();
            weeds += cur_plant->value;
        }
        else if (!cur_plant->watered)
        {
            cur_plant->last_watered = global_time;
            cur_plant->watered = true;
            PlaySoundMulti(&water_sound);
        }
    }
}

void MoneyAnimation()
{
    // display_weeds = (int)Lerp((float)display_weeds, (float)weeds, 0.1f);
    if (display_weeds < weeds)
        display_weeds += (int)(10.f * frame_time);
    else if (display_weeds > weeds)
        display_weeds -= (int)(10.f * frame_time);
    display_weeds = weeds;
}

void CameraMovement()
{
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !hover_ui)
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);

        camera.target = Vector2Add(camera.target, (Vector2){delta.x, delta.y});
    }
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.target = (Vector2){mouseWorldPos.x, mouseWorldPos.y};
        camera.offset = (Vector2){mouse_pos.x, mouse_pos.y};
        const float zoomIncrement = 10.f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < zoomIncrement)
            camera.zoom = zoomIncrement;
    }
}

void ProcessButtons()
{

    if (upgrade_button->pressed || IsKeyPressed(KEY_TAB))
    {
        if (menu == CLOSED)
            menu = FARM;
        else if (menu == FARM)
            menu = CLOSED;
    }
    if (buy_seeds->ui[0]->pressed && Buy(buy_seeds))
    {
        seeds += buy_seeds->amount * 2;
    }
    if (buy_auto_water->ui[0]->pressed)
    {
        active_selected = AUTOWATER;
        menu = SELECT;
    }
    if (buy_auto_harvest->ui[0]->pressed)
    {
        active_selected = AUTOFARM;
        menu = SELECT;
    }
    if (buy_value->ui[0]->pressed)
    {
        active_selected = MOREVALUE;
        menu = SELECT;
    }
    if (buy_time->ui[0]->pressed)
    {
        for (int i = 0; i < buy_time->amount; i++)
        {
            time_mult *= 1.25f;
        }
        printf("time mult: %f\n", time_mult);
    }
    if (buy_land->ui[0]->pressed && Buy(buy_land))
    {
        if (grid_x <= grid_y)
        {
            grid_x += 1;
            weed_array = realloc(weed_array, grid_x * sizeof(Weed *));
            weed_array[grid_x - 1] = malloc(grid_y * sizeof(Weed));
            for (int i = 0; i < grid_y; i++)
            {
                InitPlant(&weed_array[grid_x - 1][i]);
            }
        }
        else
        {
            grid_y += 1;
            for (int i = 0; i < grid_x; i++)
            {
                weed_array[i] = realloc(weed_array[i], grid_y * sizeof(Weed));
                InitPlant(&weed_array[i][grid_y - 1]);
            }
        }
        UpdatePlants();
    }
}

void update()
{
    mouse_pos = GetMousePosition();
    hover_ui = false;
    UpdateUI();
    UpdateBuyButtons();
    ProcessButtons();
    if (IsWindowResized())
    {
        int old_width = window_width;
        int old_height = window_height;
        window_width = GetScreenWidth();
        window_height = GetScreenHeight();
        ResizeUI(old_width, old_height);
        Resize();
    }
    MoneyAnimation();
    switch (menu)
    {
    case CLOSED:
        frame_time = GetFrameTime();
        global_time += frame_time * time_mult;
        day_time += frame_time * time_mult;
        time_passed += frame_time * 7;
        UpdatePlants();
        CameraMovement();
        cursor_color = WHITE;
        if ((int)(time_passed) % 2)
        {
            toggle_cursor = !toggle_cursor;
            time_passed = 0;
        }
        if (toggle_cursor)
            cursor_texture = cursor1;
        else
            cursor_texture = cursor2;

        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        {
            direction = RIGHT;
            if (CheckOutOfBounds(cursor_pos, direction))
            {
                cursor_pos.x += 1;
            }
        }
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        {
            direction = LEFT;
            if (CheckOutOfBounds(cursor_pos, direction))
            {
                cursor_pos.x -= 1;
            }
        }
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        {
            direction = UP;
            if (CheckOutOfBounds(cursor_pos, direction))
            {
                cursor_pos.y -= 1;
            }
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        {
            direction = DOWN;
            if (CheckOutOfBounds(cursor_pos, direction))
            {
                cursor_pos.y += 1;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hover_ui)
        {
            Vector2 temp = GetScreenToWorld2D(mouse_pos, camera);
            Vector2I rounded_click = (Vector2I){floorf(temp.x), floorf(temp.y)};
            cursor_pos.x = rounded_click.x;
            cursor_pos.y = rounded_click.y;
            if (rounded_click.x < 0)
                cursor_pos.x = 0;
            if (rounded_click.x > grid_x - 1)
                cursor_pos.x = grid_x - 1;
            if (rounded_click.y < 0)
                cursor_pos.y = 0;
            if (rounded_click.y > grid_y - 1)
                cursor_pos.y = grid_y - 1;
            if (last_cursor_pos.x == cursor_pos.x && last_cursor_pos.y == cursor_pos.y)
                Plant(cursor_pos);
            last_cursor_pos = cursor_pos;
        }

        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();
        if (IsKeyPressed(KEY_ENTER))
        {
            Plant(cursor_pos);
        }
        if (camera.target.x + window_width < 0)
            printf("display plaent");
        break;
    case SELECT:
        if (IsKeyPressed(KEY_TAB))
        {
            menu = CLOSED;
            active_selected = DEFAULT;
            break;
        }
        CameraMovement();
        Vector2 temp = GetScreenToWorld2D(mouse_pos, camera);
        Vector2I rounded_click = (Vector2I){floorf(temp.x), floorf(temp.y)};
        cursor_pos.x = rounded_click.x;
        cursor_pos.y = rounded_click.y;
        if (rounded_click.x < 0)
            cursor_pos.x = 0;
        if (rounded_click.x > grid_x - 1)
            cursor_pos.x = grid_x - 1;
        if (rounded_click.y < 0)
            cursor_pos.y = 0;
        if (rounded_click.y > grid_y - 1)
            cursor_pos.y = grid_y - 1;
        last_cursor_pos = cursor_pos;
        Weed *cur_weed = &weed_array[cursor_pos.x][cursor_pos.y];
        switch (active_selected)
        {
        case AUTOWATER:
            if (!cur_weed->auto_watering)
                cursor_color = WHITE;
            else
                cursor_color = RED;
            if (!cur_weed->auto_watering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hover_ui && Buy(buy_auto_water))
            {
                cur_weed->auto_watering = true;
                menu = CLOSED;
                active_selected = DEFAULT;
            }
            break;
        case AUTOFARM:
            if (!cur_weed->auto_harvest)
                cursor_color = WHITE;
            else
                cursor_color = RED;
            if (!cur_weed->auto_harvest && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hover_ui && Buy(buy_auto_harvest))
            {
                cur_weed->auto_harvest = true;
                menu = CLOSED;
                active_selected = DEFAULT;
            }
            break;
        case MOREVALUE:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hover_ui && Buy(buy_value))
            {
                cur_weed->value += 1 * buy_value->amount;
                menu = CLOSED;
                active_selected = DEFAULT;
            }
            break;
        }
        break;
    }
    UpdateMusicStream(music);
}