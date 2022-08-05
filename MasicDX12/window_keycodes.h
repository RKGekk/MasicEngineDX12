#pragma once

#include <iostream>

enum class WindowKey {
	None = 0x00, // No key was pressed
	LButton = 0x01, // Left mouse button
	RButton = 0x02, // Right mouse button
	Cancel = 0x03, // Cancel key
	MButton = 0x04, // Middle mouse button
	XButton1 = 0x05, // X1 mouse button
	XButton2 = 0x06, // X2 mouse button
	// 0x07 is undefined
	Back = 0x08,
	Tab = 0x09,
	// 0x0A-0B are reserved
	Clear = 0x0c, // The CLEAR key
	Enter = 0x0d, // The Enter key
	// 0x0E-0F are undefined
	ShiftKey = 0x10, // The Shift key
	ControlKey = 0x11, // The Ctrl key
	AltKey = 0x12, // The Alt key
	Pause = 0x13, // The Pause key
	Capital = 0x14, // The Caps Lock key
	CapsLock = 0x14, // The Caps Lock key
	KanaMode = 0x15, // IMI Kana mode
	HanguelMode = 0x15, // IMI Hanguel mode (Use HangulMode)
	HangulMode = 0x15, // IMI Hangul mode
	// 0x16 is undefined
	JunjaMode = 0x17, // IMI Janja mode
	FinalMode = 0x18, // IMI Final mode
	HanjaMode = 0x19, // IMI Hanja mode
	KanjiMode = 0x19, // IMI Kanji mode
	// 0x1A is undefined
	Escape = 0x1b, // The ESC key
	IMEConvert = 0x1c, // IMI convert key
	IMINoConvert = 0x1d, // IMI noconvert key
	IMEAccept = 0x1e, // IMI accept key
	IMIModeChange = 0x1f, // IMI mode change key
	Space = 0x20, // The Space key
	Prior = 0x21, // The Page Up key
	PageUp = 0x21, // The Page Up key
	Next = 0x22, // The Page Down key
	PageDown = 0x22, // The Page Down key
	End = 0x23, // The End key
	Home = 0x24, // The Home key
	Left = 0x25, // The Left arrow key
	Up = 0x26, // The Up arrow key
	Right = 0x27, // The Right arrow key
	Down = 0x28, // The Down arrow key
	Select = 0x29, // The Select key
	Print = 0x2a, // The Print key
	Execute = 0x2b, // The Execute key
	PrintScreen = 0x2c, // The Print Screen key
	Snapshot = 0x2c, // The Print Screen key
	Insert = 0x2d, // The Insert key
	Delete = 0x2e, // The Delete key
	Help = 0x2f, // The Help key
	D0 = 0x30, // 0
	D1 = 0x31, // 1
	D2 = 0x32, // 2
	D3 = 0x33, // 3
	D4 = 0x34, // 4
	D5 = 0x35, // 5
	D6 = 0x36, // 6
	D7 = 0x37, // 7
	D8 = 0x38, // 8
	D9 = 0x39, // 9
	// 0x3A - 40 are undefined
	A = 0x41, // A
	B = 0x42, // B
	C = 0x43, // C
	D = 0x44, // D
	E = 0x45, // E
	F = 0x46, // F
	G = 0x47, // G
	H = 0x48, // H
	I = 0x49, // I
	J = 0x4a, // J
	K = 0x4b, // K
	L = 0x4c, // L
	M = 0x4d, // M
	N = 0x4e, // N
	O = 0x4f, // O
	P = 0x50, // P
	Q = 0x51, // Q
	R = 0x52, // R
	S = 0x53, // S
	T = 0x54, // T
	U = 0x55, // U
	V = 0x56, // V
	W = 0x57, // W
	X = 0x58, // X
	Y = 0x59, // Y
	Z = 0x5a, // Z
	LWin = 0x5b, // Left Windows key
	RWin = 0x5c, // Right Windows key
	Apps = 0x5d, // Apps key
	// 0x5E is reserved
	Sleep = 0x5f, // The Sleep key
	NumPad0 = 0x60, // The Numeric keypad 0 key
	NumPad1 = 0x61, // The Numeric keypad 1 key
	NumPad2 = 0x62, // The Numeric keypad 2 key
	NumPad3 = 0x63, // The Numeric keypad 3 key
	NumPad4 = 0x64, // The Numeric keypad 4 key
	NumPad5 = 0x65, // The Numeric keypad 5 key
	NumPad6 = 0x66, // The Numeric keypad 6 key
	NumPad7 = 0x67, // The Numeric keypad 7 key
	NumPad8 = 0x68, // The Numeric keypad 8 key
	NumPad9 = 0x69, // The Numeric keypad 9 key
	Multiply = 0x6a, // The Multiply key
	Add = 0x6b, // The Add key
	Separator = 0x6c, // The Separator key
	Subtract = 0x6d, // The Subtract key
	Decimal = 0x6e, // The Decimal key
	Divide = 0x6f, // The Divide key
	F1 = 0x70, // The F1 key
	F2 = 0x71, // The F2 key
	F3 = 0x72, // The F3 key
	F4 = 0x73, // The F4 key
	F5 = 0x74, // The F5 key
	F6 = 0x75, // The F6 key
	F7 = 0x76, // The F7 key
	F8 = 0x77, // The F8 key
	F9 = 0x78, // The F9 key
	F10 = 0x79, // The F10 key
	F11 = 0x7a, // The F11 key
	F12 = 0x7b, // The F12 key
	F13 = 0x7c, // The F13 key
	F14 = 0x7d, // The F14 key
	F15 = 0x7e, // The F15 key
	F16 = 0x7f, // The F16 key
	F17 = 0x80, // The F17 key
	F18 = 0x81, // The F18 key
	F19 = 0x82, // The F19 key
	F20 = 0x83, // The F20 key
	F21 = 0x84, // The F21 key
	F22 = 0x85, // The F22 key
	F23 = 0x86, // The F23 key
	F24 = 0x87, // The F24 key
	// 0x88 - 0x8f are unassigned
	NumLock = 0x90, // The Num Lock key
	Scroll = 0x91, // The Scroll Lock key
	// 0x92 - 96 are OEM specific
	// 0x97 - 9f are unassigned
	LShiftKey = 0xa0, // The Left Shift key
	RShiftKey = 0xa1, // The Right Shift key
	LControlKey = 0xa2, // The Left Control key
	RControlKey = 0xa3, // The Right Control key
	LMenu = 0xa4, // The Left Alt key
	RMenu = 0xa5, // The Right Alt key
	BrowserBack = 0xa6, // The Browser Back key
	BrowserForward = 0xa7, // The Browser Forward key
	BrowserRefresh = 0xa8, // The Browser Refresh key
	BrowserStop = 0xa9, // The Browser Stop key
	BrowserSearch = 0xaa, // The Browser Search key
	BrowserFavorites = 0xab, // The Browser Favorites key
	BrowserHome = 0xac, // The Browser Home key
	VolumeMute = 0xad, // The Volume Mute key
	VolumeDown = 0xae, // The Volume Down key
	VolumeUp = 0xaf, // The Volume Up key
	MediaNextTrack = 0xb0, // The Next Track key
	MediaPreviousTrack = 0xb1, // The Previous Track key
	MediaStop = 0xb2, // The Stop Media key
	MediaPlayPause = 0xb3, // The Play/Pause Media key
	LaunchMail = 0xb4, // The Start Mail key
	SelectMedia = 0xb5, // The Select Media key
	LaunchApplication1 = 0xb6, // The Launch Application 1 key.
	LaunchApplication2 = 0xb7, // The Launch Application 2 key.
	// 0xB8 - B9 are reserved
	OemSemicolon = 0xba, // Used for miscellaneous characters; it can vary by keyboard.  For the US standard keyboard, the ';:' key
	Oem1 = 0xba, // Used for miscellaneous characters; it can vary by keyboard.  For the US standard keyboard, the ';:' key
	OemPlus = 0xbb, // For any country/region, the '+' key
	OemComma = 0xbc, // For any country/region, the ',' key
	OemMinus = 0xbd, // For any country/region, the '-' key
	OemPeriod = 0xbe, // For any country/region, the '.' key
	OemQuestion = 0xbf, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
	Oem2 = 0xbf, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
	OemTilde = 0xc0, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
	Oem3 = 0xc0, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
	// 0xC1 - D7 are reserved
	// 0xD8 - DA are unassigned
	OemOpenBrackets = 0xdb, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
	Oem4 = 0xdb, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
	OemPipe = 0xdc, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
	Oem5 = 0xdc, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
	OemCloseBrackets = 0xdd, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
	Oem6 = 0xdd, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
	OemQuotes = 0xde, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
	Oem7 = 0xde, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
	Oem8 = 0xdf, // Used for miscellaneous characters; it can vary by keyboard.
	// 0xE0 is reserved
	// 0xE1 is OEM specific
	OemBackslash = 0xe2, // Either the angle bracket key or the backslash key on the RT 102-key keyboard
	Oem102 = 0xe2, // Either the angle bracket key or the backslash key on the RT 102-key keyboard
	// 0xE3 - E4 OEM specific
	ProcessKey = 0xe5, // IME Process key
	// 0xE6 is OEM specific
	Packet = 0xe7, // Used to pass Unicode characters as if they were keystrokes. The Packet key value is the low word of a 32-bit virtual-key value used for non-keyboard input methods.
	// 0xE8 is unassigned
	// 0xE9 - F5 OEM specific
	Attn = 0xf6, // The Attn key
	CrSel = 0xf7, // The CrSel key
	ExSel = 0xf8, // The ExSel key
	EraseEof = 0xf9, // The Erase EOF key
	Play = 0xfa, // The Play key
	Zoom = 0xfb, // The Zoom key
	NoName = 0xfc, // Reserved
	Pa1 = 0xfd, // The PA1 key
	OemClear = 0xfe, // The Clear key
};

