#include "DoubleRayDescriptor.h"
#include "DescriptorHeapMgr.h"
#include "DoubleResourceWrapper.h"
#include "RayDescriptor.h"

void DoubleRayDescriptor::CreateSRV(std::weak_ptr<DoubleResourceWrapper> Resource, UINT NumElements, UINT FirstElement, UINT Stride)
{

	/*===== SRV‚ğ¶¬ =====*/

	// ¶¬
	rayDescriptor_[0] = std::make_shared<RayDescriptor>();
	rayDescriptor_[1] = std::make_shared<RayDescriptor>();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	rayDescriptor_[0]->CreateStructuredSRV(Resource.lock()->GetBuffer(0), NumElements, FirstElement, Stride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	rayDescriptor_[0]->CreateStructuredSRV(Resource.lock()->GetBuffer(1), NumElements, FirstElement, Stride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	rayDescriptor_[1]->CreateStructuredSRV(Resource.lock()->GetBuffer(0), NumElements, FirstElement, Stride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	rayDescriptor_[1]->CreateStructuredSRV(Resource.lock()->GetBuffer(1), NumElements, FirstElement, Stride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

}

std::weak_ptr<RayDescriptor> DoubleRayDescriptor::GetDescriptor(int Index)
{

	/*===== GPUƒAƒhƒŒƒX‚ğæ“¾ =====*/

	return rayDescriptor_[Index];

}
