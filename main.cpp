#include <cassert>
#include <iostream>
#include "raylib/src/raylib.h"
#include "raylib/src/raymath.h"
#include "ecs.h"


float window_width = 900.f;
float window_height = 900.f;
float width = 100;
const char* window_title = "ECS";

float speed = 100.f;
float enemy_speed = 100.f;

enum Component_Flag{
    POSITION_FLAG = 1, VELOCITY_FLAG = 2, BEHAVIOUR_FLAG = 4
};

enum Component_Index{
    POSITION_INDEX, VELOCITY_INDEX, BEHAVIOUR_INDEX, COMPONENT_INDEX_MAX
};

enum Entity_kind {
    PLAYER, ENEMY, 
};

enum Behaviour {
    PLAYER_CONTROL, SEEK,
};

//				POSITION	VELOCITY        BEHAVIOUR	
ECS ecs(COMPONENT_INDEX_MAX, sizeof(Vector2), sizeof(Vector2), sizeof(Behaviour));

void set_components(u64 entity_id, Vector2 position, Vector2 velocity, Behaviour behaviour) {
    ecs.write_component(entity_id, POSITION_INDEX, position);    
    ecs.write_component(entity_id, VELOCITY_INDEX, velocity);    
    ecs.write_component(entity_id, BEHAVIOUR_INDEX, behaviour);    
}

void remove_enemy(u64 entity_id = 0) {
    if (!entity_id) {
	if (ecs.entity_count() == 1) {
	    std::cout << "nothing to delete\n";
	    return;
	}
	entity_id = GetRandomValue(1, ecs.entity_count() - 1);
    }
    std::cout << "entity_id = " << entity_id << "\n";
    ecs.remove_entity(entity_id);
}

void add_enemy() {
    std::cout << "added entity with id = " << ecs.add_entity(POSITION_FLAG | VELOCITY_FLAG) << "\n";
    Vector2 position = {(float)GetRandomValue(0, window_width), (float)GetRandomValue(0, window_height)};
    Vector2 velocity = {.0, .0}; 
    set_components(ecs.entity_count() - 1, position, velocity, SEEK);
}

void movement_system() {   
    for(int i = 0; i < ecs.entity_count(); ++i) {
	if (!ecs.check_components(i, POSITION_FLAG | VELOCITY_FLAG)) continue; 
	Vector2 pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	Vector2 vel = ecs.read_component<Vector2>(i, VELOCITY_INDEX);
	vel = Vector2Scale(vel, GetFrameTime());
	pos = Vector2Add(pos, vel);
	ecs.write_component(i, POSITION_INDEX, pos);
    } 
}

void control_system() {
    int key = GetKeyPressed();
    Vector2 vel = {0};
    if (IsKeyDown(KEY_LEFT)) {
	vel = Vector2Add(vel, {-speed, 0.f});
    }
    if (IsKeyDown(KEY_RIGHT)) {
	vel = Vector2Add(vel, {speed, 0.f});
    }
    if (IsKeyDown(KEY_UP)) {
	vel = Vector2Add(vel, {0.f, -speed});
    }
    if (IsKeyDown(KEY_DOWN)) {
	vel = Vector2Add(vel, {0.f, speed});
    }
    ecs.write_component(PLAYER, VELOCITY_INDEX, vel);
    for (int i = 1; i < ecs.entity_count(); ++i) {
	if (ecs.read_component<Behaviour>(i, BEHAVIOUR_INDEX) == SEEK) { 
	    Vector2 player_pos = {(float)GetRandomValue(0, window_width),
				  (float)GetRandomValue(0, window_height)};//ecs.read_component<Vector2>(PLAYER, POSITION_INDEX);
	    Vector2 pos = ecs.read_component<Vector2>(i, POSITION_INDEX);
	    Vector2 vel = Vector2Subtract(player_pos, pos);
	    ecs.write_component(i, VELOCITY_INDEX, vel); 
	}
    }

    if (IsKeyReleased(KEY_T)) {
	add_enemy();
    }
    if (IsKeyReleased(KEY_F)) {
	remove_enemy();
    }
}

void draw() {
    Vector2 player_pos = ecs.read_component<Vector2>(PLAYER, POSITION_INDEX);
    DrawRectangleV(Vector2AddValue(player_pos, -0.5 * width), {width, width}, BLUE);
    for (int i = 1; i < ecs.entity_count(); ++i) {
	DrawCircleV(ecs.read_component<Vector2>(i, POSITION_INDEX), 10.f, RED);
    }
}

int main() {
    InitWindow(window_width, window_height, window_title);
    SetTargetFPS(60);

    //Player
    ecs.add_entity(POSITION_FLAG | VELOCITY_FLAG);
    set_components(PLAYER, {0, 0}, {0, 0}, PLAYER_CONTROL);

    #if 0 
    std::vector<u64> n = {1, 2, 3, 4, 5};
    void* p = malloc(sizeof(u64) * 5);
    for (u64 i = 1; i < 6; ++i) {
	memcpy((byte*)p + (i-1) * sizeof(u64), &i, sizeof(i));
    }
    int index = GetRandomValue(0, 4);
    remove_element(n, index);
    std::cout << "numbers in vector \n";
    for (u64 num : n) std::cout << num << "\n";
    remove_malloced_element(p, 5, sizeof(u64), index);
    std::cout << "numbers in malloced array: \n";
    for (int i = 0; i < 4; ++i) {
	std::cout << "p[" << i <<"] = " << ((u64*)p)[i] << "\n";
    }
    return 0;
    #endif

    while (!WindowShouldClose()) {
	BeginDrawing();
        ClearBackground(GRAY);     
	control_system();
	movement_system();
	draw();
	EndDrawing();
    }

    CloseWindow();
    return 0;
}
