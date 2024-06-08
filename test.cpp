#include "iostream"
#include <cassert>
#include <string>
#include "ecs.h"

u64 component_sizes[] = {sizeof(byte), sizeof(u16), sizeof(u32), sizeof(u64)};
u64 component_count = sizeof(component_sizes) / sizeof(u64);
ECS ecs(component_count, component_sizes);
enum component_masks {
    BYTE = 1,
    U16 = 2,
    U32 = 4,
    U64 = 8,
    EMPTY = 16,
};


void log(bool pass, const char* msg) {
    std::string output = pass ? "TEST PASSED: " : "TEST FAILED!!: "; 
    output += msg;
    std::cout << output << "\n";
}

u64 component_masks[] = {EMPTY, BYTE, BYTE | U16, BYTE | U16 | U32 | U64, U64 | EMPTY};
u64 entity_count = sizeof(component_masks) / sizeof(u64);
byte bytes[] = {0, 1, 2, 3, 4};
u16 u16s[] = {5, 6, 7, 8, 9};
u32 u32s[] = {10, 11, 12, 13, 14};
u64 u64s[] = {100, 110, 120, 130, 140};

void get_data(byte& b, u16& b2, u32& b4, u64& b8, u64 entity_id) {
    b = ecs.read_component<byte>(entity_id, 0);
    b2 = ecs.read_component<u16>(entity_id, 1);
    b4 = ecs.read_component<u32>(entity_id, 2);
    b8 = ecs.read_component<u64>(entity_id, 3);
}


bool test_read_write_component() {
    bool result = true;
    byte b; u16 b2; u32 b4; u64 b8;
    std::string out = "failed at ";
    for (int i = 0; i < entity_count; ++i) {
	ecs.write_component(i, 0, bytes[i]);
	ecs.write_component(i, 1, u16s[i]);
	ecs.write_component(i, 2, u32s[i]);
	ecs.write_component(i, 3, u64s[i]);
    }
    for (int i = 0; i < entity_count; ++i) {
	get_data(b, b2, b4, b8, i);
	if (b != bytes[i]) {
	    out += "byte, entity = " + std::to_string(i) + "\n"; 
	    result = false;
	}
	if (b2 != u16s[i]) {
	    out += "u16, entity = " + std::to_string(i) + "\n"; 
	    result = false;
	}
	if (b4 != u32s[i]) {
	    out += "u32, entity = " + std::to_string(i) + "\n"; 
	    result = false;
	}
	if (b8 != u64s[i]) {
	    out += "u64, entity = " + std::to_string(i) + "\n"; 
	    result = false;
	}
    } 
    if (!result) std::cout << out;
    return result;
}

bool test_entity_id_after_add(int start, int end) {
    bool entity_id_pass = true;
    for (int i = start; i < end; ++i) {
	int id = ecs.add_entity(component_masks[i]);
	if (id != i) { 
	    entity_id_pass = false;
	    std::cout << "entity id test failed at i = " << i << "\n";
	}
    }
    return entity_id_pass;
}

bool test_entities_have_components() {
    bool result = true;
    for (int i = 0; i < ecs.entity_count(); ++i) {
	if (!ecs.check_components(i, component_masks[i])) {
	    std::cout << "test entities have components failed at entity " << i << "\n";
	    result = false;
	}
    }
    return result;
}

bool test_entities_count(u64 expected_count) {
    return expected_count == ecs.entity_count();
}

bool test_component_count(u64 expected_count) {
    return expected_count == ecs.component_count();
}

int main() {

    assert((sizeof(bytes) / sizeof(byte)) == entity_count && "array size doesn't match");

    log(test_component_count(component_count), "component count after initialization");
    log(test_entity_id_after_add(0, entity_count), "return entity id after initialization");
    log(test_entities_count(entity_count), "entity count after initialization");
    log(test_entities_have_components(), "entities have components after initialization");
    log(test_read_write_component(), "component read write");
    log(test_entities_have_components(), "entities have components after writing data");

    return 0;
}
