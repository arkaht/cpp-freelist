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
	 * Returns the offset in the user data memory or -1 if there was no available space.
	 */
	uint32_t allocate( uint32_t size );
	/*
	 * Deallocate the data region at given offset and size.
	 */
	void deallocate( uint32_t offset, uint32_t size );
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
	 * Returns a pointer to the allocated memory given the offset, or nullptr if the offset is negative.
	 * You should only pass in the offsets returned by the 'allocate' method, otherwise, use it at your own risks.
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
	FreelistNode* _new_node( uint32_t offset = 0, size_t size = 0 );

private:
	uint32_t _data_size = 0;
	uint32_t _total_size = 0;
	uint32_t _internal_size = 0;
	int _node_count = 0;

	FreelistNode* _head = nullptr;
	FreelistNode* _nodes = nullptr;
	
	void* _memory = nullptr;
};