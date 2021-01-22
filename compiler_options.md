# _WIN64 compile options detailed

```cpp
	// standard behaviors off by default
	"/Zc:__cplusplus"				// Enable the __cplusplus macro to report the supported standard (off by default).
	"/Zc:externConstexpr"			// Enable external linkage for constexpr variables (off by default).
	"/Zc:inline"					// Remove unreferenced function or data if it is COMDAT or has internal linkage only (off by default).
	"/Zc:preprocessor"				// Use the new conforming preprocessor (off by default, except in C11/C17).
	"/Zc:referenceBinding"			// A UDT temporary will not bind to an non-const lvalue reference (off by default).
	"/Zc:rvalueCast"				// Enforce Standard C++ explicit type conversion rules (off by default).
	"/Zc:strictStrings"				// Disable string-literal to char* or wchar_t* conversion (off by default).
	"/Zc:ternary"					// Enforce conditional operator rules on operand types (off by default).
	"/Zc:throwingNew"				// Assume operator new throws on failure (off by default).

	// all standard behaviors
	"/Zc:alignedNew"				// Enable C++17 over-aligned dynamic allocation (on by default in C++17).
	"/Zc:auto"						// Enforce the new Standard C++ meaning for auto (on by default).
	"/Zc:__cplusplus"				// Enable the __cplusplus macro to report the supported standard (off by default).
	"/Zc:externConstexpr"			// Enable external linkage for constexpr variables (off by default).
	"/Zc:forScope"					// Enforce Standard C++ for scoping rules (on by default).
	"/Zc:implicitNoexcept"			// Enable implicit noexcept on required functions (on by default).
	"/Zc:inline"					// Remove unreferenced function or data if it is COMDAT or has internal linkage only (off by default).
	"/Zc:noexceptTypes"				// Enforce C++17 noexcept rules (on by default in C++17 or later).
	"/Zc:preprocessor"				// Use the new conforming preprocessor (off by default, except in C11/C17).
	"/Zc:referenceBinding"			// A UDT temporary will not bind to an non-const lvalue reference (off by default).
	"/Zc:rvalueCast"				// Enforce Standard C++ explicit type conversion rules (off by default).
	"/Zc:sizedDealloc"				// Enable C++14 global sized deallocation functions (on by default).
	"/Zc:strictStrings"				// Disable string-literal to char* or wchar_t* conversion (off by default).
	"/Zc:ternary"					// Enforce conditional operator rules on operand types (off by default).
	"/Zc:threadSafeInit"			// Enable thread-safe local static initialization (on by default).
	"/Zc:throwingNew"				// Assume operator new throws on failure (off by default).
	"/Zc:trigraphs"					// Enable trigraphs (obsolete, off by default).
	"/Zc:twoPhase"					// Use non-conforming template parsing behavior (conforming by default).
	"/Zc:wchar_t"					// wchar_t is a native type, not a typedef (on by default).

	"/fp:precise"					// 
	"/GS"							// 
	"/Gd"							// 
	"/Gm-"							// 
	"/EHsc"							// 
	"/FC"							// 
	"/nologo"						// 
	"/permissive-"					// 
	"/W4"							// 
	"/MD""/Od"						// 
	"/arch:AVX2"					// 
	"/WX-"							// 
	"/experimental:module"			// 
```