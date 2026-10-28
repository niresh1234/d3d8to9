/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to9.hpp"

static const D3DFORMAT AdapterFormats[] = {
	D3DFMT_A8R8G8B8,
	D3DFMT_X8R8G8B8,
	D3DFMT_R5G6B5,
	D3DFMT_X1R5G5B5,
	D3DFMT_A1R5G5B5
};

Direct3D8::Direct3D8(IDirect3D9 *ProxyInterface) :
	ProxyInterface(ProxyInterface)
{
	D3DDISPLAYMODE pMode;

	CurrentAdapterCount = ProxyInterface->GetAdapterCount();
	if (CurrentAdapterCount > MAX_ADAPTERS)
		CurrentAdapterCount = MAX_ADAPTERS;

	for (UINT Adapter = 0; Adapter < CurrentAdapterCount; Adapter++)
	{
		for (D3DFORMAT Format : AdapterFormats)
		{
			const UINT ModeCount = ProxyInterface->GetAdapterModeCount(Adapter, Format);

			for (UINT Mode = 0; Mode < ModeCount; Mode++)
			{
				ProxyInterface->EnumAdapterModes(Adapter, Format, Mode, &pMode);
				CurrentAdapterModes[Adapter].push_back(pMode);
				CurrentAdapterModeCount[Adapter]++;
			}
		}
	}
}
Direct3D8::~Direct3D8()
{
}

HRESULT STDMETHODCALLTYPE Direct3D8::QueryInterface(REFIID riid, void **ppvObj)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::QueryInterface" << std::endl;
#endif
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3D8) ||
		riid == __uuidof(IUnknown))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3D8 *>(this);

		return S_OK;
	}

	return ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3D8::AddRef()
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::AddRef" << std::endl;
#endif
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3D8::Release()
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::Release" << std::endl;
#endif
	const ULONG LastRefCount = ProxyInterface->Release();

	if (LastRefCount == 0)
		delete this;

	return LastRefCount;
}

HRESULT STDMETHODCALLTYPE Direct3D8::RegisterSoftwareDevice(void *pInitializeFunction)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::RegisterSoftwareDevice" << std::endl;
#endif
	return ProxyInterface->RegisterSoftwareDevice(pInitializeFunction);
}
UINT STDMETHODCALLTYPE Direct3D8::GetAdapterCount()
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::GetAdapterCount" << std::endl;
#endif
	return CurrentAdapterCount;
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8 *pIdentifier)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::GetAdapterIdentifier" << std::endl;
#endif
	if (pIdentifier == nullptr)
		return D3DERR_INVALIDCALL;

	D3DADAPTER_IDENTIFIER9 AdapterIndentifier;

	if ((Flags & D3DENUM_NO_WHQL_LEVEL) == 0)
	{
		Flags |= D3DENUM_WHQL_LEVEL;
	}
	else
	{
		Flags ^= D3DENUM_NO_WHQL_LEVEL;
	}

	const HRESULT hr = ProxyInterface->GetAdapterIdentifier(Adapter, Flags, &AdapterIndentifier);
	if (FAILED(hr))
		return hr;

	ConvertAdapterIdentifier(AdapterIndentifier, *pIdentifier);

	return D3D_OK;
}
UINT STDMETHODCALLTYPE Direct3D8::GetAdapterModeCount(UINT Adapter)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::GetAdapterModeCount" << std::endl;
#endif
	return CurrentAdapterModeCount[Adapter];
}
HRESULT STDMETHODCALLTYPE Direct3D8::EnumAdapterModes(UINT Adapter, UINT Mode, D3DDISPLAYMODE *pMode)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::EnumAdapterModes" << std::endl;
#endif
	if (pMode == nullptr || !(Adapter < CurrentAdapterCount && Mode < CurrentAdapterModeCount[Adapter]))
		return D3DERR_INVALIDCALL;

	pMode->Format = CurrentAdapterModes[Adapter].at(Mode).Format;
	pMode->Height = CurrentAdapterModes[Adapter].at(Mode).Height;
	pMode->RefreshRate = CurrentAdapterModes[Adapter].at(Mode).RefreshRate;
	pMode->Width = CurrentAdapterModes[Adapter].at(Mode).Width;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::GetAdapterDisplayMode" << std::endl;
#endif
	return ProxyInterface->GetAdapterDisplayMode(Adapter, pMode);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::CheckDeviceType" << std::endl;
#endif
	return ProxyInterface->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, bWindowed);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::CheckDeviceFormat" << std::endl;
