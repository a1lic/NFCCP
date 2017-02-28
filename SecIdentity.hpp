#pragma once
#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <Windows.h>
#include <string>
#include <vector>
#include <NTSecAPI.h>

#if !defined(LSA_LOOKUP_DISALLOW_CONNECTED_ACCOUNT_INTERNET_SID)
#define LSA_LOOKUP_DISALLOW_CONNECTED_ACCOUNT_INTERNET_SID 0x80000000
#endif

using std::wstring;
using std::vector;

class SecurityIdentity
{
	unsigned char * raw_sid;
	unsigned short sid_size;
	wstring name;
	SID_NAME_USE sid_type;
public:
	SecurityIdentity(const SID *);
	SecurityIdentity(const unsigned char *, unsigned short);
	~SecurityIdentity();
	void HexString(wstring &);
	void GetName(wstring &);
	void ResolveName(LSA_HANDLE);
	inline bool IsUser() { return this->sid_type == SID_NAME_USE::SidTypeUser; }
	inline bool IsGroup() { return (this->sid_type == SID_NAME_USE::SidTypeGroup) || (this->sid_type == SID_NAME_USE::SidTypeWellKnownGroup); }
};

class NtAccounts
{
	LSA_HANDLE lsa;
	vector<SecurityIdentity *> sid_list;
public:
	NtAccounts();
	~NtAccounts();
	void GetUserAccounts(vector<SecurityIdentity *> &);
	void GetAll(vector<SecurityIdentity *> &);
private:
	void EnumerateSecurityIdentities();
};
