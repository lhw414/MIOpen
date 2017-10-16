/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2017 Advanced Micro Devices, Inc.
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

#ifndef GUARD_MIOPEN_RNN_HPP_
#define GUARD_MIOPEN_RNN_HPP_

#include <miopen/miopen.h>
#include <miopen/handle.hpp>
#include <miopen/tensor.hpp>
#include <miopen/common.hpp>
#include <miopen/mlo_internal.hpp>
#include <functional>
#include <numeric>

namespace miopen {

struct PerfField
{
    std::string name;
    float time;
    std::size_t workspace;

    bool operator<(const PerfField& p) const { return (time < p.time); }
};

template <class T>
struct c_array_view
{
    T* data;
    size_t n;

    using value_type =
        typename std::remove_cv<typename std::decay<decltype(deref(*data))>::type>::type;

    size_t size() const { return size; }

    const value_type& operator[](size_t i) const { return deref(data[i]); }

    value_type& operator[](size_t i) { return deref(data[i]); }
};

struct RNNDescriptor : miopenRNNDescriptor
{

    RNNDescriptor();
    RNNDescriptor(int hsz,
                  int layers,
                  miopenRNNMode_t rmode,
                  miopenRNNInputMode_t inMode,
                  miopenRNNDirectionMode_t bidir,
                  miopenRNNBiasMode_t bias,
                  miopenRNNAlgo_t amode,
                  miopenDataType_t dType);

    size_t hsize; // DLOWELL: is this uniform over all layers?
    size_t nLayers;
    size_t nHiddenTensorsPerLayer; // TODO dlowell: set via constructor, or "set" functions
    size_t workspaceScale;

    size_t inputBatchLenSum;

    miopenRNNMode_t rnnMode;
    miopenRNNDirectionMode_t dirMode;
    miopenRNNAlgo_t algoMode;
    miopenRNNInputMode_t inputMode;
    miopenRNNBiasMode_t biasMode;
    miopenDataType_t dataType;

    size_t GetWorkspaceSize(Handle& handle,
                            const int seqLength,
                            c_array_view<miopenTensorDescriptor_t> xDesc);
    // TensorDescriptor* xDesc);

    size_t GetReserveSize(Handle& handle,
                          const int seqLength,
                          c_array_view<miopenTensorDescriptor_t> xDesc);
    // TensorDescriptor* xDesc);

    size_t
    GetParamsSize(Handle& handle, const TensorDescriptor& xDesc, miopenDataType_t dtype) const;

    void GetLayerParam(Handle& handle,
                       const TensorDescriptor& xDesc,
                       const TensorDescriptor& wDesc,
                       ConstData_t w,
                       const int layerID,
                       const TensorDescriptor& paramDesc,
                       Data_t param) const;

    void GetLayerBias(Handle& handle,
                      const TensorDescriptor& xDesc,
                      const TensorDescriptor& wDesc,
                      ConstData_t w,
                      const int layerID,
                      const TensorDescriptor& biasDesc,
                      Data_t bias) const;

    size_t GetRNNInputSuperTensorSize(Handle& handle,
                                      const int seqLength,
                                      c_array_view<miopenTensorDescriptor_t> xDesc);
    // TensorDescriptor* xDesc);
    size_t GetRNNHiddenSuperTensorSize(Handle& handle,
                                       c_array_view<miopenTensorDescriptor_t> xDesc);
    // TensorDescriptor* xDesc);

    /* Get weight super tensor size
    temporary function assuming output matrix exists */
    size_t GetRNNWeightSuperTensorSize(Handle& handle,
                                       const TensorDescriptor& xDesc,
                                       const TensorDescriptor& yDesc) const;

    void RNNForwardTraining(Handle& handle,
                            const int seqLen,
                            c_array_view<miopenTensorDescriptor_t> xDesc,
                            ConstData_t x,
                            const TensorDescriptor& hxDesc,
                            ConstData_t hx,
                            const TensorDescriptor& cxDesc,
                            ConstData_t cx,
                            const TensorDescriptor& wDesc,
                            ConstData_t w,
                            c_array_view<miopenTensorDescriptor_t> yDesc,
                            Data_t y,
                            const TensorDescriptor& hyDesc,
                            Data_t hy,
                            const TensorDescriptor& cyDesc,
                            Data_t cy,
                            Data_t workSpace,
                            size_t workSpaceSize,
                            Data_t reserveSpace,
                            size_t reserveSpaceSize) const;

