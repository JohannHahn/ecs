/*
how it should work:
the ecs has a buffer for every component type/kind
entity ids are the index (hash?) into the buffer
*/
#pragma once
#include <inttypes.h>
#include <vector>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t  byte;

#define get_attr(e, entity_id, component_id, type) (*(type*)(e.read_component(entity_id, component_id)))
#define set(e, entity_id, component_id, type, value) do { \
    type v = value; \
    e.write_component(entity_id, component_id, &value); \
}while(0)

#define bit_check(map, bit) ((u64)map >> bit & (u64)1)
#define bit_set(map, bit) (map |= (u64)1 << bit)
#define bit_reset(map, bit) (map &= ~(u64(1) << bit)) 

class ECS {
public:
    ECS();
    ECS(u64 component_count, ...);
    void add_entity(u64 mask);
    void write_component(u64 entity_id, u64 component_id, void* data); 
    void* read_component(u64 entity_id, u64 component_id, void* out = NULL);
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




