#include "iostream"
#include <cassert>
#include <string>
#include "ecs.h"

enum component_mask {
    BYTE = 1,
    U16 = 2,
    U32 = 4,
    U64 = 8,
    EMPTY = 16,
};

enum log_level {
    TEST_FAIL,
    TEST_PASS,
    ERROR,
    NORMAL
};

struct State {
    std::vector<u64> component_masks;
    std::vector<byte> bytes;
    std::vector<u16> u16s;
    std::vector<u32> u32s;
    std::vector<u64> u64s;
};

u64 component_sizes[] = {sizeof(byte), sizeof(u16), sizeof(u32), sizeof(u64)};
const u64 component_count = sizeof(component_sizes) / sizeof(u64);


void log(log_level log_level, std::string msg) {
    std::string output = ""; 
    switch (log_level) {

    case TEST_FAIL:
	output = "TEST FAILED: ";
	break;
    case TEST_PASS:
	output = "TEST PASSED: ";
	break;
    case ERROR:
	output = "ERROR: ";
	break;
    case NORMAL:
	output = "LOG: ";
	break;
      break;
    }
    output += msg;
    std::cout << output << "\n";
}

void log(bool pass, std::string msg) {
    log(pass ? TEST_PASS : TEST_FAIL, msg);
}

bool valid_state(const State& state) {
    bool entity_match = 
	state.component_masks.size() == state.bytes.size() && state.component_masks.size() == state.u16s.size()
	&& state.component_masks.size() == state.u32s.size() && state.component_masks.size() == state.u64s.size(); 
    if (!entity_match) log(ERROR, "entity count does not match");
    return entity_match;
}
bool compare_state(State& state, ECS& ecs) {
    if (!valid_state(state)) return false;
    if (state.component_masks.size() != ecs.entity_count()) {
	 std::cout << "different amout of entities!\n";
	 return false;
    }
    bool component_masks_match = true;
    for (int i = 0; i < ecs.entity_count(); ++i) {	
	if (ecs.get_component_mask(i) != state.component_masks[i]) {
	    component_masks_match = false;
	    log(ERROR, "different component mask at i = " + std::to_string(i));
	    break;
	}
    }
    bool bytes_match = true;
    for (int i = 0; i < ecs.entity_count(); ++i) {	
	if (ecs.read_component<byte>(i, 0) != state.bytes[i]) {
	    bytes_match = false;
	    log(ERROR, "different byte at i = " + std::to_string(i));
	    break;
	}
    }

    bool u16_match = true;
    for (int i = 0; i < ecs.entity_count(); ++i) {	
	if (ecs.read_component<u16>(i, 1) != state.u16s[i]) {
	    u16_match = false;
	    log(ERROR, "different u16 at i = " + std::to_string(i));
	    break;
	}
    }
    bool u32_match = true;
    for (int i = 0; i < ecs.entity_count(); ++i) {	
	if (ecs.read_component<u32>(i, 2) != state.u32s[i]) {
	    u32_match = false;
	    log(ERROR, "different u32 at i = " + std::to_string(i));
	    break;
	}
    }
    bool u64_match = true;
    for (int i = 0; i < ecs.entity_count(); ++i) {	
	if (ecs.read_component<u64>(i, 3) != state.u64s[i]) {
	    u64_match = false;
	    log(ERROR, "different u64 at i = " + std::to_string(i));
	    break;
	}
    }
    return component_masks_match && bytes_match && u16_match && u32_match && u64_match;
}



void get_data(ECS& ecs, byte& b, u16& b2, u32& b4, u64& b8, u64 entity_id) {
    b = ecs.read_component<byte>(entity_id, 0);
    b2 = ecs.read_component<u16>(entity_id, 1);
    b4 = ecs.read_component<u32>(entity_id, 2);
    b8 = ecs.read_component<u64>(entity_id, 3);
}

void add_entity_to_ecs(ECS& ecs, u64 mask, byte b, u16 s, u32 i, u64 l) {
    u64 id = ecs.add_entity(mask);
    ecs.write_component(id, 0, b);
    ecs.write_component(id, 1, s);
    ecs.write_component(id, 2, i);
    ecs.write_component(id, 3, l);
}

