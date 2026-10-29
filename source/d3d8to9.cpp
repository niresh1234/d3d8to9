/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3dx9.hpp"
#include "d3d8to9.hpp"
#include <cstdarg>
#include <cstdint>

PFN_D3DXAssembleShader D3DXAssembleShader = nullptr;
PFN_D3DXDisassembleShader D3DXDisassembleShader = nullptr;
PFN_D3DXLoadSurfaceFromSurface D3DXLoadSurfaceFromSurface = nullptr;

#ifndef D3D8TO9NOLOG
 // Very simple logging for the purpose of debugging only.
std::ofstream LOG;
#endif

#ifndef D3D8TO9NOLOG
const char *D3D8Ring[8192] = {};
volatile LONG D3D8RingPos = 0;

static void CrashEmit(HANDLE file, const char *format, ...)
{
	char text[512];
	va_list args;
	va_start(args, format);
	const int length = wvsprintfA(text, format, args);
	va_end(args);
	DWORD written = 0;
	WriteFile(file, text, static_cast<DWORD>(length), &written, nullptr);
}

static bool CrashRead(const void *address, void *buffer, SIZE_T size)
{
	SIZE_T got = 0;
	return ReadProcessMemory(GetCurrentProcess(), address, buffer, size, &got) && got == size;
}

