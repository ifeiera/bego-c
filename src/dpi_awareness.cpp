#include "../include/bego.h"
#include <Windows.h>
#include <ShellScalingApi.h>

/**
 * @file dpi_awareness.cpp
 * @author Eterninety
 * @brief Implementation of DPI awareness setting function
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

#pragma comment(lib, "Shcore.lib")

namespace bego {

/**
 * @brief Sets the process to be DPI aware
 * 
 * @details This function enables per-monitor DPI awareness for the application.
 * DPI awareness is critical for accurate mouse positioning on high-DPI displays,
 * ensuring that absolute mouse coordinates are properly scaled.
 * 
 * Without proper DPI awareness, mouse movements might be inaccurate on systems
 * with display scaling enabled, causing the mouse to move to incorrect positions.
 * 
 * Note that it's generally better to set DPI awareness in the application manifest
 * rather than programmatically, but this function is provided for convenience.
 * 
 * @return true If DPI awareness was successfully set
 * @return false If setting DPI awareness failed (possibly already set)
 */
bool set_dpi_awareness() {
    // Set the process to be per monitor DPI aware
    HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    
    // Return true if successful, false otherwise
    return SUCCEEDED(hr);
}

} // namespace bego 