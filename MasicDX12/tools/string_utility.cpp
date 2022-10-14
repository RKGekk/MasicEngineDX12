#pragma warning(disable : 4996)

#include "string_utility.h"

std::string w2s(const std::wstring& var) {
	static std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(var);
}

std::wstring s2w(const std::string& var) {
	static std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(var);
}

std::string fixedfloat(float value, int precision) {
	std::ostringstream strout;
	strout << std::fixed;
	strout << std::setprecision(precision);
	strout << value;
	std::string str = strout.str();
	size_t end = str.find_last_not_of('0') + 1;
	if (str[end - 1] == '.') {
		return str.erase(end + 1);
	}
	else {
		return str.erase(end);
	}
}

std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT2& v) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "(" << fixedfloat(v.x, 6) << ", " << fixedfloat(v.y, 6) << ")";
	os.flags(oldFlag);
	return os;
}

std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& v) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "(" << fixedfloat(v.x, 6) << ", " << fixedfloat(v.y, 6) << ", " << fixedfloat(v.z, 6) << ")";
	os.flags(oldFlag);
	return os;
}

std::ostream& operator<<(std::ostream& os, DirectX::FXMVECTOR v) {
	DirectX::XMFLOAT3 dest;
	DirectX::XMStoreFloat3(&dest, v);
	os << dest;
	return os;
}

std::ostream& operator<<(std::ostream& os, DirectX::XMFLOAT4X4 m) {
	std::ios::fmtflags oldFlag = os.flags();
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			os << std::setw(6) << fixedfloat(m(r, c), 6);
		}
		os << std::endl;
	}
	os.flags(oldFlag);
	return os;
}

std::ostream& operator<<(std::ostream& os, DirectX::CXMMATRIX m) {
	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, m);
	os << matrix;
	return os;
}

void stoupper(std::string& s) {
	std::for_each(s.begin(), s.end(), [](char& c) { c = ::toupper(c); });
}

std::string stoupper(const std::string& s) {
	std::string res = s;
	std::for_each(res.begin(), res.end(), [](char& c) { c = ::toupper(c); });
	return res;
}

bool stobool(const std::string& s) {
	std::string us = stoupper(s);
	return (us == "YES" || us == "TRUE" || us == "1") ? true : false;
}

bool ntobool(const pugi::xml_node node_with_bool) {
	bool res = false;
	if (node_with_bool) {
		std::string s(node_with_bool.first_child().value());
		res = stobool(s);
	}
	return res;
}

bool ntobool(const pugi::xml_node node_with_bool, bool def) {
	bool res = def;
	if (node_with_bool) {
		std::string s(node_with_bool.first_child().value());
		res = stobool(s);
	}
	return res;
}

float ntofloat(const pugi::xml_node node_with_float) {
	float res = 0.0f;
	if (node_with_float) {
		std::string s(node_with_float.first_child().value());
		res = std::stof(s);
	}
	return res;
}

float ntofloat(const pugi::xml_node node_with_float, float def) {
	float res = def;
	if (node_with_float) {
		std::string s(node_with_float.first_child().value());
		res = std::stof(s);
	}
	return res;
}

int ntoint(const pugi::xml_node node_with_int) {
	int res = 0;
	if (node_with_int) {
		std::string s(node_with_int.first_child().value());
		res = std::stoi(s);
	}
	return res;
}

int ntoint(const pugi::xml_node node_with_int, int def) {
	int res = def;
	if (node_with_int) {
		std::string s(node_with_int.first_child().value());
		res = std::stoi(s);
	}
	return res;
}

float attrtofloat(const pugi::xml_attribute attr_with_float) {
	float res = 0.0f;
	if (attr_with_float) {
		std::string s(attr_with_float.value());
		res = std::stof(s);
	}
	return res;
}

float attrtofloat(const pugi::xml_attribute attr_with_float, float def) {
	float res = def;
	if (attr_with_float) {
		std::string s(attr_with_float.value());
		res = std::stof(s);
	}
	return res;
}

DirectX::XMFLOAT3 colorfromattr3f(const pugi::xml_node& node_with_color) {
	DirectX::XMFLOAT3 color = { 0.0f, 0.0f, 0.0f };

	if (node_with_color) {
		std::string sr = node_with_color.attribute("r").value();
		std::string sg = node_with_color.attribute("g").value();
		std::string sb = node_with_color.attribute("b").value();

		color.x = std::stof(sr);
		color.y = std::stof(sg);
		color.z = std::stof(sb);
	}

	return color;
}

