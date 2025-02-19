/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2024 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
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
 *
 *******************************************************************************/

#include "unit_conv_solver.hpp"

#ifndef NUM_BATCH_LOOPS
#error "NUM_BATCH_LOOPS undefined"
#endif

#define MAKE_SOLVER_NAME1(name, suffix) name##suffix
#define MAKE_SOLVER_NAME(name, suffix) MAKE_SOLVER_NAME1(name, suffix)

#define SOLVER_NAME MAKE_SOLVER_NAME(ConvOclBwdWrW2N, NUM_BATCH_LOOPS)

namespace {

auto GetConvTestCases(miopenDataType_t datatype)
{
    using TestCase      = miopen::unit_tests::ConvTestCase;
    const std::size_t N = NUM_BATCH_LOOPS;

    return std::vector{
        // clang-format off
        TestCase{{N, 1, 16, 16}, {1, 1, 4, 4}, {3, 3}, {1, 1}, {1, 1}, datatype},
        // clang-format on
    };
}

const auto& GetTestParams()
{
    static const auto params = [] {
        auto p = miopen::unit_tests::UnitTestConvSolverParams(Gpu::All);
        p.EnableDeprecatedSolvers();
        p.Tunable(5);
        return p;
    }();
    return params;
}

} // namespace

TEST_P(GPU_UnitTestConvSolverWrw_FP16, SOLVER_NAME)
{
    this->RunTest(miopen::solver::conv::ConvOclBwdWrW2<NUM_BATCH_LOOPS>{});
};

TEST_P(GPU_UnitTestConvSolverWrw_BFP16, SOLVER_NAME)
{
    this->RunTest(miopen::solver::conv::ConvOclBwdWrW2<NUM_BATCH_LOOPS>{});
};

TEST_P(GPU_UnitTestConvSolverWrw_FP32, SOLVER_NAME)
{
    this->RunTest(miopen::solver::conv::ConvOclBwdWrW2<NUM_BATCH_LOOPS>{});
};

TEST_P(CPU_UnitTestConvSolverDevApplicabilityWrw_NONE, SOLVER_NAME)
{
    this->RunTest(miopen::solver::conv::ConvOclBwdWrW2<NUM_BATCH_LOOPS>{});
};

// Smoke tests
INSTANTIATE_TEST_SUITE_P(Smoke,
                         GPU_UnitTestConvSolverWrw_FP16,
                         testing::Combine(testing::Values(GetTestParams()),
                                          testing::Values(miopenConvolutionAlgoDirect),
                                          testing::ValuesIn(GetConvTestCases(miopenHalf))));

INSTANTIATE_TEST_SUITE_P(Smoke,
                         GPU_UnitTestConvSolverWrw_BFP16,
                         testing::Combine(testing::Values(GetTestParams()),
                                          testing::Values(miopenConvolutionAlgoDirect),
                                          testing::ValuesIn(GetConvTestCases(miopenBFloat16))));

INSTANTIATE_TEST_SUITE_P(Smoke,
                         GPU_UnitTestConvSolverWrw_FP32,
                         testing::Combine(testing::Values(GetTestParams()),
                                          testing::Values(miopenConvolutionAlgoDirect),
                                          testing::ValuesIn(GetConvTestCases(miopenFloat))));

// Device applicability test
INSTANTIATE_TEST_SUITE_P(Smoke,
                         CPU_UnitTestConvSolverDevApplicabilityWrw_NONE,
                         testing::Combine(testing::Values(GetTestParams()),
                                          testing::Values(GetConvTestCases(miopenFloat)[0])));
