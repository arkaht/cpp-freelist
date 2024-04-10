#include "application.h"

#include "utils.h"
#include <stdio.h>

Application::Application( const Rectangle& frame )
	: _frame( frame ), 
	 _freelist( 2048 )
{
	_font = GetFontDefault();
}

void Application::update( float dt )
{
	if ( IsKeyPressed( KEY_E ) )
	{
		show_only_user_data = !show_only_user_data;
	}
	else if ( IsKeyPressed( KEY_C ) )
	{
		clear();
	}
	else if ( IsKeyPressed( KEY_H ) )
	{
		auto entity = allocate<ExpensiveEntity>();
		if ( entity == nullptr ) 
		{
			printf( "Failed to allocate a new ExpensiveEntity!\n" );
			return;
		}

		entity->color = RED;
		entity->is_alive = true;
		entity->pos = { 10.0f, -5.0f };
		entity->size = { 2.0f, 1.0f };

		printf( "Allocated a new ExpensiveEntity!\n" );
	}
	else if ( IsKeyPressed( KEY_J ) )
	{
		auto entity = allocate<CheaperEntity>();
		if ( entity == nullptr ) 
		{
			printf( "Failed to allocate a new CheaperEntity!\n" );
			return;
		}

		entity->color = RED;
		entity->pos = { 10.0f, -5.0f };
		entity->size = { 2.0f, 1.0f };

		printf( "Allocated a new CheaperEntity!\n" );
	}

	int mem_offset = 0;
	if ( !show_only_user_data )
	{
		mem_offset += _freelist.get_internal_size();
	}

	//  User click on allocations
	for ( int i = 0; i < _allocs.size(); i++ )
	{
		const Allocation& alloc = _allocs[i];

		Rectangle region = _create_memory_region_rect( mem_offset + alloc.offset, alloc.size );

		//  Check click on allocation region to free
		bool is_hovered = CheckCollisionPointRec( GetMousePosition(), region );
		if ( is_hovered && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) )
		{
			deallocate( i );
			break;
		}
	}
}

void Application::render()
{
	Font font = GetFontDefault();

	ClearBackground( RAYWHITE );

	//  Draw total memory rectangle
	_total_memory_rect.width = _frame.width * 0.75f;
	_total_memory_rect.height = _frame.height * 0.06f;
	_total_memory_rect.x = _frame.width * 0.5f - _total_memory_rect.width * 0.5f;
	_total_memory_rect.y = _frame.height * 0.575f - _total_memory_rect.height * 0.5f;
	DrawRectangleRec( _total_memory_rect, LIGHTGRAY );

	const int data_size = _freelist.get_data_size();
	const int total_size = _freelist.get_total_size();
	const int internal_size = _freelist.get_internal_size();

	_total_size = (float)( show_only_user_data ? data_size : total_size );

	//  Draw title text
	draw_text( 
		"FREELIST",
		Vector2 {
			_frame.width * 0.5f,
			_frame.height * 0.325f,
		},
		Vector2 { 0.5f, 0.5f },
		128.0f,
		16.0f,
		LIGHTGRAY
	);

	const float font_size = 18.0f;
	const float spacing = 1.0f;

	//  Draw freelist internal state size
	int mem_offset = 0;
	if ( !show_only_user_data )
	{
		const Rectangle region = _create_memory_region_rect( 0, internal_size );
		_draw_memory_region( region, utils::bytes_to_str( internal_size ), font_size, spacing, BLUE );

		_draw_memory_region_label( region, "Internal Size" );

		mem_offset += internal_size;
	}

	//  Draw freelist nodes
	int index = 0;
	FreelistNode* head = _freelist.head();
	FreelistNode* node = head;
	while( node )
	{
		const Rectangle region = _create_memory_region_rect( mem_offset + node->offset, node->size );
		const char* text = TextFormat( "%s%s", node == head ? "*" : "", utils::bytes_to_str( node->size ) );
		_draw_memory_region( region, text, font_size, spacing, GREEN );

		index++;
		node = node->next;
	}

	const Rectangle region = _create_memory_region_rect( mem_offset, data_size );
	_draw_memory_region_label( region, "User Size" );

	//  Draw allocations
	for ( int i = 0; i < _allocs.size(); i++ )
	{
		const Allocation& alloc = _allocs[i];

		Rectangle region = _create_memory_region_rect( mem_offset + alloc.offset, alloc.size );

		bool is_hovered = CheckCollisionPointRec( GetMousePosition(), region );
		_draw_memory_region( region, utils::bytes_to_str( alloc.size ), font_size, spacing, is_hovered ? PURPLE : VIOLET );
	}

	//  Draw nodes count
	draw_text( 
		TextFormat( "%i NODES", index ), 
		Vector2 {
			_total_memory_rect.x,
			_total_memory_rect.y + _total_memory_rect.height,
		},
		Vector2 { 0.0f, 0.0f },
		font_size,
		spacing,
		BLACK
	);

	//  Draw instructions
	const int instructions_count = 5;
	const char* instructions[instructions_count] {
		"J: Allocate a CheaperEntity (64.00B)",
		"H: Allocate an ExpensiveEntity (160.00B)",
		"E: Toggle Internal Size visualisation",
		"C: Clear the freelist",
		"LMB: Click on allocated regions to free them",
	};
	Vector2 pos { 24.0f, _frame.height - 24.0f };
	for ( int i = 0; i < instructions_count; i++ )
	{
		draw_text(
			instructions[i],
			pos,
			Vector2 { 0.0f, 1.0f },
			20.0f,
			spacing,
			GRAY
		);
		pos.y -= 20.0f;
	}
}

