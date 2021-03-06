#include "StdAfx.h"
#include "MacroX.h"
#include "PeVerTool.h"
#pragma comment(linker, "/defaultlib:version.lib")
namespace base{

CAtlString PeInfoVal::GetValue(LPCTSTR name,void* data) 
{
	CAtlString retStr;

	WORD lang_codepage[8];
	int i = 0;
	// Use the language and codepage from the DLL.
	lang_codepage[i++] = language_;
	lang_codepage[i++] = code_page_;
	// Use the default language and codepage from the DLL.
	lang_codepage[i++] = ::GetUserDefaultLangID();
	lang_codepage[i++] = code_page_;
	// Use the language from the DLL and Latin codepage (most common).
	lang_codepage[i++] = language_;
	lang_codepage[i++] = 1252;
	// Use the default language and Latin codepage (most common).
	lang_codepage[i++] = ::GetUserDefaultLangID();
	lang_codepage[i++] = 1252;
	i = 0;
	while (i < arraysize(lang_codepage)) 
	{
		CAtlString	sub_block;
		WORD language = lang_codepage[i++];
		WORD code_page = lang_codepage[i++];
		sub_block.Format(_T("\\StringFileInfo\\%04x%04x\\%ls"),language,
			code_page, name);
		LPVOID value = NULL;
		UINT size;
		BOOL r = ::VerQueryValue(data,sub_block,&value, &size);
		if (r && value) 
		{
			retStr.SetString((LPCTSTR)value);
			return retStr;
		}
	}
	return retStr;
}
void PeInfoVal::Set(void* data)
{
	PeInfoVal::CodePage *pLanguage = NULL;
	UINT  uiOtherSize = 0;
	//获取资源相关的 codepage 和language
	VerQueryValue(data,_T("\\VarFileInfo\\Translation"),(PVOID*)&pLanguage,&uiOtherSize);
	if(pLanguage)
	{
		language_ = pLanguage[0].wLanguage;
		code_page_ = pLanguage[0].wCodePage;
		VS_FIXEDFILEINFO *tmpFixInfo=NULL;
		::VerQueryValue(data,TEXT("\\"), (LPVOID*)&tmpFixInfo,&uiOtherSize);
		if(tmpFixInfo)
			memcpy(&fixed_file_info_,&tmpFixInfo,sizeof(VS_FIXEDFILEINFO));
		std::vector<CAtlString> keys;
		keys.push_back(_T("CompanyName"));
		keys.push_back(_T("CompanyShortName"));
		keys.push_back(_T("InternalName"));
		keys.push_back(_T("ProductName"));
		keys.push_back(_T("ProductShortName"));
		keys.push_back(_T("Comments"));
		keys.push_back(_T("LegalCopyright"));
		keys.push_back(_T("ProductVersion"));
		keys.push_back(_T("FileDescription"));
		keys.push_back(_T("LegalTrademarks"));
		keys.push_back(_T("PrivateBuild"));
		keys.push_back(_T("FileVersion"));
		keys.push_back(_T("OriginalFilename"));
		keys.push_back(_T("SpecialBuild"));
		keys.push_back(_T("LastChange"));
		keys.push_back(_T("Official Build"));
		for(size_t t=0;t<keys.size();t++)
		{
			data_[keys[t]] = GetValue(keys[t],data);
		}
	}
}
bool PeInfoVal::IsSuc()
{
	return (data_.size()?true:false);
}
PeInfoVal::~PeInfoVal()
{
	 
}
CAtlString PeInfoVal::company_name() 
{
	return data_[_T("CompanyName")];
}
CAtlString PeInfoVal::company_short_name()
{
	return data_[_T("CompanyShortName")];
}
CAtlString PeInfoVal::internal_name() 
{
	return data_[_T("InternalName")];
}
CAtlString PeInfoVal::product_name() 
{
	return data_[_T("ProductName")];
}
CAtlString PeInfoVal::product_short_name() 
{
	return data_[_T("ProductShortName")];
}
CAtlString PeInfoVal::comments() 
{
	return data_[_T("Comments")];
}
CAtlString PeInfoVal::legal_copyright() 
{
	return data_[_T("LegalCopyright")];
}
CAtlString PeInfoVal::product_version() 
{
	return data_[_T("ProductVersion")];
}
CAtlString PeInfoVal::file_description() 
{
	return data_[_T("FileDescription")];
}
CAtlString PeInfoVal::legal_trademarks() 
{
	return data_[_T("LegalTrademarks")];
}
CAtlString PeInfoVal::private_build()
{
	return data_[_T("PrivateBuild")];
}
CAtlString PeInfoVal::file_version() 
{
	return data_[_T("FileVersion")];
}
CAtlString PeInfoVal::original_filename() 
{
	return data_[_T("OriginalFilename")];
}
CAtlString PeInfoVal::special_build() 
{
	return data_[_T("SpecialBuild")];
}
CAtlString PeInfoVal::last_change()
{
	return data_[_T("LastChange")];
}
bool PeInfoVal::is_official_build() 
{
	return (data_[_T("Official Build")].Compare(_T("1")) == 0);
}
PeInfoVal GetPeInfo(PCTSTR pcszFileName)
{
	PeInfoVal ret;
	DWORD dwSize = 0;
	UINT uiSize = GetFileVersionInfoSize(pcszFileName,&dwSize);
	if (0 == uiSize)
	{
		return ret;
	}
	uiSize = uiSize + 1;
	byte *pBuffer = (byte*)malloc(uiSize);
	if (NULL == pBuffer)
	{
		return ret;
	}
	memset((void*)pBuffer,0,uiSize);
	if(!GetFileVersionInfo(pcszFileName,0,uiSize,(PVOID)pBuffer))
	{
		free(pBuffer);
		return ret;
	}
	ret.Set(pBuffer);
	free(pBuffer);
	return ret;	
}

}