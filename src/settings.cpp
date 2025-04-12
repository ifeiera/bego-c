#include "../include/bego.h"

/**
 * @file settings.cpp
 * @author Eterninety
 * @brief Implementation of the Settings class for configuring input simulation
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Constructor for the Settings class
 * 
 * @details Initializes a Settings object with default values that are suitable
 * for most usage scenarios. The defaults are set to produce the most accurate
 * hardware-level input simulation:
 * 
 * - EVENT_MARKER (0x12345678) as the extra information for input events
 * - Keys are automatically released when the Bego object is destroyed
 * - Mouse movements bypass Windows acceleration for more predictable behavior
 * 
 * These defaults can be modified after construction if needed.
 */
Settings::Settings() 
    : windows_dw_extra_info(EVENT_MARKER),
      release_keys_when_dropped(true),
      windows_subject_to_mouse_speed_and_acceleration_level(false) {
}

} // namespace bego 