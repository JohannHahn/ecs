#include "ecs.h"
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <iostream>

ECS::ECS(u64 component_count, ...) {
    // preallocate for 1 entity with no components
    component_masks = {};

    va_list args;
    va_start(args, component_count);
    // save component byte sizes
    for(int i = 0; i < component_count; ++i) {
	component_sizes.push_back(va_arg(args, u64));
    }
    va_end(args);
    realloc_components();
}

void ECS::realloc_components() {
    if (components.size() > 0) {
	free_components();
    }
    // add_entity will create the 
    for(int i = 0; i < component_sizes.size(); ++i) {
	components.push_back(nullptr);
    }
}

void ECS::free_components() {
    for (void* p : components) {
	free(p);
    }
    components.clear();
    std::cout << "components cleared" << "\n";
}

void ECS::add_entity(u64 mask) {
    component_masks.push_back(mask); 
    for(int i = 0; i < components.size(); ++i) {
	void* old_components = components[i];
	void* new_components = malloc(component_sizes[i] * component_masks.size());
	if (old_components) {  
	    memcpy(new_components, old_components, component_sizes[i] * component_masks.size() - 1);
	    free(old_components);
	}
	components[i] = new_components;
    }
}

bool ECS::check_components(u64 entity_id, u64 component_flags) {
    return component_flags & component_masks[entity_id];
}

u64 ECS::entity_count() {
    return component_masks.size();
}

u64 ECS::component_count() {
    return components.size();
}


