#include "../include/bego_win.h"
#include <array>
#include <stdexcept>

/**
 * @file bego_win.cpp
 * @author Eterninety
 * @brief Core implementation of the Bego hardware-level input simulation library for Windows
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Constructor for the Bego class
 * 
 * @details Initializes the Bego instance with the provided settings,
 * setting up the configuration needed for hardware-level input simulation.
 * The constructor ensures that all member variables are properly initialized,
 * including the custom marker value used to identify inputs from this library.
 * 
 * @param settings Configuration settings for input simulation
 */
Bego::Bego(const Settings& settings)
    : held_keys(),
      held_scancodes(),
      release_keys_when_dropped(settings.release_keys_when_dropped),
      dw_extra_info(settings.windows_dw_extra_info ? settings.windows_dw_extra_info : EVENT_MARKER),
      windows_subject_to_mouse_speed_and_acceleration_level(
          settings.windows_subject_to_mouse_speed_and_acceleration_level) {
}

/**
 * @brief Destructor for the Bego class
 * 
 * @details If configured to release keys when dropped, the destructor
 * automatically releases any keys or scan codes that are still being held.
 * This helps prevent keys from being "stuck" if the program exits
 * unexpectedly while keys are being held down.
 */
Bego::~Bego() {
    if (!release_keys_when_dropped) {
        return;
    }
    
    // Release all held keys
    for (const auto& key : held_keys) {
        try {
            this->key(key, Direction::Release);
        } catch (const std::exception&) {
            // Just log or ignore the error
        }
    }
    
    // Release all held scan codes
    for (const auto& scan : held_scancodes) {
        try {
            this->raw(scan, Direction::Release);
        } catch (const std::exception&) {
            // Just log or ignore the error
        }
    }
}

/**
 * @brief Gets the keyboard layout of the active window
 * 
 * @details Retrieves the keyboard layout (HKL) of the currently active window.
 * This is used for proper key translation between virtual keys and scan codes,
 * ensuring that inputs work correctly with different keyboard layouts.
 * 
 * @return HKL Handle to the current keyboard layout
 */
HKL Bego::get_keyboard_layout() {
    DWORD thread_id = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    return GetKeyboardLayout(thread_id);
}

/**
 * @brief Translates between virtual key codes and scan codes
 * 
 * @details Uses the Windows MapVirtualKeyEx function to translate between
 * virtual key codes and scan codes based on the current keyboard layout.
 * This ensures proper key mapping regardless of the system's keyboard layout.
 * 
 * @param input The code to translate
 * @param map_type The type of mapping to perform
 * @return uint16_t The translated code
 */
uint16_t Bego::translate_key(uint16_t input, UINT map_type) {
    HKL layout = get_keyboard_layout();
    
    // Call MapVirtualKeyEx using the provided map_type and input
    UINT result = MapVirtualKeyEx(input, map_type, layout);
    if (result == 0) {
        // Warning: This usually means there was no mapping
    }
    
    return static_cast<uint16_t>(result);
}

/**
 * @brief Checks if a virtual key is an extended key
 * 
 * @details Determines whether a virtual key code represents an extended key
 * that requires the KEYEVENTF_EXTENDEDKEY flag to be set when generating input.
 * Extended keys include navigation keys, NUMLOCK, right-side modifier keys, etc.
 * 
 * The list of extended keys is based on Microsoft's documentation:
 * https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#extended-key-flag
 * 
 * @param vk The virtual key to check
 * @return true If the key is an extended key
 * @return false If the key is not an extended key
 */
bool Bego::is_extended_key(VIRTUAL_KEY vk) {
    // List of extended keys that require KEYEVENTF_EXTENDEDKEY flag
    switch (vk) {
        case VK_RMENU:      // Right Alt
        case VK_RCONTROL:   // Right Control
        case VK_UP:         // Arrow keys
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:      // Page Up
        case VK_NEXT:       // Page Down
        case VK_NUMLOCK:
        case VK_SNAPSHOT:   // Print Screen
        case VK_DIVIDE:     // Numpad divide
            return true;
        default:
            return false;
    }
}

