#pragma once

#include <cstdint>

/*
 * A node representing an un-reserved memory block inside the freelist linked list.
 */
struct FreelistNode
{
	/*
	 * Position of the node inside the pre-allocated memory
	 */
	uint32_t offset = 0;
	/*
	 * Size of the un-reserved memory block
	 */
	uint32_t size = 0;

	/*
	 * For linked list purposes, the next node.
	 */
	FreelistNode* next = nullptr;
};

/*
 * A data structure used to reserve memory from a pre-allocated memory block helping to avoid
 * intensive usage of dynamic memory allocation. Only one allocation is done at construction time.
 * It uses a linked list of nodes to manage the un-reserved blocks and to merge them.
 */
class Freelist
{
public:
	/*
	 * Operates a dynamic memory allocation to initialize the pre-allocated memory block
	 * for further usage.
	 */
	Freelist( uint32_t data_size );
	/*
	 * Frees the dynamic memory allocation.
	 */
	~Freelist();

	/*
	 * Finds and reserves a memory block of the given size.
	 * Returns whenever the reservation was successful.
	 * If successful, it also sets the 'offset' variable to the reserved position.
	 */
	bool reserve( uint32_t size, uint32_t& offset );
	/*
	 * Un-reserves the memory block at given offset and size.
	 */
	void unreserve( uint32_t offset, uint32_t size );
	/*
	 * Clears the freelist of all allocations and reset its nodes.
	 */
	void clear();

	/*
	 * Returns the head of the nodes list or nullptr if there is no head.
	 * If so, it's likely there is no free space available.
	 */
	FreelistNode* head() const;

	/*
	 * Returns a pointer to the memory given the offset.
	 * You should only pass in offsets returned by the 'reserve' method and that are not un-reserved.
	 * If not, you may end up overriding memory reserved for something else, use it at your own risks.
	 */
	void* pointer_to_memory( uint32_t offset, bool add_internal_size = true ) const;

	/*
	 * Returns the total size the freelist has allocated, in bytes.
	 * The total size is the sum of the internal size plus the user data size.
	 */
	uint32_t get_total_size() const;
	/*
	 * Returns the user data size, in bytes.
	 */
	uint32_t get_data_size() const;
	/*
	 * Returns the internal size used to contain the nodes, in bytes.
	 */
	uint32_t get_internal_size() const;
	/*
	 * Returns the free space size, in bytes.
	 */
	uint32_t get_free_size() const;

private:
	/*
	 * Finds and returns the first avaialble node and set it up with the given offset and size.
	 */
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