#include "platform.h"

#if MGM_PLATFORM_LINUX // @TODO Android pode reclamar

#include "linux_key_codes.h"

#include "../core/event.h"
#include "../core/logger.h"
#include "../core/mgm_memory.h"
#include "../containers/vector.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// @TODO: achar um jeito de separar criação de superfícies específicas
// do vulkan da camada de plataforma
#define VK_USE_PLATFORM_XCB_KHR

#include <vulkan/vulkan.h>
#include "../renderer/vulkan/vulkan_types.inl"

#if _POSIX_C_SOURCE >= 199309L
    #include <time.h>
#else
    #include <unistd.h>
#endif

typedef struct internal_state
{
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;

    VkSurfaceKHR surface;
} internal_state;

/**
 * @brief Guarda referências para dados criados pela xcb, que precisam ser liberados manualmente.
 */
static struct xcb_ptrs
{
    xcb_intern_atom_reply_t* delete_reply;
    xcb_intern_atom_reply_t* protocols_reply;
} xcb_ptrs;

b8 initialize_platform(platform_state* plat_state, const char* application_name, i32 x, i32 y, i32 width, i32 height)
{
    // Cria o estado interno
    plat_state->internal_state = mgm_allocate(sizeof(internal_state), MEMORY_TAG_UNKNOWN); // @TODO: descobrir a tag
    internal_state* state = (internal_state *) plat_state->internal_state;

    // Conecta com o servidor
    state->display = XOpenDisplay(NULL);

    // Desativa repetições de tecla
    XAutoRepeatOff(state->display);

    state->connection = XGetXCBConnection(state->display);

    // Conexão falhou
    if (xcb_connection_has_error(state->connection))
    {
        MGM_FATAL("Erro ao conectar com o XServer!");
        return FALSE;
    }

    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    // :(
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    i32 screen_p = 0;

    for (i32 s = screen_p; s > 0; s--)
        xcb_screen_next(&it);
    
    state->screen = it.data;

    state->window = xcb_generate_id(state->connection);

    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    u32 value_list[] = { state->screen->black_pixel, event_values };

    // Cria a janela \0/
    xcb_create_window(state->connection, XCB_COPY_FROM_PARENT, state->window, 
                                                 state->screen->root, x, y, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                                 state->screen->root_visual, event_mask, value_list);
                                    
    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
                        8, strlen(application_name), application_name);

    // Notifica o servidor quando o gerenciador de janelas tentar deletar a janela (XServer é simplesmente incrível) 
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(state->connection, 0, 
                                                                strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(state->connection, 0,
                                                                   strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");

    // Esses valores devem ser liberados (free)
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(state->connection, wm_delete_cookie, NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(state->connection, wm_protocols_cookie, NULL);

    // Salva referência para deletar no futuro
    xcb_ptrs.delete_reply = wm_delete_reply;
    xcb_ptrs.protocols_reply = wm_protocols_reply;

    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window, 
                        wm_protocols_reply->atom, 4, 32, 1, &wm_delete_reply->atom);

    xcb_map_window(state->connection, state->window);

    i32 stream_result = xcb_flush(state->connection);
    if (stream_result <= 0)
    {
        MGM_FATAL("Erro ao esvaziar stream: %d", stream_result);
        return FALSE;
    }

    return TRUE;
}

void shutdown_platform(platform_state* plat_state)
{
    internal_state* state = (internal_state *) plat_state->internal_state;
    XAutoRepeatOn(state->display); // lol

    // Libera memória manualmente (porque xcb é incrível)
    free(xcb_ptrs.delete_reply);
    free(xcb_ptrs.protocols_reply);

    xcb_destroy_window(state->connection, state->window);
}

void platform_get_required_extension_names(const char*** names)
{
    vector_push(*names, &"VK_KHR_xcb_surface"); // ou xlib
}

b8 create_vulkan_surface(platform_state* plat_state, vulkan_context* context)
{
    internal_state* state = (internal_state *) plat_state->internal_state;

    VkXcbSurfaceCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR };
    create_info.connection = state->connection;
    create_info.window = state->window;

    VkResult result = vkCreateXcbSurfaceKHR(context->instance, &create_info, context->allocator, &state->surface);
    VK_CHECK(result);

    if (result != VK_SUCCESS)
    {
        MGM_FATAL("Erro ao criar superfície do Vulkan!");
        return FALSE;
    }

    context->surface = state->surface;
    return TRUE;
}

