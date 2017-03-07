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
#include "StringClass.hpp"

using std::wstring;

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

#if false
enum Dispatch : unsigned char
{
	CreateLogonSession, DeleteLogonSession,
	AddCredential, GetCredentials, DeleteCredential,
	AllocateLsaHeap, FreeLsaHeap,
	AllocateClientBuffer, FreeClientBuffer, CopyToClientBuffer, CopyFromClientBuffer,
	Dispatch_Entries
};
#endif

extern void LoadLsaString(const LSA_STRING *, wstring &);

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

class LsaLogon
{
	PLSA_CLIENT_REQUEST Request;
	SECURITY_LOGON_TYPE Type;
	void * Buffer;
	unsigned long Size;
	void * LogonResult;
	LocalUniqId Id;
	NTSTATUS Status;
	LSA_TOKEN_INFORMATION_TYPE TokenType;
	void * TokenBuffer;
	ustring AccountName;
	ustring Authority;
public:
	inline void SetRequest(PLSA_CLIENT_REQUEST v)
	{
		this->Request = v;
	}
	inline void SetLogonType(SECURITY_LOGON_TYPE v)
	{
		this->Type = v;
	}
	inline void AssignBuffer(void * b, unsigned long s)
	{
		this->Buffer = b;
		this->Size = s;
	}
	void Logon();
	inline void * GetLogonResult()
	{
		return this->LogonResult;
	}
	inline __int64 GetLogonId()
	{
		return this->Id.RawId;
	}
	inline NTSTATUS GetLogonStatus()
	{
		return this->Status;
	}
	inline LSA_TOKEN_INFORMATION_TYPE GetTokenType()
	{
		return this->TokenType;
	}
	inline void * GetToken()
	{
		return this->TokenBuffer;
	}
	inline UNICODE_STRING * GetAccountName()
	{
		return this->AccountName;
	}
	inline UNICODE_STRING * GetAuthority()
	{
		return this->Authority;
	}
};
