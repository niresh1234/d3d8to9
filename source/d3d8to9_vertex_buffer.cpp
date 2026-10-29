/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to9.hpp"

Direct3DVertexBuffer8::Direct3DVertexBuffer8(Direct3DDevice8 *Device, IDirect3DVertexBuffer9 *ProxyInterface) :
	Device(Device), ProxyInterface(ProxyInterface)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DVertexBuffer8::~Direct3DVertexBuffer8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::QueryInterface(REFIID riid, void **ppvObj)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::QueryInterface");
#endif
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DVertexBuffer8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DVertexBuffer8 *>(this);

		return S_OK;
	}

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
}
ULONG STDMETHODCALLTYPE Direct3DVertexBuffer8::AddRef()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::AddRef");
#endif
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DVertexBuffer8::Release()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::Release");
#endif
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetDevice(IDirect3DDevice8 **ppDevice)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::GetDevice");
#endif
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::SetPrivateData");
#endif
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::GetPrivateData");
#endif
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::FreePrivateData(REFGUID refguid)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::FreePrivateData");
#endif
	return ProxyInterface->FreePrivateData(refguid);
}
DWORD STDMETHODCALLTYPE Direct3DVertexBuffer8::SetPriority(DWORD PriorityNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::SetPriority");
#endif
	return ProxyInterface->SetPriority(PriorityNew);
}
DWORD STDMETHODCALLTYPE Direct3DVertexBuffer8::GetPriority()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::GetPriority");
#endif
	return ProxyInterface->GetPriority();
}
void STDMETHODCALLTYPE Direct3DVertexBuffer8::PreLoad()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::PreLoad");
#endif
	ProxyInterface->PreLoad();
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DVertexBuffer8::GetType()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::GetType");
#endif
	return D3DRTYPE_VERTEXBUFFER;
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::Lock(UINT OffsetToLock, UINT SizeToLock, BYTE **ppbData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::Lock");
#endif
	if ((Flags & D3DLOCK_DISCARD) != 0)
	{
		D3DVERTEXBUFFER_DESC desc;
		ProxyInterface->GetDesc(&desc);

		if ((desc.Usage & D3DUSAGE_DYNAMIC) == 0 ||
			(desc.Usage & D3DUSAGE_WRITEONLY) == 0)
		{
			Flags ^= D3DLOCK_DISCARD;
		}
	}

	return ProxyInterface->Lock(OffsetToLock, SizeToLock, reinterpret_cast<void **>(ppbData), Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::Unlock()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::Unlock");
#endif
	return ProxyInterface->Unlock();
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetDesc(D3DVERTEXBUFFER_DESC *pDesc)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVertexBuffer8::GetDesc");
#endif
	return ProxyInterface->GetDesc(pDesc);
}
