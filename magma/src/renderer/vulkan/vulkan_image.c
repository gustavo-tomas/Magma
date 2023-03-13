#include "vulkan_image.h"
#include "vulkan_device.h"

#include "../../core/logger.h"
#include "../../core/mgm_memory.h" 

void create_vulkan_image(vulkan_context* context, VkImageType image_type, u32 width, u32 height, VkFormat format,
                         VkImageTiling image_tiling, VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_flags,
                         b8 create_view, VkImageAspectFlags image_aspect_flags, vulkan_image* image)
{
    // Copia parâmetros
    image->width = width;
    image->height = height;

    // Cria a imagem
    VkImageCreateInfo image_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    image_create_info.imageType = image_type;
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.format = format;
    image_create_info.tiling = image_tiling;
    image_create_info.usage = usage_flags;
    image_create_info.extent.depth = 1; // @TODO: profundidade variável
    image_create_info.mipLevels = 4;    // @TODO: mip mapping
    image_create_info.arrayLayers = 1;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateImage(context->device.logical_device, &image_create_info, context->allocator, &image->handle));

    // Requerimentos de memória
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, image->handle, &memory_requirements);

    i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == -1)
        MGM_ERROR("(Vulkan) Tipo de memória necessário não encontrado!");

    // Aloca memória
    VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    
    VK_CHECK(vkAllocateMemory(context->device.logical_device, &memory_allocate_info, context->allocator, &image->memory));

    // Conecta a memória
    VK_CHECK(vkBindImageMemory(context->device.logical_device, image->handle, image->memory, 0));

    // Cria o suporte para imagme (image view)
    if (create_view)
    {
        image->view = 0;
        create_vulkan_image_view(context, format, image, image_aspect_flags);
    }
}

void create_vulkan_image_view(vulkan_context* context, VkFormat format, vulkan_image* image, VkImageAspectFlags image_aspect_flags)
{
    VkImageViewCreateInfo view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    view_create_info.format = format;
    view_create_info.image = image->handle;
    view_create_info.subresourceRange.aspectMask = image_aspect_flags;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // @TODO: tipo variável

    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->device.logical_device, &view_create_info, context->allocator, &image->view));
}

void destroy_vulkan_image(vulkan_context* context, vulkan_image* image)
{
    if (image->view)
    {
        vkDestroyImageView(context->device.logical_device, image->view, context->allocator);
        image->view = 0;
    }

    if (image->memory)
    {
        vkFreeMemory(context->device.logical_device, image->memory, context->allocator);
        image->memory = 0;
    }

    if (image->handle)
    {
        vkDestroyImage(context->device.logical_device, image->handle, context->allocator);
        image->handle = 0;
        MGM_INFO("(Vulkan) Imagem destruída com sucesso!");
    }
}
