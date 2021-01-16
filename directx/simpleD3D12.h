/**********************************************************************************************************************************************\
	Copyright 1993-2018 NVIDIA Corporation. All rights reserved.

		Please refer to the NVIDIA end user license agreement (EULA) associated with this source code for terms
		and conditions that govern your use of this software. Any use, reproduction, disclosure, or distribution
		of this software and related documentation outside the terms of the EULA is strictly prohibited.
\**********************************************************************************************************************************************/
#pragma once

#include "DX12CudaSample.h"
#include "ShaderStructs.h"

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU, it has no understanding of the lifetime of resources
// on the GPU. Apps must account for the GPU lifetime of resources to avoid destroying objects that may still be referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

static constexpr const char* shaderstr
{
R"xkcd(	struct PSInput
	{
		float4 position : SV_POSITION;
		float4 color : COLOR;
	}

	PSInput VSMain(float3 position : POSITION, float4 color : COLOR)
	{
		PSInput result;
		result.position = float4(position, 1.0f);
		result.color = color;
		return result;
	}

	float4 PSMain(PSInput input) : SV_TARGET
	{
		return input.color;
	}
)xkcd"
};

class DX12CudaInterop : public DX12CudaSample
{
public:
	DX12CudaInterop(UINT width, UINT height, std::string name);

	virtual void OnInit();
	virtual void OnRender();
	virtual void OnDestroy();

private:
	// In this sample we overload the meaning of FrameCount to mean both the maximum number of frames that will be queued to the GPU at a time,
	// as well as the number of back buffers in the DXGI swap chain. For the majority of applications, this is convenient and works well.
	// However, there will be certain cases where an application may want to queue up more frames than there are back buffers available.
	// It should be noted that excessive buffering of frames dependent on user input may result in noticeable latency in your app.
	static const UINT FrameCount{2};
	std::string shadersSrc{shaderstr};
#if 0
		" struct PSInput \n" \
		" { \n" \
		"  float4 position : SV_POSITION; \n" \
		"  float4 color : COLOR; \n" \
		" } \n" \
		" PSInput VSMain(float3 position : POSITION, float4 color : COLOR) \n" \
		" { \n" \
		"  PSInput result;\n" \
		"  result.position = float4(position, 1.0f);\n" \
		"  result.color = color;\n"	\
		"  return result; \n" \
		" } \n" \
		" float4 PSMain(PSInput input) : SV_TARGET \n" \
		" { \n" \
		"  return input.color;\n" \
		" } \n";
#endif

	// Vertex Buffer dimension
	size_t vertBufHeight;
	size_t vertBufWidth;

	// Pipeline objects.
	D3D12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources.
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValues[FrameCount];

	// CUDA objects
	cudaExternalMemoryHandleType m_externalMemoryHandleType;
	cudaExternalMemory_t m_externalMemory;
	cudaExternalSemaphore_t m_externalSemaphore;
	cudaStream_t m_streamToRun;
	LUID m_dx12deviceluid;
	UINT m_cudaDeviceID;
	UINT m_nodeMask;
	float m_AnimTime;
	void *m_cudaDevVertptr{nullptr};

	void LoadPipeline();
	void InitCuda();
	void LoadAssets();
	void PopulateCommandList();
	void MoveToNextFrame();
	void WaitForGpu();
};
