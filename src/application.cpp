#include "application.h"

#include "utils.h"
#include "benchmark.h"
#include <stdio.h>

Application::Application( const Rectangle& frame )
	: _frame( frame ), 
	 _freelist( 2048 )
{
	_font = GetFontDefault();

	if ( ENABLE_BENCHMARKS )
	{
		Benchmark benchmark {};

		//  Benchmarking the new/delete operations
		benchmark.start();
		for ( int i = 0; i < BENCHMARK_ITERATIONS; i++ )
		{
			auto entity = new ExpensiveEntity();
			entity->is_alive = false;
			delete entity;
		}
		benchmark.stop();
		printf( "Benchmark: new(): %.3f seconds for a total of %d iterations\n", benchmark.get_seconds(), BENCHMARK_ITERATIONS );

		//  Benchmarking the freelist allocate/free operations
		benchmark.start();
		for ( int i = 0; i < BENCHMARK_ITERATIONS; i++ )
		{
			uint32_t size = sizeof( ExpensiveEntity );
			uint32_t offset;
			if ( _freelist.reserve( size, offset ) )
			{
				auto entity = (ExpensiveEntity*)_freelist.pointer_to_memory( offset );
				entity->is_alive = false;
				_freelist.unreserve( offset, size );
			}
		}
		benchmark.stop();
		printf( "Benchmark: freelist: %.3f seconds for a total of %d iterations\n", benchmark.get_seconds(), BENCHMARK_ITERATIONS );
	}
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
		auto entity = reserve<ExpensiveEntity>();
		if ( entity == nullptr ) 
		{
			printf( "Failed to reserve a new ExpensiveEntity!\n" );
			return;
		}

		entity->color = RED;
		entity->is_alive = true;
		entity->pos = { 10.0f, -5.0f };
		entity->size = { 2.0f, 1.0f };

		printf( "Reserved a new ExpensiveEntity!\n" );
	}
	else if ( IsKeyPressed( KEY_J ) )
	{
		auto entity = reserve<CheaperEntity>();
		if ( entity == nullptr ) 
		{
			printf( "Failed to reserve a new CheaperEntity!\n" );
			return;
		}

		entity->color = RED;
		entity->pos = { 10.0f, -5.0f };
		entity->size = { 2.0f, 1.0f };

		printf( "Reserved a new CheaperEntity!\n" );
	}

	int mem_offset = 0;
	if ( !show_only_user_data )
	{
		mem_offset += _freelist.get_internal_size();
	}

	//  User click on allocations
	if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) )
	{
		const Vector2 mouse_pos = GetMousePosition();

		for ( int i = 0; i < _reservations.size(); i++ )
		{
			const Reservation& reservation = _reservations[i];

			const Rectangle region = _create_memory_region_rect(
				mem_offset + reservation.offset,
				reservation.size
			);

			//  Check click on reserved region to un-reserve
			bool is_hovered = CheckCollisionPointRec( mouse_pos, region );
			if ( !is_hovered ) continue;

			unreserve( i );
			break;
		}
	}
}

void Application::render()
{
	const Font font = GetFontDefault();

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
	_draw_text( 
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
		_draw_memory_region(
			region,
			utils::bytes_to_str( internal_size ),
			font_size,
			spacing,
			BLUE
		);

		_draw_memory_region_label( region, "Internal Size" );

		mem_offset += internal_size;
	}

	//  Draw freelist nodes
	int index = 0;
	FreelistNode* head = _freelist.head();
	FreelistNode* node = head;
	while( node )
	{
		const char* text = TextFormat(
			"%s%s",
			node == head ? "*" : "",
			utils::bytes_to_str( node->size )
		);

		const Rectangle region = _create_memory_region_rect(
			mem_offset + node->offset,
			node->size
		);
		_draw_memory_region( region, text, font_size, spacing, GREEN );

		index++;
		node = node->next;
	}

	//  Draw user memory region
	{
		const Rectangle region = _create_memory_region_rect( mem_offset, data_size );
		_draw_memory_region_label( region, "User Size" );
	}

	//  Draw reservations
	for ( int i = 0; i < _reservations.size(); i++ )
	{
		const Reservation& reservation = _reservations[i];

		const Rectangle region = _create_memory_region_rect(
			mem_offset + reservation.offset,
			reservation.size
		);

		bool is_hovered = CheckCollisionPointRec( GetMousePosition(), region );
		_draw_memory_region(
			region,
			utils::bytes_to_str( reservation.size ),
			font_size,
			spacing,
			is_hovered ? PURPLE : VIOLET
		);
	}

	//  Draw nodes count
	_draw_text( 
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
		"J: Reserve a CheaperEntity (64.00B)",
		"H: Reserve an ExpensiveEntity (160.00B)",
		"E: Toggle Internal Size visualisation",
		"C: Clear the freelist",
		"LMB: Click on reserved regions to free them",
	};
	Vector2 pos { 24.0f, _frame.height - 24.0f };
	for ( int i = 0; i < instructions_count; i++ )
	{
		_draw_text(
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

int Application::reserve( uint32_t size )
{
	uint32_t offset = 0;
	if ( !_freelist.reserve( size, offset ) ) return -1;

	Reservation reservation {};
	reservation.data = _freelist.pointer_to_memory( offset );
	reservation.offset = offset;
	reservation.size = size;
	_reservations.push_back( reservation );

	return (int)_reservations.size() - 1;
}

void Application::unreserve( int id )
{
	const Reservation& reservation = _reservations.at( id );
	_freelist.unreserve( reservation.offset, reservation.size );
	_reservations.erase( _reservations.begin() + id );
}

void Application::clear()
{
	_freelist.clear();
	_reservations.clear();
}

void Application::_draw_text( 
	const char* text,
	const Vector2& pos,
	const Vector2& origin,
	float font_size,
	float spacing,
	const Color& color,
	float min_width
) const
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

Rectangle Application::_create_memory_region_rect( uint32_t offset, uint32_t size ) const
{
	Rectangle memory_rect( _total_memory_rect );
	memory_rect.x += MEMORY_RECT_PADDING + offset / _total_size * _total_memory_rect.width;
	memory_rect.y += MEMORY_RECT_PADDING;
	memory_rect.width = size / _total_size * _total_memory_rect.width;
	memory_rect.width -= MEMORY_RECT_PADDING * 2.0f;
	memory_rect.height -= MEMORY_RECT_PADDING * 2.0f;
	return memory_rect;
}

void Application::_draw_memory_region(
	const Rectangle& region,
	const char* text,
	float font_size,
	float spacing,
	const Color& color
) const
{
	//  Draw memory region
	DrawRectangleRec( region, color );

	//  Draw text
	_draw_text(
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

void Application::_draw_memory_region_label( const Rectangle& region, const char* text ) const
{
	const int height_offset = 30;
	const int height = 10;
	const int thickness = 4;

	//  Draw top bar
	DrawRectangle(
		(int)region.x,
		(int)region.y - height_offset,
		(int)region.width,
		thickness,
		GRAY
	);
	//  Draw left bar
	DrawRectangle(
		(int)region.x,
		(int)region.y - height_offset,
		thickness,
		height,
		GRAY
	);
	//  Draw right bar
	DrawRectangle(
		(int)region.x + (int)region.width - thickness,
		(int)region.y - height_offset,
		thickness,
		height,
		GRAY
	);
	
	_draw_text( 
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
