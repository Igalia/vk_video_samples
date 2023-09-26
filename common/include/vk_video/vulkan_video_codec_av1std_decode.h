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
typedef struct StdVideoDecodeAV1PictureInfoFlags {
    uint32_t    error_resilient_mode : 1;
    uint32_t    disable_cdf_update : 1;
    uint32_t    use_superres : 1;
    uint32_t    render_and_frame_size_different : 1;
    uint32_t    allow_screen_content_tools : 1;
    uint32_t    is_filter_switchable : 1;
    uint32_t    force_integer_mv : 1;
    uint32_t    frame_size_override_flag : 1;
    uint32_t    buffer_removal_time_present_flag : 1;
    uint32_t    allow_intrabc : 1;
    uint32_t    frame_refs_short_signaling : 1;
    uint32_t    allow_high_precision_mv : 1;
    uint32_t    is_motion_mode_switchable : 1;
    uint32_t    use_ref_frame_mvs : 1;
    uint32_t    disable_frame_end_update_cdf : 1;
    uint32_t    allow_warped_motion : 1;
    uint32_t    reduced_tx_set : 1;
    uint32_t    reference_select : 1;
    uint32_t    skip_mode_present : 1;
    uint32_t    delta_q_present : 1;
    uint32_t    delta_lf_present : 1;
    uint32_t    delta_lf_multi : 1;
    uint32_t    segmentation_enabled : 1;
    uint32_t    segmentation_update_map : 1;
    uint32_t    segmentation_temporal_update : 1;
    uint32_t    segmentation_update_data : 1;
    uint32_t    UsesLr : 1;
    uint32_t    usesChromaLr : 1;
    uint32_t    apply_grain : 1;
    uint32_t    reserved : 3;
} StdVideoDecodeAV1PictureInfoFlags;

typedef struct StdVideoDecodeAV1PictureInfo {
    StdVideoDecodeAV1PictureInfoFlags    flags;
    StdVideoAV1FrameType                 frame_type;
    uint32_t                             frame_presentation_time;
    uint32_t                             display_frame_id;
    uint32_t                             current_frame_id;
    uint8_t                              frame_to_show_map_idx;
    uint8_t                              order_hint;
    uint8_t                              primary_ref_frame;
    uint8_t                              refresh_frame_flags;
    uint16_t                             frame_width_minus_1;
    uint16_t                             frame_height_minus_1;
    uint16_t                             render_width_minus_1;
    uint16_t                             render_height_minus_1;
    StdVideoAV1InterpolationFilter       interpolation_filter;
    StdVideoAV1TxMode                    TxMode;
    uint8_t                              delta_q_res;
    uint8_t                              delta_lf_res;
    uint8_t                              SkipModeFrame[2];
    uint16_t                             tg_start;
    uint16_t                             tg_end;
    uint8_t                              coded_denom;
    uint8_t                              RefOrderHintValues[STD_VIDEO_AV1_REFS_PER_FRAME];
    int8_t                               RefFrameSignedBiasValues[STD_VIDEO_AV1_REFS_PER_FRAME];
    uint8_t                              reserved1;
    uint16_t                             ExpectedFrameIdValues[STD_VIDEO_AV1_REFS_PER_FRAME];
    uint16_t                             reserved2;
    StdVideoAV1LoopFilter                loop_filter;
    StdVideoAV1Quantization              quantization;
    const StdVideoAV1Segmentation*       pSegmentation;
    StdVideoAV1TileInfo                  tile_info;
    const StdVideoAV1CDEF*               pCDEF;
    StdVideoAV1LoopRestoration           lr;
    StdVideoAV1GlobalMotion              global_motion;
    const StdVideoAV1FilmGrain*          pFilmGrain;
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
} StdVideoDecodeAV1ReferenceInfo;


#ifdef __cplusplus
}
#endif

#endif
