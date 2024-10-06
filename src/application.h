#pragma once
#include <raylib.h>

#include <string>
#include <vector>

#include "freelist.h"

struct ExpensiveEntity
{
	std::string name = "ExpensiveEntity";
	std::vector<std::string> tags {};
	std::vector<std::string> useless1 {};
	std::vector<std::string> useless2 {};

	Vector2 pos {};
	Vector2 size {};
	Color color = WHITE;

	bool is_alive = true;
};

struct CheaperEntity
{
	std::string name = "CheaperEntity";

	Vector2 pos {};
	Vector2 size {};
	Color color = WHITE;

	bool is_alive = true;
};

struct Reservation
{
	uint32_t size = 0;
	uint32_t offset = 0;
	void* data = nullptr;
};

class Application
{
public:
	Application( const Rectangle& frame );

	void update( float dt );
	void render();

	template <typename T>
	T* reserve()
	{
		int id = reserve( sizeof( T ) );
		if ( id == -1 ) return nullptr;

		return (T*)_reservations[id].data;
	}
	int reserve( uint32_t size );
	void unreserve( int id );
	void clear();

public:
	bool show_only_user_data = false;

private:
	void _draw_text(
		const char* text,
		const Vector2& pos,
		const Vector2& origin,
		float font_size,
		float spacing,
		const Color& color,
		float min_width = -1
	) const;

	Rectangle _create_memory_region_rect( uint32_t offset, uint32_t size ) const;
	void _draw_memory_region( 
		const Rectangle& region, 
		const char* text, 
		float font_size,
		float spacing,
		const Color& color
	) const;
	void _draw_memory_region_label( const Rectangle& region, const char* text ) const;

private:
	const bool  ENABLE_BENCHMARKS = false;
	const int   BENCHMARK_ITERATIONS = 1000000;

	const float MEMORY_RECT_PADDING = 4.0f;

	const float MEMORY_REGION_LABEL_FONT_SIZE = 20.0f;
	const float MEMORY_REGION_LABEL_SPACING = 1.0f;

private:
	Font _font {};
	Rectangle _frame {};

	std::vector<Reservation> _reservations {};

	Rectangle _total_memory_rect {};
	float _total_size = 0.0f;

	Freelist _freelist;
};