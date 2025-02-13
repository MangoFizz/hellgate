// SPDX-License-Identifier: GPL-3.0-only

#include <unordered_map>
#include <DOOM/DOOM.h>
#include <balltze/logger.hpp>
#include <balltze/plugin.hpp>
#include <balltze/engine.hpp>
#include <balltze/engine/tag_definitions/bitmap.hpp>
#include <balltze/event.hpp>
#include <balltze/features.hpp>
#include <balltze/command.hpp>

using namespace Balltze;
using namespace Balltze::Event;
using namespace Balltze::Engine;

static std::unordered_map<KeyboardKey, doom_key_t> keyboard_input_map = {
    { KeyboardKey::TAB, DOOM_KEY_TAB },
    { KeyboardKey::ENTER, DOOM_KEY_ENTER },
    { KeyboardKey::NUM_ENTER, DOOM_KEY_ENTER },
    { KeyboardKey::ESCAPE, DOOM_KEY_UNKNOWN },
    { KeyboardKey::SPACE, DOOM_KEY_SPACE },
    { KeyboardKey::APOSTROPHE, DOOM_KEY_APOSTROPHE },
    { KeyboardKey::NUM_STAR, DOOM_KEY_MULTIPLY },
    { KeyboardKey::COMMA, DOOM_KEY_COMMA },
    { KeyboardKey::NUM_MINUS, DOOM_KEY_MINUS },
    { KeyboardKey::PERIOD, DOOM_KEY_PERIOD },
    { KeyboardKey::FORWARD_SLASH, DOOM_KEY_SLASH },
    { KeyboardKey::TOP_0, DOOM_KEY_0 },
    { KeyboardKey::TOP_1, DOOM_KEY_1 },
    { KeyboardKey::TOP_2, DOOM_KEY_2 },
    { KeyboardKey::TOP_3, DOOM_KEY_3 },
    { KeyboardKey::TOP_4, DOOM_KEY_4 },
    { KeyboardKey::TOP_5, DOOM_KEY_5 },
    { KeyboardKey::TOP_6, DOOM_KEY_6 },
    { KeyboardKey::TOP_7, DOOM_KEY_7 },
    { KeyboardKey::TOP_8, DOOM_KEY_8 },
    { KeyboardKey::TOP_9, DOOM_KEY_9 },
    { KeyboardKey::NUM_0, DOOM_KEY_0 },
    { KeyboardKey::NUM_1, DOOM_KEY_1 },
    { KeyboardKey::NUM_2, DOOM_KEY_2 },
    { KeyboardKey::NUM_3, DOOM_KEY_3 },
    { KeyboardKey::NUM_4, DOOM_KEY_4 },
    { KeyboardKey::NUM_5, DOOM_KEY_5 },
    { KeyboardKey::NUM_6, DOOM_KEY_6 },
    { KeyboardKey::NUM_7, DOOM_KEY_7 },
    { KeyboardKey::NUM_8, DOOM_KEY_8 },
    { KeyboardKey::NUM_9, DOOM_KEY_9 },
    { KeyboardKey::SEMICOLON, DOOM_KEY_SEMICOLON },
    { KeyboardKey::TOP_EQUALS, DOOM_KEY_EQUALS },
    { KeyboardKey::LEFT_BRACKET, DOOM_KEY_LEFT_BRACKET },
    { KeyboardKey::RIGHT_BRACKET, DOOM_KEY_RIGHT_BRACKET },
    { KeyboardKey::A, DOOM_KEY_A },
    { KeyboardKey::B, DOOM_KEY_B },
    { KeyboardKey::C, DOOM_KEY_C },
    { KeyboardKey::D, DOOM_KEY_D },
    { KeyboardKey::E, DOOM_KEY_E },
    { KeyboardKey::F, DOOM_KEY_F },
    { KeyboardKey::G, DOOM_KEY_G },
    { KeyboardKey::H, DOOM_KEY_H },
    { KeyboardKey::I, DOOM_KEY_I },
    { KeyboardKey::J, DOOM_KEY_J },
    { KeyboardKey::K, DOOM_KEY_K },
    { KeyboardKey::L, DOOM_KEY_L },
    { KeyboardKey::M, DOOM_KEY_M },
    { KeyboardKey::N, DOOM_KEY_N },
    { KeyboardKey::O, DOOM_KEY_O },
    { KeyboardKey::P, DOOM_KEY_ESCAPE }, // bind to escape key
    { KeyboardKey::Q, DOOM_KEY_Q },
    { KeyboardKey::R, DOOM_KEY_R },
    { KeyboardKey::S, DOOM_KEY_S },
    { KeyboardKey::T, DOOM_KEY_T },
    { KeyboardKey::U, DOOM_KEY_U },
    { KeyboardKey::V, DOOM_KEY_V },
    { KeyboardKey::W, DOOM_KEY_W },
    { KeyboardKey::X, DOOM_KEY_X },
    { KeyboardKey::Y, DOOM_KEY_Y },
    { KeyboardKey::Z, DOOM_KEY_Z },
    { KeyboardKey::BACKSPACE, DOOM_KEY_BACKSPACE },
    { KeyboardKey::LEFT_CONTROL, DOOM_KEY_CTRL },
    { KeyboardKey::RIGHT_CONTROL, DOOM_KEY_CTRL },
    { KeyboardKey::LEFT_ARROW, DOOM_KEY_LEFT_ARROW },
    { KeyboardKey::UP_ARROW, DOOM_KEY_UP_ARROW },
    { KeyboardKey::RIGHT_ARROW, DOOM_KEY_RIGHT_ARROW },
    { KeyboardKey::DOWN_ARROW, DOOM_KEY_DOWN_ARROW },
    { KeyboardKey::LEFT_SHIFT, DOOM_KEY_SHIFT },
    { KeyboardKey::RIGHT_SHIFT, DOOM_KEY_SHIFT },
    { KeyboardKey::LEFT_ALT, DOOM_KEY_ALT },
    { KeyboardKey::RIGHT_ALT, DOOM_KEY_ALT },
    { KeyboardKey::F1, DOOM_KEY_F1 },
    { KeyboardKey::F2, DOOM_KEY_F2 },
    { KeyboardKey::F3, DOOM_KEY_F3 },
    { KeyboardKey::F4, DOOM_KEY_F4 },
    { KeyboardKey::F5, DOOM_KEY_F5 },
    { KeyboardKey::F6, DOOM_KEY_F6 },
    { KeyboardKey::F7, DOOM_KEY_F7 },
    { KeyboardKey::F8, DOOM_KEY_F8 },
    { KeyboardKey::F9, DOOM_KEY_F9 },
    { KeyboardKey::F10, DOOM_KEY_F10 },
    { KeyboardKey::F11, DOOM_KEY_F11 },
    { KeyboardKey::F12, DOOM_KEY_F12 },
    { KeyboardKey::PAUSE_BREAK, DOOM_KEY_PAUSE },
};

