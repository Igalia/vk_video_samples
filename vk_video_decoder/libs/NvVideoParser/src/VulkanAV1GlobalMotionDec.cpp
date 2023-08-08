/*
* Copyright 2023 NVIDIA Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <stdint.h>

#include "VulkanVideoParserIf.h"

#ifdef ENABLE_AV1_DECODER
#include "VulkanAV1Decoder.h"


#define DIV_LUT_PREC_BITS 14
#define DIV_LUT_BITS 8
#define DIV_LUT_NUM (1 << DIV_LUT_BITS)

#define ROUND_POWER_OF_TWO(value, n) (((value) + (1 << ((n)-1))) >> (n))

/* Shift down with rounding for use when n >= 0, value >= 0 for (64 bit) */
#define ROUND_POWER_OF_TWO_64(value, n) \
  (((value) + ((((int64_t)1 << (n)) >> 1))) >> (n))

/* Shift down with rounding for signed integers, for use when n >= 0 (64 bit) */
#define ROUND_POWER_OF_TWO_SIGNED_64(value, n)           \
  (((value) < 0) ? -ROUND_POWER_OF_TWO_64(-(value), (n)) \
                 : ROUND_POWER_OF_TWO_64((value), (n)))

/* Shift down with rounding for signed integers, for use when n >= 0 */
#define ROUND_POWER_OF_TWO_SIGNED(value, n)           \
  (((value) < 0) ? -ROUND_POWER_OF_TWO(-(value), (n)) \
                 : ROUND_POWER_OF_TWO((value), (n)))

// Bits of precision used for the model
#define WARPEDMODEL_PREC_BITS 16
#define WARPEDMODEL_ROW3HOMO_PREC_BITS 16

// Bits of subpel precision for warped interpolation
#define WARPEDPIXEL_PREC_BITS 6
#define WARPEDPIXEL_PREC_SHIFTS (1 << WARPEDPIXEL_PREC_BITS)

#define SUBEXPFIN_K 3
#define GM_TRANS_PREC_BITS 6
#define GM_ABS_TRANS_BITS 12
#define GM_ABS_TRANS_ONLY_BITS (GM_ABS_TRANS_BITS - GM_TRANS_PREC_BITS + 3)
#define GM_TRANS_PREC_DIFF (WARPEDMODEL_PREC_BITS - GM_TRANS_PREC_BITS)
#define GM_TRANS_ONLY_PREC_DIFF (WARPEDMODEL_PREC_BITS - 3)
#define GM_TRANS_DECODE_FACTOR (1 << GM_TRANS_PREC_DIFF)
#define GM_TRANS_ONLY_DECODE_FACTOR (1 << GM_TRANS_ONLY_PREC_DIFF)

#define GM_ALPHA_PREC_BITS 15
#define GM_ABS_ALPHA_BITS 12
#define GM_ALPHA_PREC_DIFF (WARPEDMODEL_PREC_BITS - GM_ALPHA_PREC_BITS)
#define GM_ALPHA_DECODE_FACTOR (1 << GM_ALPHA_PREC_DIFF)

#define GM_ROW3HOMO_PREC_BITS 16
#define GM_ABS_ROW3HOMO_BITS 11
#define GM_ROW3HOMO_PREC_DIFF \
  (WARPEDMODEL_ROW3HOMO_PREC_BITS - GM_ROW3HOMO_PREC_BITS)
#define GM_ROW3HOMO_DECODE_FACTOR (1 << GM_ROW3HOMO_PREC_DIFF)

#define GM_ROW3HOMO_MAX (1 << GM_ABS_ROW3HOMO_BITS)

#define GM_TRANS_MAX (1 << GM_ABS_TRANS_BITS)
#define GM_ALPHA_MAX (1 << GM_ABS_ALPHA_BITS)
#define GM_ROW3HOMO_MAX (1 << GM_ABS_ROW3HOMO_BITS)

#define GM_TRANS_MIN -GM_TRANS_MAX
#define GM_ALPHA_MIN -GM_ALPHA_MAX
#define GM_ROW3HOMO_MIN -GM_ROW3HOMO_MAX

#define WARP_PARAM_REDUCE_BITS 6
#define WARPEDMODEL_PREC_BITS 16

