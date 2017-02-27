#include "Lsa.hpp"
#include "Util.hpp"

using std::wstring;

unsigned long package_id;

PLSA_CREATE_LOGON_SESSION CreateLogonSession;
PLSA_DELETE_LOGON_SESSION DeleteLogonSession;
PLSA_ADD_CREDENTIAL AddCredential;
PLSA_GET_CREDENTIALS GetCredentials;
PLSA_DELETE_CREDENTIAL DeleteCredential;
PLSA_ALLOCATE_LSA_HEAP AllocateLsaHeap;
PLSA_FREE_LSA_HEAP FreeLsaHeap;
PLSA_ALLOCATE_CLIENT_BUFFER AllocateClientBuffer;
PLSA_FREE_CLIENT_BUFFER FreeClientBuffer;
PLSA_COPY_TO_CLIENT_BUFFER CopyToClientBuffer;
PLSA_COPY_FROM_CLIENT_BUFFER CopyFromClientBuffer;

void LoadLsaString(const LSA_STRING * l, wstring & s)
{
	if(!l || !l->Buffer || (l->Length == 0))
	{
		// LSA_STRINGが指定されなかった場合、文字列をクリア
		s.clear();
		return;
	}

	// UCS-2に変換した際のサイズをチェック
	auto newsize = MultiByteToWideChar(CP_UTF7, 0, l->Buffer, l->Length, nullptr, 0);
	if(newsize <= 0)
	{
		// 変換結果が空文字列の場合は、文字列をクリア
		s.clear();
		return;
	}

	auto wstr = new wchar_t[newsize];
	MultiByteToWideChar(CP_UTF7, 0, l->Buffer, l->Length, wstr, newsize);

	s.assign(wstr);
	delete[] wstr;
}

LSA_STRING * CreateLsaString(const wchar_t * s, bool nullable)
{
	if(nullable && (!s || (wcslen(s) == 0)))
	{
		// 文字列無しでLSA_STRINGを生成しない場合はそのまま終了
		return nullptr;
	}

	auto wsize = static_cast<int>(wcsnlen(s, INT_MAX));
	// UTF-7に変換した際のサイズをチェック
	auto newsize = WideCharToMultiByte(CP_UTF7, 0, s, wsize, nullptr, 0, nullptr, nullptr);
	if(nullable && (newsize <= 1))
	{
		// 変換結果が空文字列でLSA_STRINGを生成しない場合はそのまま終了
		return nullptr;
	}

	auto name = static_cast<LSA_STRING*>((*AllocateLsaHeap)(sizeof(LSA_STRING)));
	if(!name)
	{
		return nullptr;
	}

	if(newsize <= 1)
	{
		// 変換結果が空文字列になる場合、長さ0としてLSA_STRINGを返す
		name->Length = 0;
		name->MaximumLength = 0;
		name->Buffer = nullptr;

		return name;
	}

	if(newsize > USHRT_MAX)
	{
		// LSA_STRINGは65535文字までなので、超えていた場合は切り捨てる
		newsize = USHRT_MAX;
	}

	// LSA_STRINGを生成
	name->Length = newsize - 1;
	// 最大長は一応null文字が入るようにする
	name->MaximumLength = newsize;
	name->Buffer = static_cast<char*>((*AllocateLsaHeap)(name->MaximumLength));

	if(!name->Buffer)
	{
		// 文字列を格納するバッファーを取れなかった場合はLSA_STRINGを破棄
		(*FreeLsaHeap)(name);
		return nullptr;
	}

	// UNICODE_STRINGの文字列にコピー
	WideCharToMultiByte(CP_UTF7, 0, s, wsize, name->Buffer, name->MaximumLength, nullptr, nullptr);

	return name;
}

LSA_STRING * CreateLsaString(wstring & s, bool nullable)
{
	return CreateLsaString(s.empty() ? nullptr : s.c_str(), nullable);
}

UNICODE_STRING * CreateUnicodeString(wstring & s, bool nullable)
{
	if(nullable && s.empty())
	{
		// 文字列無しでUNICODE_STRINGを生成しない場合はそのまま終了
		return nullptr;
	}

	auto name = static_cast<UNICODE_STRING*>((*AllocateLsaHeap)(sizeof(UNICODE_STRING)));
	if(!name)
	{
		return nullptr;
	}

	// UNICODE_STRINGを生成
	{
		auto wsize = s.size();
		name->Length = (wsize > USHRT_MAX) ? USHRT_MAX : static_cast<USHORT>(wsize);
	}
	name->MaximumLength = name->Length;
	name->Buffer = static_cast<wchar_t*>((*AllocateLsaHeap)(sizeof(wchar_t) * name->Length));

	if(!name->Buffer)
	{
		// 文字列を格納するバッファーを取れなかった場合はUNICODE_STRINGを破棄
		(*FreeLsaHeap)(name);
		return nullptr;
	}

	// UNICODE_STRINGの文字列にコピー
	memcpy(name->Buffer, s.c_str(), sizeof(wchar_t) * name->Length);

	return name;
}

