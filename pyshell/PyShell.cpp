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
#include <Python.h>
#include <clocale>
#include <cstdlib>

int main(int argc, char** argv) {
    wchar_t **wargv = new wchar_t*[argc];
    for (uint32_t j = 0; j<argc; j++) {
        int len = strlen(argv[j]);
        wargv[j] = new wchar_t[len+1];
        std::mbstowcs(wargv[j], argv[j], len);
        wargv[j][len] = 0;
    }
    const char* filename = argv[1];
    Py_Initialize();
    Py_Main(argc, wargv);
    FILE* fp = _Py_fopen(filename, "r");
    PyRun_SimpleFile(fp, filename);
    Py_Finalize();
}
