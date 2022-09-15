#pragma once

#include <algorithm>
#include <codecvt>
#include <iomanip>
#include <sstream>
#include <string>
#include <locale>

#include <pugixml/pugixml.hpp>
#include <DirectXMath.h>

#define STR1(x) #x
#define STR(x) STR1(x)
#define WSTR1(x) L##x
#define WSTR(x) WSTR1(x)
#define NAME_D3D12_OBJECT(x) x->SetName( WSTR(__FILE__ "(" STR(__LINE__) "): " L#x) )

#pragma warning( push )
#pragma warning( disable : 4996)

std::string w2s(const std::wstring& var);
std::wstring s2w(const std::string& var);

inline std::wstring ConvertString(const std::string& string) {
    static std::locale loc("");
    auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
    return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(string);
}

inline std::string ConvertString(const std::wstring& wstring) {
    static std::locale loc("");
    auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
    return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstring);
}

inline std::wstring to_wstring(const std::string& s) {
    return ConvertString(s);
}

inline const std::wstring& to_wstring(const std::wstring& s) {
    return s;
}

inline std::wstring to_wstring(char c) {
    return to_wstring(std::string(1, c));
}

#pragma warning( pop )

std::string fixedfloat(float value, int precision);
std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT2& v);
std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& v);
std::ostream& operator<<(std::ostream& os, DirectX::FXMVECTOR v);
std::ostream& operator<<(std::ostream& os, DirectX::XMFLOAT4X4 m);
std::ostream& operator<<(std::ostream& os, DirectX::CXMMATRIX m);

template<typename C, typename T>
auto insert_in_container(C& c, T&& t) -> decltype(c.push_back(std::forward<T>(t)), void()) {
    c.push_back(std::forward<T>(t));
}
template<typename C, typename T>
auto insert_in_container(C& c, T&& t) -> decltype(c.insert(std::forward<T>(t)), void()) {
    c.insert(std::forward<T>(t));
}
template<typename Container>
Container splitR(const std::string& input, const std::string& delims) {
    Container out;
    size_t delims_len = delims.size();
    auto begIdx = 0u;
    auto endIdx = input.find(delims, begIdx);
    if (endIdx == std::string::npos && input.size() != 0u) {
        insert_in_container(out, input);
    }
    else {
        size_t w = 0;
        while (endIdx != std::string::npos) {
            w = endIdx - begIdx;
            if (w != 0) insert_in_container(out, input.substr(begIdx, w));
            begIdx = endIdx + delims_len;
            endIdx = input.find(delims, begIdx);
        }
        w = input.length() - begIdx;
        if (w != 0) insert_in_container(out, input.substr(begIdx, w));
    }
    return out;
}

void stoupper(std::string& s);
std::string stoupper(const std::string& s);

bool stobool(const std::string& s);
bool ntobool(const pugi::xml_node node_with_bool);
bool ntobool(const pugi::xml_node node_with_bool, bool def);
float ntofloat(const pugi::xml_node node_with_float);
float ntofloat(const pugi::xml_node node_with_float, float def);
int ntoint(const pugi::xml_node node_with_int);
int ntoint(const pugi::xml_node node_with_int, int def);
float attrtofloat(const pugi::xml_attribute attr_with_float);
float attrtofloat(const pugi::xml_attribute attr_with_float, float def);

DirectX::XMFLOAT3 colorfromattr3f(const pugi::xml_node& node_with_color);
DirectX::XMFLOAT3 colorfromattr3f(const pugi::xml_node& node_with_color, DirectX::XMFLOAT3 def);
DirectX::XMFLOAT4 colorfromattr4f(const pugi::xml_node& node_with_color);
DirectX::XMFLOAT4 colorfromattr4f(const pugi::xml_node& node_with_color, DirectX::XMFLOAT4 def);
DirectX::XMFLOAT3 posfromattr3f(const pugi::xml_node& node_with_pos);
DirectX::XMFLOAT3 posfromattr3f(const pugi::xml_node& node_with_pos, DirectX::XMFLOAT3 def);
DirectX::XMFLOAT3 anglesfromattr3f(const pugi::xml_node& node_with_angles);
DirectX::XMFLOAT3 anglesfromattr3f(const pugi::xml_node& node_with_angles, DirectX::XMFLOAT3 def);