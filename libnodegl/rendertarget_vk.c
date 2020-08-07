#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "format.h"
#include "gctx_vk.h"
#include "log.h"
#include "memory.h"
#include "nodes.h"
#include "rendertarget_vk.h"
#include "texture_vk.h"
#include "gctx_vk.h"
#include "format_vk.h"

int ngli_vk_create_renderpass(struct gctx *s, const struct rendertarget_desc *desc, VkRenderPass *render_pass, int conservative)
{
    struct gctx_vk *gctx_vk = (struct gctx_vk *)s;
    struct vkcontext *vk = gctx_vk->vkcontext;

    int nb_descs = 0;
    VkAttachmentDescription descs[2 * (NGLI_MAX_COLOR_ATTACHMENTS + 1)] = {0};

    int nb_color_refs = 0;
    VkAttachmentReference color_refs[NGLI_MAX_COLOR_ATTACHMENTS] = {0};

    int nb_resolve_refs = 0;
    VkAttachmentReference resolve_refs[NGLI_MAX_COLOR_ATTACHMENTS+1] = {0};

    VkAttachmentReference depth_stencil_ref = {0};
    VkAttachmentReference *depth_stencil_refp = NULL;

    for (int i = 0; i < desc->nb_colors; i++) {
        VkFormat format = VK_FORMAT_UNDEFINED;
        int ret = ngli_format_get_vk_format(vk, desc->colors[i].format, &format);
        if (ret < 0)
            return ret;

        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vk->phy_device, format, &properties);

        const VkFormatFeatureFlags features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
        if ((properties.optimalTilingFeatures & features) != features) {
            LOG(ERROR, "format %d does not support features 0x%d", format, properties.optimalTilingFeatures);
            return NGL_ERROR_EXTERNAL;
        }

        descs[nb_descs] = (VkAttachmentDescription) {
            .format         = format,
            .samples        = ngli_vk_get_sample_count(desc->colors[i].samples),
            .loadOp         = conservative ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        color_refs[nb_color_refs].attachment = nb_descs;
        color_refs[nb_color_refs].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        nb_color_refs++;
        nb_descs++;

        if (desc->colors[i].resolve) {
            descs[nb_descs] = (VkAttachmentDescription) {
                .format         = format,
                .samples        = VK_SAMPLE_COUNT_1_BIT,
                .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            };
            resolve_refs[nb_resolve_refs].attachment = nb_descs;
            resolve_refs[nb_resolve_refs].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            nb_descs++;
            nb_resolve_refs++;
        }
    }

    if (desc->depth_stencil.format != NGLI_FORMAT_UNDEFINED) {
        VkFormat format = VK_FORMAT_UNDEFINED;
        int ret = ngli_format_get_vk_format(vk, desc->depth_stencil.format, &format);
        if (ret < 0)
            return ret;

        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vk->phy_device, format, &properties);

        const VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if ((properties.optimalTilingFeatures & features) != features) {
            LOG(ERROR, "format %d does not support features 0x%d", format, properties.optimalTilingFeatures);
            return NGL_ERROR_EXTERNAL;
        }

        descs[nb_descs] = (VkAttachmentDescription) {
            .format         = format,
            .samples        = ngli_vk_get_sample_count(desc->depth_stencil.samples),
            .loadOp         = conservative ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
        depth_stencil_ref.attachment = nb_descs;
        depth_stencil_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_stencil_refp = &depth_stencil_ref;
        nb_descs++;

        if (desc->depth_stencil.resolve) {
            descs[nb_descs] = (VkAttachmentDescription) {
                .format         = format,
                .samples        = VK_SAMPLE_COUNT_1_BIT,
                .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            };

            resolve_refs[nb_resolve_refs].attachment = nb_descs;
            resolve_refs[nb_resolve_refs].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            nb_descs++;
            nb_resolve_refs++;
        }
    }

    VkSubpassDescription subpass_description = {
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount    = nb_color_refs,
        .pColorAttachments       = color_refs,
        .pResolveAttachments     = nb_resolve_refs ? resolve_refs : NULL,
        .pDepthStencilAttachment = depth_stencil_refp,
    };

    VkSubpassDependency dependencies[2] = {
        {
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0,
            .srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }, {
            .srcSubpass      = 0,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }
    };

    VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = nb_descs,
        .pAttachments = descs,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = NGLI_ARRAY_NB(dependencies),
        .pDependencies = dependencies,
    };

    VkResult res = vkCreateRenderPass(vk->device, &render_pass_create_info, NULL, render_pass);
    if (res != VK_SUCCESS)
        return -1;

    return 0;
}

