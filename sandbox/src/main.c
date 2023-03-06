#include <core/logger.h>
#include <core/asserts.h>

// @TODO: testes
#include <platform/platform.h>

int main()
{
    MGM_FATAL("Deu merda grande: %f", 2.14f);
    MGM_ERROR("Deu merda: %f", 3.14f);
    MGM_WARN("Fez merda: %f", 4.14f);
    MGM_INFO("Informando: %f", 5.14f);
    MGM_DEBUG("Debugando: %f", 6.14f);
    MGM_TRACE("Traçando: %f", 7.14f);

    platform_state state;
    if (platform_startup(&state, "Magma Sandbox", 100, 100, 1280, 720))
        while (TRUE)
            platform_dispatch_messages(&state);

    platform_shutdown(&state);

    return 0;
}
