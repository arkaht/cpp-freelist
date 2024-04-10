#include "freelist.h"

#include <cstdlib>
#include <stdio.h>
#include <cstring>

#include "utils.h"

Freelist::Freelist( size_t data_size )
{
	_data_size = data_size;

	//  Maximum amount of nodes to allocate with the smallest possible size
	_node_count = _data_size / sizeof( void* );

	//  Memory layout is: 
	//  - Freelist nodes (Internal size) 
	//  - User data (Data size)
	int nodes_byte = sizeof( FreelistNode ) * _node_count;
	_total_size = nodes_byte + _data_size;
	_internal_size = _total_size - _data_size;

	//  Measure total memory size to allocate, can't wrap my head around the formula, probably too much tired
	printf( "%d %d %d\n", (int)_data_size, _node_count, _total_size );

	//  Allocating memory
	_memory = malloc( _total_size );

	//  Zero out memory
	memset( _memory, 0, _total_size );

	//  Assign nodes pointer to internal memory space
	_nodes = (FreelistNode*)_memory;

	_head = &_nodes[0];
	_head->offset = 0;
	_head->size = _data_size;
	_head->next = nullptr;
}

Freelist::~Freelist()
{
	free( _memory );
	_memory = nullptr;
}

uint32_t Freelist::allocate( size_t size )
{
	FreelistNode* previous = nullptr;
	FreelistNode* node = _head;
	while( node )
	{
		if ( node->size == size )
		{
			if ( previous )
			{
				previous->next = node->next;
			}
			//  No previous node? It means it's the head
			else
			{
				_head = node->next;
			}

			//  Invalidate node
			node->offset = 0;
			node->size = 0;
			node->next = nullptr;

			return node->offset;
		}
		else if ( node->size > size )
		{
			node->size -= size;
			return node->offset + node->size;
		}

		previous = node;
		node = node->next;
	}

	printf( 
		"Freelist couldn't find enough space to hold %s, free space: %s\n", 
		utils::bytes_to_str( size ), 
		utils::bytes_to_str( get_free_size() ) 
	);
	return -1;
}

void Freelist::deallocate( uint32_t offset, size_t size )
{
	//  Zero out memory
	memset( pointer_to_memory( offset ), 0, size );

	if ( !_head )
	{
		//  No head? It means the freelist is empty: directly assign it
		_head = _new_node( offset, size );
		return;
	}

	bool is_node_setup = false;
	FreelistNode* previous = nullptr;
	FreelistNode* current = _head;
	while( current )
	{
		//  Do we still need to insert the node?
		if ( !is_node_setup )
		{
			//  Is directly at his right? Combine them
			if ( current->offset + current->size == offset )
			{
				current->size += size;
				is_node_setup = true;
			}
			//  Is directly at his left? Combine them
			else if ( offset + size == current->offset )
			{
				current->size += size;
				current->offset -= size;
				is_node_setup = true;
			}
			//  Is somewhere on the left?
			else if ( current->offset > offset )
			{
				FreelistNode* node = _new_node( offset, size );
				if ( !previous )
				{
					_head = node;
				}
				else
				{
					previous->next = node;
				}
				node->next = current;
				is_node_setup = true;
			}
			//  Is further away on the right that no nodes actually covered?
			else if ( current->next == nullptr )
			{
				FreelistNode* node = _new_node( offset, size );
				current->next = node;
				is_node_setup = true;
			}
		}
		//  Combine fragmented nodes
		else
		{
			if ( previous->offset + previous->size == current->offset )
			{
				previous->size += current->size;
				previous->next = current->next;

				current->offset = 0;
				current->size = 0;
				current->next = nullptr;

				current = previous->next;
				previous = previous;
				continue;
			}
		}

		previous = current;
		current = current->next;
	}
}

FreelistNode* Freelist::head() const
{
	return _head;
}

int Freelist::get_total_size() const
{
	return _total_size;
}

int Freelist::get_data_size() const
{
	return _data_size;
}

int Freelist::get_internal_size() const
{
	return _internal_size;
}

int Freelist::get_free_size() const
{
	int bytes = 0;

	FreelistNode* node = _head;
	while( node )
	{
		bytes += node->size;
		node = node->next;
	}

	return bytes;
}

FreelistNode* Freelist::_new_node( uint32_t offset, uint32_t size ) const
{
	for ( int i = 0; i < _node_count; i++ )
	{
		auto node = &_nodes[i];
		if ( node->size == 0 )
		{
			node->offset = offset;
			node->size = size;
			node->next = nullptr;
			return node;
		}
	}

	return nullptr;
}

void* Freelist::pointer_to_memory( uint32_t offset, bool add_internal_size ) const
{
	auto ptr = (char*)_memory;

	if ( add_internal_size )
	{
		ptr = ptr + _internal_size;
	}

	return ptr + offset;
}

void Freelist::clear()
{
	//  Zero out user data memory
	memset( pointer_to_memory( 0 ), 0, _data_size );

	//  Reset nodes
	FreelistNode* node = _head;
	while ( node )
	{
		auto next = node->next;

		node->size = 0;
		node->offset = 0;
		node->next = nullptr;

		node = next;
	}

	if ( !_head )
	{
		_head = _new_node( 0, _data_size );
	}
	else
	{
		_head->size = _data_size;
	}
}
