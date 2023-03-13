#include "vulkan_device.h"
#include "../../core/logger.h"
#include "../../core/mgm_memory.h"
#include "../../containers/vector.h"

#include <string.h>

b8 create_vulkan_device(vulkan_context* context)
{
    if (!select_physical_device(context))
        return FALSE;

    // NÃO criar queues para índices compartilhados!
    b8 present_shares_graphics_queue  = context->device.graphics_queue_index == context->device.present_queue_index;
    b8 transfer_shares_graphics_queue = context->device.graphics_queue_index == context->device.transfer_queue_index;
    u32 index_count = 1;

    if (!present_shares_graphics_queue)
        index_count++;
    
    if (!transfer_shares_graphics_queue)
        index_count++;

    u32 indices[index_count];
    u8 index = 0;

    indices[index++] = context->device.graphics_queue_index;

    if (!present_shares_graphics_queue)
        indices[index++] = context->device.present_queue_index;

    if (!transfer_shares_graphics_queue)
        indices[index++] = context->device.transfer_queue_index;

    VkDeviceQueueCreateInfo queue_create_infos[index_count];
    for (u32 i = 0; i < index_count; i++)
    {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;

        // @TODO: o futuro já começou
        // if (indices[i] == context->device.graphics_queue_index)
        //     queue_create_infos[i].queueCount = 2;
        
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
        queue_create_infos[i].pQueuePriorities = (f32*) mgm_allocate(sizeof(f32), MEMORY_TAG_RENDERER);
    }

    // @TODO: deve ser escolhido pela configuração
    VkPhysicalDeviceFeatures device_features = { };
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;

    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    // Cria o dispositivo lógico
    VK_CHECK(vkCreateDevice(context->device.physical_device, &device_create_info, context->allocator, &context->device.logical_device));
    MGM_INFO("(Vulkan) Dispositivo lógico criado com sucesso!");

    // Obtém os 'Queues'
    vkGetDeviceQueue(context->device.logical_device, context->device.graphics_queue_index, 0, &context->device.graphics_queue);
    vkGetDeviceQueue(context->device.logical_device, context->device.present_queue_index, 0, &context->device.present_queue);
    vkGetDeviceQueue(context->device.logical_device, context->device.transfer_queue_index, 0, &context->device.transfer_queue);

    MGM_INFO("(Vulkan) 'Queue' obtido com sucesso!");

    // Libera recursos @TODO: provável vazamento aqui
    for (u32 i = 0; i < index_count; i++)
        mgm_free((void *) queue_create_infos[i].pQueuePriorities, sizeof(f32), MEMORY_TAG_RENDERER);

    return TRUE;
}

void destroy_vulkan_device(vulkan_context* context)
{
    // Libera os 'Queues'
    context->device.graphics_queue = 0;
    context->device.present_queue = 0;
    context->device.transfer_queue = 0;

    // Deleta dispositivos lógicos
    if (context->device.logical_device)
    {
        vkDestroyDevice(context->device.logical_device, context->allocator);
        context->device.logical_device = 0;
        MGM_INFO("(Vulkan) Dispositivo lógico destruído com sucesso!");
    }

    // Dispositivos físicos NÃO são deletados
    context->device.physical_device = 0;
    MGM_INFO("(Vulkan) Dispositivo físico liberado com sucesso!");

    if (context->device.swapchain_support.formats)
    {
        mgm_free(context->device.swapchain_support.formats,
                 sizeof(VkSurfaceFormatKHR) * context->device.swapchain_support.format_count,
                 MEMORY_TAG_RENDERER);

        context->device.swapchain_support.formats = 0;
        context->device.swapchain_support.format_count = 0;
    }

    if (context->device.swapchain_support.present_modes)
    {
        mgm_free(context->device.swapchain_support.present_modes,
                 sizeof(VkPresentModeKHR) * context->device.swapchain_support.present_mode_count,
                 MEMORY_TAG_RENDERER);

        context->device.swapchain_support.present_modes = 0;
        context->device.swapchain_support.present_mode_count = 0;
    }

    mgm_zero_memory(&context->device.swapchain_support.capabilities, sizeof(context->device.swapchain_support.capabilities));

    context->device.graphics_queue_index = -1;
    context->device.present_queue_index  = -1;
    context->device.transfer_queue_index = -1;
}

b8 detect_device_depth_format(vulkan_device* device)
{
    // Possíveis formatos
    const u64 possible_formats_count = 3;
    VkFormat formats[3] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < possible_formats_count; i++)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, formats[i], &properties);

        if ((properties.linearTilingFeatures & flags) == flags)
        {
            device->depth_format = formats[i];
            return TRUE;
        }

        else if ((properties.optimalTilingFeatures & flags) == flags)
        {
            device->depth_format = formats[i];
            return TRUE;
        }
    }

    return FALSE;
}

