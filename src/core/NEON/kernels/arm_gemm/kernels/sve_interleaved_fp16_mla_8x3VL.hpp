/*
 * Copyright (c) 2019-2021, 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#ifdef ARM_COMPUTE_ENABLE_SVE

#include "../std_transforms_sve.hpp"
#include "../performance_parameters.hpp"

#define ARGLIST  \
    const __fp16 *, const __fp16 *, \
    __fp16 *, int, int, int

namespace arm_gemm
{
// Actual kernel implementations
void sve_interleaved_fp16_mla_8x3VL( ARGLIST );
void sve_interleaved_fp16_mla_8x3VL_a64fx( ARGLIST );

class cls_sve_interleaved_fp16_mla_8x3VL
{
public:
    typedef __fp16 lhs_operand_type;
    typedef __fp16 rhs_operand_type;
    typedef __fp16 result_type;

    typedef void (*kern_type)( ARGLIST );

    /* Kernel blocking parameters */
    static constexpr unsigned int out_height()
    {
        return 8;
    }

    static unsigned int out_width()
    {
        return get_vector_length<__fp16>() * 3;
    }

    static constexpr unsigned int k_unroll()
    {
        return 1;
    }


    StdTransformsSVE<lhs_operand_type, rhs_operand_type, result_type, 8, 3, 1, 1> transforms = {};
    StdTransformsSVE<lhs_operand_type, rhs_operand_type, result_type, 8, 3, 1, 1, true> transforms_quantized = {};
    template<typename T>
    static inline PerformanceParameters get_performance_parameters(const CPUInfo *ci)
    {

        if (std::is_same<T, __fp16>::value) {
            switch (ci->get_cpu_model()) {
                default:
                    return { 15.96, 3.85, 6.91 };
                case CPUModel::A510:
                    return { 13.84, 2.07, 2.52 };
                case CPUModel::V1:
                    return { 31.90, 5.15, 10.34 };
                case CPUModel::A64FX:
                    return { 44.34, 3.23, 7.06 };
            }
        }

        return { 1.0 };
    }

    // Default to the generic kernel
    kern_type kernel=sve_interleaved_fp16_mla_8x3VL;
    cls_sve_interleaved_fp16_mla_8x3VL(const CPUInfo *ci)
    {
        switch(ci->get_cpu_model()) {
            default:
                break;
            case CPUModel::A64FX:
                kernel=sve_interleaved_fp16_mla_8x3VL_a64fx;
                break;
        }
    }
};

} // namespace arm_gemm

#undef ARGLIST
#endif // ARM_COMPUTE_ENABLE_SVE
