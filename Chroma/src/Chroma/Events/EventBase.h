#pragma once

#include <vector>
#include "DelegateBase.h"

namespace Chroma
{
	template <typename Ret, typename... Args>
	class EventBase :  private DelegateBase<Ret(Args...)> {
		template <typename O, typename R, typename... A>
		friend class Event;
		
		std::vector<typename DelegateBase<Ret(Args...)>::InvocationElement*> invocation_list;
	};
}