extern "C" NTSTATUS NTAPI LsaApCallPackage(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	void *ProtocolSubmitBuffer,
	void *ClientBufferBase,
	unsigned long SubmitBufferLength,
	// returns
	void **ProtocolReturnBuffer,
	unsigned long *ReturnBufferLength,
	NTSTATUS *ProtocolStatus)
{
	DebugPrint(L"%hs", "LsaApCallPackage");
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApCallPackagePassthrough(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	unsigned long SubmitBufferLength,
	// returns
	void ** ProtocolReturnBuffer,
	unsigned long * ReturnBufferLength,
	NTSTATUS * ProtocolStatus)
{
	DebugPrint(L"%hs", "LsaApCallPackagePassthrough");
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApCallPackageUntrusted(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	unsigned long SubmitBufferLength,
	// returns
	void ** ProtocolReturnBuffer,
	unsigned long * ReturnBufferLength,
	NTSTATUS * ProtocolStatus)
{
	DebugPrint(L"%hs", "LsaApCallPackageUntrusted");
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApInitializePackage(
	// arguments
	unsigned long AuthenticationPackageId,
	LSA_DISPATCH_TABLE * LsaDispatchTable,
	LSA_STRING * Database,
	LSA_STRING * Confidentiality,
	// returns
	LSA_STRING ** AuthenticationPackageName)
{
	DebugPrint(L"%hs", "LsaApInitializePackage");

	// LSAの関数
	CreateLogonSession = LsaDispatchTable->CreateLogonSession;
	DeleteLogonSession = LsaDispatchTable->DeleteLogonSession;
	AddCredential = LsaDispatchTable->AddCredential;
	GetCredentials = LsaDispatchTable->GetCredentials;
	DeleteCredential = LsaDispatchTable->DeleteCredential;
	AllocateLsaHeap = LsaDispatchTable->AllocateLsaHeap;
	FreeLsaHeap = LsaDispatchTable->FreeLsaHeap;
	AllocateClientBuffer = LsaDispatchTable->AllocateClientBuffer;
	FreeClientBuffer = LsaDispatchTable->FreeClientBuffer;
	CopyToClientBuffer = LsaDispatchTable->CopyToClientBuffer;
	CopyFromClientBuffer = LsaDispatchTable->CopyFromClientBuffer;

	package_id = AuthenticationPackageId;
	DebugPrint(L"Authentication package ID:%u", package_id);

	// データベース名と守秘義務？は未使用のため意味なし…
	LoadLsaString(Database, *database);
	LoadLsaString(Confidentiality, *confidentiality);

	DebugPrint(L"Database:%s", database->c_str());
	DebugPrint(L"Confidentiality:%s", confidentiality->c_str());

	auto package_name = CreateLsaString(L"nfcidauth");
	if(!package_name)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	*AuthenticationPackageName = package_name;

	return STATUS_SUCCESS;
}

extern "C" void NTAPI LsaApLogonTerminated(LUID * LogonId)
{
	DebugPrint(L"%hs", "LsaApLogonTerminated");
}

/*
 * ログオン時に呼び出される処理
 * ProtocolSubmitBufferはLsaLogonUser関数のAuthenticationInformation引数で指定されるものと同じ
 *
 * LogonId、ProfileBuffer、トークン情報を返す
 * LogonIdはランダムで生成されたID(その実体は符号付き64bit整数)
 * ProfileBufferはログオン結果(ユーザー名など)を呼び出し元に伝えるもの
 * トークン情報はユーザーとグループのSIDをはじめとし、特権、DACLを指定したもの
 * Exはコンピューター名を、Ex2はさらに追加の資格情報を返す
 */

extern "C" NTSTATUS NTAPI LsaApLogonUser(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	SECURITY_LOGON_TYPE LogonType,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	unsigned long SubmitBufferSize,
	// returns
	void ** ProfileBuffer,
	unsigned long * ProfileBufferSize,
	LUID * LogonId,
	NTSTATUS * SubStatus,
	LSA_TOKEN_INFORMATION_TYPE * TokenInformationType,
	void ** TokenInformation,
	UNICODE_STRING ** AccountName,
	UNICODE_STRING ** AuthenticatingAuthority)
{
	DebugPrint(L"%hs", "LsaApLogonUser");
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApLogonUserEx(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	SECURITY_LOGON_TYPE LogonType,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	unsigned long SubmitBufferSize,
	// returns
	void ** ProfileBuffer,
	unsigned long * ProfileBufferSize,
	LUID * LogonId,
	NTSTATUS * SubStatus,
	LSA_TOKEN_INFORMATION_TYPE * TokenInformationType,
	void ** TokenInformation,
	UNICODE_STRING ** AccountName,
	UNICODE_STRING ** AuthenticatingAuthority,
	UNICODE_STRING ** MachineName)
{
	DebugPrint(L"%hs", "LsaApLogonUserEx");
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApLogonUserEx2(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	SECURITY_LOGON_TYPE LogonType,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	unsigned long SubmitBufferSize,
	// returns
	void ** ProfileBuffer,
	unsigned long * ProfileBufferSize,
	LUID * LogonId,
	NTSTATUS * SubStatus,
	LSA_TOKEN_INFORMATION_TYPE * TokenInformationType,
	void ** TokenInformation,
	UNICODE_STRING ** AccountName,
	UNICODE_STRING ** AuthenticatingAuthority,
	UNICODE_STRING ** MachineName,
	SECPKG_PRIMARY_CRED * PrimaryCredentials,
	SECPKG_SUPPLEMENTAL_CRED_ARRAY ** SupplementalCredentials)
{
	DebugPrint(L"%hs", "LsaApLogonUserEx2");
	return STATUS_SUCCESS;
}
