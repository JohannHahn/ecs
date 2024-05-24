#include "ecs.h"
#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <iostream>

ECS::ECS() {
    component_masks = {};
}

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
    for(int i = 0; i < component_sizes.size(); ++i) {
	u64 size = component_sizes[i];
	components.push_back(malloc(size * component_masks.size()));
	std::cout << "created container with " << size * component_masks.size() << " byte chunks\n";
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
	memcpy(new_components, old_components, component_sizes[i] * component_masks.size() - 1);
	free(old_components);
	components[i] = new_components;
    }
}

void* ECS::read_component(u64 entity_id, u64 component_id, void* out) {
    assert(entity_id < component_masks.size());
    assert(component_id < components.size());
    u64 size = component_sizes[component_id];
    void* p = (void*)((u64)components[component_id] + size * entity_id);
    if (out) {
	memcpy(out, p, size);
    }
    return p;
}

void ECS::write_component(u64 entity_id, u64 component_id, void* data) {
    assert(entity_id < component_masks.size());
    assert(component_id < components.size());
    u64 size = component_sizes[component_id];
    byte* component_field = (byte*)components[component_id];
    component_field += size * entity_id;
    memcpy(component_field, data, size); 
}

bool ECS::check_components(u64 entity_id, u64 component_flags) {
    int bit = 0;
    while(component_flags) {
	if (bit_check(component_flags, bit)) {
	    if (!bit_check(component_masks[entity_id], bit)) return false;
	}
	component_flags <<= 1;
    } 
    return true;
}

u64 ECS::entity_count() {
    return component_masks.size();
}
u64 ECS::component_count() {
    return components.size();
}