int Application::allocate( uint32_t size )
{
	uint32_t offset;
	if ( !_freelist.allocate( size, offset ) ) return -1;

	Allocation alloc {};
	alloc.data = _freelist.pointer_to_memory( offset );
	alloc.offset = offset;
	alloc.size = size;
	_allocs.push_back( alloc );

	return (int)_allocs.size() - 1;
}

void Application::deallocate( int id )
{
	const Allocation& alloc = _allocs.at( id );
	_freelist.free( alloc.offset, alloc.size );
	_allocs.erase( _allocs.begin() + id );
}

void Application::clear()
{
	_freelist.clear();
	_allocs.clear();
}

void Application::draw_text( const char* text, Vector2 pos, Vector2 origin, float font_size, float spacing, Color color, float min_width )
{
	Vector2 text_size = MeasureTextEx( _font, text, font_size, spacing );

	//  Hides text if below minimum width
	if ( min_width > 0 && text_size.x > min_width ) return;

	DrawTextEx( 
		_font, 
		text,
		Vector2 {
			pos.x - text_size.x * origin.x, 
			pos.y - text_size.y * origin.y,
		},
		font_size,
		spacing,
		color 
	);
}

Rectangle Application::_create_memory_region_rect( uint32_t offset, uint32_t bytes ) const
{
	Rectangle memory_rect( _total_memory_rect );
	memory_rect.x += MEMORY_RECT_PADDING + offset / _total_size * _total_memory_rect.width;
	memory_rect.y += MEMORY_RECT_PADDING;
	memory_rect.width = bytes / _total_size * _total_memory_rect.width;
	memory_rect.width -= MEMORY_RECT_PADDING * 2.0f;
	memory_rect.height -= MEMORY_RECT_PADDING * 2.0f;
	return memory_rect;
}

void Application::_draw_memory_region( const Rectangle& region, const char* text, float font_size, float spacing, Color color )
{
	//  Draw memory region
	DrawRectangleRec( region, color );

	//  Draw text
	draw_text(
		text,
		Vector2 {
			region.x + region.width * 0.5f,
			region.y + region.height * 0.5f,
		},
		Vector2 { 0.5f, 0.5f },
		font_size,
		spacing,
		WHITE,
		region.width
	);
}

void Application::_draw_memory_region_label( const Rectangle& region, const char* text )
{
	const int height_offset = 30;
	const int height = 10;
	const int thickness = 4;

	DrawRectangle(
		(int)region.x,
		(int)region.y - height_offset,
		(int)region.width,
		thickness,
		GRAY
	);
	DrawRectangle(
		(int)region.x,
		(int)region.y - height_offset,
		thickness,
		height,
		GRAY
	);
	DrawRectangle(
		(int)region.x + (int)region.width - thickness,
		(int)region.y - height_offset,
		thickness,
		height,
		GRAY
	);
	
	draw_text( 
		text, 
		Vector2 {
			region.x + region.width * 0.5f, 
			region.y - height_offset
		},
		Vector2 { 0.5f, 1.0f },
		MEMORY_REGION_LABEL_FONT_SIZE,
		MEMORY_REGION_LABEL_SPACING,
		GRAY
	);
}
