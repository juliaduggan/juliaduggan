/*
Copyright (C) 2024 The XLang Foundation
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once
#include "xpackage.h"
#include "xlang.h"

namespace X
{
	class Smtp
	{
		std::string mCertPath;
		std::string mClientId;
		std::string mClientSecret;
		std::string mTenantId;
		std::string mSmtpScope;
		std::string mSmtpServer;
		int mSmtpPort = 25;
		std::string GetAccessToken();
	public:
		BEGIN_PACKAGE(Smtp)
			APISET().AddFunc<4>("send", &Smtp::Send);
			APISET().AddPropWithType<std::string>("cert_path", &Smtp::mCertPath);
			APISET().AddPropWithType<std::string>("client_id", &Smtp::mClientId);
			APISET().AddPropWithType<std::string>("client_secret", &Smtp::mClientSecret);
			APISET().AddPropWithType<std::string>("tenant_id", &Smtp::mTenantId);
			APISET().AddPropWithType<std::string>("smtp_scope", &Smtp::mSmtpScope);
			APISET().AddPropWithType<std::string>("smtp_server", &Smtp::mSmtpServer);
			APISET().AddPropWithType<int>("smtp_port", &Smtp::mSmtpPort);
		END_PACKAGE
	std::string Send(std::string from, std::string to, std::string subject, std::string content );
	};

}
