#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/slab.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#endif
#include "cb.h"

/*----------------------------------------------------------------------------*/
int circular_buffer_init(struct s_circular_buffer	*pcb, int buffer_lenght, int use_header)
{
	if (pcb && (buffer_lenght > 0))
	{
#ifdef __KERNEL__
		pcb->buffer = kmalloc(buffer_lenght, GFP_KERNEL);
#else
		pcb->buffer = malloc(buffer_lenght);
#endif
		pcb->buffer_lenght = buffer_lenght;
		pcb->use_header = use_header;
		pcb->head = 0;
		pcb->tail = 0;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int circular_buffer_free(struct s_circular_buffer	*pcb)
{
	if (pcb)
	{
#ifdef __KERNEL__
		if (pcb->buffer) kfree(pcb->buffer);
#else
		if (pcb->buffer) free(pcb->buffer);
#endif
		pcb->buffer = NULL;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int circular_buffer_push(struct s_circular_buffer	*pcb, void *item, int item_size)
{
	int res = -1;	
	int i, j, next, size, overflow = 0;
	unsigned char *ptr8, *ptr8_itemsize, *ptr8_item;
	
	if (pcb && item && (item_size > 0))
	{
		size = circular_buffer_lenght(pcb);
		next = pcb->head + item_size;
		size = size + item_size;
		if (pcb->use_header)
		{
			next += sizeof(item_size);
			size += sizeof(item_size);
		}
		if (next >= pcb->buffer_lenght)
		{
			overflow = 1;
			next -= pcb->buffer_lenght;
		}

		if ((size < pcb->buffer_lenght) && pcb->buffer)
		{
			ptr8 = (unsigned char *)pcb->buffer;
			ptr8_item = (unsigned char *)item;
			ptr8_itemsize = (unsigned char *)(&item_size);
			if (overflow)
			{
				ptr8+=pcb->head;
				j = pcb->head;
				
				/* copy sizeof item_size */
				if (pcb->use_header)
				{
					for (i=0;i<sizeof(item_size);i++,j++)
					{	
						if (j >= pcb->buffer_lenght)
						{
							j -= pcb->buffer_lenght;
							ptr8 = (unsigned char *)pcb->buffer;
						}
						*ptr8++=*ptr8_itemsize++;
					}
				}

				/* copy item_size */
				for (i=0;i<item_size;i++,j++)
				{	
					if (j >= pcb->buffer_lenght)
					{
						j -= pcb->buffer_lenght;
						ptr8 = (unsigned char *)pcb->buffer;
					}
					*ptr8++=*ptr8_item++;
				}
			}
			else
			{
				ptr8+=pcb->head;
				if (pcb->use_header)
				{
					memcpy(ptr8, ptr8_itemsize, sizeof(item_size));
					ptr8+=sizeof(item_size);
				}
				memcpy(ptr8, item, item_size);
			}
			pcb->head = next;
			res = item_size;
		}
	}
	
	return res;
}
/*----------------------------------------------------------------------------*/
static int circular_buffer_get_pop(struct s_circular_buffer	*pcb, void *item, int item_size, int pop)
{
	int res = -1;
	int i,j;
	int next, size, pop_item_size = 0;
	unsigned char *ptr8, *ptr8_pop_itemsize, *ptr8_pop_item;

	if (pcb && pcb->buffer && item && (item_size > 0))
	{
		size = circular_buffer_lenght(pcb);
		if (size > 0)
		{
			ptr8 = (unsigned char *)pcb->buffer;
			ptr8_pop_item = (unsigned char *)item;
			ptr8_pop_itemsize = (unsigned char *)(&pop_item_size);
			
			ptr8+=pcb->tail;
			j = pcb->tail;
			/* copy sizeof pop_item_size */
			if (pcb->use_header)
			{			
				for (i=0;i<sizeof(pop_item_size);i++,j++)
				{	
					if (j >= pcb->buffer_lenght)
					{
						j -= pcb->buffer_lenght;
						ptr8 = (unsigned char *)pcb->buffer;
					}
					*ptr8_pop_itemsize++=*ptr8++;
				}
				size -= sizeof(pop_item_size);
			}
			else
			{
				pop_item_size = item_size;
			}
			
			if ((pop_item_size <= size) && (pop_item_size <= item_size))
			{
				/* copy pop_item_size */
				for (i=0;i<pop_item_size;i++,j++)
				{	
					if (j >= pcb->buffer_lenght)
					{
						j -= pcb->buffer_lenght;
						ptr8 = (unsigned char *)pcb->buffer;
					}
					*ptr8_pop_item++ = *ptr8++;
				}
				res = pop_item_size;
				
				if (pop)
				{
					next = pcb->tail + pop_item_size;
					if (pcb->use_header)
					{
						next +=  sizeof(pop_item_size);
					}					
					if(next >= pcb->buffer_lenght)
						next -= pcb->buffer_lenght;

					pcb->tail = next;
				}
			}
		}
		else
		{			
			res = 0;
		}
		
		if (pcb->head == pcb->tail)
		{
			pcb->head = pcb->tail = 0;
		}
	}

	return res;
}
/*----------------------------------------------------------------------------*/
int circular_buffer_pop(struct s_circular_buffer	*pcb, void *item, int item_size)
{
	return circular_buffer_get_pop(pcb, item, item_size, 1);
}
/*----------------------------------------------------------------------------*/
int circular_buffer_get(struct s_circular_buffer	*pcb, void *item, int item_size)
{
	return circular_buffer_get_pop(pcb, item, item_size, 0);
}
/*----------------------------------------------------------------------------*/
int circular_buffer_lenght(struct s_circular_buffer	*pcb)
{
	int lenght = 0;
	if (pcb && (pcb->buffer_lenght > 0))
	{
#if 0		
		lenght = (pcb->buffer_lenght + pcb->tail - pcb->head);
		if (lenght > 0)
			lenght = lenght % pcb->buffer_lenght;
		else
			lenght = 0;
#else
		lenght = (pcb->head - pcb->tail);
		if (lenght < 0)
			lenght += pcb->buffer_lenght;
		if (lenght < 0)
			lenght = 0;
#endif
	}
	return lenght;
}
/*----------------------------------------------------------------------------*/
int circular_buffer_set_lenght(struct s_circular_buffer	*pcb, int buffer_lenght)
{
	int lenght = 0;
	if (pcb && (pcb->buffer_lenght > 0) && (pcb->buffer_lenght > buffer_lenght))
	{
		pcb->tail = pcb->buffer_lenght + pcb->head - buffer_lenght;
		if (pcb->tail > 0)
			pcb->tail = pcb->tail % pcb->buffer_lenght;
		else
			pcb->tail = pcb->head = 0;
			
		lenght = circular_buffer_lenght(pcb);
	}
	return lenght;
}
/*----------------------------------------------------------------------------*/
int circular_buffer_clear(struct s_circular_buffer	*pcb)
{
	if (pcb)
	{
		pcb->head = pcb->tail = 0;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
#ifdef __KERNEL__
EXPORT_SYMBOL(circular_buffer_init);
EXPORT_SYMBOL(circular_buffer_free);
EXPORT_SYMBOL(circular_buffer_push);
EXPORT_SYMBOL(circular_buffer_pop);
EXPORT_SYMBOL(circular_buffer_get);
EXPORT_SYMBOL(circular_buffer_lenght);
EXPORT_SYMBOL(circular_buffer_set_lenght);
EXPORT_SYMBOL(circular_buffer_clear);
#endif
