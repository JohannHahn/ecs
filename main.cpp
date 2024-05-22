#include <cassert>
#include <iostream>
#include "raylib/src/raylib.h"
#include "raylib/src/raymath.h"
#include "ecs.h"


float window_width = 900.f;
float window_height = 900.f;
float width = 100;
const char* window_title = "ECS";

float speed = 50.f;

enum component_name {
    POSITION = 1, VELOCITY = 2,
};

enum component_index{
    POSITION_I, VELOCITY_I,
};

enum entity_kind {
    PLAYER, ENEMY, 
};

Vector2 position[2]= {{10.f, 10.f}, {100.f, 100.f}};
Vector2 velocity[2] = {{0.f, 0.f}, {1.f, 1.f}};

ECS ecs(2, sizeof(Vector2), sizeof(Vector2));

void set_vec(u64 e_id, u64 c_id, Vector2 vec) {
    ecs.write_component(e_id, c_id, &vec);
}

Vector2 get_vec(u64 e_id, u64 c_id) {
    return *(Vector2*)ecs.read_component(e_id, c_id);
}

void add_enemy() {
    ecs.add_entity(POSITION | VELOCITY);
    ecs.write_component(ecs.entity_count() - 1, POSITION_I, &position[ENEMY]);
    Vector2 vel = {.x = velocity[ENEMY].x - ecs.entity_count(), .y = velocity[ENEMY].y - ecs.entity_count()};
    ecs.write_component(ecs.entity_count() - 1, VELOCITY_I, &vel);
}

void movement_system() {   
    for(int i = 0; i < ecs.entity_count(); ++i) {
	if (!ecs.check_components(i, POSITION | VELOCITY)) continue; 
	Vector2 pos = get_vec(i, POSITION_I);
	Vector2 vel = get_vec(i, VELOCITY_I);
	vel.x *= speed * GetFrameTime();
	vel.y *= speed * GetFrameTime();
	pos = Vector2Add(pos, vel);
	ecs.write_component(i, POSITION_I, &pos);
    } 
}

void control_system() {
    int key = GetKeyPressed();
    Vector2 vel = {0};
    if (IsKeyDown(KEY_LEFT)) {
	vel = Vector2Add(vel, {-1.f, 0.f});
    }
    if (IsKeyDown(KEY_RIGHT)) {
	vel = Vector2Add(vel, {1.f, 0.f});
    }
    if (IsKeyDown(KEY_UP)) {
	vel = Vector2Add(vel, {0.f, -1.f});
    }
    if (IsKeyDown(KEY_DOWN)) {
	vel = Vector2Add(vel, {0.f, 1.f});
    }
    set_vec(PLAYER, VELOCITY_I, vel);

    if (IsKeyReleased(KEY_SPACE)) {
	add_enemy();
    }
}


enum test {
    ONE = 1, TWO = 2, THREE = 4
};

int main() {
    InitWindow(window_width, window_height, window_title);
    SetTargetFPS(60);

    ecs.add_entity(POSITION | VELOCITY);
    ecs.write_component(PLAYER, POSITION_I, &position[PLAYER]);
    ecs.write_component(PLAYER, VELOCITY_I, &velocity[PLAYER]);

    while (!WindowShouldClose()) {
	BeginDrawing();
        ClearBackground(GRAY);     
	control_system();
	movement_system();
	DrawRectangleV(get_vec(PLAYER, POSITION_I), {100, 100}, BLUE);
	for (int i = 1; i < ecs.entity_count(); ++i) {
	    assert(i != PLAYER);
	    DrawCircleV(get_vec(i, POSITION_I), 10.f, RED);
	}
	EndDrawing();
    }

    CloseWindow();
    return 0;
}
