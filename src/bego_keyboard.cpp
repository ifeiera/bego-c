#include "../include/bego_win.h"
#include <array>
#include <vector>
#include <string>

/**
 * @file bego_keyboard.cpp
 * @author Eterninety
 * @brief Implementation of hardware-level keyboard simulation functions
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Attempts to use a fast text entry method if available
 * 
 * @details This implementation returns std::nullopt to indicate that no special
 * fast text method is available, falling back to regular text input.
 * A custom implementation could be added for Windows to use clipboard or other methods.
 * 
 * @param text The text to enter
 * @return std::optional<bool> std::nullopt indicating no fast method available
 */
std::optional<bool> Bego::fast_text(const std::string& text) {
    // Return None to indicate that we don't have a special implementation
    return std::nullopt;
}

/**
 * @brief Simulates typing text by sending individual keyboard events
 * 
 * @details This method simulates typing text by generating hardware-level keyboard events
 * for each character. It handles special characters like newlines, tabs, and carriage returns
 * by mapping them to the appropriate key events.
 * 
 * For regular characters, it uses queue_char which handles proper Unicode conversion
 * and sends appropriate keyboard events. The implementation handles text at
 * the hardware level, making it compatible with applications that use low-level
 * input detection.
 * 
 * @param text The string of text to type
 * @throws InputError If the text contains a null byte
 */
void Bego::text(const std::string& text) {
    if (text.empty()) {
        return; // Nothing to simulate
    }
    
    std::vector<INPUT> input;
    input.reserve(2 * text.size()); // Each char needs at least press and release
    
    std::array<uint16_t, 2> buffer;
    
    for (char c : text) {
        // Convert char to wchar_t for proper Unicode handling
        wchar_t wc = static_cast<wchar_t>(c);
        
        // Handle special characters
        switch (c) {
            case '\n':
                queue_key(input, Key::Return, Direction::Click);
                break;
            case '\r':
                // Carriage return is mapped to the Enter key in Windows
                queue_key(input, Key::Return, Direction::Click);
                break;
            case '\t':
                queue_key(input, Key::Tab, Direction::Click);
                break;
            case '\0':
                throw InputError(InputError::Type::InvalidInput, "The text contained a null byte");
            default:
                queue_char(input, wc, buffer);
                break;
        }
    }
    
    // Send all the queued input events
    send_input(input);
}

/**
 * @brief Simulates pressing, releasing, or clicking a key
 * 
 * @details This method generates hardware-level key events based on the specified key
 * and direction. For presses and releases, it also maintains a list of held keys
 * so they can be properly released if needed.
 * 
 * The method uses the queue_key helper to generate the appropriate INPUT structures
 * and then sends them using the send_input function. This approach ensures that
 * the key events are indistinguishable from real hardware key events.
 * 
 * @param key The key to simulate
 * @param direction Whether to press, release, or click (press+release) the key
 */
void Bego::key(Key key, Direction direction) {
    std::vector<INPUT> input;
    
    // Queue the key event(s)
    queue_key(input, key, direction);
    
    // Send the input events
    send_input(input);
    
    // Update held keys
    switch (direction) {
        case Direction::Press:
            held_keys.push_back(key);
            break;
        case Direction::Release:
            // Remove the key from held keys
            held_keys.erase(
                std::remove(held_keys.begin(), held_keys.end(), key),
                held_keys.end()
            );
            break;
        case Direction::Click:
            // No need to update held keys for click
            break;
    }
}

/**
 * @brief Sends raw keyboard scan codes directly to the system
 * 
 * @details This method provides the lowest-level keyboard input capability,
 * allowing direct sending of hardware scan codes. It automatically translates
 * between scan codes and virtual keys, and handles extended key flags.
 * 
 * The implementation uses the KEYEVENTF_SCANCODE flag to indicate that
 * scan codes are being used, which ensures maximum compatibility with applications
 * that might inspect the hardware-level details of input events.
 * 
 * This method also maintains a list of held scan codes, similar to the key method.
 * 
 * @param scan The hardware scan code to send
 * @param direction Whether to press, release, or click (press+release) the key
 */
void Bego::raw(uint16_t scan, Direction direction) {
    std::vector<INPUT> input;
    
    // Translate scan code to virtual key
    WORD vk = translate_key(scan, MAPVK_VSC_TO_VK_EX);
    
    // Set key flags
    DWORD keyflags = KEYEVENTF_SCANCODE;
    
    // Check if it's an extended key
    if (is_extended_key(static_cast<VIRTUAL_KEY>(vk))) {
        keyflags |= KEYEVENTF_EXTENDEDKEY;
    }
    
    // Add key down event if needed
    if (direction == Direction::Click || direction == Direction::Press) {
        input.push_back(create_keybd_event(keyflags, vk, scan, dw_extra_info));
    }
    
    // Add key up event if needed
    if (direction == Direction::Click || direction == Direction::Release) {
        input.push_back(create_keybd_event(keyflags | KEYEVENTF_KEYUP, vk, scan, dw_extra_info));
    }
    
    // Send the input events
    send_input(input);
    
    // Update held scan codes
    switch (direction) {
        case Direction::Press:
            held_scancodes.push_back(scan);
            break;
        case Direction::Release:
            // Remove the scan code from held scan codes
            held_scancodes.erase(
                std::remove(held_scancodes.begin(), held_scancodes.end(), scan),
                held_scancodes.end()
            );
            break;
        case Direction::Click:
            // No need to update held scan codes for click
            break;
    }
}

} // namespace bego 