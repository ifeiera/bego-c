#include "../include/bego.h"
#include <Windows.h>
#include <unordered_map>
#include <stdexcept>

/**
 * @file key_converter.cpp
 * @author Eterninety
 * @brief Implementation of key conversion between Bego's Key enum and Windows virtual key codes
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Converts from Bego's Key enum to Windows virtual key code
 * 
 * @details This function maps the library's platform-independent Key enum values
 * to Windows-specific virtual key codes (VK_*). The mapping covers all standard
 * keys, including letters, numbers, function keys, navigation keys, and modifiers.
 * 
 * This conversion is essential for translating high-level key representations
 * into the low-level codes required by the Windows input system. Without this mapping,
 * the library couldn't generate proper hardware-level keyboard events.
 * 
 * @param key The Key enum value to convert
 * @return WORD The corresponding Windows virtual key code
 * @throws InputError If the key cannot be mapped to a virtual key code
 */
WORD key_to_vk(Key key) {
    static const std::unordered_map<Key, WORD> key_map = {
        // Letters
        {Key::A, 'A'}, {Key::B, 'B'}, {Key::C, 'C'}, {Key::D, 'D'}, {Key::E, 'E'},
        {Key::F, 'F'}, {Key::G, 'G'}, {Key::H, 'H'}, {Key::I, 'I'}, {Key::J, 'J'},
        {Key::K, 'K'}, {Key::L, 'L'}, {Key::M, 'M'}, {Key::N, 'N'}, {Key::O, 'O'},
        {Key::P, 'P'}, {Key::Q, 'Q'}, {Key::R, 'R'}, {Key::S, 'S'}, {Key::T, 'T'},
        {Key::U, 'U'}, {Key::V, 'V'}, {Key::W, 'W'}, {Key::X, 'X'}, {Key::Y, 'Y'},
        {Key::Z, 'Z'},
        
        // Numbers
        {Key::Num0, '0'}, {Key::Num1, '1'}, {Key::Num2, '2'}, {Key::Num3, '3'}, {Key::Num4, '4'},
        {Key::Num5, '5'}, {Key::Num6, '6'}, {Key::Num7, '7'}, {Key::Num8, '8'}, {Key::Num9, '9'},
        
        // Function keys
        {Key::F1, VK_F1}, {Key::F2, VK_F2}, {Key::F3, VK_F3}, {Key::F4, VK_F4},
        {Key::F5, VK_F5}, {Key::F6, VK_F6}, {Key::F7, VK_F7}, {Key::F8, VK_F8},
        {Key::F9, VK_F9}, {Key::F10, VK_F10}, {Key::F11, VK_F11}, {Key::F12, VK_F12},
        {Key::F13, VK_F13}, {Key::F14, VK_F14}, {Key::F15, VK_F15}, {Key::F16, VK_F16},
        {Key::F17, VK_F17}, {Key::F18, VK_F18}, {Key::F19, VK_F19}, {Key::F20, VK_F20},
        {Key::F21, VK_F21}, {Key::F22, VK_F22}, {Key::F23, VK_F23}, {Key::F24, VK_F24},
        
        // Special keys
        {Key::Return, VK_RETURN}, {Key::Tab, VK_TAB}, {Key::Space, VK_SPACE},
        {Key::Backspace, VK_BACK}, {Key::Escape, VK_ESCAPE}, {Key::Delete, VK_DELETE},
        {Key::CapsLock, VK_CAPITAL},
        
        // Control keys
        {Key::Control, VK_CONTROL}, {Key::Alt, VK_MENU}, {Key::Shift, VK_SHIFT},
        {Key::Super, VK_LWIN}, {Key::RightControl, VK_RCONTROL}, {Key::RightAlt, VK_RMENU},
        {Key::RightShift, VK_RSHIFT}, {Key::RightSuper, VK_RWIN},
        
        // Navigation
        {Key::Up, VK_UP}, {Key::Down, VK_DOWN}, {Key::Left, VK_LEFT}, {Key::Right, VK_RIGHT},
        {Key::Home, VK_HOME}, {Key::End, VK_END}, {Key::PageUp, VK_PRIOR}, {Key::PageDown, VK_NEXT},
        {Key::Insert, VK_INSERT},
        
        // Numpad
        {Key::Numpad0, VK_NUMPAD0}, {Key::Numpad1, VK_NUMPAD1}, {Key::Numpad2, VK_NUMPAD2},
        {Key::Numpad3, VK_NUMPAD3}, {Key::Numpad4, VK_NUMPAD4}, {Key::Numpad5, VK_NUMPAD5},
        {Key::Numpad6, VK_NUMPAD6}, {Key::Numpad7, VK_NUMPAD7}, {Key::Numpad8, VK_NUMPAD8},
        {Key::Numpad9, VK_NUMPAD9}, {Key::NumpadMultiply, VK_MULTIPLY}, {Key::NumpadAdd, VK_ADD},
        {Key::NumpadSubtract, VK_SUBTRACT}, {Key::NumpadDivide, VK_DIVIDE}, 
        {Key::NumpadDecimal, VK_DECIMAL},
        
        // Others
        {Key::PrintScreen, VK_SNAPSHOT}, {Key::ScrollLock, VK_SCROLL}, {Key::Pause, VK_PAUSE},
        {Key::Menu, VK_APPS}
    };
    
    auto it = key_map.find(key);
    if (it != key_map.end()) {
        return it->second;
    }
    
    if (key == Key::Unicode) {
        // For Unicode, the VK should be 0
        return 0;
    }
    
    throw InputError(InputError::Type::Mapping, "Unsupported key");
}

