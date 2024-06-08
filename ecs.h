/*
how it should work:
the ecs has a buffer for every component type/kind
entity ids are the index (hash?) into the buffer
*/
#pragma once
#include <cstring>
#include <inttypes.h>
#include <cassert>
#include <iostream>
#include <vector>
#include "raylib/src/raylib.h"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  byte;

#define bit_check(map, bit) ((u64)map >> bit & (u64)1)
#define bit_set(map, bit) (map |= (u64)1 << bit)
#define bit_reset(map, bit) (map &= ~(u64(1) << bit)) 
#define array_at(array, index, size) (((byte*)array) + (index) * (size))

// removes elemnt from array

class ECS {
public:
    ECS(u64 component_count, u64 sizes[]); 
    u64 add_entity(u64 mask);
    template<class T> void write_component(u64 entity_id, u64 component_id, T data) {
        assert(entity_id < component_masks.size());
        assert(component_id < components.size());
        u64 size = component_sizes[component_id];
        T* component_field = (T*)components[component_id] + entity_id;
        memcpy(component_field, &data, size); 
    };
    template <class T> T read_component(u64 entity_id, u64 component_id) {
        assert(entity_id < component_masks.size());
        assert(component_id < components.size());
        T result = *((T*)components[component_id] + entity_id);
        return result;
    };
    u64 entity_count();
    u64 component_count();
    bool check_components(u64 entity_id, u64 component_flags);
    void remove_entity(u64 enitiy_id);
    u64 size_of_component(u64 component_id);
private:
    std::vector<u64> component_masks;
    std::vector<void*> components;
    std::vector<u64> component_sizes;

    void realloc_components();
    void free_components();
    void remove_malloced_element (void** array, u64 count, u64 component_size_bytes, u64 index);
};
