#pragma once

#include <ntstatus.h>
#define WIN32_NO_STATUS

typedef long NTSTATUS;

#include <Windows.h>
#include <wincred.h>
#include <Sspi.h>
#include <NTSecAPI.h>
#include <NTSecPKG.h>

extern "C" NTSTATUS NTAPI SpInitialize(ULONG_PTR, SECPKG_PARAMETERS *, LSA_SECPKG_FUNCTION_TABLE *);
