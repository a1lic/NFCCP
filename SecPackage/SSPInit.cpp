#define SSP_CPP
#include "SSP.hpp"
#include "Lsa.hpp"
#include "../Common/Util.hpp"

namespace Lsa
{
	ULONG_PTR package_id;
	LSA_SECPKG_FUNCTION_TABLE F;
	const SECPKG_FUNCTION_TABLE S = {
		/* Version 1 */
		/* InitializePackage                   */ LsaApInitializePackage,
		/* LogonUser                           */ LsaApLogonUser,
		/* CallPackage                         */ LsaApCallPackage,
		/* LogonTerminated                     */ LsaApLogonTerminated,
		/* CallPackageUntrusted                */ LsaApCallPackageUntrusted,
		/* CallPackagePassthrough              */ LsaApCallPackagePassthrough,
		/* LogonUserEx                         */ nullptr,
		/* LogonUserEx2                        */ nullptr,
		/* Initialize                          */ nullptr,
		/* Shutdown                            */ nullptr,
		/* GetInfo                             */ nullptr,
		/* AcceptCredentials                   */ nullptr,
		/* AcquireCredentialsHandle            */ nullptr,
		/* QueryCredentialsAttributes          */ nullptr,
		/* FreeCredentialsHandle               */ nullptr,
		/* SaveCredentials                     */ nullptr,
		/* GetCredentials                      */ nullptr,
		/* DeleteCredentials                   */ nullptr,
		/* InitLsaModeContext                  */ nullptr,
		/* AcceptLsaModeContext                */ nullptr,
		/* DeleteContext                       */ nullptr,
		/* ApplyControlToken                   */ nullptr,
		/* GetUserInfo                         */ nullptr,
		/* GetExtendedInformation              */ nullptr,
		/* QueryContextAttributes              */ nullptr,
		/* AddCredentials                      */ nullptr,
		/* SetExtendedInformation              */ nullptr,
		/* Version 2 */
		/* SetContextAttributes                */ nullptr,
		/* Version 3 */
		/* SetCredentialsAttributes            */ nullptr,
		/* Version 4 */
		/* ChangeAccountPassword               */ nullptr,
		/* Version 5 */
		/* QueryMetaData                       */ nullptr,
		/* ExchangeMetaData                    */ nullptr,
		/* GetCredUIContext                    */ nullptr,
		/* UpdateCredentials                   */ nullptr,
		/* Version 6 */
		/* ValidateTargetInfo                  */ nullptr,
		/* Version 7 */
		/* PostLogonUser                       */ nullptr,
		/* Version 8 */
		/* GetRemoteCredGuardLogonBuffer       */ nullptr,
		/* GetRemoteCredGuardSupplementalCreds */ nullptr
	};
	const SECPKG_USER_FUNCTION_TABLE U = {
		/* Version 1 */
		/* InstanceInit              */ nullptr,
		/* InitUserModeContext       */ nullptr,
		/* MakeSignature             */ nullptr,
		/* VerifySignature           */ nullptr,
		/* SealMessage               */ nullptr,
		/* UnsealMessage             */ nullptr,
		/* GetContextToken           */ nullptr,
		/* QueryContextAttributes    */ nullptr,
		/* CompleteAuthToken         */ nullptr,
		/* DeleteUserModeContext     */ nullptr,
		/* FormatCredentials         */ nullptr,
		/* MarshallSupplementalCreds */ nullptr,
		/* ExportContext             */ nullptr,
		/* ImportContext             */ nullptr
	};
};

