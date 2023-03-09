#ifndef INPUT_H
#define INPUT_H

/**
 * @file input.h
 * @brief Gerencia os estados de input (tecla pressionada, botão pressionado, etc).
 * O estado de cada tecla é um booleano (mais detalhes na implementação).
 */

#include "../defines.h"
#include "key_codes.h"

void initialize_input();
void shutdown_input();
void update_input(f64 delta_time);

// Teclado
MGM_API b8 input_is_key_down(keys key);
MGM_API b8 input_is_key_up(keys key);
MGM_API b8 input_was_key_down(keys key);
MGM_API b8 input_was_key_up(keys key);

void input_process_key(keys key, b8 pressed);

// Mouse
MGM_API b8 input_is_button_down(buttons button);
MGM_API b8 input_is_button_up(buttons button);
MGM_API b8 input_was_button_down(buttons button);
MGM_API b8 input_was_button_up(buttons button);

MGM_API void input_get_mouse_position(i32* x, i32* y);
MGM_API void input_get_previous_mouse_position(i32* x, i32* y);

void input_process_button(buttons button, b8 pressed);
void input_process_mouse_move(i16 x, i16 y);
void input_process_mouse_wheel(i8 z_delta);

#endif // INPUT_H