void destroy_vulkan_surface(struct vulkan_context* context)
{
    vkDestroySurfaceKHR(context->instance, context->surface, context->allocator);
}

b8 platform_dispatch_messages(platform_state* plat_state)
{
    internal_state* state = (internal_state *) plat_state->internal_state;    
    
    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    b8 quit_flagged = FALSE;

    do
    {
        event = xcb_poll_for_event(state->connection);
        if (event == 0)
            break;

        // Eventos de Input @TODO
        switch (event->response_type & ~0x80)
        {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
            {
                xcb_key_press_event_t* kp_event = (xcb_key_press_event_t*) event;
                b8 pressed = event->response_type == XCB_KEY_PRESS;
                xcb_keycode_t code = kp_event->detail;
                KeySym key_sym = XkbKeycodeToKeysym(state->display, (KeyCode) code, 0, code & ShiftMask ? 1 : 0);

                keys key = translate_keycode(key_sym);

                input_process_key(key, pressed);
            } break;

            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
            {
                xcb_button_press_event_t* mouse_event = (xcb_button_press_event_t*) event;
                b8 pressed = event->response_type == XCB_BUTTON_PRESS;
                buttons mouse_button = BUTTON_MAX_BUTTONS;
                
                switch (mouse_event->detail)
                {
                    case XCB_BUTTON_INDEX_1:
                        mouse_button = BUTTON_LEFT;
                        break;
                    
                    case XCB_BUTTON_INDEX_2:
                        mouse_button = BUTTON_MIDDLE;
                        break;

                    case XCB_BUTTON_INDEX_3:
                        mouse_button = BUTTON_RIGHT;
                        break;

                    default:
                        break;
                }

                if (mouse_button != BUTTON_MAX_BUTTONS)
                    input_process_button(mouse_button, pressed);
            } break;

            case XCB_MOTION_NOTIFY:
            {
                xcb_motion_notify_event_t* move_event = (xcb_motion_notify_event_t*) event;
                input_process_mouse_move(move_event->event_x, move_event->event_y);
            } break;

            case XCB_CONFIGURE_NOTIFY:
                break;

            case XCB_CLIENT_MESSAGE:
            {
                cm = (xcb_client_message_event_t *) event;

                // Fecha a janela
                if (cm->data.data32[0] == state->wm_delete_win)
                    quit_flagged = TRUE;
            } break;
            
            default:
                break;
        }

        free(event);
    } while (event != 0);
    
    return !quit_flagged;
}

// @TODO
void* platform_allocate(u64 size, b8 aligned)
{
    return malloc(size);
}

void platform_free(void* block, b8 aligned)
{
    free(block);   
}

void* platform_zero_memory(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size)
{
    return memcpy(dest, source, size);
}

void* platform_set_memory(void* dest, i32 value, u64 size)
{
    return memset(dest, value, size);
}

void platform_console_write(const char* message, u8 color)
{
    const char* color_strings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;37" };

    printf("\033[%sm%s\033[0m", color_strings[color], message);
}

void platform_console_write_error(const char* message, u8 color)
{
    const char* color_strings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;37" };

    printf("\033[%sm%s\033[0m", color_strings[color], message);
}

f64 platform_get_absolute_time()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + (now.tv_nsec * 0.000000001);
}

void platform_sleep(u64 ms)
{
    #if _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000000;
        nanosleep(&ts, 0);
    #else
        if (ms >= 1000)
            sleep(ms / 1000);
        usleep((ms % 1000) * 1000);
    #endif
}

#endif