static const uint16_t div_lut[DIV_LUT_NUM + 1] = {
    16384, 16320, 16257, 16194, 16132, 16070, 16009, 15948, 15888, 15828, 15768,
    15709, 15650, 15592, 15534, 15477, 15420, 15364, 15308, 15252, 15197, 15142,
    15087, 15033, 14980, 14926, 14873, 14821, 14769, 14717, 14665, 14614, 14564,
    14513, 14463, 14413, 14364, 14315, 14266, 14218, 14170, 14122, 14075, 14028,
    13981, 13935, 13888, 13843, 13797, 13752, 13707, 13662, 13618, 13574, 13530,
    13487, 13443, 13400, 13358, 13315, 13273, 13231, 13190, 13148, 13107, 13066,
    13026, 12985, 12945, 12906, 12866, 12827, 12788, 12749, 12710, 12672, 12633,
    12596, 12558, 12520, 12483, 12446, 12409, 12373, 12336, 12300, 12264, 12228,
    12193, 12157, 12122, 12087, 12053, 12018, 11984, 11950, 11916, 11882, 11848,
    11815, 11782, 11749, 11716, 11683, 11651, 11619, 11586, 11555, 11523, 11491,
    11460, 11429, 11398, 11367, 11336, 11305, 11275, 11245, 11215, 11185, 11155,
    11125, 11096, 11067, 11038, 11009, 10980, 10951, 10923, 10894, 10866, 10838,
    10810, 10782, 10755, 10727, 10700, 10673, 10645, 10618, 10592, 10565, 10538,
    10512, 10486, 10460, 10434, 10408, 10382, 10356, 10331, 10305, 10280, 10255,
    10230, 10205, 10180, 10156, 10131, 10107, 10082, 10058, 10034, 10010, 9986,
    9963,  9939,  9916,  9892,  9869,  9846,  9823,  9800,  9777,  9754,  9732,
    9709,  9687,  9664,  9642,  9620,  9598,  9576,  9554,  9533,  9511,  9489,
    9468,  9447,  9425,  9404,  9383,  9362,  9341,  9321,  9300,  9279,  9259,
    9239,  9218,  9198,  9178,  9158,  9138,  9118,  9098,  9079,  9059,  9039,
    9020,  9001,  8981,  8962,  8943,  8924,  8905,  8886,  8867,  8849,  8830,
    8812,  8793,  8775,  8756,  8738,  8720,  8702,  8684,  8666,  8648,  8630,
    8613,  8595,  8577,  8560,  8542,  8525,  8508,  8490,  8473,  8456,  8439,
    8422,  8405,  8389,  8372,  8355,  8339,  8322,  8306,  8289,  8273,  8257,
    8240,  8224,  8208,  8192,
};

int get_msb(unsigned int n) 
{
    int log = 0;
    unsigned int value = n;
    int i;

    assert(n != 0);

    for (i = 4; i >= 0; --i) {
        const int shift = (1 << i);
        const unsigned int x = value >> shift;
        if (x != 0) {
            value = x;
            log += shift;
        }
    }

    return log;
}

// Inverse recenters a non-negative literal v around a reference r
static uint16_t inv_recenter_nonneg(uint16_t r, uint16_t v) 
{
    if (v > (r << 1))
        return v;
    else if ((v & 1) == 0)
        return (v >> 1) + r;
    else
        return r - ((v + 1) >> 1);
}

// Inverse recenters a non-negative literal v in [0, n-1] around a
// reference r also in [0, n-1]
static uint16_t inv_recenter_finite_nonneg(uint16_t n, uint16_t r, uint16_t v) 
{
    if ((r << 1) <= n) {
        return inv_recenter_nonneg(r, v);
    } else {
        return n - 1 - inv_recenter_nonneg(n - 1 - r, v);
    }
}

uint16_t VulkanAV1Decoder::Read_primitive_quniform(uint16_t n) 
{
    if (n <= 1) return 0;
    const int l = get_msb(n - 1) + 1;
    const int m = (1 << l) - n;
    const int v = u(l - 1);
    return v < m ? v : (v << 1) - m + u(1);
}

uint16_t VulkanAV1Decoder::Read_primitive_subexpfin(uint16_t n, uint16_t k) 
{
    int i = 0;
    int mk = 0;

    while (1) {
        int b = (i ? k + i - 1 : k);
        int a = (1 << b);

        if (n <= mk + 3 * a) {
            return Read_primitive_quniform(n - mk) + mk;
        }

        if (!u(1)) {
        return u(b) + mk;
        }

        i = i + 1;
        mk += a;
    }

    assert(0);
    return 0;
}

uint16_t VulkanAV1Decoder::Read_primitive_refsubexpfin(uint16_t n, uint16_t k, uint16_t ref) 
{
    return inv_recenter_finite_nonneg(n, ref,
                                    Read_primitive_subexpfin(n, k));
}

int16_t VulkanAV1Decoder::Read_signed_primitive_refsubexpfin(uint16_t n, uint16_t k, int16_t ref) 
{
    ref += n - 1;
    const uint16_t scaled_n = (n << 1) - 1;
    return Read_primitive_refsubexpfin(scaled_n, k, ref) - n + 1;
}

