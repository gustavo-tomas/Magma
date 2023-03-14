#ifndef VULKAN_COMMAND_BUFFER_H
#define VULKAN_COMMAND_BUFFER_H

/**
 * @file vulkan_command_buffer.h
 * @brief Funções para alocar/gerenciar buffers de comando.
 */

#include "vulkan_types.inl"

void allocate_vulkan_command_buffer(vulkan_context* context, VkCommandPool pool, b8 is_primary, vulkan_command_buffer* command_buffer);
void free_vulkan_command_buffer(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer);

void begin_vulkan_command_buffer(vulkan_command_buffer* command_buffer, b8 is_single_use, b8 is_renderpass_continue, b8 is_simultaneous_use);
void end_vulkan_command_buffer(vulkan_command_buffer* command_buffer);

void update_submitted_vulkan_command_buffer(vulkan_command_buffer* command_buffer);

void reset_submitted_vulkan_command_buffer(vulkan_command_buffer* command_buffer);

void allocate_and_begin_single_use_command_buffer(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer);
void end_single_use_command_buffer(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer, VkQueue queue);

#endif // VULKAN_COMMAND_BUFFER_H
