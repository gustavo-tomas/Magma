#include "input.h"
#include "event.h"
#include "mgm_memory.h"
#include "logger.h"

typedef struct keyboard_state 
{
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state
{
    i16 x, y;
    u8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state
{
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;

// Estado interno do input
static b8 is_initialized = FALSE;
static input_state state = { };

void initialize_input()
{
    is_initialized = TRUE;
}

void shutdown_input()
{
    is_initialized = FALSE;
}

void update_input(f64 delta_time)
{
    if (!is_initialized)
        return;

    // Atualiza estados passados
    mgm_copy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    mgm_copy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

void input_process_key(keys key, b8 pressed)
{
    // Estado não mudou
    if (state.keyboard_current.keys[key] == pressed)
        return;

    state.keyboard_current.keys[key] = pressed;

    // Processa o evento
    event_context context;
    context.data.u16[0] = key;
    fire_event(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
}

void input_process_button(buttons button, b8 pressed)
{
    // Estado não mudou
    if (state.mouse_current.buttons[button] == pressed)
        return;

    state.mouse_current.buttons[button] = pressed;

    // Processa o evento
    event_context context;
    context.data.u16[0] = button;
    fire_event(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
}

void input_process_mouse_move(i16 x, i16 y)
{
    // Estado não mudou
    if (state.mouse_current.x == x && state.mouse_current.y == y)
        return;

    state.mouse_current.x = x;
    state.mouse_current.y = y;

    // Processa o evento
    event_context context;
    context.data.u16[0] = x;
    context.data.u16[1] = y;
    fire_event(EVENT_CODE_MOUSE_MOVED, 0, context);
}

void input_process_mouse_wheel(i8 z_delta)
{
    // Processa o evento
    event_context context;
    context.data.u8[0] = z_delta;
    fire_event(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 input_is_key_down(keys key)
{
    if (!is_initialized)
        return FALSE;

    return state.keyboard_current.keys[key] == TRUE;
}

b8 input_is_key_up(keys key)
{
    if (!is_initialized)
        return TRUE;

    return state.keyboard_current.keys[key] == FALSE;
}

b8 input_was_key_down(keys key)
{
    if (!is_initialized)
        return FALSE;

    return state.keyboard_previous.keys[key] == TRUE;
}

b8 input_was_key_up(keys key)
{
    if (!is_initialized)
        return TRUE;

    return state.keyboard_previous.keys[key] == FALSE;
}

b8 input_is_button_down(buttons button)
{
    if (!is_initialized)
        return FALSE;

    return state.mouse_current.buttons[button] == TRUE;
}

b8 input_is_button_up(buttons button)
{
    if (!is_initialized)
        return TRUE;

    return state.mouse_current.buttons[button] == FALSE;
}

b8 input_was_button_down(buttons button)
{
    if (!is_initialized)
        return FALSE;

    return state.mouse_previous.buttons[button] == TRUE;
}

b8 input_was_button_up(buttons button)
{
    if (!is_initialized)
        return TRUE;

    return state.mouse_previous.buttons[button] == FALSE;
}

void input_get_mouse_position(i32* x, i32* y)
{
    if (!is_initialized)
    {
        *x = 0;
        *y = 0;
        return;
    }
        
    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

void input_get_previous_mouse_position(i32* x, i32* y)
{
    if (!is_initialized)
    {
        *x = 0;
        *y = 0;
        return;
    }
        
    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}