int VulkanAV1Decoder::ReadGlobalMotionParams(AV1WarpedMotionParams *params, const AV1WarpedMotionParams *ref_params, int allow_hp) 
{
    AV1_TRANSFORMATION_TYPE type = (AV1_TRANSFORMATION_TYPE)u(1);
    if (type != IDENTITY) 
    {
        if (u(1))
        type = ROTZOOM;
        else
        type = u(1) ? TRANSLATION : AFFINE;
    }

    *params = default_warp_params;
    params->wmtype = type;

    if (type >= ROTZOOM) {
        params->wmmat[2] = Read_signed_primitive_refsubexpfin(
                           GM_ALPHA_MAX + 1, SUBEXPFIN_K,
                           (ref_params->wmmat[2] >> GM_ALPHA_PREC_DIFF) -
                               (1 << GM_ALPHA_PREC_BITS)) *
                           GM_ALPHA_DECODE_FACTOR +
                       (1 << WARPEDMODEL_PREC_BITS);
        params->wmmat[3] = Read_signed_primitive_refsubexpfin(
                           GM_ALPHA_MAX + 1, SUBEXPFIN_K,
                           (ref_params->wmmat[3] >> GM_ALPHA_PREC_DIFF)) *
                       GM_ALPHA_DECODE_FACTOR;
    }

    if (type >= AFFINE) {
        params->wmmat[4] = Read_signed_primitive_refsubexpfin(
                           GM_ALPHA_MAX + 1, SUBEXPFIN_K,
                           (ref_params->wmmat[4] >> GM_ALPHA_PREC_DIFF)) *
                       GM_ALPHA_DECODE_FACTOR;
        params->wmmat[5] = Read_signed_primitive_refsubexpfin(
                           GM_ALPHA_MAX + 1, SUBEXPFIN_K,
                           (ref_params->wmmat[5] >> GM_ALPHA_PREC_DIFF) -
                               (1 << GM_ALPHA_PREC_BITS)) *
                           GM_ALPHA_DECODE_FACTOR +
                       (1 << WARPEDMODEL_PREC_BITS);
    } else {
        params->wmmat[4] = -params->wmmat[3];
        params->wmmat[5] = params->wmmat[2];
    }

    if (type >= TRANSLATION) {
        const int trans_bits = (type == TRANSLATION)
                                   ? GM_ABS_TRANS_ONLY_BITS - !allow_hp
                                   : GM_ABS_TRANS_BITS;
        const int trans_dec_factor =
            (type == TRANSLATION) ? GM_TRANS_ONLY_DECODE_FACTOR * (1 << (allow_hp ? 0 : 1)) : GM_TRANS_DECODE_FACTOR;
        const int trans_prec_diff = (type == TRANSLATION)
                                        ? GM_TRANS_ONLY_PREC_DIFF + !allow_hp
                                        : GM_TRANS_PREC_DIFF;
        params->wmmat[0] = Read_signed_primitive_refsubexpfin(
                               (1 << trans_bits) + 1, SUBEXPFIN_K,
                               (ref_params->wmmat[0] >> trans_prec_diff)) *
                           trans_dec_factor;
        params->wmmat[1] = Read_signed_primitive_refsubexpfin(
                               (1 << trans_bits) + 1, SUBEXPFIN_K,
                               (ref_params->wmmat[1] >> trans_prec_diff)) *
                               trans_dec_factor;
    }

    return 1;
}

uint32_t VulkanAV1Decoder::DecodeGlobalMotionParams()
{
    VkParserAv1PictureData *pic_info = &m_PicData;

    AV1WarpedMotionParams prev_models[GM_GLOBAL_MODELS_PER_FRAME];

    for(int i=0;i<GM_GLOBAL_MODELS_PER_FRAME;++i)
        prev_models[i] = default_warp_params;

    if(primary_ref_frame != PRIMARY_REF_NONE) {
        if (m_pBuffers[active_ref_names[primary_ref_frame]].buffer)
            memcpy(prev_models, m_pBuffers[active_ref_names[primary_ref_frame]].global_models, sizeof(AV1WarpedMotionParams)*GM_GLOBAL_MODELS_PER_FRAME);
    }
  
    for (int frame = 0; frame < GM_GLOBAL_MODELS_PER_FRAME; ++frame) {
        AV1WarpedMotionParams *ref_params = &prev_models[frame];

        int good_params = ReadGlobalMotionParams(&global_motions[frame], ref_params, pic_info->allow_high_precision_mv);
        if (!good_params) {
          global_motions[frame].invalid = 1;
        }
    }
    return 1;
}
#endif // ENABLE_AV1_DECODER
