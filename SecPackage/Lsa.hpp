#pragma once

typedef long NTSTATUS, *PNTSTATUS;

#include <ntstatus.h>
#define WIN32_NO_STATUS

#include <Windows.h>
#include <LsaLookup.h>
#include <NTSecAPI.h>
#include <sspi.h>
#include <NTSecPKG.h>
#include <string>
#include "../Common/StringClass.hpp"

using std::wstring;

union LocalUniqId
{
	LUID Luid;
	__int64 RawId;
};

union int64u
{
	int64_t i64;
	uint64_t ui64;
	LARGE_INTEGER li;
	LUID id;
	FILETIME ft;
};

namespace Lsa
{
	extern PLSA_CREATE_LOGON_SESSION CreateLogonSession;
	extern PLSA_DELETE_LOGON_SESSION DeleteLogonSession;
	extern PLSA_ADD_CREDENTIAL AddCredential;
	extern PLSA_GET_CREDENTIALS GetCredentials;
	extern PLSA_DELETE_CREDENTIAL DeleteCredential;
	extern PLSA_ALLOCATE_LSA_HEAP AllocateLsaHeap;
	extern PLSA_FREE_LSA_HEAP FreeLsaHeap;
	extern PLSA_ALLOCATE_CLIENT_BUFFER AllocateClientBuffer;
	extern PLSA_FREE_CLIENT_BUFFER FreeClientBuffer;
	extern PLSA_COPY_TO_CLIENT_BUFFER CopyToClientBuffer;
	extern PLSA_COPY_FROM_CLIENT_BUFFER CopyFromClientBuffer;
};

extern unsigned long package_id;
extern wstring * database;
extern wstring * confidentiality;

extern void LoadLsaString(const LSA_STRING *, wstring &);

// LSA Authentication Package Functions Begin
extern "C"
{
	extern NTSTATUS NTAPI LsaApInitializePackage(ULONG, LSA_DISPATCH_TABLE*, LSA_STRING*, LSA_STRING*, LSA_STRING**);

	extern NTSTATUS NTAPI            LsaApCallPackage(PLSA_CLIENT_REQUEST, void*, void*, ULONG, void**, ULONG*, NTSTATUS*);
	extern NTSTATUS NTAPI LsaApCallPackagePassthrough(PLSA_CLIENT_REQUEST, void*, void*, ULONG, void**, ULONG*, NTSTATUS*);
	extern NTSTATUS NTAPI   LsaApCallPackageUntrusted(PLSA_CLIENT_REQUEST, void*, void*, ULONG, void**, ULONG*, NTSTATUS*);

	extern NTSTATUS NTAPI    LsaApLogonUser(PLSA_CLIENT_REQUEST, SECURITY_LOGON_TYPE, void *, void *, ULONG, void**, ULONG*, LUID*, NTSTATUS*, LSA_TOKEN_INFORMATION_TYPE*, void**, UNICODE_STRING**, UNICODE_STRING**);
	extern NTSTATUS NTAPI  LsaApLogonUserEx(PLSA_CLIENT_REQUEST, SECURITY_LOGON_TYPE, void *, void *, ULONG, void**, ULONG*, LUID*, NTSTATUS*, LSA_TOKEN_INFORMATION_TYPE*, void**, UNICODE_STRING**, UNICODE_STRING**, UNICODE_STRING**);
	extern NTSTATUS NTAPI LsaApLogonUserEx2(PLSA_CLIENT_REQUEST, SECURITY_LOGON_TYPE, void *, void *, ULONG, void**, ULONG*, LUID*, NTSTATUS*, LSA_TOKEN_INFORMATION_TYPE*, void**, UNICODE_STRING**, UNICODE_STRING**, UNICODE_STRING**, SECPKG_PRIMARY_CRED*, SECPKG_SUPPLEMENTAL_CRED_ARRAY**);

	extern     void NTAPI   LsaApLogonTerminated(LUID*);
}
// LSA Authentication Package Functions End
