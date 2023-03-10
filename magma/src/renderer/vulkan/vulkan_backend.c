#include "vulkan_backend.h"
#include "vulkan_types.inl"
#include "vulkan_platform.h"

#include "../../core/logger.h"
#include "../../containers/vector.h"
#include "../../platform/platform.h"

#include <string.h>

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
                MGM_FATAL("Camada %s não foi encontrada!", validation_layer_names[i]);
                return FALSE;
            }
        }

        MGM_DEBUG("Todas as camadas de validação foram encontradas!");
    #endif

    create_info.enabledLayerCount = validation_layer_count;
    create_info.ppEnabledLayerNames = validation_layer_names;

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));

    MGM_INFO("Instância do vulkan inicializada com sucesso!");

    // Debugger
    #if defined(_DEBUG)
        MGM_DEBUG("Criando debugger do Vulkan");
        u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debug_create_info.messageSeverity = log_severity;
        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debug_create_info.pfnUserCallback = debug_callback;

        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
        MGM_ASSERT(func, "Erro ao criar debugger do Vulkan!");
        VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));

        MGM_DEBUG("Debugger do Vulkan criado com sucesso!");
    #endif

    vector_destroy(available_layers);
    vector_destroy(extensions);
    vector_destroy(validation_layer_names);

    return TRUE;
}

void shutdown_vulkan_renderer_backend(struct renderer_backend* backend)
{
    #if defined(_DEBUG)
        MGM_DEBUG("Destruindo debugger do Vulkan");
        if (context.debug_messenger)
        {
            PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
            func(context.instance, context.debug_messenger, context.allocator);
        }
    #endif
    vkDestroyInstance(context.instance, context.allocator);
    MGM_INFO("Instância do Vulkan destruída com sucesso!");
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
