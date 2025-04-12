#pragma once

#include "bego.h"
#include <tuple>
#include <Windows.h>

/**
 * @file bego_win.h
 * @author Eterninety
 * @brief Windows-specific implementation of the Bego input simulation library
 * @version 1.0
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Windows-specific constant for marking extended keys
 * @details Used to distinguish extended keys that require special handling in the Windows API
 */
constexpr uint16_t EXT = 0xFF00;

/**
 * @typedef ScanCode
 * @brief Hardware scan code type for direct keyboard input
 */
typedef uint16_t ScanCode;

/**
 * @typedef VIRTUAL_KEY
 * @brief Windows virtual key type alias for better readability
 */
using VIRTUAL_KEY = WORD;

/**
 * @brief Convert from Key enum to Windows Virtual Key code
 * @param key The Bego key enum value
 * @return WORD The corresponding Windows VK_ code
 */
WORD key_to_vk(Key key);

/**
 * @brief Convert from Windows Virtual Key code to Key enum
 * @param vk The Windows VK_ code
 * @return Key The corresponding Bego key enum value
 */
Key vk_to_key(WORD vk);

/**
 * @class Bego
 * @brief The main class for hardware-level input simulation on Windows
 * 
 * @details This class provides methods for simulating hardware-level keyboard and mouse inputs
 * using the Windows SendInput API. The generated inputs are indistinguishable from real physical
 * device inputs by most applications, including games with anti-cheat systems.
 */
class Bego : public Mouse, public Keyboard {
public:
    /**
     * @brief Construct a new Bego object with specified settings
     * @param settings Configuration settings for the input simulation
     */
    explicit Bego(const Settings& settings);
    
    /**
     * @brief Destroy the Bego object and release any held keys if configured
     */
    ~Bego() override;
    
    // Mouse interface implementations
    /**
     * @brief Simulate a mouse button press, release, or click
     * @param button The mouse button to simulate (Left, Right, Middle, etc.)
     * @param direction Whether to press, release, or click (press+release) the button
     */
    void button(Button button, Direction direction) override;
    
    /**
     * @brief Simulate mouse wheel scrolling
     * @param length The amount to scroll (positive or negative)
     * @param axis Whether to scroll vertically or horizontally
     */
    void scroll(int length, Axis axis) override;
    
    /**
     * @brief Move the mouse cursor to a position
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param coordinate Whether the coordinates are absolute or relative
     */
    void move_mouse(int x, int y, Coordinate coordinate) override;
    
    /**
     * @brief Get the main display dimensions
     * @return A pair containing the width and height of the main display
     */
    std::pair<int, int> main_display() override;
    
    /**
     * @brief Get the current mouse cursor position
     * @return A pair containing the x and y coordinates of the cursor
     */
    std::pair<int, int> location() override;
    
    // Keyboard interface implementations
    /**
     * @brief Attempt to use a fast text entry method if available
     * @param text The text to enter
     * @return Optional value indicating if fast entry was used
     */
    std::optional<bool> fast_text(const std::string& text) override;
    
    /**
     * @brief Type text by simulating individual key presses
     * @param text The string of text to type
     */
    void text(const std::string& text) override;
    
    /**
     * @brief Simulate a key press, release, or click
     * @param key The key to simulate
     * @param direction Whether to press, release, or click (press+release) the key
     */
    void key(Key key, Direction direction) override;
    
    /**
     * @brief Send a raw keyboard scan code
     * @param scan The hardware scan code to send
     * @param direction Whether to press, release, or click (press+release) the key
     */
    void raw(uint16_t scan, Direction direction) override;
    
    // Additional methods
    /**
     * @brief Get lists of currently held keys and scan codes
     * @return A tuple containing vectors of held keys and scan codes
     */
    std::tuple<std::vector<Key>, std::vector<ScanCode>> held();
    
    /**
     * @brief Get the event marker value used to identify inputs from this library
     * @return The marker value (typically 0x12345678)
     */
    size_t get_marker_value() const;
    
    // Static helpers
    /**
     * @brief Get the current keyboard layout
     * @return HKL handle to the current keyboard layout
     */
    static HKL get_keyboard_layout();
    
    /**
     * @brief Translate between scan codes and virtual keys
     * @param input The code to translate
     * @param map_type The type of mapping to perform
     * @return The translated code
     */
    static uint16_t translate_key(uint16_t input, UINT map_type);
    
    /**
     * @brief Check if a virtual key is an extended key requiring special flags
     * @param vk The virtual key to check
     * @return true if it's an extended key, false otherwise
     */
    static bool is_extended_key(VIRTUAL_KEY vk);

private:
    /**
     * @brief Queue key events for later sending
     * @details Adds the appropriate key events to the input queue based on direction
     * @param input_queue The queue to add events to
     * @param key The key to simulate
     * @param direction Whether to press, release, or click the key
     */
    void queue_key(std::vector<INPUT>& input_queue, Key key, Direction direction);
    
    /**
     * @brief Queue character events for later sending
     * @details Handles proper Unicode character simulation including surrogate pairs
     * @param input_queue The queue to add events to
     * @param character The character to simulate
     * @param buffer Buffer for UTF-16 encoding
     */
    void queue_char(std::vector<INPUT>& input_queue, wchar_t character, std::array<uint16_t, 2>& buffer);
    
    // Currently held keys
    std::vector<Key> held_keys;
    std::vector<ScanCode> held_scancodes;
    
    // Configuration
    /**
     * @brief Whether to automatically release held keys when object is destroyed
     */
    bool release_keys_when_dropped;
    
    /**
     * @brief Custom event marker value (typically 0x12345678)
     * @details Used to identify inputs generated by this library
     */
    size_t dw_extra_info;
    
    /**
     * @brief Whether mouse movements are subject to Windows acceleration
     */
    bool windows_subject_to_mouse_speed_and_acceleration_level;
};

/**
 * @brief Send input events to the system using Windows SendInput API
 * @details This is the core function that performs true hardware-level input simulation
 * @param input Vector of INPUT structures to send
 */
void send_input(const std::vector<INPUT>& input);

/**
 * @brief Create a mouse input event structure
 * @details Configures all fields needed for hardware-level mouse simulation
 * @param flags The type of mouse event (move, click, etc.)
 * @param data Additional data (scroll amount, button data)
 * @param dx X-coordinate or movement amount
 * @param dy Y-coordinate or movement amount
 * @param dw_extra_info Marker value to identify the event source
 * @return A fully configured INPUT structure for a mouse event
 */
INPUT create_mouse_event(DWORD flags, int data, int dx, int dy, size_t dw_extra_info);

/**
 * @brief Create a keyboard input event structure
 * @details Configures all fields needed for hardware-level keyboard simulation
 * @param flags The type of keyboard event (key down, key up, etc.)
 * @param vk The virtual key code
 * @param scan The hardware scan code
 * @param dw_extra_info Marker value to identify the event source
 * @return A fully configured INPUT structure for a keyboard event
 */
INPUT create_keybd_event(DWORD flags, WORD vk, WORD scan, size_t dw_extra_info);

} // namespace bego 