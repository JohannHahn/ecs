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
template<class T> inline void remove_element(std::vector<T>& array, u64 index) {
    assert(index < array.size());
    T* data_pointer = array.data();
    memmove(data_pointer + index, data_pointer + index + 1, (array.size() - index) * sizeof(T));
    array.pop_back();
}

inline void remove_element (void** array, u64 count, u64 component_size_bytes, u64 index) {
    assert(index < count && "trying to remove out of bounds\n");
    byte* element_to_delete = array_at(*array, index, component_size_bytes);
    memmove(element_to_delete, element_to_delete + component_size_bytes, component_size_bytes * (count - index));
    void* new_array = malloc(component_size_bytes * (count - 1));
    memcpy(new_array, *array, component_size_bytes * (count - 1));
    free(*array);
    *array = new_array;
}

class ECS {
public:
    ECS(u64 component_count, u64 sizes[]); 
    ~ECS();
    u64 add_entity(u64 mask);
    template<class T> void write_component(u64 entity_id, u64 component_id, T data) {
        assert(entity_id < entity_count());
        assert(component_id < component_count());
        assert(components[component_id]);
        u64 size = component_sizes[component_id];
        T* component_field = (T*)components[component_id] + entity_id;
        memcpy(component_field, &data, size); 
    };
    template <class T> T read_component(u64 entity_id, u64 component_id) {
        assert(entity_id < entity_count());
        assert(component_id < component_count());
        assert(components[component_id]);
        T result = *((T*)components[component_id] + entity_id);
        return result;
    };
    u64 entity_count();
    u64 component_count();
    u64 size_of_component(u64 component_id);
    u64 get_component_mask(u64 entity_id);
    bool check_components(u64 entity_id, u64 component_flags);
    void remove_entity(u64 enitiy_id);
private:
    std::vector<u64> component_masks;
    std::vector<void*> components;
    std::vector<u64> component_sizes;

    void realloc_components();
    void free_components();
};