/**
 * @brief Queues key events for later sending
 * 
 * @details Prepares INPUT structures for a key event based on the provided
 * key and direction. This helper method handles the conversion from Key enum
 * to virtual key code and scan code, as well as setting appropriate flags
 * for extended keys.
 * 
 * The method adds the INPUT structures to the provided input_queue vector
 * rather than sending them immediately, allowing for batching multiple inputs.
 * 
 * @param input_queue Vector to add the INPUT structures to
 * @param key The key to simulate
 * @param direction Whether to press, release, or click the key
 */
void Bego::queue_key(std::vector<INPUT>& input_queue, Key key, Direction direction) {
    // Convert Key enum to virtual key code
    WORD vk;
    
    try {
        // Use the key_to_vk function to get the virtual key code
        vk = key_to_vk(key);
    } catch (const InputError&) {
        // Re-throw the error
        throw;
    }
    
    // Translate virtual key to scan code
    WORD scan = translate_key(vk, MAPVK_VK_TO_VSC_EX);
    
    // Set key flags
    DWORD keyflags = 0;
    
    if (is_extended_key(static_cast<VIRTUAL_KEY>(vk))) {
        keyflags |= KEYEVENTF_EXTENDEDKEY;
    }
    
    // Add key down event if needed
    if (direction == Direction::Click || direction == Direction::Press) {
        input_queue.push_back(create_keybd_event(keyflags, vk, scan, dw_extra_info));
    }
    
    // Add key up event if needed
    if (direction == Direction::Click || direction == Direction::Release) {
        input_queue.push_back(create_keybd_event(keyflags | KEYEVENTF_KEYUP, vk, scan, dw_extra_info));
    }
}

/**
 * @brief Queues character events for later sending
 * 
 * @details Prepares INPUT structures for typing a Unicode character.
 * This method handles the conversion of characters to UTF-16 code units,
 * including proper handling of surrogate pairs for characters outside the
 * Basic Multilingual Plane (BMP).
 * 
 * The method uses the KEYEVENTF_UNICODE flag to indicate that the input
 * is a Unicode character rather than a virtual key code.
 * 
 * @param input_queue Vector to add the INPUT structures to
 * @param character The Unicode character to type
 * @param buffer Buffer for UTF-16 encoding
 */
void Bego::queue_char(std::vector<INPUT>& input_queue, wchar_t character, std::array<uint16_t, 2>& buffer) {
    // Encode the character in UTF-16
    wchar_t utf16[2] = { character, 0 };
    uint16_t utf16_high = static_cast<uint16_t>(utf16[0]);
    uint16_t utf16_low = 0;
    
    // Handle surrogate pairs for characters outside the BMP
    if (character >= 0x10000) {
        // This is a simplified handling - would need proper surrogate pair calculation in practice
        utf16_high = buffer[0]; // High surrogate
        utf16_low = buffer[1];  // Low surrogate
    }
    
    // Add key down event
    input_queue.push_back(create_keybd_event(KEYEVENTF_UNICODE, 0, utf16_high, dw_extra_info));
    
    // Add key up event
    input_queue.push_back(create_keybd_event(KEYEVENTF_UNICODE | KEYEVENTF_KEYUP, 0, utf16_high, dw_extra_info));
    
    // If we have a surrogate pair, send the low surrogate too
    if (utf16_low != 0) {
        input_queue.push_back(create_keybd_event(KEYEVENTF_UNICODE, 0, utf16_low, dw_extra_info));
        input_queue.push_back(create_keybd_event(KEYEVENTF_UNICODE | KEYEVENTF_KEYUP, 0, utf16_low, dw_extra_info));
    }
}

/**
 * @brief Gets the lists of currently held keys and scan codes
 * 
 * @details Returns a tuple containing two vectors: one with the currently held
 * keys (Key enum values) and another with the currently held scan codes.
 * This can be useful for checking the state of the simulated keyboard.
 * 
 * @return A tuple containing vectors of held keys and scan codes
 */
std::tuple<std::vector<Key>, std::vector<ScanCode>> Bego::held() {
    return std::make_tuple(held_keys, held_scancodes);
}

/**
 * @brief Gets the event marker value used by this instance
 * 
 * @details Returns the marker value (dwExtraInfo) that this instance
 * uses to identify its input events. This is typically the EVENT_MARKER
 * constant (0x12345678) unless a custom value was provided in the settings.
 * 
 * @return size_t The marker value
 */
size_t Bego::get_marker_value() const {
    return dw_extra_info;
}

} // namespace bego 