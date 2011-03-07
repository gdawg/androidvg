/*
 * Copyright (C) 2010 Shuo-Hung Chen, Hsiao-Mei Lin
 * 
 * The AndroidVG Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef OPENVG_VG11_H
#define OPENVG_VG11_H

#define SUCCESS 1
#define FALSE 0


#ifdef __cplusplus
extern "C" {
#endif

#include <android/log.h>
#include "openvg.h"

#define LOG_TAG "VG11"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#ifdef __cplusplus
}
#endif


#endif /* OPENVG_VG11_H */
