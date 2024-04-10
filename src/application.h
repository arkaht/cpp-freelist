#pragma once
#include <raylib.h>

#include <string>
#include <vector>

#include "freelist.h"

struct ExpensiveEntity
{
	std::string name;
	std::vector<std::string> tags;
	std::vector<std::string> useless1;
	std::vector<std::string> useless2;

	Vector2 pos;
	Vector2 size;
	Color color;

	bool is_alive;
};

struct CheaperEntity
{
	std::string name;

	Vector2 pos;
	Vector2 size;
	Color color;

	bool is_alive;
};

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

	template <typename T>
	T* allocate()
	{
		int id = allocate( sizeof( T ) );
		if ( id == -1 ) return nullptr;

		return (T*)_allocs[id].data;
	}
	int allocate( size_t size );
	void deallocate( int id );
	void clear();

	void draw_text( const char* text, Vector2 pos, Vector2 origin, float font_size, float spacing, Color color, float min_width = -1 );

public:
	bool show_only_user_data = false;

private:
	Rectangle _create_memory_region_rect( uint32_t offset, uint32_t bytes ) const;
	void _draw_memory_region( 
		const Rectangle& region, 
		const char* text, 
		float font_size,
		float spacing,
		Color color
	);
	void _draw_memory_region_label(
		const Rectangle& region,
		const char* text
	);

private:
	const float MEMORY_RECT_PADDING = 4.0f;

	const float MEMORY_REGION_LABEL_FONT_SIZE = 20.0f;
	const float MEMORY_REGION_LABEL_SPACING = 1.0f;

private:
	Font _font {};
	Rectangle _frame {};

	std::vector<Allocation> _allocs;

	Rectangle _total_memory_rect {};
	float _total_size = 0.0f;

	Freelist _freelist;
};