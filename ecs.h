/*
how it should work:
the ecs has a buffer for every component type/kind
entity ids are the index (hash?) into the buffer
*/
#pragma once
#include <inttypes.h>
#include <cassert>
#include <vector>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t  byte;

#define bit_check(map, bit) ((u64)map >> bit & (u64)1)
#define bit_set(map, bit) (map |= (u64)1 << bit)
#define bit_reset(map, bit) (map &= ~(u64(1) << bit)) 

class ECS {
public:
    ECS();
    ECS(u64 component_count, ...);
    void add_entity(u64 mask);
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
private:
    std::vector<u64> component_masks;
    std::vector<void*> components;
    std::vector<u64> component_sizes;

    void realloc_components();
    void free_components();
};




