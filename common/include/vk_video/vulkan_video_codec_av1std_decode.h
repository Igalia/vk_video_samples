#ifndef VULKAN_VIDEO_CODEC_AV1STD_DECODE_H_
#define VULKAN_VIDEO_CODEC_AV1STD_DECODE_H_ 1

/*
** Copyright 2015-2023 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/

/*
** This header is generated from the Khronos Vulkan XML API Registry.
**
*/


#ifdef __cplusplus
extern "C" {
#endif



// vulkan_video_codec_av1std_decode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_av1std_decode 1
#include "vulkan_video_codec_av1std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_API_VERSION_0_9_2 VK_MAKE_VIDEO_STD_VERSION(0, 9, 2)

#define VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_API_VERSION_0_9_2
#define VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_av1_decode"
typedef struct StdVideoDecodeAV1PictureInfo {
    const StdVideoAV1FrameHeader*    pFrameHeader;
    uint16_t                         tg_start;
    uint16_t                         tg_end;
} StdVideoDecodeAV1PictureInfo;

typedef struct StdVideoDecodeAV1ReferenceInfoFlags {
    uint32_t    disable_frame_end_update_cdf : 1;
    uint32_t    segmentation_enabled : 1;
    uint32_t    reserved : 30;
} StdVideoDecodeAV1ReferenceInfoFlags;

typedef struct StdVideoDecodeAV1ReferenceInfo {
    StdVideoDecodeAV1ReferenceInfoFlags    flags;
    uint8_t                                frame_type;
    uint8_t                                base_q_idx;
    uint8_t                                GmType;
    uint8_t                                reserved1;
    int32_t                                gm_params[6];
    int8_t                                 RefFrameSignedBiasValues[8]; // This is what will be proposed. It is the calculated signed distance between a ref_order_hint array of a frame and the frame_offset aka order_hint.
} StdVideoDecodeAV1ReferenceInfo;


#ifdef __cplusplus
}
#endif

#endif