#endif
	if (CheckFormat == D3DFMT_UYVY ||
		CheckFormat == D3DFMT_YUY2 ||
		CheckFormat == MAKEFOURCC('Y', 'V', '1', '2') ||
		CheckFormat == MAKEFOURCC('N', 'V', '1', '2'))
	{
		return D3DERR_NOTAVAILABLE;
	}

	return ProxyInterface->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::CheckDeviceMultiSampleType" << std::endl;
#endif
	return ProxyInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, nullptr);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::CheckDepthStencilMatch" << std::endl;
#endif
	return ProxyInterface->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8 *pCaps)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::GetDeviceCaps" << std::endl;
#endif
	if (pCaps == nullptr)
		return D3DERR_INVALIDCALL;

	D3DCAPS9 DeviceCaps;

	const HRESULT hr = ProxyInterface->GetDeviceCaps(Adapter, DeviceType, &DeviceCaps);
	if (FAILED(hr))
		return hr;

	ConvertCaps(DeviceCaps, *pCaps);

	return D3D_OK;
}
HMONITOR STDMETHODCALLTYPE Direct3D8::GetAdapterMonitor(UINT Adapter)
{
#ifndef D3D8TO9NOLOG
	LOG << "TRACE Direct3D8::GetAdapterMonitor" << std::endl;
#endif
	return ProxyInterface->GetAdapterMonitor(Adapter);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS8 *pPresentationParameters, IDirect3DDevice8 **ppReturnedDeviceInterface)
{
#ifndef D3D8TO9NOLOG
	LOG << "Redirecting '" << "IDirect3D8::CreateDevice" << "(" << this << ", " << Adapter << ", " << DeviceType << ", " << hFocusWindow << ", " << BehaviorFlags << ", " << pPresentationParameters << ", " << ppReturnedDeviceInterface << ")' ..." << std::endl;
#endif

	if (pPresentationParameters == nullptr || ppReturnedDeviceInterface == nullptr)
		return D3DERR_INVALIDCALL;

	*ppReturnedDeviceInterface = nullptr;

	D3DPRESENT_PARAMETERS PresentParams;
	ConvertPresentParameters(*pPresentationParameters, PresentParams);
#ifndef D3D8TO9NOLOG
	LOG << "> D3D8 params: " << pPresentationParameters->BackBufferWidth << "x" << pPresentationParameters->BackBufferHeight << " backbufferFormat=" << pPresentationParameters->BackBufferFormat << " count=" << pPresentationParameters->BackBufferCount << " multisample=" << pPresentationParameters->MultiSampleType << " swapEffect=" << pPresentationParameters->SwapEffect << " windowed=" << pPresentationParameters->Windowed << " autoDepth=" << pPresentationParameters->EnableAutoDepthStencil << " depthFormat=" << pPresentationParameters->AutoDepthStencilFormat << " flags=" << pPresentationParameters->Flags << " refresh=" << pPresentationParameters->FullScreen_RefreshRateInHz << " interval=" << pPresentationParameters->FullScreen_PresentationInterval << std::endl;
	LOG << "> D3D9 params: " << PresentParams.BackBufferWidth << "x" << PresentParams.BackBufferHeight << " backbufferFormat=" << PresentParams.BackBufferFormat << " count=" << PresentParams.BackBufferCount << " multisample=" << PresentParams.MultiSampleType << " swapEffect=" << PresentParams.SwapEffect << " windowed=" << PresentParams.Windowed << " autoDepth=" << PresentParams.EnableAutoDepthStencil << " depthFormat=" << PresentParams.AutoDepthStencilFormat << " flags=" << PresentParams.Flags << " refresh=" << PresentParams.FullScreen_RefreshRateInHz << " interval=" << PresentParams.PresentationInterval << std::endl;
#endif

	IDirect3DDevice9 *DeviceInterface = nullptr;

	const HRESULT hr = ProxyInterface->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &PresentParams, &DeviceInterface);
#ifndef D3D8TO9NOLOG
	LOG << "> IDirect3D9::CreateDevice returned " << std::hex << hr << std::dec << std::endl;
#endif
	if (FAILED(hr))
		return hr;

	*ppReturnedDeviceInterface = new Direct3DDevice8(this, DeviceInterface, BehaviorFlags, PresentParams.EnableAutoDepthStencil ? PresentParams.AutoDepthStencilFormat : D3DFMT_UNKNOWN, (PresentParams.Flags & D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL) != 0);

	// Set default vertex declaration
	DeviceInterface->SetFVF(D3DFVF_XYZ);

	return D3D_OK;
}
