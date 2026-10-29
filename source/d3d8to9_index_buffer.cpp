/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to9.hpp"

Direct3DIndexBuffer8::Direct3DIndexBuffer8(Direct3DDevice8 *Device, IDirect3DIndexBuffer9 *ProxyInterface) :
	Device(Device), ProxyInterface(ProxyInterface)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DIndexBuffer8::~Direct3DIndexBuffer8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::QueryInterface(REFIID riid, void **ppvObj)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::QueryInterface");
#endif
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DIndexBuffer8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DIndexBuffer8 *>(this);

		return S_OK;
	}

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
}
ULONG STDMETHODCALLTYPE Direct3DIndexBuffer8::AddRef()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::AddRef");
#endif
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DIndexBuffer8::Release()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::Release");
#endif
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::GetDevice(IDirect3DDevice8 **ppDevice)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::GetDevice");
#endif
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::SetPrivateData");
#endif
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::GetPrivateData");
#endif
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::FreePrivateData(REFGUID refguid)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::FreePrivateData");
#endif
	return ProxyInterface->FreePrivateData(refguid);
}
DWORD STDMETHODCALLTYPE Direct3DIndexBuffer8::SetPriority(DWORD PriorityNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::SetPriority");
#endif
	return ProxyInterface->SetPriority(PriorityNew);
}
DWORD STDMETHODCALLTYPE Direct3DIndexBuffer8::GetPriority()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::GetPriority");
#endif
	return ProxyInterface->GetPriority();
}
void STDMETHODCALLTYPE Direct3DIndexBuffer8::PreLoad()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::PreLoad");
#endif
	ProxyInterface->PreLoad();
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DIndexBuffer8::GetType()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::GetType");
#endif
	return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::Lock(UINT OffsetToLock, UINT SizeToLock, BYTE **ppbData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::Lock");
#endif
	if ((Flags & D3DLOCK_DISCARD) != 0)
	{
		D3DINDEXBUFFER_DESC desc;
		ProxyInterface->GetDesc(&desc);

		if ((desc.Usage & D3DUSAGE_DYNAMIC) == 0 ||
			(desc.Usage & D3DUSAGE_WRITEONLY) == 0)
		{
			Flags ^= D3DLOCK_DISCARD;
		}
	}

	return ProxyInterface->Lock(OffsetToLock, SizeToLock, reinterpret_cast<void **>(ppbData), Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::Unlock()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::Unlock");
#endif
	return ProxyInterface->Unlock();
}
HRESULT STDMETHODCALLTYPE Direct3DIndexBuffer8::GetDesc(D3DINDEXBUFFER_DESC *pDesc)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DIndexBuffer8::GetDesc");
#endif
	return ProxyInterface->GetDesc(pDesc);
}
