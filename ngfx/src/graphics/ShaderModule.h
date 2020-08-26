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
#include "graphics/Device.h"
#include "graphics/GraphicsCore.h"
#include <cstdint>
#include <string>
#include <memory>
#include <map>
#include <vector>
#define ENABLE_NGL_INTEGRATION

namespace ngfx {
    class ShaderModule {
    public:
        virtual ~ShaderModule() {}
        struct DescriptorInfo {
            std::string name;
            uint32_t set;
            DescriptorType type;
        };
        typedef std::vector<DescriptorInfo> DescriptorInfos;
        DescriptorInfos descriptors;
        struct BufferMemberInfo { uint32_t offset, size, arrayCount, arrayStride; };
        typedef std::map<std::string, BufferMemberInfo> BufferMemberInfos;
        struct BufferInfo {
            std::string name;
            uint32_t set;
            ShaderStageFlags shaderStages;
            BufferMemberInfos memberInfos;
        };
        typedef std::map<std::string, BufferInfo> BufferInfos;
        BufferInfos uniformBufferInfos, shaderStorageBufferInfos;
        void initBindings(std::ifstream& in, ShaderStageFlags shaderStages);
        void initBindings(const std::string& filename, ShaderStageFlags shaderStages);
    };
    class VertexShaderModule : public ShaderModule {
    public:
        static std::unique_ptr<VertexShaderModule> create(Device* device, const std::string& filename);
        virtual ~VertexShaderModule() {}
        struct AttributeDescription {
            std::string semantic;
            uint32_t    location;
            VertexFormat format;
            std::string name;
            uint32_t count, elementSize;
        };
        std::vector<AttributeDescription> attributes;
        void initBindings(const std::string& filename);
    };
    class FragmentShaderModule : public ShaderModule {
    public:
        static std::unique_ptr<FragmentShaderModule> create(Device* device, const std::string& filename);
        virtual ~FragmentShaderModule() {}
        void initBindings(const std::string& filename) {
            ShaderModule::initBindings(filename, SHADER_STAGE_FRAGMENT_BIT);
        }
    };
    class ComputeShaderModule : public ShaderModule {
    public:
        static std::unique_ptr<ComputeShaderModule> create(Device* device, const std::string& filename);
        virtual ~ComputeShaderModule() {}
        void initBindings(const std::string& filename) {
            ShaderModule::initBindings(filename, SHADER_STAGE_COMPUTE_BIT);
        }
    };
}