struct InputBufferKeyState {
    bool pressed;
    bool processed;
};

Logger logger("Hellgate");

bool enabled = false;
bool render_screen_on_hud = false;
bool capture_input = false;
std::string screen_bitmap_tag_path;
std::unordered_map<KeyboardKey, InputBufferKeyState> input_buffer;

void hellgate_print(const char *message) {
    std::string buffer;
    for(const char *c = message; *c != '\0'; c++) {
        if (*c == '\n') {
            logger.info(buffer);
            buffer.clear();
        } 
        else {
            buffer += *c;
        }
    }
}

void hellgate_exit(int code) {
    logger.debug("DOOM exited with code " + std::to_string(code));
}

TagDefinitions::BitmapData *hellgate_get_screen_bitmap() {
    if(screen_bitmap_tag_path.empty()) {
        return nullptr;
    }
    auto *screen_bitmap = get_tag(screen_bitmap_tag_path, Engine::TAG_CLASS_BITMAP);
    if(screen_bitmap == nullptr) {
        return nullptr;
    }
    auto *bitmap = reinterpret_cast<TagDefinitions::Bitmap *>(screen_bitmap->data);
    auto *bitmap_data = &bitmap->bitmap_data.elements[0];
    return bitmap_data;
}

void hellgate_update_screen_resolution() {
    auto *bitmap = hellgate_get_screen_bitmap();
    if(!bitmap) {
        return;
    }
    doom_set_resolution(bitmap->width, bitmap->height);
}

void hellgate_update_frame() {
    auto *bitmap = hellgate_get_screen_bitmap();
    if(!bitmap || !bitmap->texture) {
        load_bitmap_data_texture(bitmap, true, true);
        return;
    }

    auto *texture = reinterpret_cast<IDirect3DTexture9 *>(bitmap->texture);

    // write data to texture
    const std::uint8_t *framebuffer = doom_get_framebuffer(4 /* RGBA */);
    D3DLOCKED_RECT lockedRect;
    texture->LockRect(0, &lockedRect, nullptr, 0);
    for(size_t p = 0; p < 320 * 200; p++) {
        std::uint8_t *pixel = reinterpret_cast<std::uint8_t *>(lockedRect.pBits) + p * 4;
        pixel[0] = framebuffer[p * 4 + 2];
        pixel[1] = framebuffer[p * 4 + 1];
        pixel[2] = framebuffer[p * 4 + 0];
        pixel[3] = framebuffer[p * 4 + 3];
    }
    texture->UnlockRect(0);
}