extern "C" NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, SECPKG_PARAMETERS * Parameters, LSA_SECPKG_FUNCTION_TABLE * FunctionTable)
{
	DebugPrint(L"Function %hs", "SpInitialize");
	DebugPrint(L"Package ID=%Iu", PackageId);

	Lsa::package_id = PackageId;
	Lsa::F = *FunctionTable;

	DebugPrint(L"%32hs=0x%p", "CreateLogonSession", Lsa::F.CreateLogonSession);
	DebugPrint(L"%32hs=0x%p", "DeleteLogonSession", Lsa::F.DeleteLogonSession);
	DebugPrint(L"%32hs=0x%p", "AddCredential", Lsa::F.AddCredential);
	DebugPrint(L"%32hs=0x%p", "GetCredentials", Lsa::F.GetCredentials);
	DebugPrint(L"%32hs=0x%p", "DeleteCredential", Lsa::F.DeleteCredential);
	DebugPrint(L"%32hs=0x%p", "AllocateLsaHeap", Lsa::F.AllocateLsaHeap);
	DebugPrint(L"%32hs=0x%p", "FreeLsaHeap", Lsa::F.FreeLsaHeap);
	DebugPrint(L"%32hs=0x%p", "AllocateClientBuffer", Lsa::F.AllocateClientBuffer);
	DebugPrint(L"%32hs=0x%p", "FreeClientBuffer", Lsa::F.FreeClientBuffer);
	DebugPrint(L"%32hs=0x%p", "CopyFromClientBuffer", Lsa::F.CopyFromClientBuffer);
	DebugPrint(L"%32hs=0x%p", "CopyFromClientBuffer", Lsa::F.CopyFromClientBuffer);
	DebugPrint(L"%32hs=0x%p", "ImpersonateClient", Lsa::F.ImpersonateClient);
	DebugPrint(L"%32hs=0x%p", "UnloadPackage", Lsa::F.UnloadPackage);
	DebugPrint(L"%32hs=0x%p", "DuplicateHandle", Lsa::F.DuplicateHandle);
	DebugPrint(L"%32hs=0x%p", "SaveSupplementalCredentials", Lsa::F.SaveSupplementalCredentials);
	DebugPrint(L"%32hs=0x%p", "CreateThread", Lsa::F.CreateThread);
	DebugPrint(L"%32hs=0x%p", "GetClientInfo", Lsa::F.GetClientInfo);
	DebugPrint(L"%32hs=0x%p", "RegisterNotification", Lsa::F.RegisterNotification);
	DebugPrint(L"%32hs=0x%p", "CancelNotification", Lsa::F.CancelNotification);
	DebugPrint(L"%32hs=0x%p", "MapBuffer", Lsa::F.MapBuffer);
	DebugPrint(L"%32hs=0x%p", "CreateToken", Lsa::F.CreateToken);
	DebugPrint(L"%32hs=0x%p", "AuditLogon", Lsa::F.AuditLogon);
	DebugPrint(L"%32hs=0x%p", "CallPackage", Lsa::F.CallPackage);
	DebugPrint(L"%32hs=0x%p", "FreeReturnBuffer", Lsa::F.FreeReturnBuffer);
	DebugPrint(L"%32hs=0x%p", "GetCallInfo", Lsa::F.GetCallInfo);
	DebugPrint(L"%32hs=0x%p", "CallPackageEx", Lsa::F.CallPackageEx);
	DebugPrint(L"%32hs=0x%p", "CreateSharedMemory", Lsa::F.CreateSharedMemory);
	DebugPrint(L"%32hs=0x%p", "AllocateSharedMemory", Lsa::F.AllocateSharedMemory);
	DebugPrint(L"%32hs=0x%p", "FreeSharedMemory", Lsa::F.FreeSharedMemory);
	DebugPrint(L"%32hs=0x%p", "DeleteSharedMemory", Lsa::F.DeleteSharedMemory);
	DebugPrint(L"%32hs=0x%p", "OpenSamUser", Lsa::F.OpenSamUser);
	DebugPrint(L"%32hs=0x%p", "GetUserCredentials", Lsa::F.GetUserCredentials);
	DebugPrint(L"%32hs=0x%p", "GetUserAuthData", Lsa::F.GetUserAuthData);
	DebugPrint(L"%32hs=0x%p", "CloseSamUser", Lsa::F.CloseSamUser);
	DebugPrint(L"%32hs=0x%p", "ConvertAuthDataToToken", Lsa::F.ConvertAuthDataToToken);
	DebugPrint(L"%32hs=0x%p", "ClientCallback", Lsa::F.ClientCallback);
	DebugPrint(L"%32hs=0x%p", "UpdateCredentials", Lsa::F.UpdateCredentials);
	DebugPrint(L"%32hs=0x%p", "GetAuthDataForUser", Lsa::F.GetAuthDataForUser);
	DebugPrint(L"%32hs=0x%p", "CrackSingleName", Lsa::F.CrackSingleName);
	DebugPrint(L"%32hs=0x%p", "AuditAccountLogon", Lsa::F.AuditAccountLogon);
	DebugPrint(L"%32hs=0x%p", "CallPackagePassthrough", Lsa::F.CallPackagePassthrough);
	DebugPrint(L"%32hs=0x%p", "CrediRead", Lsa::F.CrediRead);
	DebugPrint(L"%32hs=0x%p", "CrediReadDomainCredentials", Lsa::F.CrediReadDomainCredentials);
	DebugPrint(L"%32hs=0x%p", "CrediFreeCredentials", Lsa::F.CrediFreeCredentials);
	DebugPrint(L"%32hs=0x%p", "LsaProtectMemory", Lsa::F.LsaProtectMemory);
	DebugPrint(L"%32hs=0x%p", "LsaUnprotectMemory", Lsa::F.LsaUnprotectMemory);
	DebugPrint(L"%32hs=0x%p", "OpenTokenByLogonId", Lsa::F.OpenTokenByLogonId);
	DebugPrint(L"%32hs=0x%p", "ExpandAuthDataForDomain", Lsa::F.ExpandAuthDataForDomain);
	DebugPrint(L"%32hs=0x%p", "AllocatePrivateHeap", Lsa::F.AllocatePrivateHeap);
	DebugPrint(L"%32hs=0x%p", "FreePrivateHeap", Lsa::F.FreePrivateHeap);
	DebugPrint(L"%32hs=0x%p", "CreateTokenEx", Lsa::F.CreateTokenEx);
	DebugPrint(L"%32hs=0x%p", "CrediWrite", Lsa::F.CrediWrite);
	DebugPrint(L"%32hs=0x%p", "CrediUnmarshalandDecodeString", Lsa::F.CrediUnmarshalandDecodeString);
	DebugPrint(L"%32hs=0x%p", "DummyFunction6", Lsa::F.DummyFunction6);
	DebugPrint(L"%32hs=0x%p", "GetExtendedCallFlags", Lsa::F.GetExtendedCallFlags);
	DebugPrint(L"%32hs=0x%p", "DuplicateTokenHandle", Lsa::F.DuplicateTokenHandle);
	DebugPrint(L"%32hs=0x%p", "GetServiceAccountPassword", Lsa::F.GetServiceAccountPassword);
	DebugPrint(L"%32hs=0x%p", "DummyFunction7", Lsa::F.DummyFunction7);
	DebugPrint(L"%32hs=0x%p", "AuditLogonEx", Lsa::F.AuditLogonEx);
	DebugPrint(L"%32hs=0x%p", "CheckProtectedUserByToken", Lsa::F.CheckProtectedUserByToken);
	DebugPrint(L"%32hs=0x%p", "QueryClientRequest", Lsa::F.QueryClientRequest);
	DebugPrint(L"%32hs=0x%p", "GetAppModeInfo", Lsa::F.GetAppModeInfo);
	DebugPrint(L"%32hs=0x%p", "SetAppModeInfo", Lsa::F.SetAppModeInfo);

	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI SpLsaModeInitialize(ULONG LsaVersion, ULONG * PackageVersion, SECPKG_FUNCTION_TABLE ** ppTables, ULONG * pcTables)
{
	DebugPrint(L"Function %hs", "SpLsaModeInitialize");
	DebugPrint(L"LSA version=%ul", LsaVersion);

	if(LsaVersion < 1)
	{
		DebugPrint(L"LSA version is too old");
		return STATUS_INVALID_PARAMETER_1;
	}

	*PackageVersion = SECPKG_INTERFACE_VERSION;
	*ppTables = const_cast<SECPKG_FUNCTION_TABLE*>(&Lsa::S);
	*pcTables = 1;

	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI SpUserModeInitialize(ULONG LsaVersion, ULONG * PackageVersion, SECPKG_USER_FUNCTION_TABLE ** ppTables, ULONG * pcTables)
{
	DebugPrint(L"Function %hs", "SpUserModeInitialize");
	DebugPrint(L"LSA version=%ul", LsaVersion);

	*PackageVersion = SECPKG_INTERFACE_VERSION;
	*ppTables = const_cast<SECPKG_USER_FUNCTION_TABLE*>(&Lsa::U);
	*pcTables = 1;

	return STATUS_SUCCESS;
}