    void RNNForwardInference(Handle& handle,
                             const int seqLen,
                             c_array_view<miopenTensorDescriptor_t> xDesc,
                             ConstData_t x,
                             const TensorDescriptor& hxDesc,
                             ConstData_t hx,
                             const TensorDescriptor& cxDesc,
                             ConstData_t cx,
                             const TensorDescriptor& wDesc,
                             ConstData_t w,
                             c_array_view<miopenTensorDescriptor_t> yDesc,
                             Data_t y,
                             const TensorDescriptor& hyDesc,
                             Data_t hy,
                             const TensorDescriptor& cyDesc,
                             Data_t cy,
                             Data_t workSpace,
                             size_t workSpaceSize) const;

    void RNNBackwardData(Handle& handle,
                         const int seqLen,
                         c_array_view<miopenTensorDescriptor_t> yDesc,
                         ConstData_t y,
                         c_array_view<miopenTensorDescriptor_t> dyDesc,
                         ConstData_t dy,
                         const TensorDescriptor& dhyDesc,
                         ConstData_t dhy,
                         const TensorDescriptor& dcyDesc,
                         ConstData_t dcy,
                         const TensorDescriptor& wDesc,
                         ConstData_t w,
                         const TensorDescriptor& hxDesc,
                         ConstData_t hx,
                         const TensorDescriptor& cxDesc,
                         ConstData_t cx,
                         c_array_view<miopenTensorDescriptor_t> dxDesc,
                         Data_t dx,
                         const TensorDescriptor& dhxDesc,
                         Data_t dhx,
                         const TensorDescriptor& dcxDesc,
                         Data_t dcx,
                         Data_t workSpace,
                         size_t workSpaceSize,
                         ConstData_t reserveSpace,
                         size_t reserveSpaceSize) const;

    void RNNBackwardWeights(Handle& handle,
                            const int seqLen,
                            c_array_view<miopenTensorDescriptor_t> xDesc,
                            ConstData_t x,
                            const TensorDescriptor& hxDesc,
                            ConstData_t hx,
                            c_array_view<miopenTensorDescriptor_t> dyDesc,
                            ConstData_t dy,
                            const TensorDescriptor& dwDesc,
                            Data_t dw,
                            ConstData_t workSpace,
                            size_t workSpaceSize,
                            ConstData_t reserveSpace,
                            size_t reserveSpaceSize) const;

    // DLOWELL : These will be implemented once all the other elements are in place

    void ForwardRNNInferCell(Handle& handle,
                             const TensorDescriptor& xDesc,
                             ConstData_t x,
                             const TensorDescriptor& hxDesc,
                             ConstData_t hx,
                             const TensorDescriptor& wDesc,
                             ConstData_t w,
                             const TensorDescriptor& yDesc,
                             Data_t y,
                             const TensorDescriptor& hyDesc,
                             Data_t hy,
                             Data_t workSpace,
                             size_t workSpaceSize) const;

    void ForwardRNNTrainCell(Handle& handle,
                             const TensorDescriptor& xDesc,
                             ConstData_t x,
                             const TensorDescriptor& hxDesc,
                             ConstData_t hx,
                             const TensorDescriptor& wDesc,
                             ConstData_t w,
                             const TensorDescriptor& yDesc,
                             Data_t y,
                             const TensorDescriptor& hyDesc,
                             Data_t hy,
                             Data_t workSpace,
                             size_t workSpaceSize,
                             Data_t reserveSpace,
                             size_t reserveSpaceSize) const;

    void BackwardRNNDataCell(Handle& handle,
                             const TensorDescriptor& yDesc,
                             ConstData_t y,
                             const TensorDescriptor& dyDesc,
                             ConstData_t dy,
                             const TensorDescriptor& dhyDesc,
                             ConstData_t dhy,
                             const TensorDescriptor& wDesc,
                             ConstData_t w,
                             const TensorDescriptor& hxDesc,
                             ConstData_t hx,
                             const TensorDescriptor& dxDesc,
                             Data_t dx,
                             const TensorDescriptor& dhxDesc,
                             Data_t dhx,
                             Data_t workSpace,
                             size_t workSpaceSize,
                             ConstData_t reserveSpace,
                             size_t reserveSpaceSize) const;

    void BackwardRNNWeightsCell(Handle& handle,
                                const TensorDescriptor& xDesc,
                                ConstData_t x,
                                const TensorDescriptor& hxDesc,
                                ConstData_t hx,
                                const TensorDescriptor& yDesc,
                                ConstData_t y,
                                const TensorDescriptor& dwDesc,
                                Data_t dw,
                                ConstData_t workSpace,
                                size_t workSpaceSize,
                                ConstData_t reserveSpace,
                                size_t reserveSpaceSize) const;
};

std::ostream& operator<<(std::ostream& stream, const RNNDescriptor& c);

} // namespace miopen
MIOPEN_DEFINE_OBJECT(miopenRNNDescriptor, miopen::RNNDescriptor);

#endif // GUARD_MIOPEN_RNN_HPP_
