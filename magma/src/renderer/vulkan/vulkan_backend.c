#include "vulkan_backend.h"
#include "vulkan_device.h"
#include "vulkan_types.inl"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"
#include "vulkan_command_buffer.h"
#include "vulkan_platform.h"

#include "../../core/logger.h"
#include "../../core/mgm_memory.h"
#include "../../containers/vector.h"
#include "../../platform/platform.h"

#include <string.h>

// :(
#if defined(_DEBUG)
    #include "../../core/logger.h"

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_types,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data)
        {
            switch (message_severity)
            {
                default:
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    MGM_ERROR(callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    MGM_WARN(callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    MGM_INFO(callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    MGM_TRACE(callback_data->pMessage);
                    break;
            }
            return VK_FALSE;
        }
#endif

i32 _find_memory_index(u32 type_filter, u32 property_flags);

void _create_command_buffers(renderer_backend* backend);

// Há apenas 1 contexto
static vulkan_context context;

b8 initialize_vulkan_renderer_backend (struct renderer_backend* backend, const char* application_name, struct platform_state* plat_state)
{   
    context.find_memory_index = _find_memory_index;

    // @TODO: Alocadores (maybe? huuuum?)
    context.allocator = 0;

    // Inicializa a instância
    VkApplicationInfo app_info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
    app_info.pEngineName = "Magma Engine";
    app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);

    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;

    // Lista de extensões
    const char** extensions = vector_create(const char *);
    vector_push(extensions, &VK_KHR_SURFACE_EXTENSION_NAME); 
    platform_get_required_extension_names(&extensions);      // Extensão específica da plataforma
    
    #if defined(_DEBUG)
        vector_push(extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        MGM_DEBUG("Extensões do vulkan necessárias:");
        u32 length = vector_length(extensions);
        for (u32 i = 0; i < length; i++)
            MGM_DEBUG(extensions[i]);
    #endif

    create_info.enabledExtensionCount = vector_length(extensions);
    create_info.ppEnabledExtensionNames = extensions;

    const char** validation_layer_names = 0;
    u32 validation_layer_count = 0;

    #if defined(_DEBUG)
        MGM_DEBUG("Verificando camadas de validação:");

        // Camadas necessárias
        validation_layer_names = vector_create(const char*);
        vector_push(validation_layer_names, &"VK_LAYER_KHRONOS_validation");
        validation_layer_count = vector_length(validation_layer_names);

        // Camadas disponíveis
        u32 available_layer_count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
        VkLayerProperties* available_layers = vector_reserve(VkLayerProperties, available_layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

        // Verificação de camadas
        for (u32 i = 0; i < validation_layer_count; i++)
        {
            MGM_DEBUG("Procurando camada %s", validation_layer_names[i]);
            b8 found = FALSE;
            for (u32 j = 0; j < available_layer_count; j++)
            {
                if (!strcmp(validation_layer_names[i], available_layers[j].layerName))
                {
                    found = TRUE;
                    MGM_DEBUG("Camada %s encontrada!", validation_layer_names[i]);
                    break;
                }
            }

            if (!found)
            {
                MGM_FATAL("(Vulkan) Camada de validação %s não foi encontrada!", validation_layer_names[i]);
                return FALSE;
            }
        }
        vector_destroy(available_layers);
        
        MGM_DEBUG("(Vulkan) Todas as camadas de validação foram encontradas!");
    #endif

    create_info.enabledLayerCount = validation_layer_count;
    create_info.ppEnabledLayerNames = validation_layer_names;
    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
    
    vector_destroy(validation_layer_names);
    vector_destroy(extensions);

    MGM_INFO("(Vulkan) Instância inicializada com sucesso!");

    // Debugger
    #if defined(_DEBUG)
        u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debug_create_info.messageSeverity = log_severity;
        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debug_create_info.pfnUserCallback = debug_callback;

        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
        MGM_ASSERT(func, "Erro ao criar debugger do Vulkan!");
        VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));

        MGM_DEBUG("(Vulkan) Debugger do Vulkan criado com sucesso!");
    #endif

    // Cria a superfície
    if (!create_vulkan_surface(plat_state, &context))
    {
        MGM_ERROR("(Vulkan) Erro ao criar superfície do Vulkan!");
        return FALSE;
    }
    MGM_DEBUG("(Vulkan) Superfície do Vulkan criada com sucesso!");

    // Cria o dispositivo
    if (!create_vulkan_device(&context))
    {
        MGM_ERROR("(Vulkan) Erro ao criar dispositivo do Vulkan!");
        return FALSE;
    }
    MGM_INFO("(Vulkan) Dispositivo do Vulkan criado com sucesso!");

    // Cria a swapchain
    create_vulkan_swapchain(&context, context.framebuffer_width, context.framebuffer_height, &context.swapchain);
    MGM_INFO("(Vulkan) Swapchain criada com sucesso!");

    // Cria os passes de renderização
    create_vulkan_renderpass(&context, &context.main_renderpass, 0, 0, context.framebuffer_width, context.framebuffer_height,
                             0.0f, 0.4f, 0.5f, 1.0f, 
                             1.0f, 0);

    // Cria os buffers de comando
    _create_command_buffers(backend);
    MGM_INFO("(Vulkan) Buffers de comando criados com sucesso!");

    MGM_INFO("(Vulkan) Renderizador inicializado com sucesso!");

    return TRUE;
}

void shutdown_vulkan_renderer_backend(struct renderer_backend* backend)
{
    // Destrói o debugger (se existir)
    #if defined(_DEBUG)
        if (context.debug_messenger)
        {
            PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
            func(context.instance, context.debug_messenger, context.allocator);
            MGM_DEBUG("(Vulkan) Debugger destruído com sucesso!");
        }
    #endif

    // Destrói os buffers de comando
    /* @NOTE: os buffers de comando são liberados automaticamente quando a pool de comandos é destruída! */
    vector_destroy(context.graphics_command_buffers);
    context.graphics_command_buffers = 0;
    MGM_INFO("(Vulkan) Buffers de comando destruídos com sucesso!");

    // Destrói os passes
    destroy_vulkan_renderpass(&context, &context.main_renderpass);
    MGM_INFO("(Vulkan) Passe de renderização destruído com sucesso!");

    // Destrói a swapchain
    destroy_vulkan_swapchain(&context, &context.swapchain);
    MGM_INFO("(Vulkan) Swapchain destruída com sucesso!");

    // Destrói o dispositivo
    destroy_vulkan_device(&context);
    MGM_INFO("(Vulkan) Dispositivo destruído com sucesso!");

    // Destrói a superfície
    if (context.surface)
    {
        destroy_vulkan_surface(&context);
        context.surface = 0;
        MGM_INFO("(Vulkan) Superfície destruída com sucesso!");
    }

    // Destrói a instância
    vkDestroyInstance(context.instance, context.allocator);
    MGM_INFO("(Vulkan) Instância destruída com sucesso!");
}

b8 begin_frame_vulkan_renderer_backend(struct renderer_backend* backend, f32 delta_time)
{
    return TRUE;
}

b8 end_frame_vulkan_renderer_backend(struct renderer_backend* backend, f32 delta_time)
{
    return TRUE;
}

void on_resize_vulkan_renderer_backend(struct renderer_backend* backend, u16 width, u16 height)
{

}

i32 _find_memory_index(u32 type_filter, u32 property_flags)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_device, &memory_properties);

    for (u32 i = 0; i < memory_properties.memoryTypeCount; i++)
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
            return i;

    MGM_WARN("(Vulkan) Não foi possível encontrar o tipo de memória desejado!");
    return -1;
}

void _create_command_buffers(renderer_backend* backend)
{
    if (!context.graphics_command_buffers)
    {
        context.graphics_command_buffers = vector_reserve(vulkan_command_buffer, context.swapchain.image_count);
        for (u32 i = 0; i < context.swapchain.image_count; i++)
            mgm_zero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
    }

    for (u32 i = 0; i < context.swapchain.image_count; i++)
    {
        if (context.graphics_command_buffers[i].handle)
            free_vulkan_command_buffer(&context, context.device.graphics_command_pool, &context.graphics_command_buffers[i]);

        allocate_vulkan_command_buffer(&context, context.device.graphics_command_pool, TRUE, &context.graphics_command_buffers[i]);
    }
}
