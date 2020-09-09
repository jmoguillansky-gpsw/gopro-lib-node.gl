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
extern "C" {
#include <sxplayer.h>
}
#include <string>
#include <memory>

namespace ngfx {
    class MediaPlayer {
    public:
        MediaPlayer(const std::string& filename);
        ~MediaPlayer();
        void start();
        struct Frame {
            ~Frame() { if (v) { sxplayer_release_frame(v); v = nullptr; } }
            sxplayer_frame *v = nullptr;
        };
        Frame* getFrame(double time);
        sxplayer_ctx* ctx = nullptr;
        sxplayer_info info;
    };
}
