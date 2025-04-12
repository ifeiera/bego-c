#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <optional>

/**
 * @file bego.h
 * @author Eterninety
 * @brief Core declarations for the Bego hardware-level input simulation library
 * @version 1.0
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @enum Direction
 * @brief Specifies the direction of key or button activation
 */
enum class Direction {
    Click,    ///< Press and release in a single action
    Press,    ///< Press down and hold
    Release   ///< Release a previously pressed key/button
};

/**
 * @enum Button
 * @brief Available mouse buttons that can be simulated
 */
enum class Button {
    Left,        ///< Left mouse button
    Middle,      ///< Middle mouse button (wheel click)
    Right,       ///< Right mouse button
    Back,        ///< Back button (typically "X1")
    Forward,     ///< Forward button (typically "X2")
    ScrollUp,    ///< Scroll wheel up
    ScrollDown,  ///< Scroll wheel down
    ScrollLeft,  ///< Horizontal scroll left
    ScrollRight  ///< Horizontal scroll right
};

/**
 * @enum Axis
 * @brief Axis for scrolling or other directional input
 */
enum class Axis {
    Horizontal,  ///< Horizontal axis (left-right)
    Vertical     ///< Vertical axis (up-down)
};

/**
 * @enum Coordinate
 * @brief Specifies whether coordinates are absolute or relative
 */
enum class Coordinate {
    Abs,  ///< Absolute screen coordinates
    Rel   ///< Relative to current position
};

/**
 * @class InputError
 * @brief Exception class for input-related errors
 */
class InputError : public std::runtime_error {
public:
    /**
     * @enum Type
     * @brief The specific type of input error
     */
    enum class Type {
        Simulate,     ///< Error during simulation of input
        InvalidInput, ///< Invalid input parameter provided
        Mapping       ///< Error in mapping between keys/buttons
    };

    /**
     * @brief Construct a new Input Error object
     * @param type The type of error
     * @param message Descriptive error message
     */
    InputError(Type type, const std::string& message);
    
    /**
     * @brief Get the error type
     * @return Type The error type
     */
    Type get_type() const;

private:
    Type type;  ///< The type of error
};

/**
 * @class NewConError
 * @brief Exception for errors in creating new connections
 */
class NewConError : public std::runtime_error {
public:
    /**
     * @brief Construct a new connection error
     * @param message Descriptive error message
     */
    explicit NewConError(const std::string& message);
};

// Forward declaration for Key enum
enum class Key;

/**
 * @class Settings
 * @brief Configuration settings for input simulation
 * @details Controls behavior of the input simulation system
 */
class Settings {
public:
    /**
     * @brief Construct Settings with default values
     */
    Settings();

    /**
     * @brief Windows-specific extra information value
     * @details Sets the dwExtraInfo field in INPUT structures
     */
    size_t windows_dw_extra_info = 0;
    
    /**
     * @brief Whether to release held keys when object is destroyed
     */
    bool release_keys_when_dropped = true;
    
    /**
     * @brief Whether mouse movements are subject to Windows acceleration
     */
    bool windows_subject_to_mouse_speed_and_acceleration_level = false;
};

/**
 * @brief Special marker value for identifying events from this library
 * @details Used for dwExtraInfo field in INPUT structures
 */
constexpr unsigned int EVENT_MARKER = 0x12345678;

/**
 * @class Mouse
 * @brief Interface for mouse functionality
 */
class Mouse {
public:
    /**
     * @brief Virtual destructor for interface
     */
    virtual ~Mouse() = default;
    
    /**
     * @brief Simulate a mouse button press, release, or click
     * @param button The mouse button to simulate
     * @param direction The direction (press/release/click)
     */
    virtual void button(Button button, Direction direction) = 0;
    
    /**
     * @brief Simulate mouse wheel scrolling
     * @param length The amount to scroll
     * @param axis The axis to scroll on (horizontal/vertical)
     */
    virtual void scroll(int length, Axis axis) = 0;
    
    /**
     * @brief Move the mouse cursor to a position
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param coordinate Whether the coordinates are absolute or relative
     */
    virtual void move_mouse(int x, int y, Coordinate coordinate) = 0;
    
    /**
     * @brief Get the main display dimensions
     * @return A pair containing the width and height of the main display
     */
    virtual std::pair<int, int> main_display() = 0;
    
    /**
     * @brief Get the current mouse cursor position
     * @return A pair containing the x and y coordinates of the cursor
     */
    virtual std::pair<int, int> location() = 0;
};

/**
 * @class Keyboard
 * @brief Interface for keyboard functionality
 */
class Keyboard {
public:
    /**
     * @brief Virtual destructor for interface
     */
    virtual ~Keyboard() = default;
    
    /**
     * @brief Attempt to use a fast text entry method if available
     * @param text The text to enter
     * @return Optional value indicating if fast entry was used
     */
    virtual std::optional<bool> fast_text(const std::string& text) = 0;
    
    /**
     * @brief Type text by simulating individual key presses
     * @param text The string of text to type
     */
    virtual void text(const std::string& text) = 0;
    
    /**
     * @brief Simulate a key press, release, or click
     * @param key The key to simulate
     * @param direction Whether to press, release, or click the key
     */
    virtual void key(Key key, Direction direction) = 0;
    
    /**
     * @brief Send a raw keyboard scan code
     * @param scan The hardware scan code to send
     * @param direction Whether to press, release, or click the key
     */
    virtual void raw(uint16_t scan, Direction direction) = 0;
};

/**
 * @enum Key
 * @brief All keyboard keys that can be simulated
 * @details Based on Windows Virtual Key codes
 */
enum class Key {
    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // Numbers
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    
    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
    
    // Special keys
    Return, Tab, Space, Backspace, Escape, Delete, CapsLock,
    
    // Control keys
    Control, Alt, Shift, Super, RightControl, RightAlt, RightShift, RightSuper,
    
    // Navigation
    Up, Down, Left, Right, Home, End, PageUp, PageDown, Insert,
    
    // Numpad
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
    Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
    NumpadMultiply, NumpadAdd, NumpadSubtract, NumpadDivide, NumpadDecimal,
    
    // Others
    PrintScreen, ScrollLock, Pause, Menu,
    
    // Unicode character (for special characters)
    Unicode
};

/**
 * @brief Helper function to set DPI awareness (Windows-specific)
 * @return bool True if successful, false otherwise
 */
bool set_dpi_awareness();

} // namespace bego 