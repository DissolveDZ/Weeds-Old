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
    BeginDrawing();
    ClearBackground(GREEN);
    DrawTextureRec(background_texture, (Rectangle){0, 0, window_width, window_height}, (Vector2){0, 0}, WHITE);
    BeginMode2D(camera);
    for (int o = 0; o < grid_x; o++)
    {
        for (int i = 0; i < grid_y; i++)
        {
            Weed *cur_plant = &weed_array[o][i];
            if (!cur_plant->watered && cur_plant->planted && !cur_plant->auto_watering)
                DrawTextureVI(plant_stages[7], cur_plant->pos, WHITE);
            if(cur_plant->watered)
                DrawTextureVI(plant_stages[0], cur_plant->pos, WHITE);
            if (cur_plant->type != DIRT)
                DrawTextureVI(cur_plant->texture, cur_plant->pos, WHITE);
            if (cur_plant->auto_watering)
            {
                float width = 0.5f;
                DrawTextureEx(water_bucket, (Vector2){(float)cur_plant->pos.x - width / 4, (float)cur_plant->pos.y - width / 4}, 0, width, WHITE);
            }
            if (cur_plant->auto_harvest)
            {
                float width = 0.5f;
                DrawTextureEx(shovel, (Vector2){(float)cur_plant->pos.x + 1 - width / 2, (float)cur_plant->pos.y - width / 4}, 0, width, WHITE);
            }
            if (cur_plant->warning && cur_plant->planted)
            {
                float width = 0.5f;
                DrawTextureEx(warning, (Vector2){(float)cur_plant->pos.x + 1 - width/2, (float)cur_plant->pos.y + 1 - width / 2}, 0, width, WHITE);
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
                float text_size = 0.75f;
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

    DrawTexture(seed, seed_pos.x, seed_pos.y, WHITE);
    // DrawTexture(weed_texture, 200, window_height * .035 - weed_texture.height / 4, WHITE);

    if (menu != CLOSED && menu != SELECT)
    {
        DrawRectangleRec((Rectangle){0, 0, window_width, window_height}, (Color){0, 0, 0, 125});
        DrawRectangleRec((Rectangle){100, 100, window_width - 200, window_height - 200}, (Color){65, 65, 65, 125});
    }
    else if (menu == SELECT)
    {
        DrawText("Press TAB to exit select mode", window_width / 2 - MeasureText("Press TAB to exit select mode", 60) / 2, 60, 60.f, RED);
    }

    RenderUI();
    DrawAmount();
    DrawText(TextFormat("money: %i$ ", display_weeds), 20, window_height * .035, 50, BLACK);
    DrawText(TextFormat("%i: ", seeds), seed_pos.x - MeasureText(TextFormat("%i: ", seeds), 50), window_height * .035, 50, BLACK);
    int oclock = (int){day_time * 24 / 120 + 1};
    if (oclock > 24)
    {
        day_time = 24 / 120;
        day++;
    }
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
    DrawFPS(10, 10);
    EndDrawing();
}