void query_device_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, vulkan_swapchain_support_info* support_info)
{
    // Verifica capacidades da superfície
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &support_info->capabilities));

    // Formatos da superfície
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &support_info->format_count, 0));

    if (support_info->format_count != 0)
    {
        if (!support_info->formats)
            support_info->formats = mgm_allocate(sizeof(VkSurfaceFormatKHR) * support_info->format_count, MEMORY_TAG_RENDERER);
    
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &support_info->format_count, support_info->formats));
    }

    // Modos de 'Present'
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &support_info->present_mode_count, 0));
    
    if (support_info->present_mode_count != 0)
    {
        if (!support_info->present_modes)
            support_info->present_modes = mgm_allocate(sizeof(VkPresentModeKHR) * support_info->present_mode_count, MEMORY_TAG_RENDERER);
    
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &support_info->present_mode_count, support_info->present_modes));
    }
}

b8 select_physical_device(vulkan_context* context)
{
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));
    if (physical_device_count == 0)
    {
        MGM_FATAL("Não há dispositivos físicos que suportam Vulkan!");
        return FALSE;
    }

    VkPhysicalDevice physical_devices[physical_device_count];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));

    for (u32 i = 0; i < physical_device_count; i++)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        // Requerimentos
        vulkan_physical_device_requirements requirements = { };
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = vector_create(const char *);

        vector_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // Verifica se o dispositivo possui todos os requerimentos
        vulkan_physical_device_queue_family_info queue_info = { };
        b8 result = physical_device_meets_requirements(physical_devices[i], context->surface,
            &properties, &features, &requirements, &queue_info, &context->device.swapchain_support);

        if (result)
        {
            MGM_INFO("(Vulkan) Dispositivo selecionado: %s", properties.deviceName);
            switch (properties.deviceType)
            {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    MGM_INFO("(Vulkan) Tipo da GPU desconhecido");
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    MGM_INFO("(Vulkan) Tipo da GPU é integrado");
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    MGM_INFO("(Vulkan) Tipo da GPU é discreto");
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    MGM_INFO("(Vulkan) Tipo da GPU é virtual");
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    MGM_INFO("(Vulkan) Tipo da GPU é CPU");
                    break;
            }

            // Informação sobre os drivers
            MGM_INFO("(Vulkan) Versão dos Drivers da GPU: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));

            // Informação sobre a API do Vulkan
            MGM_INFO("(Vulkan) Versão da API do Vulkan: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            // Memória da GPU
            for (u32 j = 0; j < memory.memoryHeapCount; j++)
            {
                f32 memory_size_gib = ((f32) memory.memoryHeaps[j].size) / 1024.f / 1024.f / 1024.f;
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    MGM_INFO("(Vulkan) Memória da GPU: %.2f GiB", memory_size_gib);
                else
                    MGM_INFO("(Vulkan) Memória compartilhada do sistema: %.2f GiB", memory_size_gib);
            }

            // Salva propriedades do dispositivo
            context->device.physical_device = physical_devices[i];
            context->device.graphics_queue_index = queue_info.graphics_family_index;
            context->device.present_queue_index = queue_info.present_family_index;
            context->device.transfer_queue_index = queue_info.transfer_family_index;

            context->device.properties = properties;
            context->device.features   = features;
            context->device.memory     = memory;

            break;
        }
    }

    // Verifica se um dispositivo foi selecionado
    if (!context->device.physical_device)
    {
        MGM_ERROR("(Vulkan) Nenhum dispositivo físico encontrado possui os requerimentos necessários!");
        return FALSE;
    }

    MGM_INFO("(Vulkan) Dispositivo físico selecionado!");
    return TRUE;
}

