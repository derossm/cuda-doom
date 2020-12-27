/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Queue of waiting callbacks, stored in a binary min heap, so that we can always get the first callback.
\**********************************************************************************************************************************************/

#include "opl_queue.h"

opl_callback_queue_t* OPL_Queue_Create()
{
	opl_callback_queue_t* queue = static_cast<decltype(queue)>(malloc(sizeof(opl_callback_queue_t)));
	queue->num_entries = 0;

	return queue;
}

void OPL_Queue_Destroy(opl_callback_queue_t* queue)
{
	free(queue);
}

int OPL_Queue_IsEmpty(opl_callback_queue_t* queue)
{
	return queue->num_entries == 0;
}

void OPL_Queue_Clear(opl_callback_queue_t* queue)
{
	queue->num_entries = 0;
}

void OPL_Queue_Push(opl_callback_queue_t* queue, opl_callback_t callback, delay_data_t* data, uint64_t time)
{
	if (queue->num_entries >= MAX_OPL_QUEUE)
	{
		fprintf(stderr, "OPL_Queue_Push: Exceeded maximum callbacks\n");
		return;
	}

	// Add to last queue entry.
	auto entry_id{queue->num_entries};
	++queue->num_entries;

	// Shift existing entries down in the heap.
	while (entry_id > 0)
	{
		auto parent_id{(entry_id - 1) / 2};

		// Is the heap condition satisfied?
		if (time >= queue->entries[parent_id].time)
		{
			break;
		}

		// Move the existing entry down in the heap.
		memcpy(&queue->entries[entry_id], &queue->entries[parent_id], sizeof(opl_queue_entry_t));

		// Advance to the parent.
		entry_id = parent_id;
	}

	// Insert new callback data.
	queue->entries[entry_id].callback = callback;
	queue->entries[entry_id].data = data;
	queue->entries[entry_id].time = time;
}

bool OPL_Queue_Pop(opl_callback_queue_t* queue, opl_callback_t* callback, delay_data_t** data)
{
	// Empty?
	if (queue->num_entries <= 0)
	{
		return false;
	}

	// Store the result:
	*callback = queue->entries[0].callback;
	*data = queue->entries[0].data;

	// Decrease the heap size, and keep pointer to the last entry in the heap, which must now be percolated down from the top.
	--queue->num_entries;
	auto entry{&queue->entries[queue->num_entries]};

	// Percolate down.
	auto i{0};
	for (;; i = next_i)
	{
		auto child1{i * 2 + 1};
		auto child2{i * 2 + 2};

		int next_i{0};

		if (child1 < queue->num_entries && queue->entries[child1].time < entry->time)
		{
			// Left child is less than entry. Use the minimum of left and right children.
			if (child2 < queue->num_entries && queue->entries[child2].time < queue->entries[child1].time)
			{
				next_i = child2;
			}
			else
			{
				next_i = child1;
			}
		}
		else if (child2 < queue->num_entries && queue->entries[child2].time < entry->time)
		{
			// Right child is less than entry. Go down the right side.
			next_i = child2;
		}
		else
		{
			// Finished percolating.
			break;
		}

		// Percolate the next value up and advance.
		memcpy(&queue->entries[i], &queue->entries[next_i], sizeof(opl_queue_entry_t));
	}

	// Store the old last-entry at its new position.
	memcpy(&queue->entries[i], entry, sizeof(opl_queue_entry_t));

	return true;
}

uint64_t OPL_Queue_Peek(opl_callback_queue_t* queue)
{
	if (queue->num_entries > 0)
	{
		return queue->entries[0].time;
	}
	else
	{
		return 0ull;
	}
}

void OPL_Queue_AdjustCallbacks(opl_callback_queue_t* queue, size_t time, float factor)
{
	for (size_t i{0u}; i < queue->num_entries; ++i)
	{
		size_t offset = queue->entries[i].time - time;
		queue->entries[i].time = time + size_t(offset / factor);
	}
}

#ifdef TEST

static void PrintQueueNode(opl_callback_queue_t *queue, int node, int depth)
{
	int i;

	if (node >= queue->num_entries)
	{
		return;
	}

	for (i=0; i<depth * 3; ++i)
	{
		printf(" ");
	}

	printf("%i\n", queue->entries[node].time);

	PrintQueueNode(queue, node * 2 + 1, depth + 1);
	PrintQueueNode(queue, node * 2 + 2, depth + 1);
}

static void PrintQueue(opl_callback_queue_t *queue)
{
	PrintQueueNode(queue, 0, 0);
}

int main()
{
	opl_callback_queue_t *queue;
	int iteration;

	queue = OPL_Queue_Create();

	for (iteration=0; iteration<5000; ++iteration)
	{
		opl_callback_t callback;
		delay_data_t* data;
		unsigned int time;
		unsigned int newtime;
		int i;

		for (i=0; i<MAX_OPL_QUEUE; ++i)
		{
			time = rand() % 0x10000;
			OPL_Queue_Push(queue, NULL, NULL, time);
		}

		time = 0;

		for (i=0; i<MAX_OPL_QUEUE; ++i)
		{
			assert(!OPL_Queue_IsEmpty(queue));
			newtime = OPL_Queue_Peek(queue);
			assert(OPL_Queue_Pop(queue, &callback, &data));

			assert(newtime >= time);
			time = newtime;
		}

		assert(OPL_Queue_IsEmpty(queue));
		assert(!OPL_Queue_Pop(queue, &callback, &data));
	}
}

#endif
