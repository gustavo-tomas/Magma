#include <core/logger.h>
#include <core/asserts.h>

int main()
{
    MGM_FATAL("Deu merda grande: %f", 2.14f);
    MGM_ERROR("Deu merda: %f", 3.14f);
    MGM_WARN("Fez merda: %f", 4.14f);
    MGM_INFO("Informando: %f", 5.14f);
    MGM_DEBUG("Debugando: %f", 6.14f);
    MGM_TRACE("Traçando: %f", 7.14f);

    MGM_ASSERT(1 == 0, "1 não é 0 :(");

    return 0;
}
