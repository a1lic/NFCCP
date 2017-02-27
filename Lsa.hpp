#pragma once

typedef long NTSTATUS, *PNTSTATUS;

#include <ntstatus.h>

#define WIN32_NO_STATUS
#define SECURITY_WIN32

#include <Windows.h>
#include <LsaLookup.h>
#include <NTSecAPI.h>
#include <sspi.h>
#include <NTSecPKG.h>
#include <string>

using std::wstring;

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
extern LSA_STRING * CreateLsaString(const wchar_t *, bool = false);
extern LSA_STRING * CreateLsaString(wstring &, bool = false);
extern UNICODE_STRING * CreateUnicodeString(wstring &, bool = false);

union LocalUniqId
{
	LUID Luid;
	__int64 RawId;
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
	wstring AccountName;
	wstring Authority;
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
		return CreateUnicodeString(this->AccountName);
	}
	inline UNICODE_STRING * GetAuthority()
	{
		return CreateUnicodeString(this->Authority, true);
	}
};


class LsaSession
{
	LocalUniqId LogonId;
public:
	inline __int64 GetLogonID()
	{
		return this->LogonId.RawId;
	}
	NTSTATUS CreateLogonSession();
	NTSTATUS DeleteLogonSession();
	NTSTATUS AddCredential(ULONG AuthenticationPackage, const LSA_STRING * PrimaryKeyValue, const LSA_STRING * Credentials);
	NTSTATUS GetCredentials(ULONG AuthenticationPackage, ULONG * QueryContext, BOOLEAN RetriveAllCredentials, const LSA_STRING * PrimaryKeyValue, ULONG * PrimaryKeyLength, const LSA_STRING * Credentials);
	NTSTATUS DeleteCredential(ULONG AuthenticationPackage, const LSA_STRING * PrimaryKeyValue);
};

class Lsa
{
	static LSA_DISPATCH_TABLE Dispatch;
public:
	void * AllocateLsaHeap(ULONG Length);
	void FreeLsaHeap(void * Base);
	void * AllocateClientBuffer(PLSA_CLIENT_REQUEST ClientRequest, ULONG LengthRequired, void ** ClientBaseAddress);
	void FreeClientBuffer(PLSA_CLIENT_REQUEST ClientRequest, void * ClientBaseAddress);
	NTSTATUS CopyToClientBuffer(PLSA_CLIENT_REQUEST ClientRequest, ULONG Length, void * ClientBaseAddress, void * BufferToCopy);
	NTSTATUS CopyFromClientBuffer(PLSA_CLIENT_REQUEST CkientRequest, ULONG Length, void * BufferToCopy, void * ClientBaseAddress);
};
