#include "ecs.h"
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <iostream>

ECS::ECS(u64 component_count, u64 sizes[]){
    // preallocate for 1 entity with no components
    component_masks = {};

    // save component byte sizes
    for(int i = 0; i < component_count; ++i) {
	component_sizes.push_back(sizes[i]);
    }
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
	if (p) free(p);
    }
    components.clear();
}

u64 ECS::add_entity(u64 mask) {
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
    return component_masks.size() - 1;
}


void ECS::remove_entity(u64 entity_id) {
    assert(entity_id < component_masks.size());
    u64 entity_count_before = entity_count();
    for (int i = 0; i < components.size(); ++i) {
	void* p = components[i];
	remove_element(&components[i], entity_count_before, component_sizes[i], entity_id);
    }
    remove_element(component_masks, entity_id);
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

u64 ECS::get_component_mask(u64 entity_id) {
    assert(entity_id < entity_count());
    return component_masks[entity_id];
}


ECS::~ECS() {
    for(void* p : components) {
	free(p);
    }
}
