#pragma once

#include <ntstatus.h>
#define WIN32_NO_STATUS

typedef long NTSTATUS;

#include <Windows.h>
#include <wincred.h>
#include <Sspi.h>
#include <NTSecAPI.h>
#include <NTSecPKG.h>

extern "C"
{
	extern NTSTATUS NTAPI SpInitialize(ULONG_PTR,SECPKG_PARAMETERS*,LSA_SECPKG_FUNCTION_TABLE*);
	extern NTSTATUS NTAPI SpInstanceInit(ULONG,SECPKG_DLL_FUNCTIONS*,void**);
	extern NTSTATUS NTAPI SpLsaModeInitialize(ULONG,ULONG*,SECPKG_FUNCTION_TABLE**,ULONG*);
	extern NTSTATUS NTAPI SpUserModeInitialize(ULONG,ULONG*,SECPKG_USER_FUNCTION_TABLE**,ULONG*);
}
