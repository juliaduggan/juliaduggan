﻿/*
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
#include "HttpClient.h"
#include "websocket_srv.h"

namespace X
{
    namespace WebCore
    {
		class WebCore
		{
		public:
			BEGIN_PACKAGE(WebCore)
				APISET().AddClass<1, HttpRequest>("HttpRequest");
				APISET().AddClass<1, WebSocketServer>("WebSocketServer");
			END_PACKAGE
		};
	}
}