VkSampleCountFlagBits ngli_vk_get_sample_count(int samples)
{
    switch(samples) {
    case 0:
    case 1:
        return VK_SAMPLE_COUNT_1_BIT;
    case 2:
        return VK_SAMPLE_COUNT_2_BIT;
    case 4:
        return VK_SAMPLE_COUNT_4_BIT;
    case 8:
        return VK_SAMPLE_COUNT_8_BIT;
    case 16:
        return VK_SAMPLE_COUNT_16_BIT;
    case 32:
        return VK_SAMPLE_COUNT_32_BIT;
    case 64:
        return VK_SAMPLE_COUNT_64_BIT;
    default:
        ngli_assert(0);
    }
}

static VkImageAspectFlags get_vk_image_aspect_flags(VkFormat format)
{
    switch (format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

struct rendertarget *ngli_rendertarget_vk_create(struct gctx *gctx)
{
    struct rendertarget_vk *s = ngli_calloc(1, sizeof(*s));
    if (!s)
        return NULL;
    s->parent.gctx = gctx;
    return (struct rendertarget *)s;
}

int ngli_rendertarget_vk_init(struct rendertarget *s, const struct rendertarget_params *params)
{
    struct rendertarget_vk *s_priv = (struct rendertarget_vk *)s;
    struct gctx_vk *gctx_vk = (struct gctx_vk *)s->gctx;
    struct vkcontext *vk = gctx_vk->vkcontext;

    s->width = params->width;
    s->height = params->height;
    s->params = *params;

    ngli_assert(params->nb_colors <= NGLI_MAX_COLOR_ATTACHMENTS);

    struct rendertarget_desc desc = {0};
    for (int i = 0; i < params->nb_colors; i++) {
        const struct attachment *attachment = &params->colors[i];
        const struct texture *texture = attachment->attachment;
        const struct texture_params *texture_params = &texture->params;
        desc.colors[desc.nb_colors].format = texture_params->format;
        desc.colors[desc.nb_colors].samples = texture_params->samples;
        desc.colors[desc.nb_colors].resolve = attachment->resolve_target != NULL;
        desc.nb_colors++;
    }

    const struct attachment *attachment = &params->depth_stencil;
    const struct texture *texture = attachment->attachment;
    if (texture) {
        const struct texture_params *texture_params = &texture->params;
        desc.depth_stencil.format = texture_params->format;
        desc.depth_stencil.samples = texture_params->samples;
        desc.depth_stencil.resolve = attachment->resolve_target != NULL;
    }

    int ret = ngli_vk_create_renderpass(s->gctx, &desc, &s_priv->render_pass, 0);
    if (ret < 0)
        return ret;

    ret = ngli_vk_create_renderpass(s->gctx, &desc, &s_priv->conservative_render_pass, 1);
    if (ret < 0)
        return ret;

    for (int i = 0; i < params->nb_colors; i++) {
        const struct attachment *attachment = &params->colors[i];
        const struct texture_vk *texture_vk = (struct texture_vk *)attachment->attachment;
        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture_vk->image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = texture_vk->format,
            .subresourceRange.aspectMask = get_vk_image_aspect_flags(texture_vk->format),
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = attachment->attachment_layer,
            .subresourceRange.layerCount = 1,
        };

        VkResult vkret = vkCreateImageView(vk->device, &view_info, NULL, &s_priv->attachments[s_priv->nb_attachments]);
        if (vkret != VK_SUCCESS)
            return -1;
        s_priv->nb_attachments++;

        if (attachment->resolve_target) {
            const struct texture_vk *texture_vk = (struct texture_vk *)attachment->resolve_target;
            VkImageViewCreateInfo view_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = texture_vk->image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = texture_vk->format,
                .subresourceRange.aspectMask = get_vk_image_aspect_flags(texture_vk->format),
                .subresourceRange.baseMipLevel = 0,
                .subresourceRange.levelCount = 1,
                .subresourceRange.baseArrayLayer = attachment->resolve_target_layer,
                .subresourceRange.layerCount = 1,
            };

            VkResult vkret = vkCreateImageView(vk->device, &view_info, NULL, &s_priv->attachments[s_priv->nb_attachments]);
            if (vkret != VK_SUCCESS)
                return -1;
            s_priv->nb_attachments++;
        }
    }

    attachment = &params->depth_stencil;
    texture = attachment->attachment;
    if (texture) {
        struct texture_vk *texture_vk = (struct texture_vk *)texture;
        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture_vk->image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = texture_vk->format,
            .subresourceRange.aspectMask = get_vk_image_aspect_flags(texture_vk->format),
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1,
        };

        VkResult vkret = vkCreateImageView(vk->device, &view_info, NULL, &s_priv->attachments[s_priv->nb_attachments]);
        if (vkret != VK_SUCCESS)
            return -1;
        s_priv->nb_attachments++;

        if (attachment->resolve_target) {
            struct texture_vk *resolve_target_vk = (struct texture_vk *)attachment->resolve_target;
            VkImageViewCreateInfo view_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = resolve_target_vk->image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = resolve_target_vk->format,
                .subresourceRange.aspectMask = get_vk_image_aspect_flags(resolve_target_vk->format),
                .subresourceRange.baseMipLevel = 0,
                .subresourceRange.levelCount = 1,
                .subresourceRange.baseArrayLayer = 0,
                .subresourceRange.layerCount = 1,
            };
            VkResult vkret = vkCreateImageView(vk->device, &view_info, NULL, &s_priv->attachments[s_priv->nb_attachments]);
            if (vkret != VK_SUCCESS)
                return -1;
            s_priv->nb_attachments++;
        }
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass      = s_priv->render_pass,
        .attachmentCount = s_priv->nb_attachments,
        .pAttachments    = s_priv->attachments,
        .width           = s->width,
        .height          = s->height,
        .layers          = 1
    };

    VkResult res = vkCreateFramebuffer(vk->device, &framebuffer_create_info, NULL, &s_priv->framebuffer);
    if (res != VK_SUCCESS)
        return -1;

    if (params->readable) {
        struct texture_params texture_params = NGLI_TEXTURE_PARAM_DEFAULTS;
        texture_params.width = s->width;
        texture_params.height = s->height;
        texture_params.format = NGLI_FORMAT_R8G8B8A8_UNORM;
        texture_params.staging = 1;

        s_priv->staging_texture = ngli_texture_create(s->gctx);
        if (!s_priv->staging_texture)
            return NGL_ERROR_MEMORY;

        int ret = ngli_texture_init(s_priv->staging_texture, &texture_params);
        if (ret < 0)
            return ret;
    }

    return 0;
}

