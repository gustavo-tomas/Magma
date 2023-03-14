#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_image.h"

#include "../../core/logger.h"
#include "../../core/mgm_memory.h"

b8 acquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout_ns, VkSemaphore image_available, VkFence fence, u32* image_index)
{
    VkResult result = vkAcquireNextImageKHR(context->device.logical_device, swapchain->handle, timeout_ns, image_available, fence, image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_vulkan_swapchain(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return FALSE;
    }

    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        MGM_FATAL("(Vulkan) Erro ao requisitar imagem do swapchain!");
        return FALSE;
    }

    return TRUE;
}

void swapchain_present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete, u32 present_image_index)
{
    VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        recreate_vulkan_swapchain(context, context->framebuffer_width, context->framebuffer_height, swapchain);

    else if (result != VK_SUCCESS)
        MGM_FATAL("(Vulkan) Erro ao apresentar imagem do swapchain!");
}

void create_vulkan_swapchain(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    VkExtent2D image_extent = { width, height, };
    swapchain->max_frames_in_flight = 2; // Triple buffering

    // Escolhe o formato
    b8 found_format = FALSE;
    for (u32 i = 0; i < context->device.swapchain_support.format_count; i++)
    {
        VkSurfaceFormatKHR format = context->device.swapchain_support.formats[i];

        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            swapchain->image_format = format;
            found_format = TRUE;
            break;
        }
    }

    if (!found_format)
        swapchain->image_format = context->device.swapchain_support.formats[0];

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < context->device.swapchain_support.present_mode_count; i++)
    {
        VkPresentModeKHR mode = context->device.swapchain_support.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = mode;
            break;
        }
    }

    query_device_swapchain_support(context->device.physical_device, context->surface, &context->device.swapchain_support);

    if (context->device.swapchain_support.capabilities.currentExtent.width != UINT32_MAX)
        image_extent = context->device.swapchain_support.capabilities.currentExtent;

    VkExtent2D min_image_extent = context->device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max_image_extent = context->device.swapchain_support.capabilities.maxImageExtent;

    image_extent.width = MGM_CLAMP(image_extent.width, min_image_extent.width, max_image_extent.width);
    image_extent.height = MGM_CLAMP(image_extent.height, min_image_extent.height, max_image_extent.height);

    u32 image_count = context->device.swapchain_support.capabilities.minImageCount + 1;
    if (context->device.swapchain_support.capabilities.maxImageCount > 0 && image_count > context->device.swapchain_support.capabilities.maxImageCount)
        image_count = context->device.swapchain_support.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchain_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchain_create_info.surface = context->surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = swapchain->image_format.format;
    swapchain_create_info.imageColorSpace = swapchain->image_format.colorSpace;
    swapchain_create_info.imageExtent = image_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Cria a fila de índices da família (?)
    if (context->device.graphics_queue_index != context->device.present_queue_index)
    {
        u32 queue_family_indices[] = { (u32) context->device.graphics_queue_index, (u32) context->device.present_queue_index };
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    }

    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = 0;
    }

    swapchain_create_info.preTransform = context->device.swapchain_support.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = 0;

    // Cria a swapchain
    VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device, &swapchain_create_info, context->allocator, &swapchain->handle));

    context->current_frame = 0;
    swapchain->image_count = 0;

    // Recupera imagens
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->handle, &swapchain->image_count, 0));
    if (!swapchain->images)
        swapchain->images = (VkImage *) mgm_allocate(sizeof(VkImage) * swapchain->image_count, MEMORY_TAG_RENDERER);

    if (!swapchain->views)
        swapchain->views = (VkImageView *) mgm_allocate(sizeof(VkImageView) * swapchain->image_count, MEMORY_TAG_RENDERER);

    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->handle, &swapchain->image_count, swapchain->images));

    // Cria o suporte para as imagens (image view)
    for (u32 i = 0; i < swapchain->image_count; i++)
    {
        VkImageViewCreateInfo view_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        view_info.image = swapchain->images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain->image_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(context->device.logical_device, &view_info, context->allocator, &swapchain->views[i]));
    }

    // Seleciona o formato de medidor de profundidade (depth format)
    if (!detect_device_depth_format(&context->device))
    {
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        MGM_FATAL("(Vulkan) Nenhum formato de medidor de profundidade encontrado!");
    }

    // Cria o buffer de profundidade
    create_vulkan_image(context, VK_IMAGE_TYPE_2D, image_extent.width, image_extent.height, context->device.depth_format,
                        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        TRUE, VK_IMAGE_ASPECT_DEPTH_BIT, &swapchain->depth_attachment);
}

void recreate_vulkan_swapchain(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    destroy_vulkan_swapchain(context, swapchain);
    create_vulkan_swapchain(context, width, height, swapchain);
}

void destroy_vulkan_swapchain(vulkan_context* context, vulkan_swapchain* swapchain)
{
    destroy_vulkan_image(context, &swapchain->depth_attachment);

    for (u32 i = 0; i < swapchain->image_count; i++)
        vkDestroyImageView(context->device.logical_device, swapchain->views[i], context->allocator);

    // Atualiza o gerenciador de memória
    mgm_free(swapchain->images, sizeof(VkImage) * swapchain->image_count, MEMORY_TAG_RENDERER);
    mgm_free(swapchain->views, sizeof(VkImageView) * swapchain->image_count, MEMORY_TAG_RENDERER);

    vkDestroySwapchainKHR(context->device.logical_device, swapchain->handle, context->allocator);
}