b8 physical_device_meets_requirements(VkPhysicalDevice device, VkSurfaceKHR surface, 
                                      const VkPhysicalDeviceProperties* properties, 
                                      const VkPhysicalDeviceFeatures* features,
                                      const vulkan_physical_device_requirements* requirements,
                                      vulkan_physical_device_queue_family_info* queue_info,
                                      vulkan_swapchain_support_info* swapchain_support)
{
    // Wrap around (unsigned 32)
    queue_info->graphics_family_index = -1;
    queue_info->present_family_index = -1;
    queue_info->compute_family_index = -1;
    queue_info->transfer_family_index = -1;

    // Verifica se dispositivo é uma GPU discreta
    if (requirements->discrete_gpu)
    {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            MGM_INFO("(Vulkan) Dispositivo '%s' não é uma GPU Discreta!", properties->deviceName);
            return FALSE;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    // Verifica os tipos de fila suportados
    MGM_INFO("Graphic | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_family_count; i++)
    {
        u8 current_transfer_score = 0;

        // Bit de Gráficos/Computação
        if (queue_families[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
        {
            queue_info->graphics_family_index = i;
            queue_info->compute_family_index = i;
            current_transfer_score++;
        }

        // Bit de Transferência
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            if (current_transfer_score <= min_transfer_score)
            {
                min_transfer_score = current_transfer_score;
                queue_info->transfer_family_index = i;
            }
        }

        // Bit de 'Present'
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present)
            queue_info->present_family_index = i;
    }

    // Imprime informações sobre o dispositivo
    MGM_INFO("      %d |       %d |       %d |        %d | %s", 
        queue_info->graphics_family_index != -1,
        queue_info->present_family_index  != -1,
        queue_info->compute_family_index  != -1,
        queue_info->transfer_family_index != -1,
        properties->deviceName);

    if (
        (!requirements->graphics || queue_info->graphics_family_index != -1) &&
        (!requirements->present  || queue_info->present_family_index  != -1) &&
        (!requirements->compute  || queue_info->compute_family_index  != -1) &&
        (!requirements->transfer || queue_info->transfer_family_index != -1))
    {
        MGM_INFO("(Vulkan) Dispositivo possui todos os requerimentos necessários!");
        MGM_INFO("(Vulkan) Índice de Gráficos: %d", queue_info->graphics_family_index);
        MGM_INFO("(Vulkan) Índice de 'Present': %d", queue_info->present_family_index);
        MGM_INFO("(Vulkan) Índice de Transferência: %d", queue_info->transfer_family_index);
        MGM_INFO("(Vulkan) Índice de Computação: %d", queue_info->compute_family_index);
        
        // Verifica suporte de swapchain do dispositivo
        query_device_swapchain_support(device, surface, swapchain_support);

        if (swapchain_support->format_count < 1 || swapchain_support->present_mode_count < 1)
        {
            if (swapchain_support->formats)
                mgm_free(swapchain_support->formats, sizeof(VkSurfaceFormatKHR) * swapchain_support->format_count, MEMORY_TAG_RENDERER);
                
            if (swapchain_support->present_modes)
                mgm_free(swapchain_support->present_modes, sizeof(VkSurfaceFormatKHR) * swapchain_support->present_mode_count, MEMORY_TAG_RENDERER);
        
            MGM_INFO("(Vulkan) Suporte necessário para swapchain não encontrado para '%s'", properties->deviceName);
            return FALSE;
        }

        // Extensões do dispositivo
        if (requirements->device_extension_names)
        {
            u32 available_extensions_count = 0;
            VkExtensionProperties* available_extensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &available_extensions_count, 0));

            if (available_extensions_count != 0)
            {
                available_extensions = mgm_allocate(sizeof(VkExtensionProperties) * available_extensions_count, MEMORY_TAG_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &available_extensions_count, available_extensions));
            
                // Verifica se o dispositivo possui as extensões necessárias
                u32 required_extension_count = vector_length(requirements->device_extension_names);
                for (u32 i = 0; i < required_extension_count; i++)
                {
                    b8 found = FALSE;
                    for (u32 j = 0; j < available_extensions_count; j++)
                    {
                        if (strcmp(requirements->device_extension_names[i], available_extensions[j].extensionName))
                        {
                            found = TRUE;
                            break;
                        }
                    }

                    if (!found)
                    {
                        MGM_INFO("(Vulkan) Extensão necessária '%s' não foi encontrada para esse dispositivo", requirements->device_extension_names[i]);
                        mgm_free(available_extensions, sizeof(VkExtensionProperties) * available_extensions_count, MEMORY_TAG_RENDERER);
                        return FALSE;
                    }
                }

                mgm_free(available_extensions, sizeof(VkExtensionProperties) * available_extensions_count, MEMORY_TAG_RENDERER);
            }

            // Lista de extensões não é mais necessária
            vector_destroy(requirements->device_extension_names);
        }

        /* Verifica requerimentos individuais */

        // Sampler anisotropy
        if (requirements->sampler_anisotropy && !features->samplerAnisotropy)
        {
            MGM_INFO("(Vulkan) Dispositivo não suporta 'Sampler Anisotropy'");
            return FALSE;
        }

        // Dispositivo possui todos os requerimentos necessários
        MGM_INFO("(Vulkan) Dispositivo '%s' possui todos os requerimentos necessários!", properties->deviceName);
        return TRUE;
    }

    return FALSE;
}
