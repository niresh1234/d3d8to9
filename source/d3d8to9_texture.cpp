/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to9.hpp"

Direct3DTexture8::Direct3DTexture8(Direct3DDevice8 *Device, IDirect3DTexture9 *ProxyInterface) :
	Device(Device), ProxyInterface(ProxyInterface)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DTexture8::~Direct3DTexture8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DTexture8::QueryInterface(REFIID riid, void **ppvObj)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::QueryInterface");
#endif
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DTexture8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8) ||
		riid == __uuidof(IDirect3DBaseTexture8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DTexture8 *>(this);

		return S_OK;
	}

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
}
ULONG STDMETHODCALLTYPE Direct3DTexture8::AddRef()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::AddRef");
#endif
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DTexture8::Release()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::Release");
#endif
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetDevice(IDirect3DDevice8 **ppDevice)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetDevice");
#endif
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::SetPrivateData");
#endif
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetPrivateData");
#endif
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::FreePrivateData(REFGUID refguid)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::FreePrivateData");
#endif
	return ProxyInterface->FreePrivateData(refguid);
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::SetPriority(DWORD PriorityNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::SetPriority");
#endif
	return ProxyInterface->SetPriority(PriorityNew);
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::GetPriority()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetPriority");
#endif
	return ProxyInterface->GetPriority();
}
void STDMETHODCALLTYPE Direct3DTexture8::PreLoad()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::PreLoad");
#endif
	ProxyInterface->PreLoad();
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DTexture8::GetType()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetType");
#endif
	return D3DRTYPE_TEXTURE;
}

DWORD STDMETHODCALLTYPE Direct3DTexture8::SetLOD(DWORD LODNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::SetLOD");
#endif
	return ProxyInterface->SetLOD(LODNew);
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::GetLOD()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetLOD");
#endif
	return ProxyInterface->GetLOD();
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::GetLevelCount()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetLevelCount");
#endif
	return ProxyInterface->GetLevelCount();
}

HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetLevelDesc(UINT Level, D3DSURFACE_DESC8 *pDesc)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetLevelDesc");
#endif
	if (pDesc == nullptr)
		return D3DERR_INVALIDCALL;

	D3DSURFACE_DESC SurfaceDesc;

	const HRESULT hr = ProxyInterface->GetLevelDesc(Level, &SurfaceDesc);
	if (FAILED(hr))
		return hr;

	ConvertSurfaceDesc(SurfaceDesc, *pDesc);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetSurfaceLevel(UINT Level, IDirect3DSurface8 **ppSurfaceLevel)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::GetSurfaceLevel");
#endif
	if (ppSurfaceLevel == nullptr)
		return D3DERR_INVALIDCALL;

	*ppSurfaceLevel = nullptr;

	IDirect3DSurface9 *SurfaceInterface = nullptr;

	const HRESULT hr = ProxyInterface->GetSurfaceLevel(Level, &SurfaceInterface);
	if (FAILED(hr))
		return hr;

	*ppSurfaceLevel = Device->ProxyAddressLookupTable->FindAddress<Direct3DSurface8>(SurfaceInterface);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::LockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::LockRect");
#endif
	return ProxyInterface->LockRect(Level, pLockedRect, pRect, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::UnlockRect(UINT Level)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::UnlockRect");
#endif
	return ProxyInterface->UnlockRect(Level);
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::AddDirtyRect(const RECT *pDirtyRect)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DTexture8::AddDirtyRect");
#endif
	return ProxyInterface->AddDirtyRect(pDirtyRect);
}

Direct3DCubeTexture8::Direct3DCubeTexture8(Direct3DDevice8 *device, IDirect3DCubeTexture9 *ProxyInterface) :
	Device(device),
	ProxyInterface(ProxyInterface)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DCubeTexture8::~Direct3DCubeTexture8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::QueryInterface(REFIID riid, void **ppvObj)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::QueryInterface");
#endif
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DCubeTexture8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8) ||
		riid == __uuidof(IDirect3DBaseTexture8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DCubeTexture8 *>(this);

		return S_OK;
	}

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
}
ULONG STDMETHODCALLTYPE Direct3DCubeTexture8::AddRef()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::AddRef");
#endif
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DCubeTexture8::Release()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::Release");
#endif
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetDevice(IDirect3DDevice8 **ppDevice)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetDevice");
#endif
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::SetPrivateData");
#endif
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetPrivateData");
#endif
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::FreePrivateData(REFGUID refguid)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::FreePrivateData");
#endif
	return ProxyInterface->FreePrivateData(refguid);
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::SetPriority(DWORD PriorityNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::SetPriority");
#endif
	return ProxyInterface->SetPriority(PriorityNew);
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::GetPriority()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetPriority");
#endif
	return ProxyInterface->GetPriority();
}
void STDMETHODCALLTYPE Direct3DCubeTexture8::PreLoad()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::PreLoad");
#endif
	ProxyInterface->PreLoad();
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DCubeTexture8::GetType()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetType");
#endif
	return D3DRTYPE_CUBETEXTURE;
}

DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::SetLOD(DWORD LODNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::SetLOD");
#endif
	return ProxyInterface->SetLOD(LODNew);
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::GetLOD()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetLOD");
#endif
	return ProxyInterface->GetLOD();
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::GetLevelCount()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetLevelCount");
#endif
	return ProxyInterface->GetLevelCount();
}

HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetLevelDesc(UINT Level, D3DSURFACE_DESC8 *pDesc)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetLevelDesc");
#endif
	if (pDesc == nullptr)
		return D3DERR_INVALIDCALL;

	D3DSURFACE_DESC SurfaceDesc;

	const HRESULT hr = ProxyInterface->GetLevelDesc(Level, &SurfaceDesc);
	if (FAILED(hr))
		return hr;

	ConvertSurfaceDesc(SurfaceDesc, *pDesc);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface8 **ppCubeMapSurface)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::GetCubeMapSurface");
#endif
	if (ppCubeMapSurface == nullptr)
		return D3DERR_INVALIDCALL;

	*ppCubeMapSurface = nullptr;

	IDirect3DSurface9 *SurfaceInterface = nullptr;

	const HRESULT hr = ProxyInterface->GetCubeMapSurface(FaceType, Level, &SurfaceInterface);
	if (FAILED(hr))
		return hr;

	*ppCubeMapSurface = Device->ProxyAddressLookupTable->FindAddress<Direct3DSurface8>(SurfaceInterface);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::LockRect");
#endif
	return ProxyInterface->LockRect(FaceType, Level, pLockedRect, pRect, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::UnlockRect");
#endif
	return ProxyInterface->UnlockRect(FaceType, Level);
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT *pDirtyRect)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DCubeTexture8::AddDirtyRect");
#endif
	return ProxyInterface->AddDirtyRect(FaceType, pDirtyRect);
}

Direct3DVolumeTexture8::Direct3DVolumeTexture8(Direct3DDevice8 *device, IDirect3DVolumeTexture9 *ProxyInterface) :
	Device(device),
	ProxyInterface(ProxyInterface)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DVolumeTexture8::~Direct3DVolumeTexture8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::QueryInterface(REFIID riid, void **ppvObj)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::QueryInterface");
#endif
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DVolumeTexture8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8) ||
		riid == __uuidof(IDirect3DBaseTexture8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DVolumeTexture8 *>(this);

		return S_OK;
	}

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
}
ULONG STDMETHODCALLTYPE Direct3DVolumeTexture8::AddRef()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::AddRef");
#endif
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DVolumeTexture8::Release()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::Release");
#endif
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::GetDevice(IDirect3DDevice8 **ppDevice)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetDevice");
#endif
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::SetPrivateData");
#endif
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetPrivateData");
#endif
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::FreePrivateData(REFGUID refguid)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::FreePrivateData");
#endif
	return ProxyInterface->FreePrivateData(refguid);
}
DWORD STDMETHODCALLTYPE Direct3DVolumeTexture8::SetPriority(DWORD PriorityNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::SetPriority");
#endif
	return ProxyInterface->SetPriority(PriorityNew);
}
DWORD STDMETHODCALLTYPE Direct3DVolumeTexture8::GetPriority()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetPriority");
#endif
	return ProxyInterface->GetPriority();
}
void STDMETHODCALLTYPE Direct3DVolumeTexture8::PreLoad()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::PreLoad");
#endif
	ProxyInterface->PreLoad();
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DVolumeTexture8::GetType()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetType");
#endif
	return D3DRTYPE_VOLUMETEXTURE;
}

DWORD STDMETHODCALLTYPE Direct3DVolumeTexture8::SetLOD(DWORD LODNew)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::SetLOD");
#endif
	return ProxyInterface->SetLOD(LODNew);
}
DWORD STDMETHODCALLTYPE Direct3DVolumeTexture8::GetLOD()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetLOD");
#endif
	return ProxyInterface->GetLOD();
}
DWORD STDMETHODCALLTYPE Direct3DVolumeTexture8::GetLevelCount()
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetLevelCount");
#endif
	return ProxyInterface->GetLevelCount();
}

HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::GetLevelDesc(UINT Level, D3DVOLUME_DESC8 *pDesc)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetLevelDesc");
#endif
	if (pDesc == nullptr)
		return D3DERR_INVALIDCALL;

	D3DVOLUME_DESC VolumeDesc;

	const HRESULT hr = ProxyInterface->GetLevelDesc(Level, &VolumeDesc);
	if (FAILED(hr))
		return hr;

	ConvertVolumeDesc(VolumeDesc, *pDesc);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::GetVolumeLevel(UINT Level, IDirect3DVolume8 **ppVolumeLevel)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::GetVolumeLevel");
#endif
	if (ppVolumeLevel == nullptr)
		return D3DERR_INVALIDCALL;

	*ppVolumeLevel = nullptr;

	IDirect3DVolume9 *VolumeInterface = nullptr;

	const HRESULT hr = ProxyInterface->GetVolumeLevel(Level, &VolumeInterface);
	if (FAILED(hr))
		return hr;

	*ppVolumeLevel = Device->ProxyAddressLookupTable->FindAddress<Direct3DVolume8>(VolumeInterface);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::LockBox(UINT Level, D3DLOCKED_BOX *pLockedVolume, const D3DBOX *pBox, DWORD Flags)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::LockBox");
#endif
	return ProxyInterface->LockBox(Level, pLockedVolume, pBox, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::UnlockBox(UINT Level)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::UnlockBox");
#endif
	return ProxyInterface->UnlockBox(Level);
}
HRESULT STDMETHODCALLTYPE Direct3DVolumeTexture8::AddDirtyBox(const D3DBOX *pDirtyBox)
{
#ifndef D3D8TO9NOLOG
	D3D8RING("Direct3DVolumeTexture8::AddDirtyBox");
#endif
	return ProxyInterface->AddDirtyBox(pDirtyBox);
}
