#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

#define GENERATE_CLEANUP_FUNC(func)          \
    static void func##_cleanup(void *pointer) { \
        func(pointer);                          \
    }                                           \

/**
 * @typedef resource_cleanup_func_t
 * @brief Function pointer type for resource cleanup functions.
 *
 * This function is called to release or clean up a registered resource.
 *
 * @param resource Pointer to the resource to be cleaned up.
 */
typedef void (*resource_cleanup_func_t)(void *resource);

/**
 * @brief Registers a resource with the resource manager.
 *
 * This function registers a dynamically allocated resource and associates
 * it with a cleanup function. The cleanup function will be called automatically
 * when the resource is unregistered or during a full cleanup.
 *
 * @param resource Pointer to the resource to register.
 * @param cleanup Function to clean up the resource.
 * @return true if the resource was successfully registered, false otherwise.
 */
bool resource_manager_register(void *resource, resource_cleanup_func_t cleanup);

/**
 * @brief Unregisters a resource from the resource manager and cleans it up.
 *
 * This function removes a resource from the manager and calls its associated
 * cleanup function to release any allocated resources.
 *
 * @param resource Pointer to the resource to unregister.
 *  the resource is removed without cleanup.
 */
void resource_manager_unregister(void *resource);

/**
 * @brief Frees all resources registered in the resource manager.
 *
 * This function is designed to handle the cleanup of all resources managed by
 * the resource manager. It is particularly useful in scenarios such as:
 * - **Hot Reload:** During development (e.g., in Flutter), where application
 *   state needs to be reset and resources reallocated to avoid memory leaks
 *   or dangling pointers.
 * - **Application Shutdown:** Ensures proper cleanup of resources during
 *   application termination to prevent resource leaks.
 *
 * Internally, this function calls `resource_manager_clear`, which:
 * - Iterates through all currently registered resources.
 * - Executes the cleanup function for each registered resource.
 * - Frees the memory associated with the resource manager.
 *
 * @note After calling this function, previously registered resources become invalid.
 * Ensure resources are reinitialized if this function is used during a hot reload.
 *
 *
 * This function iterates through all resources managed by the resource manager,
 * calls their associated cleanup functions, and removes them from the manager.
 *
 * Use this function during application shutdown or hot reload to ensure all
 * resources are properly released.
 */
void resource_manager_clear(void);

#endif  // RESOURCE_MANAGER_H
