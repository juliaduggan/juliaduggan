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

#ifndef _X_PROXY_H_
#define _X_PROXY_H_

#include "xlang.h"

namespace X
{
	typedef struct
	{
		unsigned long long pid;
		void* objId;
	} ROBJ_ID;
	typedef int ROBJ_MEMBER_ID;
	class XProxy
	{
	public:
		virtual int AddRef() = 0;
		virtual int Release() = 0;
		virtual void AddObject(XObj* obj) = 0;
		virtual void RemoveOject(XObj* obj) = 0;
		virtual void SetRootObjectName(const char* name) = 0;
		virtual ROBJ_ID QueryRootObject(std::string& name) = 0;
		virtual ROBJ_MEMBER_ID QueryMember(ROBJ_ID id,std::string& name,
			int& memberFlags) = 0;
		virtual long long QueryMemberCount(X::ROBJ_ID id) = 0;
		virtual bool FlatPack(X::ROBJ_ID parentObjId,X::ROBJ_ID id,
			Port::vector<std::string>& IdList, int id_offset,
			long long startIndex,long long count, Value& retList) = 0;
		virtual X::Value UpdateItemValue(X::ROBJ_ID parentObjId, X::ROBJ_ID id,
			Port::vector<std::string>& IdList, int id_offset,
			std::string itemName, X::Value& val) = 0;
		virtual ROBJ_ID GetMemberObject(ROBJ_ID id, 
				ROBJ_MEMBER_ID memId,bool bGetOnly,X::Value& retValue) = 0;
		virtual bool ReleaseObject(ROBJ_ID id) = 0;
		virtual bool Call(XRuntime* rt, XObj* pContext,
			ROBJ_ID parent_id, ROBJ_ID id, ROBJ_MEMBER_ID memId,
			ARGS& params, KWARGS& kwParams, X::Value& trailer,Value& retValue) = 0;
		virtual void SetTimeout(int timeout) = 0;
	};
}

#endif //_X_PROXY_H_