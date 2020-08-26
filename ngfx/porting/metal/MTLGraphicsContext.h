/*
 * Copyright 2016 GoPro Inc.
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#pragma once
#include "graphics/GraphicsContext.h"
#include "porting/metal/MTLCommandBuffer.h"
#include "porting/metal/MTLDevice.h"
#include "porting/metal/MTLFramebuffer.h"
#include "porting/metal/MTLPipelineCache.h"
#include "porting/metal/MTLRenderPass.h"
#include "porting/metal/MTLUtil.h"
#include "porting/metal/MTLDepthStencilTexture.h"
#include "DebugUtil.h"
#include <MetalKit/MetalKit.h>

namespace ngfx {
    class MTLGraphicsContext: public GraphicsContext {
    public:
        void create(const char* appName, bool enableDepthStencil, bool debug);
        virtual ~MTLGraphicsContext();
        void setSurface(Surface *surface) override;
        CommandBuffer* drawCommandBuffer(int32_t index = -1) override;
        CommandBuffer* copyCommandBuffer() override;
        CommandBuffer* computeCommandBuffer() override;
        void submit(CommandBuffer* commandBuffer) override;
        struct MTLRenderPassData {
            RenderPassConfig config;
            MTLRenderPass mtlRenderPass;
        };
        RenderPass* getRenderPass(RenderPassConfig config) override;
        std::vector<std::unique_ptr<MTLRenderPassData>> mtlRenderPassCache;
        MTLCommandBuffer mtlDrawCommandBuffer;
        MTLCommandBuffer mtlCopyCommandBuffer;
        MTLCommandBuffer mtlComputeCommandBuffer;
        MTLDevice mtlDevice;
        id<MTLCommandQueue> mtlCommandQueue;
        MTLPipelineCache mtlPipelineCache;
        ::MTLPixelFormat mtlSurfaceFormat;
        MTKView* mtkView;
        uint32_t numSwapchainImages;
        std::vector<MTLFramebuffer> mtlSwapchainFramebuffers;
        MTLRenderPass *mtlDefaultRenderPass = nullptr, *mtlDefaultOffscreenRenderPass = nullptr;
        std::unique_ptr<MTLDepthStencilTexture> mtlDepthStencilTexture;
        bool offscreen = true;
        uint32_t numSamples = 1;
    private:
        void createBindings();
        void createSwapchainFramebuffers(uint32_t w, uint32_t h);
    };
    MTL_CAST(GraphicsContext);
}
