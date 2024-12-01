#include "../include/resource_manager.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct resource_node
 * @brief Represents a single node in the resource manager's linked list.
 */
typedef struct resource_node {
    void *resource;                   // Pointer to the managed resource
    resource_cleanup_func_t cleanup;  // Cleanup function for the resource
    struct resource_node *next;       // Pointer to the next node in the list
} resource_node_t;

/**
 * @brief The global Resource Manager instance.
 *
 * This static instance manages all registered resources and their cleanup.
 */
static struct {
    _Atomic(resource_node_t *) head;  // Head of the linked list
    atomic_bool is_clearing;          // Atomic flag indicating clearing state
} global_resource_manager = {
    .head = NULL,
    .is_clearing = false,
};

bool resource_manager_register(void *resource, resource_cleanup_func_t cleanup) {
    if (!resource || !cleanup) {
        return false;  // Invalid input
    }

    // Allocate a new node for the resource
    resource_node_t *node = (resource_node_t *)malloc(sizeof(resource_node_t));
    if (!node) {
        return false;  // Memory allocation failed
    }

    // Initialize the node
    node->resource = resource;
    node->cleanup = cleanup;

    // Atomically insert the node at the head of the list
    do {
        node->next = atomic_load(&global_resource_manager.head);
    } while (!atomic_compare_exchange_weak(&global_resource_manager.head, &node->next, node));

    return true;
}

void resource_manager_unregister(void *resource) {
    if (!resource) {
        return;  // Nothing to unregister
    }

    // If clearing is in progress, skip unregistration
    if (atomic_load(&global_resource_manager.is_clearing)) {
        return;
    }

    resource_node_t **current = (resource_node_t **)&global_resource_manager.head;

    // Traverse the list and find the node to unregister
    while (*current) {
        if ((*current)->resource == resource) {
            resource_node_t *toDelete = *current;

            // Atomically remove the node
            *current = toDelete->next;
            free(toDelete);

            return;
        }

        current = &((*current)->next);
    }
}

void resource_manager_clear(void) {
    // Set the clearing flag
    atomic_store(&global_resource_manager.is_clearing, true);

    // Detach the list atomically
    resource_node_t *current = atomic_exchange(&global_resource_manager.head, NULL);

    // Traverse and clean up the list
    while (current) {
        if (current->cleanup && current->resource) {
            current->cleanup(current->resource);
        }

        resource_node_t *toDelete = current;
        current = current->next;
        free(toDelete);
    }

    // Clear the clearing flag
    atomic_store(&global_resource_manager.is_clearing, false);
}