void ngli_rendertarget_vk_blit(struct rendertarget *s, struct rendertarget *dst, int vflip)
{
}

void ngli_rendertarget_vk_resolve(struct rendertarget *s)
{
}

void ngli_rendertarget_vk_read_pixels(struct rendertarget *s, uint8_t *data)
{
    struct rendertarget_vk *s_priv = (struct rendertarget_vk *)s;
    struct gctx_vk *gctx_vk = (struct gctx_vk *)s->gctx;
    struct vkcontext *vk = gctx_vk->vkcontext;
    struct rendertarget_params *params = &s->params;

    if (!params->readable) {
        LOG(ERROR, "rendertarget is not readable");
        return;
    }

    ngli_gctx_vk_end_render_pass(s->gctx);

    struct attachment *color = &params->colors[0];
    struct texture *src = color->resolve_target ? color->resolve_target : color->attachment;
    struct texture_vk *src_vk = (struct texture_vk *)src;

    int ret = ngli_texture_vk_transition_layout(src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    if (ret < 0)
        return;

    struct texture *dst = s_priv->staging_texture;
    struct texture_vk *dst_vk = (struct texture_vk *)s_priv->staging_texture;
    ret = ngli_texture_vk_transition_layout(dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    if (ret < 0)
        return;

    VkCommandBuffer command_buffer = gctx_vk->cur_command_buffer;

    const VkImageCopy imageCopyRegion = {
        .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .srcSubresource.layerCount = 1,
        .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .dstSubresource.layerCount = 1,
        .extent.width = s->width,
        .extent.height = s->height,
        .extent.depth = 1,
    };

    vkCmdCopyImage(command_buffer, src_vk->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dst_vk->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);

    ret = ngli_texture_vk_transition_layout(dst, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    if (ret < 0)
        return;

    VkResult res = vkEndCommandBuffer(command_buffer);
    if (res != VK_SUCCESS)
        return;
    gctx_vk->cur_command_buffer_state = 0;

    VkSubmitInfo submit_info = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = ngli_darray_count(&gctx_vk->wait_semaphores),
        .pWaitSemaphores      = ngli_darray_data(&gctx_vk->wait_semaphores),
        .pWaitDstStageMask    = ngli_darray_data(&gctx_vk->wait_stages),
        .commandBufferCount   = 1,
        .pCommandBuffers      = &command_buffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores    = NULL,
    };
    ngli_darray_clear(&gctx_vk->wait_semaphores);
    ngli_darray_clear(&gctx_vk->wait_stages);

    res = vkQueueSubmit(vk->graphic_queue, 1, &submit_info, gctx_vk->fences[gctx_vk->frame_index]);
    if (res != VK_SUCCESS) {
        return;
    }

    res = vkWaitForFences(vk->device, 1, &gctx_vk->fences[gctx_vk->frame_index], VK_TRUE, UINT64_MAX);
    if (res != VK_SUCCESS)
        return;

    res = vkResetFences(vk->device, 1, &gctx_vk->fences[gctx_vk->frame_index]);
    if (res != VK_SUCCESS)
        return;

    uint8_t *mapped_data = NULL;
    res = vkMapMemory(vk->device, dst_vk->image_memory, 0, VK_WHOLE_SIZE, 0, (void**)&mapped_data);
    if (res != VK_SUCCESS)
        return;
    memcpy(data, mapped_data, s->width * s->height * 4);
    vkUnmapMemory(vk->device, dst_vk->image_memory);

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    res = vkBeginCommandBuffer(gctx_vk->cur_command_buffer, &command_buffer_begin_info);
    if (res != VK_SUCCESS)
        return;;
    gctx_vk->cur_command_buffer_state = 1;
}

void ngli_rendertarget_vk_freep(struct rendertarget **sp)
{
    if (!*sp)
        return;

    struct rendertarget *s = *sp;
    struct rendertarget_vk *s_priv = (struct rendertarget_vk *)s;
    struct gctx_vk *gctx_vk = (struct gctx_vk *)s->gctx;

    struct vkcontext *vk = gctx_vk->vkcontext;
    vkDestroyRenderPass(vk->device, s_priv->render_pass, NULL);
    vkDestroyRenderPass(vk->device, s_priv->conservative_render_pass, NULL);
    vkDestroyFramebuffer(vk->device, s_priv->framebuffer, NULL);

    for (int i = 0; i < s_priv->nb_attachments; i++)
        vkDestroyImageView(vk->device, s_priv->attachments[i], NULL);

    ngli_texture_freep(&s_priv->staging_texture);

    memset(s, 0, sizeof(*s));
}
