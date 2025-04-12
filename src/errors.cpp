#include "../include/bego.h"

/**
 * @file errors.cpp
 * @author Eterninety
 * @brief Implementation of custom error classes for input simulation
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Constructs an InputError with specified type and message
 * 
 * @details Creates an exception object that indicates a problem with input simulation.
 * The error type categorizes the issue (simulation error, invalid input, or mapping error),
 * which helps with proper error handling in client code.
 * 
 * @param type The specific type of input error
 * @param message A descriptive error message
 */
InputError::InputError(Type type, const std::string& message)
    : std::runtime_error(message), type(type) {
}

/**
 * @brief Gets the error type for an InputError
 * 
 * @details Allows client code to determine the specific category of the error
 * to handle different errors appropriately.
 * 
 * @return Type The specific error type
 */
InputError::Type InputError::get_type() const {
    return type;
}

/**
 * @brief Constructs a NewConError with a specified message
 * 
 * @details Creates an exception object that indicates a problem with creating
 * a new connection for input simulation. This typically happens when initializing
 * the input simulation system.
 * 
 * @param message A descriptive error message
 */
NewConError::NewConError(const std::string& message)
    : std::runtime_error(message) {
}

} // namespace bego 