std::ostream& operator<<(std::ostream& os, const WindowKey& k) {
	std::ios::fmtflags oldFlag = os.flags();
	switch (k) {
		case WindowKey::None: { os << "None" << std::endl; } break;
		case WindowKey::LButton: { os << "LButton" << std::endl; } break;
		case WindowKey::RButton: { os << "RButton" << std::endl; } break;
		case WindowKey::Cancel: { os << "Cancel" << std::endl; } break;
		case WindowKey::MButton: { os << "MButton" << std::endl; } break;
		case WindowKey::XButton1: { os << "XButton1" << std::endl; } break;
		case WindowKey::XButton2: { os << "XButton2" << std::endl; } break;
		case WindowKey::Back: { os << "Back" << std::endl; } break;
		case WindowKey::Tab: { os << "Tab" << std::endl; } break;
		case WindowKey::Clear: { os << "Clear" << std::endl; } break;
		case WindowKey::Enter: { os << "Enter" << std::endl; } break;
		case WindowKey::ShiftKey: { os << "ShiftKey" << std::endl; } break;
		case WindowKey::ControlKey: { os << "ControlKey" << std::endl; } break;
		case WindowKey::AltKey: { os << "AltKey" << std::endl; } break;
		case WindowKey::Pause: { os << "Pause" << std::endl; } break;
		case WindowKey::Capital: { os << "CapsLock" << std::endl; } break;
		case WindowKey::KanaMode: { os << "KanaMode" << std::endl; } break;
		case WindowKey::JunjaMode: { os << "JunjaMode" << std::endl; } break;
		case WindowKey::FinalMode: { os << "FinalMode" << std::endl; } break;
		case WindowKey::HanjaMode: { os << "HanjaMode" << std::endl; } break;
		case WindowKey::Escape: { os << "Escape" << std::endl; } break;
		case WindowKey::IMEConvert: { os << "IMEConvert" << std::endl; } break;
		case WindowKey::IMINoConvert: { os << "IMINoConvert" << std::endl; } break;
		case WindowKey::IMEAccept: { os << "IMEAccept" << std::endl; } break;
		case WindowKey::IMIModeChange: { os << "IMIModeChange" << std::endl; } break;
		case WindowKey::Space: { os << "Space" << std::endl; } break;
		case WindowKey::Prior: { os << "Prior" << std::endl; } break;
		case WindowKey::Next: { os << "Next" << std::endl; } break;
		case WindowKey::End: { os << "End" << std::endl; } break;
		case WindowKey::Home: { os << "Home" << std::endl; } break;
		case WindowKey::Left: { os << "Left" << std::endl; } break;
		case WindowKey::Up: { os << "Up" << std::endl; } break;
		case WindowKey::Right: { os << "Right" << std::endl; } break;
		case WindowKey::Down: { os << "Down" << std::endl; } break;
		case WindowKey::Select: { os << "Select" << std::endl; } break;
		case WindowKey::Print: { os << "Print" << std::endl; } break;
		case WindowKey::Execute: { os << "Execute" << std::endl; } break;
		case WindowKey::PrintScreen: { os << "PrintScreen" << std::endl; } break;
		case WindowKey::Insert: { os << "Insert" << std::endl; } break;
		case WindowKey::Delete: { os << "Delete" << std::endl; } break;
		case WindowKey::Help: { os << "Help" << std::endl; } break;
		case WindowKey::D0: { os << "D0" << std::endl; } break;
		case WindowKey::D1: { os << "D1" << std::endl; } break;
		case WindowKey::D2: { os << "D2" << std::endl; } break;
		case WindowKey::D3: { os << "D3" << std::endl; } break;
		case WindowKey::D4: { os << "D4" << std::endl; } break;
		case WindowKey::D5: { os << "D5" << std::endl; } break;
		case WindowKey::D6: { os << "D6" << std::endl; } break;
		case WindowKey::D7: { os << "D7" << std::endl; } break;
		case WindowKey::D8: { os << "D8" << std::endl; } break;
		case WindowKey::D9: { os << "D9" << std::endl; } break;
		case WindowKey::A: { os << "A" << std::endl; } break;
		case WindowKey::B: { os << "B" << std::endl; } break;
		case WindowKey::C: { os << "C" << std::endl; } break;
		case WindowKey::D: { os << "D" << std::endl; } break;
		case WindowKey::E: { os << "E" << std::endl; } break;
		case WindowKey::F: { os << "F" << std::endl; } break;
		case WindowKey::G: { os << "G" << std::endl; } break;
		case WindowKey::H: { os << "H" << std::endl; } break;
		case WindowKey::I: { os << "I" << std::endl; } break;
		case WindowKey::J: { os << "J" << std::endl; } break;
		case WindowKey::K: { os << "K" << std::endl; } break;
		case WindowKey::L: { os << "L" << std::endl; } break;
		case WindowKey::M: { os << "M" << std::endl; } break;
		case WindowKey::N: { os << "N" << std::endl; } break;
		case WindowKey::O: { os << "O" << std::endl; } break;
		case WindowKey::P: { os << "P" << std::endl; } break;
		case WindowKey::Q: { os << "Q" << std::endl; } break;
		case WindowKey::R: { os << "R" << std::endl; } break;
		case WindowKey::S: { os << "S" << std::endl; } break;
		case WindowKey::T: { os << "T" << std::endl; } break;
		case WindowKey::U: { os << "U" << std::endl; } break;
		case WindowKey::V: { os << "V" << std::endl; } break;
		case WindowKey::W: { os << "W" << std::endl; } break;
		case WindowKey::X: { os << "X" << std::endl; } break;
		case WindowKey::Y: { os << "Y" << std::endl; } break;
		case WindowKey::Z: { os << "Z" << std::endl; } break;
		case WindowKey::LWin: { os << "LWin" << std::endl; } break;
		case WindowKey::RWin: { os << "RWin" << std::endl; } break;
		case WindowKey::Apps: { os << "Apps" << std::endl; } break;
		case WindowKey::Sleep: { os << "Sleep" << std::endl; } break;
		case WindowKey::NumPad0: { os << "NumPad0" << std::endl; } break;
		case WindowKey::NumPad1: { os << "NumPad1" << std::endl; } break;
		case WindowKey::NumPad2: { os << "NumPad2" << std::endl; } break;
		case WindowKey::NumPad3: { os << "NumPad3" << std::endl; } break;
		case WindowKey::NumPad4: { os << "NumPad4" << std::endl; } break;
		case WindowKey::NumPad5: { os << "NumPad5" << std::endl; } break;
		case WindowKey::NumPad6: { os << "NumPad6" << std::endl; } break;
		case WindowKey::NumPad7: { os << "NumPad7" << std::endl; } break;
		case WindowKey::NumPad8: { os << "NumPad8" << std::endl; } break;
		case WindowKey::NumPad9: { os << "NumPad9" << std::endl; } break;
		case WindowKey::Multiply: { os << "Multiply" << std::endl; } break;
		case WindowKey::Add: { os << "Add" << std::endl; } break;
		case WindowKey::Separator: { os << "Separator" << std::endl; } break;
		case WindowKey::Subtract: { os << "Subtract" << std::endl; } break;
		case WindowKey::Decimal: { os << "Decimal" << std::endl; } break;
		case WindowKey::Divide: { os << "Divide" << std::endl; } break;
		case WindowKey::F1: { os << "F1" << std::endl; } break;
		case WindowKey::F2: { os << "F2" << std::endl; } break;
		case WindowKey::F3: { os << "F3" << std::endl; } break;
		case WindowKey::F4: { os << "F4" << std::endl; } break;
		case WindowKey::F5: { os << "F5" << std::endl; } break;
		case WindowKey::F6: { os << "F6" << std::endl; } break;
		case WindowKey::F7: { os << "F7" << std::endl; } break;
		case WindowKey::F8: { os << "F8" << std::endl; } break;
		case WindowKey::F9: { os << "F9" << std::endl; } break;
		case WindowKey::F10: { os << "F10" << std::endl; } break;
		case WindowKey::F11: { os << "F11" << std::endl; } break;
		case WindowKey::F12: { os << "F12" << std::endl; } break;
		case WindowKey::F13: { os << "F13" << std::endl; } break;
		case WindowKey::F14: { os << "F14" << std::endl; } break;
		case WindowKey::F15: { os << "F15" << std::endl; } break;
		case WindowKey::F16: { os << "F16" << std::endl; } break;
		case WindowKey::F17: { os << "F17" << std::endl; } break;
		case WindowKey::F18: { os << "F18" << std::endl; } break;
		case WindowKey::F19: { os << "F19" << std::endl; } break;
		case WindowKey::F20: { os << "F20" << std::endl; } break;
		case WindowKey::F21: { os << "F21" << std::endl; } break;
		case WindowKey::F22: { os << "F22" << std::endl; } break;
		case WindowKey::F23: { os << "F23" << std::endl; } break;
		case WindowKey::F24: { os << "F24" << std::endl; } break;
		case WindowKey::NumLock: { os << "NumLock" << std::endl; } break;
		case WindowKey::Scroll: { os << "Scroll" << std::endl; } break;
		case WindowKey::LShiftKey: { os << "LShiftKey" << std::endl; } break;
		case WindowKey::RShiftKey: { os << "RShiftKey" << std::endl; } break;
		case WindowKey::LControlKey: { os << "LControlKey" << std::endl; } break;
		case WindowKey::RControlKey: { os << "RControlKey" << std::endl; } break;
		case WindowKey::LMenu: { os << "LMenu" << std::endl; } break;
		case WindowKey::RMenu: { os << "RMenu" << std::endl; } break;
		case WindowKey::BrowserBack: { os << "BrowserBack" << std::endl; } break;
		case WindowKey::BrowserForward: { os << "BrowserForward" << std::endl; } break;
		case WindowKey::BrowserRefresh: { os << "BrowserRefresh" << std::endl; } break;
		case WindowKey::BrowserStop: { os << "BrowserStop" << std::endl; } break;
		case WindowKey::BrowserSearch: { os << "BrowserSearch" << std::endl; } break;
		case WindowKey::BrowserFavorites: { os << "BrowserFavorites" << std::endl; } break;
		case WindowKey::BrowserHome: { os << "BrowserHome" << std::endl; } break;
		case WindowKey::VolumeMute: { os << "VolumeMute" << std::endl; } break;
		case WindowKey::VolumeDown: { os << "VolumeDown" << std::endl; } break;
		case WindowKey::VolumeUp: { os << "VolumeUp" << std::endl; } break;
		case WindowKey::MediaNextTrack: { os << "MediaNextTrack" << std::endl; } break;
		case WindowKey::MediaPreviousTrack: { os << "MediaPreviousTrack" << std::endl; } break;
		case WindowKey::MediaStop: { os << "MediaStop" << std::endl; } break;
		case WindowKey::MediaPlayPause: { os << "MediaPlayPause" << std::endl; } break;
		case WindowKey::LaunchMail: { os << "LaunchMail" << std::endl; } break;
		case WindowKey::SelectMedia: { os << "SelectMedia" << std::endl; } break;
		case WindowKey::LaunchApplication1: { os << "LaunchApplication1" << std::endl; } break;
		case WindowKey::LaunchApplication2: { os << "LaunchApplication2" << std::endl; } break;
		case WindowKey::OemSemicolon: { os << "OemSemicolon" << std::endl; } break;
		case WindowKey::OemPlus: { os << "OemPlus" << std::endl; } break;
		case WindowKey::OemComma: { os << "OemComma" << std::endl; } break;
		case WindowKey::OemMinus: { os << "OemMinus" << std::endl; } break;
		case WindowKey::OemPeriod: { os << "OemPeriod" << std::endl; } break;
		case WindowKey::OemQuestion: { os << "OemQuestion" << std::endl; } break;
		case WindowKey::OemTilde: { os << "OemTilde" << std::endl; } break;
		case WindowKey::OemOpenBrackets: { os << "OemOpenBrackets" << std::endl; } break;
		case WindowKey::OemPipe: { os << "OemPipe" << std::endl; } break;
		case WindowKey::OemCloseBrackets: { os << "OemCloseBrackets" << std::endl; } break;
		case WindowKey::OemQuotes: { os << "OemQuotes" << std::endl; } break;
		case WindowKey::Oem8: { os << "Oem8" << std::endl; } break;
		case WindowKey::OemBackslash: { os << "OemBackslash" << std::endl; } break;
		case WindowKey::ProcessKey: { os << "ProcessKey" << std::endl; } break;
		case WindowKey::Packet: { os << "Packet" << std::endl; } break;
		case WindowKey::Attn: { os << "Attn" << std::endl; } break;
		case WindowKey::CrSel: { os << "CrSel" << std::endl; } break;
		case WindowKey::ExSel: { os << "ExSel" << std::endl; } break;
		case WindowKey::EraseEof: { os << "EraseEof" << std::endl; } break;
		case WindowKey::Play: { os << "Play" << std::endl; } break;
		case WindowKey::Zoom: { os << "Zoom" << std::endl; } break;
		case WindowKey::NoName: { os << "NoName" << std::endl; } break;
		case WindowKey::Pa1: { os << "Pa1" << std::endl; } break;
		case WindowKey::OemClear: { os << "OemClear" << std::endl; } break;
		default:
			break;
	}
	os.flags(oldFlag);
	return os;
}