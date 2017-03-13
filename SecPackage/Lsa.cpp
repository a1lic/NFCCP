#include "Lsa.hpp"
#include "SSP.hpp"
#include "../Common/Util.hpp"
#include <intrin.h>

using std::wstring;

wstring * database;
wstring * confidentiality;

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

	auto wstr = new wchar_t[1 + newsize];
	wstr[newsize] = L'\0';
	MultiByteToWideChar(CP_UTF7, 0, l->Buffer, l->Length, wstr, newsize);

	s.assign(wstr);
	delete[] wstr;
}

extern "C" NTSTATUS NTAPI LsaApCallPackage(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	void *ProtocolSubmitBuffer,
	void *ClientBufferBase,
	ULONG SubmitBufferLength,
	// returns
	void **ProtocolReturnBuffer,
	ULONG *ReturnBufferLength,
	NTSTATUS *ProtocolStatus)
{
	DebugPrint(L"%hs", "LsaApCallPackage");
	return STATUS_NOT_IMPLEMENTED;
}

extern "C" NTSTATUS NTAPI LsaApCallPackagePassthrough(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	ULONG SubmitBufferLength,
	// returns
	void ** ProtocolReturnBuffer,
	ULONG * ReturnBufferLength,
	NTSTATUS * ProtocolStatus)
{
	DebugPrint(L"%hs", "LsaApCallPackagePassthrough");
	return STATUS_NOT_IMPLEMENTED;
}

extern "C" NTSTATUS NTAPI LsaApCallPackageUntrusted(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	ULONG SubmitBufferLength,
	// returns
	void ** ProtocolReturnBuffer,
	ULONG * ReturnBufferLength,
	NTSTATUS * ProtocolStatus)
{
	DebugPrint(L"%hs", "LsaApCallPackageUntrusted");
	return STATUS_NOT_IMPLEMENTED;
}

extern "C" NTSTATUS NTAPI LsaApInitializePackage(
	// arguments
	ULONG AuthenticationPackageId,
	LSA_DISPATCH_TABLE * LsaDispatchTable,
	LSA_STRING * Database,
	LSA_STRING * Confidentiality,
	// returns
	LSA_STRING ** AuthenticationPackageName)
{
	DebugPrint(L"%hs", "LsaApInitializePackage");

	DebugPrint(L"Authentication package ID:%u", AuthenticationPackageId);

	// データベース名と守秘義務？は未使用のため意味なし…
	LoadLsaString(Database, *database);
	LoadLsaString(Confidentiality, *confidentiality);

	DebugPrint(L"Database:%s", database->c_str());
	DebugPrint(L"Confidentiality:%s", confidentiality->c_str());

	auto package_name = ustring(L"nfcidauth");
	package_name.set_allocater(SpAlloc, SpFree);
	*AuthenticationPackageName = package_name.to_lsa_string();

	return STATUS_NOT_IMPLEMENTED;
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
	ULONG SubmitBufferSize,
	// returns
	void ** ProfileBuffer,
	ULONG * ProfileBufferSize,
	LUID * LogonId,
	NTSTATUS * SubStatus,
	LSA_TOKEN_INFORMATION_TYPE * TokenInformationType,
	void ** TokenInformation,
	UNICODE_STRING ** AccountName,
	UNICODE_STRING ** AuthenticatingAuthority)
{
	DebugPrint(L"%hs", "LsaApLogonUser");


	auto token_info = static_cast<LSA_TOKEN_INFORMATION_V1 *>((*Lsa::F.AllocateLsaHeap)(sizeof(LSA_TOKEN_INFORMATION_V1)));
	if(!token_info)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	*TokenInformationType = LSA_TOKEN_INFORMATION_TYPE::LsaTokenInformationV1;
	*TokenInformation = token_info;

	{
		int64u exp;
		exp.i64 = 0;
		token_info->ExpirationTime = exp.li;
	}

	// ProtocolSubmitBufferの内容に基づいてSIDを設定
	token_info->User.User.Attributes = 0;
	token_info->User.User.Sid = nullptr;

	token_info->Groups->GroupCount = 1;
	token_info->Groups->Groups[0].Attributes = 0;
	token_info->Groups->Groups[0].Sid = nullptr;

	token_info->PrimaryGroup.PrimaryGroup = nullptr;

	token_info->Privileges->PrivilegeCount = 1;
	token_info->Privileges->Privileges[0].Attributes = 0;
	token_info->Privileges->Privileges[0].Luid.LowPart = 0;
	token_info->Privileges->Privileges[0].Luid.HighPart = 0;

	token_info->Owner.Owner = nullptr;

	token_info->DefaultDacl.DefaultDacl = nullptr;

	{
		ustring account_name(L"Administrator");
		account_name.set_allocater(SpAlloc, SpFree);
		*AccountName = account_name.to_unicode_string();
	}

	*SubStatus = STATUS_SUCCESS;
	{
		int64u luid;
		luid.ui64 = __rdtsc();
		//GetSystemTimePreciseAsFileTime(&luid.ft);
		*LogonId = luid.id;
	}

	*ProfileBuffer = nullptr;
	*ProfileBufferSize = 0;

	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApLogonUserEx(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	SECURITY_LOGON_TYPE LogonType,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	ULONG SubmitBufferSize,
	// returns
	void ** ProfileBuffer,
	ULONG * ProfileBufferSize,
	LUID * LogonId,
	NTSTATUS * SubStatus,
	LSA_TOKEN_INFORMATION_TYPE * TokenInformationType,
	void ** TokenInformation,
	UNICODE_STRING ** AccountName,
	UNICODE_STRING ** AuthenticatingAuthority,
	UNICODE_STRING ** MachineName)
{
	DebugPrint(L"%hs", "LsaApLogonUserEx");
	return STATUS_NOT_IMPLEMENTED;
}

extern "C" NTSTATUS NTAPI LsaApLogonUserEx2(
	// arguments
	PLSA_CLIENT_REQUEST ClientRequest,
	SECURITY_LOGON_TYPE LogonType,
	void * ProtocolSubmitBuffer,
	void * ClientBufferBase,
	ULONG SubmitBufferSize,
	// returns
	void ** ProfileBuffer,
	ULONG * ProfileBufferSize,
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
	return STATUS_NOT_IMPLEMENTED;
}
