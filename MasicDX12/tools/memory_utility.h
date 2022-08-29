#pragma once

#include <memory>

#include "string_utility.h"

#include <string>
#include <type_traits>
#include <Windows.h>
#include <comdef.h>

class DxException {
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) : ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(lineNumber) {}

    std::wstring ToString()const {
        _com_error err(ErrorCode);
        std::wstring msg = err.ErrorMessage();
        return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
    }

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

template <class Type>
std::shared_ptr<Type> MakeStrongPtr(std::weak_ptr<Type> pWeakPtr) {
    if (!pWeakPtr.expired())
        return std::shared_ptr<Type>(pWeakPtr);
    else
        return std::shared_ptr<Type>();
}

template<class T>
struct SortBy_SharedPtr_Content {
    bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
        return *lhs < *rhs;
    }
};

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

#ifndef ThrowIfFailedDX
#define ThrowIfFailedDX(x)                                            \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = s2ws(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}