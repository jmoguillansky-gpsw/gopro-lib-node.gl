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
#include "Application.h"
#include "drawOps/DrawTextureOp.h"
#include "MediaPlayer.h"
#include "Timer.h"
#include <memory>

namespace ngfx {
    class MediaPlayerApp : public Application {
    public:
        MediaPlayerApp();
        virtual void onInit();
        virtual void onRecordCommandBuffer(CommandBuffer* commandBuffer);
        virtual void onUpdate();
    private:
        std::unique_ptr<DrawTextureOp> drawTextureOp;
        std::unique_ptr<Texture> texture;
        std::unique_ptr<MediaPlayer> mediaPlayer;
        Timer timer;
        double time = 0.0;
    };
}
