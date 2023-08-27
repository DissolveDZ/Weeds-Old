void draw()
{
    if (cur_plant_messages_on_screen > 0)
    {
        BeginTextureMode(plant_messages_rendertexture);
        ClearBackground((Color){0, 0, 0, 0});
        for (size_t i = 0; i < cur_plant_messages_on_screen; i++)
        {
            plant_message_array[i].lifetime += frame_time * 6;
            DrawText(TextSubtext(plant_messages[plant_message_array[i].random], 0, (int)plant_message_array[i].lifetime), plant_message_array[i].position.x, plant_message_array[i].position.y, plant_message_array[i].size, (Color){255, 33, 0, plant_message_array[i].opacity});
            plant_message_array[i].position.y += plant_message_array[i].velocity.y * frame_time;
            plant_message_array[i].opacity = Lerp(plant_message_array[i].opacity, 0, .2 * frame_time);
            if (plant_message_array[i].opacity < 10)
            {
                for (int o = i; o < cur_plant_messages_on_screen; o++)
                {
                    plant_message_array[o] = plant_message_array[o + 1];
                }
                cur_plant_messages_on_screen -= 1;
            }
        }
        DrawRectangleRec((Rectangle){0, 0, (float)plant_messages_rendertexture.texture.width, (float)-plant_messages_rendertexture.texture.height}, WHITE);
        EndTextureMode();
    }
    float time_adjusted = day_time * 24 / 120 + 1;
    int oclock = (int)time_adjusted;
    if (oclock > 24)
    {
        day_time = 24 / 120;
        day++;
    }
    if (menu == CLOSED)
    {
        if (oclock < 12)
        {
            darkness = Lerp(darkness, (float)time_adjusted / 12, 1.f * frame_time * time_mult);
        }
        else
            darkness = Lerp(darkness, 1 - 1 * ((float)time_adjusted - 12) / 12, 1.f * frame_time * time_mult);
        darkness = Clamp(darkness, 0, 1);
    }
    unsigned int day_times = 4;
    for (int steps = 0; steps < day_times; steps++)
    {
        int cur_step = darkness / day_times * steps;
        switch (cur_step)
        {
        }
    }
    Vector3 day_color_mult = {Clamp(darkness + (1 - darkness) * 0.2f, 0, 1), Clamp(darkness + (1 - darkness) * 0.2f, 0, 1), Clamp(darkness + (1 - darkness) * 0.4f, 0, 1)};
    background1 = (Color){0, (121 - 50) * day_color_mult.y, (241 - 100) * day_color_mult.z, 255};
    background2 = (Color){150 * day_color_mult.x, 200 * day_color_mult.y, 255 * day_color_mult.z, 255};
    BeginDrawing();
    ClearBackground(WHITE);
    // printf("before: %f", background1.r);
    // Vector2Clamp((Vector2){background1.r, background1.g}, Vector2Zero(), (Vector2){255, 255});
    // Vector2Clamp((Vector2){background1.b, background1.a}, Vector2Zero(), (Vector2){255, 255});
    // printf("after: %f", background1.r);
    DrawRectangleGradientV(0, 0, window_width, window_height, background1, background2);
    BeginMode2D(camera);
    for (int o = 0; o < grid_x; o++)
    {
        bool left_plant = o != 0;
        bool right_plant = o + 1 < grid_x;
        for (int i = 0; i < grid_y; i++)
        {
            Weed *cur_plant = &weed_array[o][i];
            int sides = 0;
            if (left_plant && right_plant)
                sides = 0;
            else if (left_plant)
                sides = 2;
            else if (right_plant)
                sides = 1;
            if (cur_plant->watered)
                DrawTextureVI(dirt[sides], cur_plant->pos, WHITE);
            else
                DrawTextureVI(dirt_dry[sides], cur_plant->pos, WHITE);

            if (cur_plant->type == GROWN)
                DrawTextureV(plant_glow, (Vector2){cur_plant->pos.x - 0.5f, cur_plant->pos.y - 0.5f}, WHITE);

            if (cur_plant->planted)
                DrawTextureVI(cur_plant->texture, cur_plant->pos, WHITE);
        }
    }

    for (int o = 0; o < grid_x; o++)
    {
        for (int i = 0; i < grid_y; i++)
        {
            Weed *cur_plant = &weed_array[o][i];
            if (cur_plant->auto_watering)
            {
                float width = 0.5f;
                DrawTextureEx(water_bucket, (Vector2){(float)cur_plant->pos.x - width / 4, (float)cur_plant->pos.y - width / 4}, 0, width, WHITE);
            }
            if (cur_plant->auto_harvest)
            {
                float width = 0.5f;
                DrawTextureEx(shovel, (Vector2){(float)cur_plant->pos.x + 1 - width + width / 4, (float)cur_plant->pos.y - width / 4}, 0, width, WHITE);
            }
            if (cur_plant->warning && cur_plant->planted)
            {
                float width = 0.5f;
                DrawTextureEx(warning, (Vector2){(float)cur_plant->pos.x + 1 - width / 2, (float)cur_plant->pos.y + 1 - width / 2}, 0, width, WHITE);
            }
        }
    }

    if (active_selected == MOREVALUE)
    {
        for (int o = 0; o < grid_x; o++)
        {
            for (int i = 0; i < grid_y; i++)
            {
                Weed *cur_plant = &weed_array[o][i];
                const char *text = TextFormat("%i", cur_plant->value);
                float text_size = 0.5f;
                Vector2 text_offset = MeasureTextEx(pixelfont, text, text_size, 0.f);
                while (text_offset.x > 1)
                {
                    text_offset = MeasureTextEx(pixelfont, text, text_size, 0.f);
                    text_size *= 0.95f;
                }
                DrawTextEx(pixelfont, text, (Vector2){(float)cur_plant->pos.x, (float)cur_plant->pos.y}, text_size, 0.f, RED);
            }
        }
    }

    DrawTextureV(cursor_texture, (Vector2){cursor_pos.x, cursor_pos.y}, cursor_color);
    EndMode2D();
    if (cur_plant_messages_on_screen > 0)
    {
        BeginShaderMode(plant_message_displacement);
        DrawTextureRec(plant_messages_rendertexture.texture, (Rectangle){0, 0, (float)plant_messages_rendertexture.texture.width, (float)-plant_messages_rendertexture.texture.height}, (Vector2){0, 0}, WHITE);
        EndShaderMode();
    }

    DrawTexture(seed, seed_pos.x + 25, seed_pos.y, WHITE);
    if (menu != CLOSED && menu != SELECT)
    {
        DrawRectangleRec((Rectangle){0, 0, window_width, window_height}, (Color){0, 0, 0, 125});
        DrawRectangleRec((Rectangle){400, 125, window_width - 800, window_height - 250}, (Color){65, 65, 65, 125});
    }
    if (seeds == 0)
    {
        DrawText("Out of seeds!", window_width / 2 - MeasureText("Out of seeds!", 60) / 2, 60, 60.f, RED);
    }
    else if (menu == SELECT)
    {
        DrawText("Press TAB to exit select mode", window_width / 2 - MeasureText("Press TAB to exit select mode", 60) / 2, 60, 60.f, RED);
    }

    RenderUI();
    DrawButtonInfo();
    DrawTextEx(pixelfont, TextFormat("money: %i$ ", display_weeds), (Vector2){20, window_height * .035}, 50, 0.0f, BLACK);
    DrawTextEx(pixelfont, TextFormat("%i: ", seeds), (Vector2){window_width - upgrade_button->rec.width - 100 - MeasureTextEx(pixelfont, TextFormat("%i: ", seeds), 50, 0.f).x, window_height * .035}, 50, 0.f, BLACK);

    DrawTextEx(pixelfont, TextFormat("day: %i time: %i", day, oclock), (Vector2){window_width / 2 - GetTextMiddle(TextFormat("day: %i time: %i", day, oclock), 40, pixelfont, 3), 20}, 40, 3, BLACK);

    Rectangle toolrec = {0};
    int ID = GetTooltipIndex();
    if (ID != -1)
    {
        char *tooltip = buy_buttons[ID].tooltip;
        float padding = 20.0f;
        float offset = padding / 2;
        Vector2 pos = (Vector2){mouse_pos.x + offset, mouse_pos.y + offset};
        toolrec.x = pos.x - offset;
        toolrec.y = pos.y - offset;
        float text_size = 30.f;
        Vector2 size = MeasureTextEx(pixelfont, tooltip, text_size, 0.f);
        toolrec.width = size.x + padding;
        toolrec.height = size.y + padding;
        DrawRectangleRec(toolrec, GRAY);
        DrawTextEx(pixelfont, tooltip, pos, text_size, 0.f, BLACK);
    }
    DrawRectangleGradientV(0, 0, window_width, window_height, fade1, fade2);
    fade1.a = Lerp(fade1.a, 0, 0.00002f * frame_time);
    fade2.a = Lerp(fade2.a, 0, 0.00001f * frame_time);

    // DrawFPS(10, 10);
    EndDrawing();
}