DirectX::XMFLOAT3 colorfromattr3f(const pugi::xml_node& node_with_color, DirectX::XMFLOAT3 def) {
	DirectX::XMFLOAT3 color = def;

	std::string sr = node_with_color.attribute("r").value();
	std::string sg = node_with_color.attribute("g").value();
	std::string sb = node_with_color.attribute("b").value();

	color.x = sr.empty() ? def.x : std::stof(sr);
	color.y = sg.empty() ? def.y : std::stof(sg);
	color.z = sb.empty() ? def.z : std::stof(sb);

	return color;
}

DirectX::XMFLOAT4 colorfromattr4f(const pugi::xml_node& node_with_color) {
	DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (node_with_color) {
		std::string sr = node_with_color.attribute("r").value();
		std::string sg = node_with_color.attribute("g").value();
		std::string sb = node_with_color.attribute("b").value();
		std::string sa = node_with_color.attribute("a").value();

		color.x = std::stof(sr);
		color.y = std::stof(sg);
		color.z = std::stof(sb);
		color.w = std::stof(sa);
	}

	return color;
}

DirectX::XMFLOAT4 colorfromattr4f(const pugi::xml_node& node_with_color, DirectX::XMFLOAT4 def) {
	DirectX::XMFLOAT4 color = def;

	std::string sr = node_with_color.attribute("r").value();
	std::string sg = node_with_color.attribute("g").value();
	std::string sb = node_with_color.attribute("b").value();
	std::string sa = node_with_color.attribute("a").value();

	color.x = sr.empty() ? def.x : std::stof(sr);
	color.y = sg.empty() ? def.y : std::stof(sg);
	color.z = sb.empty() ? def.z : std::stof(sb);
	color.w = sa.empty() ? def.w : std::stof(sa);

	return color;
}

DirectX::XMFLOAT3 posfromattr3f(const pugi::xml_node& node_with_pos) {
	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };

	if (node_with_pos) {
		std::string sx = node_with_pos.attribute("x").value();
		std::string sy = node_with_pos.attribute("y").value();
		std::string sz = node_with_pos.attribute("z").value();

		pos.x = std::stof(sx);
		pos.y = std::stof(sy);
		pos.z = std::stof(sz);
	}

	return pos;
}

DirectX::XMFLOAT3 posfromattr3f(const pugi::xml_node& node_with_pos, DirectX::XMFLOAT3 def) {
	DirectX::XMFLOAT3 pos = def;

	if (node_with_pos) {
		std::string sx = node_with_pos.attribute("x").value();
		std::string sy = node_with_pos.attribute("y").value();
		std::string sz = node_with_pos.attribute("z").value();

		pos.x = std::stof(sx);
		pos.y = std::stof(sy);
		pos.z = std::stof(sz);
	}

	return pos;
}

DirectX::XMFLOAT3 anglesfromattr3f(const pugi::xml_node& node_with_angles) {
	DirectX::XMFLOAT3 angles = { 0.0f, 0.0f, 0.0f };

	if (node_with_angles) {
		std::string sx = node_with_angles.attribute("x").value();
		std::string sy = node_with_angles.attribute("y").value();
		std::string sz = node_with_angles.attribute("z").value();

		float yaw = std::stof(sx);
		float pitch = std::stof(sy);
		float roll = std::stof(sz);

		angles.x = DirectX::XMConvertToRadians(yaw);
		angles.y = DirectX::XMConvertToRadians(pitch);
		angles.z = DirectX::XMConvertToRadians(roll);
	}

	return angles;
}

DirectX::XMFLOAT3 anglesfromattr3f(const pugi::xml_node& node_with_angles, DirectX::XMFLOAT3 def) {
	DirectX::XMFLOAT3 angles = def;

	if (node_with_angles) {
		std::string sx = node_with_angles.attribute("x").value();
		std::string sy = node_with_angles.attribute("y").value();
		std::string sz = node_with_angles.attribute("z").value();

		float yaw = std::stof(sx);
		float pitch = std::stof(sy);
		float roll = std::stof(sz);

		angles.x = DirectX::XMConvertToRadians(yaw);
		angles.y = DirectX::XMConvertToRadians(pitch);
		angles.z = DirectX::XMConvertToRadians(roll);
	}

	return angles;
}