void add_entity_to_state(State& state, u64 mask, byte b, u16 s, u32 i, u64 l) {
    state.component_masks.push_back(mask);
    state.bytes.push_back(b);
    state.u16s.push_back(s);
    state.u32s.push_back(i);
    state.u64s.push_back(l);
}

void remove_entity_from_state(State& state, u64 entity_id) {
    remove_element(state.component_masks, entity_id);
    remove_element(state.bytes, entity_id);
    remove_element(state.u16s, entity_id);
    remove_element(state.u32s, entity_id);
    remove_element(state.u64s, entity_id);
}

bool test_read_write_component(ECS& ecs) {
    bool result = true;
    //byte b; u16 b2; u32 b4; u64 b8;
    //std::string out = "failed at ";
    //for (int i = 0; i < entity_count; ++i) {
    //    ecs.write_component(i, 0, bytes[i]);
    //    ecs.write_component(i, 1, u16s[i]);
    //    ecs.write_component(i, 2, u32s[i]);
    //    ecs.write_component(i, 3, u64s[i]);
    //}
    //for (int i = 0; i < entity_count; ++i) {
    //    get_data(ecs, b, b2, b4, b8, i);
    //    if (b != bytes[i]) {
    //        out += "byte, entity = " + std::to_string(i) + "\n"; 
    //        result = false;
    //    }
    //    if (b2 != u16s[i]) {
    //        out += "u16, entity = " + std::to_string(i) + "\n"; 
    //        result = false;
    //    }
    //    if (b4 != u32s[i]) {
    //        out += "u32, entity = " + std::to_string(i) + "\n"; 
    //        result = false;
    //    }
    //    if (b8 != u64s[i]) {
    //        out += "u64, entity = " + std::to_string(i) + "\n"; 
    //        result = false;
    //    }
    //} 
    //if (!result) std::cout << out;
    return result;
}

ECS ecs_from_state(const State& state) {
    ECS ecs(component_count, component_sizes);
    for (int i = 0; i < state.component_masks.size(); ++i) {
	ecs.add_entity(state.component_masks[i]);
	ecs.write_component(i, 0, state.bytes[i]);
	ecs.write_component(i, 1, state.u16s[i]);
	ecs.write_component(i, 2, state.u32s[i]);
	ecs.write_component(i, 3, state.u64s[i]);
    }
    return ecs;
}

void test_create_ecs_from_state() {
    State state = {
	.component_masks = {EMPTY, BYTE, BYTE | U16, BYTE | U16 | U32 | U64, U64 | EMPTY},
	.bytes = {0, 1, 2, 3, 4},
	.u16s = {5, 6, 7, 8, 9},
	.u32s = {10, 11, 12, 13, 14},
	.u64s = {100, 110, 120, 130, 140}};
    ECS ecs = ecs_from_state(state);
    log(compare_state(state, ecs), "initial compare");
}

void test_add_entity(u64 max = 10) {
    State state = {
	.component_masks = {},
	.bytes = {},
	.u16s = {},
	.u32s = {},
	.u64s = {},
    };
    ECS ecs(component_count, component_sizes);
    for (int i = 0; i < max; ++i) {
	add_entity_to_ecs(ecs, U32, 0, 0, i, 0);
	add_entity_to_state(state, U32, 0, 0, i, 0);
    }
    log(compare_state(state, ecs), "adding entities");
}

void test_remove_entity() {
    State state = {
	.component_masks = {EMPTY, BYTE, BYTE | U16, BYTE | U16 | U32 | U64, U64 | EMPTY},
	.bytes = {0, 1, 2, 3, 4},
	.u16s = {5, 6, 7, 8, 9},
	.u32s = {10, 11, 12, 13, 14},
	.u64s = {100, 110, 120, 130, 140}};
    ECS ecs = ecs_from_state(state);
    bool result = compare_state(state, ecs);
    if (!result) log(ERROR, "test remove entity : state and ecs do not match after creation");
    for (int i = 0; i < ecs.entity_count(); ++i) {
	ecs.remove_entity(0);
	remove_entity_from_state(state, 0);
	if (!compare_state(state, ecs)) {
	    result = false;
	    log(ERROR, "test remove entities failed at i = " + std::to_string(i));
	    break;
	}
    }
    log(result, "test remove entity");
}

int main() {

    test_create_ecs_from_state();
    test_add_entity(1000);
    test_remove_entity();
    return 0;
}
