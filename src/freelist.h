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
	Freelist( uint32_t data_size );
	~Freelist();

	/*
	 * Finds and allocates a data region for the given size.
	 * Returns whenever the allocation was successful. If successful, it also sets the 'offset' variable to the allocated position.
	 */
	bool allocate( uint32_t size, uint32_t& offset );
	/*
	 * Free the data region at given offset and size.
	 */
	void free( uint32_t offset, uint32_t size );
	/*
	 * Clear the freelist of all allocations and reset its nodes.
	 */
	void clear();

	/*
	 * Returns the head of the nodes list or nullptr if there is no head.
	 * If so, it's likely there is no free space available.
	 */
	FreelistNode* head() const;

	/*
	 * Returns a pointer to the allocated memory given the offset.
	 * You should only pass in offsets returned by the 'allocate' method, otherwise, use it at your own risks.
	 */
	void* pointer_to_memory( uint32_t offset, bool add_internal_size = true ) const;

	/*
	 * Returns the total size the freelist has allocated, in bytes.
	 * The total size is the sum of the internal size plus the user data size.
	 */
	int get_total_size() const;
	/*
	 * Returns the user data size, in bytes.
	 */
	int get_data_size() const;
	/*
	 * Returns the internal size used to contain the nodes, in bytes.
	 */
	int get_internal_size() const;
	/*
	 * Returns the free space size, in bytes.
	 */
	int get_free_size() const;

private:
	FreelistNode* _new_node( uint32_t offset = 0, uint32_t size = 0 );

private:
	uint32_t _data_size = 0;
	uint32_t _total_size = 0;
	uint32_t _internal_size = 0;
	int _node_count = 0;

	FreelistNode* _head = nullptr;
	FreelistNode* _nodes = nullptr;
	
	void* _memory = nullptr;
};