void hellgate_render() {
    auto *bitmap = hellgate_get_screen_bitmap();
    if(!bitmap || !bitmap->texture) {
        return;
    }
    static Engine::Rectangle2D draw_rect = { 0, 0, 200, 320 };
    static ColorARGBInt color_mask = { 255, 255, 255, 255 };
    draw_bitmap_in_rect(bitmap, draw_rect, color_mask);
}

void hellgate_keypress(KeyboardKey key) {
    if(input_buffer.find(key) == input_buffer.end()) {
        input_buffer[key] = { true, false };
    }
    else {
        input_buffer[key].pressed = true;
    }
}

doom_key_t hellgate_translate_key(KeyboardKey key) {
    auto it = keyboard_input_map.find(key);
    if(it != keyboard_input_map.end()) {
        return it->second;
    }
    return DOOM_KEY_UNKNOWN;
}

void hellgate_process_input() {
    auto it = input_buffer.begin();
    while(it != input_buffer.end()) {
        auto &[key, state] = *it;
        auto doom_key = hellgate_translate_key(key);
        if(state.pressed) {
            if(!state.processed) {
                doom_key_down(doom_key);
                state.processed = true;
            }
            state.pressed = false;
            it++;
        } 
        else {
            doom_key_up(doom_key);
            it = input_buffer.erase(it);
        }
    }
}

void hellgate_initialize() {
    logger.info("Initializing DOOM...");
    logger.mute_ingame(true);
    
    doom_set_print(hellgate_print);
    doom_set_exit(hellgate_exit);

    auto commandline = GetCommandLineA();
    doom_init(0, nullptr, DOOM_FLAG_MENU_DARKEN_BG);

    TickEvent::subscribe_const([](const TickEvent &event) {
        if(event.time == EVENT_TIME_AFTER && enabled) {
            hellgate_process_input();
        }
    });

    FrameEvent::subscribe_const([](const FrameEvent &event) {
        if(event.time == EVENT_TIME_BEFORE && enabled) {
            doom_update();
            hellgate_update_frame();
        }
    });

    HUDRenderEvent::subscribe_const([](const HUDRenderEvent &event) {
        if(event.time == EVENT_TIME_BEFORE) {
            if(render_screen_on_hud) {
                hellgate_render();
            }
        }
    });

    GameInputEvent::subscribe([](GameInputEvent &event) {
        if(event.time == EVENT_TIME_BEFORE && enabled) {
            if(event.context.device == INPUT_DEVICE_KEYBOARD) {
                if(capture_input) {
                    auto key = static_cast<KeyboardKey>(event.context.button.key_code);
                    hellgate_keypress(key);
                }
            }
        }
    });

    MapLoadEvent::subscribe_const([](const MapLoadEvent &event) {
        if(event.time == EVENT_TIME_AFTER) {
            screen_bitmap_tag_path.clear();
            enabled = false;
        }
    }, EVENT_PRIORITY_HIGHEST);

    register_command("render_on_screen", "debug", "Set whene to render the DOOM screen in the UI", "<enable: boolean>", [](int arg_count, const char **args) -> bool {
        if(arg_count == 1) {
            render_screen_on_hud = STR_TO_BOOL(args[0]);
        }
        return true;
    }, true, 0, 1, true, true);

    register_command("set_screen_bitmap", "debug", "Set the bitmap tag path for the DOOM screen", "<tag_path: string>", [](int arg_count, const char **args) -> bool {
        if(arg_count == 1) {
            screen_bitmap_tag_path = args[0];
            hellgate_update_screen_resolution();
        }
        return true;
    }, true, 0, 1, true, true);

    register_command("resume", "debug", "Resume DOOM", "", [](int arg_count, const char **args) -> bool {
        enabled = true;
        capture_input = false;
        doom_key_down(DOOM_KEY_ESCAPE);
        doom_key_up(DOOM_KEY_ESCAPE);
        return true;
    }, true, 0, 0, true, true);

    register_command("pause", "debug", "Shutdown DOOM", "", [](int arg_count, const char **args) -> bool {
        enabled = false;
        capture_input = false;
        doom_key_down(DOOM_KEY_ESCAPE);
        doom_key_up(DOOM_KEY_ESCAPE);
        doom_force_update();
        hellgate_update_frame();
        return true;
    }, true, 0, 0, true, true);

    register_command("capture_input", "debug", "Set whether to capture input", "<enable: boolean>", [](int arg_count, const char **args) -> bool {
        if(arg_count == 1) {
            capture_input = STR_TO_BOOL(args[0]);
        }
        return true;
    }, true, 0, 1, true, true);
}
