#define SECIDENTITY_CPP
#include "SecIdentity.hpp"
#include "../Common/Util.hpp"

SecurityIdentity::SecurityIdentity(const SID * sid)
{
	SecurityIdentity(reinterpret_cast<const unsigned char*>(sid), sizeof(SID));
}

SecurityIdentity::SecurityIdentity(const unsigned char * sid, unsigned short sid_size)
{
	this->raw_sid = new unsigned char[sid_size];
	this->sid_size = sid_size;
	this->sid_type = SID_NAME_USE::SidTypeUnknown;
	memcpy(this->raw_sid, sid, sid_size);
}

SecurityIdentity::~SecurityIdentity()
{
	delete[] this->raw_sid;
}

void SecurityIdentity::HexString(wstring & s)
{
	BinaryToString(this->raw_sid, this->sid_size, s);
}

void SecurityIdentity::GetName(wstring & n)
{
	n.assign(this->name);
}

void SecurityIdentity::ResolveName(LSA_HANDLE lsa)
{
	if(this->name.size() != 0)
	{
		return;
	}

	void * sidss[1];
	sidss[0] = this->raw_sid;
	LSA_REFERENCED_DOMAIN_LIST * domains = nullptr;
	LSA_TRANSLATED_NAME * names = nullptr;
	auto lsa_result = LsaLookupSids2(lsa, LSA_LOOKUP_DISALLOW_CONNECTED_ACCOUNT_INTERNET_SID, 1, sidss, &domains, &names);
	if(lsa_result != STATUS_SUCCESS)
	{
		if(lsa_result == STATUS_NONE_MAPPED)
		{
			this->name.assign(L"<NONE>");
		}
		else
		{
			wchar_t fail[32];
			swprintf_s(fail, 32, L"!!FAILED 0x%08X", lsa_result);
			this->name.assign(fail);
		}

		if(domains)
		{
			LsaFreeMemory(domains);
		}
		if(names)
		{
			LsaFreeMemory(names);
		}

		return;
	}

	UNICODE_STRING u;
	switch(names->Use)
	{
	case SID_NAME_USE::SidTypeDomain:
		this->name.assign(L"!!THIS WAS DOMAIN SID!!");
		break;
	case SID_NAME_USE::SidTypeInvalid:
	case SID_NAME_USE::SidTypeUnknown:
		this->name.assign(L"!!INVALID OR UNKNOWN NAME!!");
		break;
	default:
		u.Buffer = names->Name.Buffer;
		u.Length = names->Name.Length;
		u.MaximumLength = names->Name.MaximumLength;
		this->name.assign(u.Buffer, u.Length / sizeof(wchar_t));
		this->sid_type = names->Use;
		break;
	}

	LsaFreeMemory(domains);
	LsaFreeMemory(names);
}

NtAccounts::NtAccounts()
{
	LSA_OBJECT_ATTRIBUTES oa;
	memset(&oa, 0, sizeof(LSA_OBJECT_ATTRIBUTES));

	if(LsaOpenPolicy(nullptr, &oa, POLICY_LOOKUP_NAMES, &this->lsa) != STATUS_SUCCESS)
	{
		DebugPrint(L"LsaOpenPolicy failed.");
		this->lsa = nullptr;
		return;
	}

	this->EnumerateSecurityIdentities();
}

NtAccounts::~NtAccounts()
{
	while(this->sid_list.size() != 0)
	{
		delete this->sid_list.back();
		this->sid_list.pop_back();
	}

	if(this->lsa)
	{
		LsaClose(this->lsa);
	}
}

void NtAccounts::GetUserAccounts(vector<SecurityIdentity*> & l)
{
	l.clear();

	for(auto i = this->sid_list.begin(), e = this->sid_list.end(); i != e; i++)
	{
		if((*i)->IsUser())
		{
			l.push_back(*i);
		}
	}
}

void NtAccounts::GetAll(vector<SecurityIdentity*> & l)
{
	l.clear();
	l.assign(this->sid_list.begin(), this->sid_list.end());
}

void NtAccounts::EnumerateSecurityIdentities()
{
	if(!this->lsa)
	{
		DebugPrint(L"No LSA policy.");
		return;
	}

	while(this->sid_list.size() != 0)
	{
		delete this->sid_list.back();
		this->sid_list.pop_back();
	}

	LSTATUS reg_result;
	HKEY accounts;
	reg_result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, LR"(SECURITY\Policy\Accounts)", 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &accounts);
	if(reg_result != ERROR_SUCCESS)
	{
		DebugPrint(L"RegOpenKeyEx failed. 0x%08X", reg_result);
		return;
	}

	DWORD subkeys;
	DWORD key_name_size;
	FILETIME time;

	RegQueryInfoKeyW(accounts, nullptr, nullptr, nullptr, &subkeys, &key_name_size, nullptr, nullptr, nullptr, nullptr, nullptr, &time);
	wchar_t * key_name = new wchar_t[++key_name_size]; // null文字を含まない文字数なのでそれを考慮

	for(DWORD i = 0; i < subkeys; i++)
	{
		memset(key_name, 0, sizeof(wchar_t) * key_name_size);

		DWORD key_name_size_cur = key_name_size;
		reg_result = RegEnumKeyExW(accounts, i, key_name, &key_name_size_cur, nullptr, nullptr, 0, &time);
		if(reg_result != ERROR_SUCCESS)
		{
			break;
		}

		HKEY sid_key;
		{
			wstring reg_path(key_name);
			reg_path.append(L"\\Sid");
			reg_result = RegOpenKeyExW(accounts, reg_path.c_str(), 0, KEY_QUERY_VALUE, &sid_key);
		}
		if(reg_result != ERROR_SUCCESS)
		{
			break;
		}

		DWORD sid_s;
		reg_result = RegQueryValueExW(sid_key, nullptr, nullptr, nullptr, nullptr, &sid_s);
		if((reg_result != ERROR_SUCCESS) || (sid_s == 0))
		{
			RegCloseKey(sid_key);
			break;
		}

		auto sid = new unsigned char[sid_s];
		RegQueryValueExW(sid_key, nullptr, nullptr, nullptr, sid, &sid_s);

		RegCloseKey(sid_key);

		auto sid_cls = new SecurityIdentity(sid, static_cast<unsigned short>(sid_s));
		this->sid_list.push_back(sid_cls);
		sid_cls->ResolveName(lsa);

		delete[] sid;
	}

	delete[] key_name;

	RegCloseKey(accounts);
}
