#pragma once

#include <cstdint>

struct FreelistNode
{
	uint32_t size = 0;
	uint32_t offset = 0;

	FreelistNode* next = nullptr;
};

class Freelist
{
public:
	Freelist( size_t data_size );
	~Freelist();

	/*
	 * nullptr if failed
	 */
	uint32_t allocate( size_t size );
	void deallocate( uint32_t offset, size_t size );
	void clear();

	FreelistNode* head() const;
	//bool next( FreelistNode* node );

	int get_total_size() const;
	int get_data_size() const;
	int get_internal_size() const;
	int get_free_size() const;

	void* pointer_to_memory( uint32_t offset, bool add_internal_size = true ) const;

private:
	FreelistNode* _new_node( uint32_t offset = 0, uint32_t size = 0 ) const;

private:
	int _data_size = 0;
	int _total_size = 0;
	int _internal_size = 0;
	int _node_count = 0;

	FreelistNode* _head = nullptr;
	FreelistNode* _nodes = nullptr;
	
	void* _memory = nullptr;
};