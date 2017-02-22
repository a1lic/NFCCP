typedef long NTSTATUS, *PNTSTATUS;

#include <ntstatus.h>

#define WIN32_NO_STATUS
#define SECURITY_WIN32

#include <Windows.h>
#include <LsaLookup.h>
#include <NTSecAPI.h>
#include <sspi.h>
#include <NTSecPKG.h>

extern "C" NTSTATUS NTAPI LsaApCallPackage(
	PLSA_CLIENT_REQUEST ClientRequest,
	void *ProtocolSubmitBuffer,
	void *ClientBufferBase,
	unsigned long SubmitBufferLength,
	void **ProtocolReturnBuffer,
	unsigned long *ReturnBufferLength,
	NTSTATUS *ProtocolStatus)
{
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApCallPackagePassthrough(
	PLSA_CLIENT_REQUEST ClientRequest,
	void *ProtocolSubmitBuffer,
	void *ClientBufferBase,
	unsigned long SubmitBufferLength,
	void **ProtocolReturnBuffer,
	unsigned long *ReturnBufferLength,
	NTSTATUS *ProtocolStatus)
{
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApCallPackageUntrusted(
	PLSA_CLIENT_REQUEST ClientRequest,
	void *ProtocolSubmitBuffer,
	void *ClientBufferBase,
	unsigned long SubmitBufferLength,
	void **ProtocolReturnBuffer,
	unsigned long *ReturnBufferLength,
	NTSTATUS *ProtocolStatus)
{
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS NTAPI LsaApInitializePackage(
	unsigned long AuthenticationPackageId,
	PLSA_DISPATCH_TABLE LsaDispatchTable,
	LSA_STRING *Database,
	LSA_STRING *Confidentiality,
	LSA_STRING **AuthenticationPackageName)
{
	return STATUS_SUCCESS;
}

extern "C" void NTAPI LsaApLogonTerminated(LUID *LogonId)
{
}

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
	return STATUS_SUCCESS;
}
