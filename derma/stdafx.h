/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

// Concepts library
#include <concepts>

// Coroutines library
#include <coroutine>

// Utilities library
//#include <cstdlib>
//#include <csignal>
//#include <csetjmp>
//#include <cstdarg>
//#include <typeinfo>
//#include <typeindex>
#include <type_traits>
#include <bitset>
#include <functional>
#include <utility>
//#include <ctime>
#include <chrono>
//#include <cstddef>
//#include <initializer_list>
//#include <tuple>
#include <any>
#include <optional>
#include <variant>
//#include <compare>
//#include <version>
//#include <source_location> // DOES NOT EXIST YET

// Dynamic memory management
#include <new>
#include <memory>
#include <scoped_allocator>
#include <memory_resource>

// Numeric limits
//#include <climits>
//#include <cfloat>
//#include <cstdint>
//#include <cinttypes>
#include <limits>

// Error handling
//#include <exception>
//#include <stdexcept>
#include <cassert>
//#include <system_error>
//#include <cerrno>

// Strings library
#include <cctype>
//#include <cwctype>
//#include <cstring>
//#include <cwchar>
//#include <cuchar>
#include <string>
#include <string_view>
#include <charconv>
//#include <format> // DOES NOT EXIST YET

// Containers library
#include <array>
#include <vector>
#include <deque>
#include <list>
//#include <forward_list>
#include <set>
#include <map>
//#include <unordered_set>
//#include <unordered_map>
#include <stack>
#include <queue>
#include <span>

// Iterators library
//#include <iterator>

// Ranges library
#include <ranges>

// Algorithms library
#include <algorithm>
#include <execution>

// Numerics library
//#include <cmath>
//#include <complex>
//#include <valarray>
#include <random>
#include <numeric>
//#include <ratio>
//#include <cfenv>
#include <bit>
#include <numbers>

// Localization library
//#include <locale>
//#include <clocale>

// Input/output library
#include <iosfwd>
//#include <ios>
//#include <istream>
//#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <syncstream> // DOES NOT EXIST YET
#include <iomanip>
//#include <streambuf>
//#include <cstdio>

// Filesystem
#include <filesystem>

// Regular Expressions library
#include <regex>

// Atomic Operations library
#include <atomic>

// Thread support library
#include <thread>
//#include <stop_token>
#include <mutex>
//#include <shared_mutex>
#include <future>
//#include <condition_variable>
//#include <semaphore>
//#include <latch>
//#include <barrier>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif

#if _WIN32
#include <windows.h>
#include <wrl.h>
#include <shellapi.h>
#include <aclapi.h>

//#include <cuda_runtime.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../directx/d3dx12.h"

#endif
