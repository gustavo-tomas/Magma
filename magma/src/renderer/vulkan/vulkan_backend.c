#include "vulkan_backend.h"
#include "vulkan_types.inl"

#include "../../core/logger.h"

// Há apenas 1 contexto
static vulkan_context context;

b8 initialize_vulkan_renderer_backend (struct renderer_backend* backend, const char* application_name, struct platform_state* plat_state)
{
    // @TODO: Alocadores (maybe? huuuum?)
    context.allocator = 0;

    // Inicializa a instância
    VkApplicationInfo app_info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_API_VERSION(1, 3, 0, 0);
    app_info.pEngineName = "Magma Engine";
    app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);

    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = 0;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
    if (result != VK_SUCCESS)
    {
        MGM_ERROR("Erro ao criar instância (vkCreateInstance) - Erro: %u", result);
        return FALSE;
    }

    MGM_INFO("Instância do vulkan inicializada com sucesso!");
    return TRUE;
}

void shutdown_vulkan_renderer_backend (struct renderer_backend* backend)
{
    vkDestroyInstance(context.instance, context.allocator);
}

b8 begin_frame_vulkan_renderer_backend (struct renderer_backend* backend, f32 delta_time)
{
    return TRUE;
}

b8 end_frame_vulkan_renderer_backend (struct renderer_backend* backend, f32 delta_time)
{
    return TRUE;
}

void on_resize_vulkan_renderer_backend (struct renderer_backend* backend, u16 width, u16 height)
{

}