static void CrashDumpPointer(HANDLE file, const char *name, DWORD value)
{
	DWORD block[16];

	if (value >= 0x10000 && CrashRead(reinterpret_cast<const void *>(static_cast<uintptr_t>(value)), block, sizeof(block)))
	{
		CrashEmit(file, "%s=%08X -> %08X %08X %08X %08X %08X %08X %08X %08X\r\n", name, value, block[0], block[1], block[2], block[3], block[4], block[5], block[6], block[7]);
		CrashEmit(file, "                 %08X %08X %08X %08X %08X %08X %08X %08X\r\n", block[8], block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
	}
}

// Writes a report for every access violation. The file is overwritten each time, so it ends up holding the last (normally fatal) one.
static LONG WINAPI D3D8CrashHandler(PEXCEPTION_POINTERS info)
{
	const PEXCEPTION_RECORD record = info->ExceptionRecord;

	if (record->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	const HANDLE file = CreateFileA("d3d8_crash.log", GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (file == INVALID_HANDLE_VALUE)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	const DWORD moduleBase = static_cast<DWORD>(reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)));
	const DWORD address = static_cast<DWORD>(reinterpret_cast<uintptr_t>(record->ExceptionAddress));
	const DWORD faultAddress = static_cast<DWORD>(record->ExceptionInformation[1]);

	CrashEmit(file, "Access violation at %08X (exe base %08X, exe offset %08X), thread %u\r\n", address, moduleBase, address - moduleBase, GetCurrentThreadId());
	CrashEmit(file, "Operation %u (0=read 1=write 8=execute) on address %08X\r\n", static_cast<DWORD>(record->ExceptionInformation[0]), faultAddress);

#ifdef _M_IX86
	const PCONTEXT context = info->ContextRecord;

	CrashEmit(file, "EAX=%08X EBX=%08X ECX=%08X EDX=%08X\r\n", context->Eax, context->Ebx, context->Ecx, context->Edx);
	CrashEmit(file, "ESI=%08X EDI=%08X EBP=%08X ESP=%08X\r\n", context->Esi, context->Edi, context->Ebp, context->Esp);

	// Memory just before the faulting address: shows what the code was walking through right before it ran off
	{
		DWORD block[64];
		const uintptr_t start = (static_cast<uintptr_t>(faultAddress) & ~static_cast<uintptr_t>(3)) - sizeof(block);

		if (CrashRead(reinterpret_cast<const void *>(start), block, sizeof(block)))
		{
			CrashEmit(file, "\r\n64 dwords of memory just before the faulting address:\r\n");

			for (int i = 0; i < 64; i += 8)
			{
				CrashEmit(file, "%08X: %08X %08X %08X %08X %08X %08X %08X %08X\r\n", static_cast<DWORD>(start) + i * 4, block[i], block[i + 1], block[i + 2], block[i + 3], block[i + 4], block[i + 5], block[i + 6], block[i + 7]);
			}
		}
		else
		{
			CrashEmit(file, "\r\n(memory before the faulting address is not readable)\r\n");
		}
	}

	// Memory that the registers point to
	CrashEmit(file, "\r\nRegisters that point to readable memory:\r\n");
	CrashDumpPointer(file, "EAX", context->Eax);
	CrashDumpPointer(file, "EBX", context->Ebx);
	CrashDumpPointer(file, "ECX", context->Ecx);
	CrashDumpPointer(file, "EDX", context->Edx);
	CrashDumpPointer(file, "ESI", context->Esi);
	CrashDumpPointer(file, "EDI", context->Edi);
	CrashDumpPointer(file, "EBP", context->Ebp);

	// Values on the stack that look like return addresses into the game's code
	{
		DWORD stack[512];
		const DWORD low = moduleBase + 0x1000;
		const DWORD high = moduleBase + 0x385000;

		for (SIZE_T size = sizeof(stack); size >= 64; size /= 2)
		{
			if (CrashRead(reinterpret_cast<const void *>(static_cast<uintptr_t>(context->Esp)), stack, size))
			{
				CrashEmit(file, "\r\nStack values that point into the game's code (candidate return addresses):\r\n");

				int printed = 0;

				for (SIZE_T i = 0; i < size / sizeof(DWORD) && printed < 60; ++i)
				{
					if (stack[i] >= low && stack[i] < high)
					{
						CrashEmit(file, "esp+%03X: %08X (exe offset %08X)\r\n", static_cast<DWORD>(i * sizeof(DWORD)), stack[i], stack[i] - moduleBase);
						++printed;
					}
				}

				break;
			}
		}
	}
#endif

	const LONG position = D3D8RingPos;

	CrashEmit(file, "\r\nLast Direct3D calls (%d so far), oldest first:\r\n", position);

	for (LONG sequence = (position > 400 ? position - 399 : 1); sequence <= position; ++sequence)
	{
		const char *const name = D3D8Ring[static_cast<unsigned int>(sequence) & 8191u];
		CrashEmit(file, "%s\r\n", name != nullptr ? name : "?");
	}

	CloseHandle(file);

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

extern "C" HRESULT WINAPI ValidatePixelShader(const DWORD* pPixelShader, const D3DCAPS8* pCaps, BOOL ReturnErrors, char** pErrorsString)
{
#ifndef D3D8TO9NOLOG
	LOG << "Redirecting '" << "ValidatePixelShader " << "(" << pPixelShader << ", " << pCaps << ", " << ReturnErrors << ", " << pErrorsString << ")' ..." << std::endl;
#endif

	HRESULT hr = E_FAIL;
	const char* errorMessage = "";

	if (!pPixelShader)
	{
		errorMessage = "Invalid code pointer.\n";
	}
	else
	{
		switch (*pPixelShader)
		{
		case D3DPS_VERSION(1, 0):
		case D3DPS_VERSION(1, 1):
		case D3DPS_VERSION(1, 2):
		case D3DPS_VERSION(1, 3):
		case D3DPS_VERSION(1, 4):
			if (pCaps && *pPixelShader > pCaps->PixelShaderVersion)
			{
				errorMessage = "Shader version not supported by caps.\n";
				break;
			}
			hr = S_OK;
			break;

		default:
			errorMessage = "Unsupported shader version.\n";
		}
	}

	if (!ReturnErrors)
	{
		errorMessage = "";
	}

	if (pErrorsString)
	{
		const size_t size = strlen(errorMessage) + 1;

		*pErrorsString = (char*) HeapAlloc(GetProcessHeap(), 0, size);
		if (*pErrorsString)
		{
			memcpy(*pErrorsString, errorMessage, size);
		}
	}

	return hr;
}

extern "C" HRESULT WINAPI ValidateVertexShader(const DWORD* pVertexShader, const DWORD* pVertexDecl, const D3DCAPS8* pCaps, BOOL ReturnErrors, char** pErrorsString)
{
	UNREFERENCED_PARAMETER(pVertexDecl);

#ifndef D3D8TO9NOLOG
	LOG << "Redirecting '" << "ValidateVertexShader " << "(" << pVertexShader << ", " << pVertexDecl << ", " << pCaps << ", " << ReturnErrors << ", " << pErrorsString << ")' ..." << std::endl;
#endif

	HRESULT hr = E_FAIL;
	const char* errorMessage = "";

	if (!pVertexShader)
	{
		errorMessage = "Invalid code pointer.\n";
	}
	else
	{
		switch (*pVertexShader)
		{
		case D3DVS_VERSION(1, 0):
		case D3DVS_VERSION(1, 1):
			if (pCaps && *pVertexShader > pCaps->VertexShaderVersion)
			{
				errorMessage = "Shader version not supported by caps.\n";
				break;
			}
			hr = S_OK;
			break;

		default:
			errorMessage = "Unsupported shader version.\n";
		}
	}

	if (!ReturnErrors)
	{
		errorMessage = "";
	}

	if (pErrorsString)
	{
		const size_t size = strlen(errorMessage) + 1;

		*pErrorsString = (char*) HeapAlloc(GetProcessHeap(), 0, size);
		if (*pErrorsString)
		{
			memcpy(*pErrorsString, errorMessage, size);
		}
	}

	return hr;
}

extern "C" void WINAPI DebugSetMute()
{
#ifndef D3D8TO9NOLOG
	LOG << "Redirecting '" << "DebugSetMute" << "(" << ")' ..." << std::endl;
#endif
}

extern "C" IDirect3D8 *WINAPI Direct3DCreate8(UINT SDKVersion)
{
#ifndef D3D8TO9NOLOG
	static bool LogMessageFlag = true;

	if (!LOG.is_open())
	{
		LOG.open("d3d8.log", std::ios::trunc);
	}

	if (!LOG.is_open() && LogMessageFlag)
	{
		LogMessageFlag = false;
		MessageBox(nullptr, TEXT("Failed to open debug log file \"d3d8.log\"!"), nullptr, MB_ICONWARNING);
	}

	LOG << "Redirecting '" << "Direct3DCreate8" << "(" << SDKVersion << ")' ..." << std::endl;
	LOG << "> Passing on to 'Direct3DCreate9':" << std::endl;

	static bool CrashHandlerInstalled = false;
	if (!CrashHandlerInstalled)
	{
		CrashHandlerInstalled = true;
		AddVectoredExceptionHandler(1, D3D8CrashHandler);
	}
#endif

	IDirect3D9 *const d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d == nullptr)
	{
		return nullptr;
	}

	// Load D3DX
	if (!D3DXAssembleShader || !D3DXDisassembleShader || !D3DXLoadSurfaceFromSurface)
	{
		const HMODULE module = LoadLibrary(TEXT("d3dx9_43.dll"));

		if (module != nullptr)
		{
			D3DXAssembleShader = reinterpret_cast<PFN_D3DXAssembleShader>(GetProcAddress(module, "D3DXAssembleShader"));
			D3DXDisassembleShader = reinterpret_cast<PFN_D3DXDisassembleShader>(GetProcAddress(module, "D3DXDisassembleShader"));
			D3DXLoadSurfaceFromSurface = reinterpret_cast<PFN_D3DXLoadSurfaceFromSurface>(GetProcAddress(module, "D3DXLoadSurfaceFromSurface"));
		}
		else
		{
#ifndef D3D8TO9NOLOG
			LOG << "Failed to load d3dx9_43.dll! Some features will not work correctly." << std::endl;
#endif
			if (MessageBox(nullptr, TEXT(
					"Failed to load d3dx9_43.dll! Some features will not work correctly.\n\n"
					"It's required to install the \"Microsoft DirectX End-User Runtime\" in order to use d3d8to9, or alternatively get the DLLs from this NuGet package:\nhttps://www.nuget.org/packages/Microsoft.DXSDK.D3DX\n\n"
					"Please click \"OK\" to open the official download page or \"Cancel\" to continue anyway."), nullptr, MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND | MB_OKCANCEL | MB_DEFBUTTON1) == IDOK)
			{
				ShellExecute(nullptr, TEXT("open"), TEXT("https://www.microsoft.com/download/details.aspx?id=35"), nullptr, nullptr, SW_SHOW);

				return nullptr;
			}
		}
	}

	return new Direct3D8(d3d);
}
