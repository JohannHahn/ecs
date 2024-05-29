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
typedef uint8_t  byte;

#define bit_check(map, bit) ((u64)map >> bit & (u64)1)
#define bit_set(map, bit) (map |= (u64)1 << bit)
#define bit_reset(map, bit) (map &= ~(u64(1) << bit)) 
#define array_at(array, index, size) (((byte*)array) + (index) * (size))

inline void print(void* elem, u64 elem_size = 0) {
    if (elem_size == 8) {
        Vector2 v = *(Vector2*)elem;
        std::cout << "vec = " << v.x << ", y = " << v.y << "\n";
    }
    if (elem_size == 4) {
        std::string out = "";
        u64 behavior = *(u64*)elem;
        if (behavior == 0) out += "Player control behavior\n";
        if (behavior == 1) out += "Seek behavior\n";
        std::cout << out << "\n";
    }
}

// removes elemnt from array
template<class T> inline void remove_element(std::vector<T>& array, u64 index) {
    std::cout << "---- removing element = " << index << " from vector\n";
    std::cout << "array size = " << array.size() << "\n";
    assert(index < array.size());
    u64 last_index = array.size() - 1;
    if (index < last_index) {
        array[index] = array[last_index]; 
    }
    array.pop_back();
}
inline void remove_malloced_element (void** array, u64 count, u64 component_size_bytes, u64 index) {
    assert(index < count && "trying to remove out of bounds\n");
    std::cout << "----------- remove malloced start ----------\n";
    std::cout << "removing entity from components at entity_id = " << index << ", component size for this contaier is " << component_size_bytes << " bytes\n";

    byte* element_to_delete = array_at(*array, index, component_size_bytes);
    byte* last_element = array_at(*array, count - 1, component_size_bytes);

    std::cout << "removing from array at adress = " << *array << " ending at adress " <<  (void*)(last_element + component_size_bytes) << "\n";
    print(element_to_delete, component_size_bytes);
    print(last_element, component_size_bytes);

    memmove(element_to_delete, element_to_delete + 1, component_size_bytes * (count - index));

    void* new_array = malloc(component_size_bytes * (count - 1));
    memcpy(new_array, *array, component_size_bytes * (count - 1));
    free(*array);
    print(new_array, component_size_bytes);
    *array = new_array;


}

class ECS {
public:
    ECS(u64 component_count, ...);
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
};
