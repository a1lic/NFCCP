#pragma once

#include <ntstatus.h>
#define WIN32_NO_STATUS

typedef long NTSTATUS;

#include <Windows.h>
#include <wincred.h>
#include <Sspi.h>
#include <NTSecAPI.h>
#include <NTSecPKG.h>

namespace Lsa
{
	extern ULONG_PTR package_id;
	extern LSA_SECPKG_FUNCTION_TABLE F;
	extern const SECPKG_FUNCTION_TABLE S;
	extern const SECPKG_USER_FUNCTION_TABLE U;
};

extern "C"
{
	extern NTSTATUS NTAPI SpInitialize(ULONG_PTR,SECPKG_PARAMETERS*,LSA_SECPKG_FUNCTION_TABLE*);
	extern NTSTATUS NTAPI SpLsaModeInitialize(ULONG,ULONG*,SECPKG_FUNCTION_TABLE**,ULONG*);
	extern NTSTATUS NTAPI SpUserModeInitialize(ULONG,ULONG*,SECPKG_USER_FUNCTION_TABLE**,ULONG*);
}
