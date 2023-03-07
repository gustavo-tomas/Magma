#include "event.h"
#include "logger.h"
#include "mgm_memory.h"

#include "../containers/vector.h"

#define MAX_MESSAGE_CODES 4096

typedef struct registered_event
{
    void* listener;
    on_event_ptr callback;
} registered_event;

typedef struct event_code_entry
{
    registered_event* events;
} event_code_entry;

typedef struct event_system_state
{
    event_code_entry registered[MAX_MESSAGE_CODES];
} event_system_state;

static b8 is_initialized = FALSE;
static event_system_state state;

b8 initialize_event()
{
    if (is_initialized == TRUE)
        return FALSE;

    mgm_zero_memory(&state, sizeof(state));
    is_initialized = TRUE;

    return TRUE;
}

void event_shutdown()
{
    for (u16 i = 0; i < MAX_MESSAGE_CODES; i++)
    {
        if (state.registered[i].events != 0)
        {
            vector_destroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

MGM_API b8 event_register(u16 code, void* listener, on_event_ptr on_event)
{
    if (is_initialized == FALSE)
        return FALSE;

    if (state.registered[code].events == 0)
        state.registered[code].events = vector_create(registered_event);

    u64 registered_count = vector_length(state.registered[code].events);
    for (u64 i = 0; i < registered_count; i++)
    {
        if (state.registered[code].events[i].listener == listener)
        {
            MGM_WARN("Evento já foi registrado! Código: %d", code); // @TODO testar essa warning
            return FALSE;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    vector_push(state.registered[code].events, event);

    return TRUE;
}

MGM_API b8 event_unregister(u16 code, void* listener, on_event_ptr on_event)
{
    if (is_initialized == FALSE)
        return FALSE;

    if (state.registered[code].events == 0)
    {
        MGM_WARN("Não há eventos registrados para esse código! Código: %d", code); // @TODO testar essa warning
        return FALSE;
    }

    u64 registered_count = vector_length(state.registered[code].events);
    for (u64 i = 0; i < registered_count; i++)
    {
        registered_event event = state.registered[code].events[i];
        if (event.listener == listener && event.callback == on_event)
        {
            registered_event popped_event;
            vector_pop_at(state.registered[code].events, i, &popped_event);
            return TRUE;
        }
    }

    return FALSE;
}

MGM_API b8 event_fire(u16 code, void* sender, event_context context)
{
    if (is_initialized == FALSE)
        return FALSE;

    if (state.registered[code].events == 0)
    {
        MGM_WARN("Não há eventos registrados para esse código! Código: %d", code); // @TODO testar essa warning
        return FALSE;
    }

    u64 registered_count = vector_length(state.registered[code].events);
    for (u64 i = 0; i < registered_count; i++)
    {
        registered_event event = state.registered[code].events[i];
        if (event.callback(code, sender, event.listener, context))
            return TRUE;
    }

    return FALSE;
}
