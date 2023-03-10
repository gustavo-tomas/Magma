#ifndef VULKAN_PLATFORM_H
#define VULKAN_PLATFORM_H

/**
 * @file vulkan_platform.h
 * @brief Assinatura de funções da vulkan que dependem da plataforma utilizada.
 */

#include "../../defines.h"

struct platform_state;
struct vulkan_context;

/* Por enquanto implementados pela plataforma */

b8 create_vulkan_surface(struct platform_state* plat_state, struct vulkan_context* context);

void destroy_vulkan_surface(struct vulkan_context* context);

/**
 * @brief Adiciona o nome da extensão necessária pela plataforma à lista de extensões passada.
 * 
 * @param names A lista de extensões.
 */
void platform_get_required_extension_names(const char*** names); // Definido pela plataforma!

#endif // VULKAN_PLATFORM_H
