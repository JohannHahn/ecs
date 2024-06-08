#include "ecs.h"
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <iostream>

template<class T> inline void remove_element(std::vector<T>& array, u64 index) {
    assert(index < array.size());
    u64 last_index = array.size() - 1;
    if (index < last_index) {
        array[index] = array[last_index]; 
    }
    array.pop_back();
}

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
	free(p);
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
	remove_malloced_element(&components[i], entity_count_before, component_sizes[i], entity_id);
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

void ECS::remove_malloced_element (void** array, u64 count, u64 component_size_bytes, u64 index) {
    assert(index < count && "trying to remove out of bounds\n");
    byte* element_to_delete = array_at(*array, index, component_size_bytes);
    memmove(element_to_delete, element_to_delete + component_size_bytes, component_size_bytes * (count - index));
    void* new_array = malloc(component_size_bytes * (count - 1));
    memcpy(new_array, *array, component_size_bytes * (count - 1));
    free(*array);
    *array = new_array;
}
