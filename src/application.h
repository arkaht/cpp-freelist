#pragma once
#include <raylib.h>

#include <string>
#include <vector>

#include "freelist.h"

struct Allocation
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

	void allocate( size_t size );
	void deallocate( int id );
	void clear();

	void draw_text( const char* text, Vector2 pos, Vector2 origin, float font_size, float spacing, Color color );

public:
	bool show_only_user_data = true;

private:
	Rectangle _create_memory_region_rect( uint32_t offset, uint32_t bytes ) const;
	void _draw_memory_region( 
		const Rectangle& region, 
		uint32_t bytes, 
		float font_size,
		float spacing,
		Color color
	);

private:
	const float MEMORY_RECT_PADDING = 4.0f;

private:
	Font _font {};
	Rectangle _frame {};

	std::vector<Allocation> _allocs;

	Rectangle _total_memory_rect {};
	float _total_size = 0.0f;

	Freelist _freelist;
};