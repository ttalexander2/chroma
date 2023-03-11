#pragma once

#include "Chroma/Core/Log.h"
#include "Chroma/Core/Core.h"
#include "Chroma/Events/Event.h"
#include "Chroma/Events/Delegate.h"
#include "Chroma/Events/MulticastDelegate.h"

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <cstdint>


#ifdef CHROMA_PLATFORM_WINDOWS
#include <Windows.h>
#endif
