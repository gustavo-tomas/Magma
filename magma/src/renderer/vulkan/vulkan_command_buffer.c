#include "vulkan_command_buffer.h"

#include "../../core/mgm_memory.h"

void allocate_vulkan_command_buffer(vulkan_context* context, VkCommandPool pool, b8 is_primary, vulkan_command_buffer* command_buffer)
{
    mgm_zero_memory(command_buffer, sizeof(vulkan_command_buffer));

    // Aloca um buffer de comandos
    VkCommandBufferAllocateInfo allocate_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocate_info.commandPool = pool;
    allocate_info.level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocate_info.commandBufferCount = 1;

    command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(context->device.logical_device, &allocate_info, &command_buffer->handle));
    command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void free_vulkan_command_buffer(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer)
{
    vkFreeCommandBuffers(context->device.logical_device, pool, 1, &command_buffer->handle);
    command_buffer->handle = 0;
    command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void begin_vulkan_command_buffer(vulkan_command_buffer* command_buffer, b8 is_single_use, b8 is_renderpass_continue, b8 is_simultaneous_use)
{
    VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    
    begin_info.flags = 0;
    if (is_single_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (is_renderpass_continue)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    
    if (is_simultaneous_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    
    VK_CHECK(vkBeginCommandBuffer(command_buffer->handle, &begin_info));
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

void end_vulkan_command_buffer(vulkan_command_buffer* command_buffer)
{
    // @TODO: tratamento de erro
    VK_CHECK(vkEndCommandBuffer(command_buffer->handle));
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void update_submitted_vulkan_command_buffer(vulkan_command_buffer* command_buffer)
{
    command_buffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void reset_submitted_vulkan_command_buffer(vulkan_command_buffer* command_buffer)
{
    command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void allocate_and_begin_single_use_command_buffer(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer)
{
    allocate_vulkan_command_buffer(context, pool, TRUE, command_buffer);
    begin_vulkan_command_buffer(command_buffer, TRUE, FALSE, FALSE);    // Assume que o buffer de comandos é de uso único
}

void end_single_use_command_buffer(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer, VkQueue queue)
{
    // Termina o buffer de comando
    end_vulkan_command_buffer(command_buffer);

    // Envia o buffer para a fila
    VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;
    VK_CHECK(vkQueueSubmit(queue, submit_info.commandBufferCount, &submit_info, 0));

    // Espera a operação terminar
    VK_CHECK(vkQueueWaitIdle(queue));

    // Libera o buffer
    free_vulkan_command_buffer(context, pool, command_buffer);
}
