/* 
 * Copyright (C) 2013 Jordi Mas i Hern�ndez <jmas@softcatala.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#pragma once

#include "IRegistry.h"
#include "IRunner.h"
#include "XmlParser.h"
#include "IOpenOffice.h"
#include "OpenOfficeJavaConfiguration.h"

//
// This class contains the logic for the OpenOffice.org (OOo) project that was discontinued in April 2011
// This class is used as base class for supporting derivated suites like Apache OpenOffice and LibreOffice
//
// The class OpenOfficeAction contains logic that we may consider porting here
class OpenOffice : public IOpenOffice
{
public:

		OpenOffice(IRegistry* registry);
		
		virtual wchar_t * GetMachineRegistryKey() { return L"SOFTWARE\\OpenOffice.org\\OpenOffice.org"; }
		virtual wchar_t * GetUserDirectory() { return L"\\OpenOffice.org\\%u\\user\\"; }

		virtual bool IsInstalled();
		virtual wstring GetVersion();
		virtual bool IsExtensionInstalled(wstring extension);
		virtual void InstallExtension(IRunner* runner, wstring file);
		virtual wstring GetJavaConfiguredVersion();

protected:

		bool _readVersionInstalled();
		void _readInstallPath();
		virtual wstring _getAppDataDir();
		virtual wstring _getPreferencesDirectory();
		virtual wstring _getExtensionsFile();
		wstring _getInstallationPath();

private:

		void _parseXmlConfiguration(vector <wstring>& extensions);
		void _readListOfExtensions(vector <wstring>& extensions);
		static bool _readNodeCallback(XmlNode node, void *data);		
		
		TriBool m_isInstalled;
		wstring m_version;
		wstring m_installationPath;
		bool m_installationPathRead;
		IRegistry* m_registry;
		OpenOfficeJavaConfiguration m_javaConfiguration;
};

