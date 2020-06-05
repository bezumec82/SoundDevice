#ifndef __CB_H_
#define __CB_H_

/*! @file Circular buffer header */


struct s_circular_buffer
{
	void * 		 	buffer;
	int				head;
	int				tail;
	int				buffer_lenght;
	int				use_header;
};

int circular_buffer_init(struct s_circular_buffer	*pcb, int buffer_lenght, int use_header);
int circular_buffer_free(struct s_circular_buffer	*pcb);
int circular_buffer_push(struct s_circular_buffer	*pcb, void *item, int item_size);
int circular_buffer_pop(struct s_circular_buffer	*pcb, void *item, int item_size);
int circular_buffer_get(struct s_circular_buffer	*pcb, void *item, int item_size);
int circular_buffer_lenght(struct s_circular_buffer	*pcb);
int circular_buffer_set_lenght(struct s_circular_buffer	*pcb, int buffer_lenght);
int circular_buffer_clear(struct s_circular_buffer	*pcb);

#endif /* __CB_H_ */