/**
 * @brief Converts from Windows virtual key code to Bego's Key enum
 * 
 * @details This function performs the reverse mapping of key_to_vk(), converting
 * Windows-specific virtual key codes back to the library's platform-independent
 * Key enum values.
 * 
 * This conversion is useful for identifying keys from system events, enabling
 * the library to recognize and report keys in a platform-independent way.
 * 
 * @param vk The Windows virtual key code to convert
 * @return Key The corresponding Key enum value
 * @throws InputError If the virtual key code cannot be mapped to a Key enum value
 */
Key vk_to_key(WORD vk) {
    switch (vk) {
        // Letters
        case 'A': return Key::A;
        case 'B': return Key::B;
        case 'C': return Key::C;
        case 'D': return Key::D;
        case 'E': return Key::E;
        case 'F': return Key::F;
        case 'G': return Key::G;
        case 'H': return Key::H;
        case 'I': return Key::I;
        case 'J': return Key::J;
        case 'K': return Key::K;
        case 'L': return Key::L;
        case 'M': return Key::M;
        case 'N': return Key::N;
        case 'O': return Key::O;
        case 'P': return Key::P;
        case 'Q': return Key::Q;
        case 'R': return Key::R;
        case 'S': return Key::S;
        case 'T': return Key::T;
        case 'U': return Key::U;
        case 'V': return Key::V;
        case 'W': return Key::W;
        case 'X': return Key::X;
        case 'Y': return Key::Y;
        case 'Z': return Key::Z;
        
        // Numbers
        case '0': return Key::Num0;
        case '1': return Key::Num1;
        case '2': return Key::Num2;
        case '3': return Key::Num3;
        case '4': return Key::Num4;
        case '5': return Key::Num5;
        case '6': return Key::Num6;
        case '7': return Key::Num7;
        case '8': return Key::Num8;
        case '9': return Key::Num9;
        
        // Function keys
        case VK_F1: return Key::F1;
        case VK_F2: return Key::F2;
        case VK_F3: return Key::F3;
        case VK_F4: return Key::F4;
        case VK_F5: return Key::F5;
        case VK_F6: return Key::F6;
        case VK_F7: return Key::F7;
        case VK_F8: return Key::F8;
        case VK_F9: return Key::F9;
        case VK_F10: return Key::F10;
        case VK_F11: return Key::F11;
        case VK_F12: return Key::F12;
        case VK_F13: return Key::F13;
        case VK_F14: return Key::F14;
        case VK_F15: return Key::F15;
        case VK_F16: return Key::F16;
        case VK_F17: return Key::F17;
        case VK_F18: return Key::F18;
        case VK_F19: return Key::F19;
        case VK_F20: return Key::F20;
        case VK_F21: return Key::F21;
        case VK_F22: return Key::F22;
        case VK_F23: return Key::F23;
        case VK_F24: return Key::F24;
        
        // Special keys
        case VK_RETURN: return Key::Return;
        case VK_TAB: return Key::Tab;
        case VK_SPACE: return Key::Space;
        case VK_BACK: return Key::Backspace;
        case VK_ESCAPE: return Key::Escape;
        case VK_DELETE: return Key::Delete;
        case VK_CAPITAL: return Key::CapsLock;
        
        // Control keys
        case VK_CONTROL: return Key::Control;
        case VK_MENU: return Key::Alt;
        case VK_SHIFT: return Key::Shift;
        case VK_LWIN: return Key::Super;
        case VK_RCONTROL: return Key::RightControl;
        case VK_RMENU: return Key::RightAlt;
        case VK_RSHIFT: return Key::RightShift;
        case VK_RWIN: return Key::RightSuper;
        
        // Navigation
        case VK_UP: return Key::Up;
        case VK_DOWN: return Key::Down;
        case VK_LEFT: return Key::Left;
        case VK_RIGHT: return Key::Right;
        case VK_HOME: return Key::Home;
        case VK_END: return Key::End;
        case VK_PRIOR: return Key::PageUp;
        case VK_NEXT: return Key::PageDown;
        case VK_INSERT: return Key::Insert;
        
        // Numpad
        case VK_NUMPAD0: return Key::Numpad0;
        case VK_NUMPAD1: return Key::Numpad1;
        case VK_NUMPAD2: return Key::Numpad2;
        case VK_NUMPAD3: return Key::Numpad3;
        case VK_NUMPAD4: return Key::Numpad4;
        case VK_NUMPAD5: return Key::Numpad5;
        case VK_NUMPAD6: return Key::Numpad6;
        case VK_NUMPAD7: return Key::Numpad7;
        case VK_NUMPAD8: return Key::Numpad8;
        case VK_NUMPAD9: return Key::Numpad9;
        case VK_MULTIPLY: return Key::NumpadMultiply;
        case VK_ADD: return Key::NumpadAdd;
        case VK_SUBTRACT: return Key::NumpadSubtract;
        case VK_DIVIDE: return Key::NumpadDivide;
        case VK_DECIMAL: return Key::NumpadDecimal;
        
        // Others
        case VK_SNAPSHOT: return Key::PrintScreen;
        case VK_SCROLL: return Key::ScrollLock;
        case VK_PAUSE: return Key::Pause;
        case VK_APPS: return Key::Menu;
        
        default:
            throw InputError(InputError::Type::Mapping, "Unsupported virtual key code");
    }
}

} // namespace bego 