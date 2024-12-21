/*
 *  Synopsis:
 *	A hierarchical replacement for malloc() with callbacks on free()
 *  Description:
 *	jmscott_halloc is a memory allocator dependent upon and compatible with
 *	the well known malloc().  halloc() organizes memory into hierarchical
 *	allocations;  each halloc()ed chunk has a parent.  freeing the parent
 *	frees all the descendents.
 *
 *	halloc() is suitable for long running processes with complex memory
 *	management.  halloc() is NOT thread safe, yet.
 *
 *	For example,
 *
 *		parent = halloc(NULL, 10);
 *		child = halloc(parent, 20);
 *
 *		... do some work ...
 *	
 *		halloc_free(parent);
 *
 *	will free all parent and all descendents, invoking callbacks deepest
 *	child first.
 *
 *	This version of halloc is a clean room rewrite of a similar
 *	production version owned by partners of august.com (and inspired
 *	by Britton-Lee PHI, from back in the day).
 *  Note:
 *	Should callbacks be called in LIFO order, instead of deepest first?
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "jmscott/libjmscott.h"

/*
 *  Private struct tracks halloc'ed memory.
 */
struct jmscott_memory
{
	struct jmscott_memory			*parent;
	struct jmscott_memory			*child_head;
	struct jmscott_memory			*child_tail;
	struct jmscott_memory			*next;
	struct jmscott_memory			*previous;

	struct jmscott_halloc_callback		*callback_head;
};

/*
 *  A new parent adopts a halloc()ed memory blob.
 *
 *  Note:
 *	No loop detections.  uggh.
 */
void *
jmscott_halloc_adopt(void *parent, void *p) 
{
	struct jmscott_memory *parent_mp;
	struct jmscott_memory *mp;

	/*
	 *  Die if no adoptive parent.
	 *  Should this simply clear the parent structure?
	 */
	if (!p) {
		errno = EINVAL;
		return (void *)0;
	}

	parent_mp = (struct jmscott_memory *)
			((char *)parent - sizeof *parent_mp)
	;
	mp = (struct jmscott_memory *)((char *)p - sizeof *mp);

	/*
	 *  Already a child of this parent?
	 */
	if (mp->parent == parent_mp)
		return mp;
	/*
	 *  Remove old parent's reference's
	 */
	if (mp->parent) {
		/*
		 *  Old parent's first child points to next sibling.
		 */
		if (mp->parent->child_head == mp)
			mp->parent->child_head = mp->next;
		/*
		 *  Old parent's last child points to previous sibling.
		 */
		if (mp->parent->child_tail == mp)
			mp->parent->child_tail = mp->previous;
		/*
		 *  Previous sibling points to our next sibling.
		 */
		if (mp->previous)
			mp->previous->next = mp->next;
		/*
		 *  Next sibling points back to our previous sibling.
		 */
		if (mp->next)
			mp->next->previous = mp->previous;
		/*
		 *  Previous sibling points forward to our next sibling.
		 */
		if ((mp->previous = parent_mp->child_tail))
			mp->previous->next = mp;
	}
	/*
	 *  Wire into new parent.
	 */
	mp->next = 0;
	if ((mp->previous = parent_mp->child_tail))
		mp->previous->next = mp;
	else
		parent_mp->child_head = mp;
	parent_mp->child_tail = mp;
	return mp;
}

/*
 *  Hierarchical memory allocation.
 *  Freeing parent memory frees all descendent memory chunks.
 *
 *  Note:
 *	halloc'ed chunk may not be aligned on a boundary that is
 *	conducive to fast loads from ram.  alternative would be
 *	move memory header at tail of memory chunk.
 */
void *
jmscott_halloc(void *parent, size_t size)
{
	struct jmscott_memory *parent_p;
	struct jmscott_memory *p;

	/*
	 *  Assume malloc set errno?
	 */
	errno = 0;
	p = (struct jmscott_memory *)malloc(
		sizeof (struct jmscott_memory) + size
	);
	if (p == (void *)0)
		return (void *)0;

	bzero((void *)p, sizeof *p);

	if (parent)
		parent_p = (struct jmscott_memory *)
			((char *)parent - sizeof *parent_p)
		;
	else
		parent_p = (struct jmscott_memory *)0;

	if ((p->parent = parent_p)) {
		p->previous = parent_p->child_tail;
		if (p->previous)
			parent_p->child_tail->next = p;
		else
			parent_p->child_head = p;
		parent_p->child_tail = p;
	}
	return (void *)((char *)p + sizeof (struct jmscott_memory));
}

/*
 *  Descent first free of memory and children.
 */
static void
_free(struct jmscott_memory *m)
{
	struct jmscott_memory *c;
	struct jmscott_memory *next;

	/*
	 *  Free the children first.
	 */
	c = m->child_head;
	while (c) {
		next = c->next;
		_free(c);
		c = next;
	}
	/*
	 *  Free the callback functions
	 */
	if (m->callback_head) {
		struct jmscott_halloc_callback *cb = m->callback_head;
		struct jmscott_halloc_callback *cb_next;

		while (cb) {
			cb_next = cb->next;
			free((void *)cb);
			cb = cb_next;
		}
		m->callback_head = 0;
	}
	free(m);	/* Free this blob */
}

/*
 *  Call the associated callback functions BEFORE freeing memory.
 *  Note, the children of this chunk have already been freed,
 *  so the callback must NOT reference such children.
 *
 *  The functions are call in FIFO order, although the called
 *  function should not assume any order.  Need to think about this.
 */
static void
fire_free_callbacks(struct jmscott_memory *m)
{
	struct jmscott_memory *c;
	struct jmscott_halloc_callback *cb;

	/*
	 *  Call the descendents callbacks first.
	 */
	for (c = m->child_head;  c;  c = c->next)
		fire_free_callbacks(c);
	/*
	 *  Fire this blob's callbacks.
	 */
	for (cb = m->callback_head;  cb;  cb = cb->next)
		(*cb->func)((void *)(m + 1), cb->private_data);
}

void
jmscott_halloc_free(void *p)
{
	struct jmscott_memory *m;

	m = ((struct jmscott_memory *)p - 1);

	fire_free_callbacks(m);

	/*
	 *  Unhook from siblings.
	 */
	if (m->next)
		m->next->previous = m->previous;
	if (m->previous)
		m->previous->next = m->next;
	/*
	 *  Unhook from parent.
	 */
	if (m->parent) {
		if (m->parent->child_head == m)
			m->parent->child_head = m->next;
		if (m->parent->child_tail == m)
			m->parent->child_tail = m->previous;
	}

	/*
	 *  Free the memory, descendents first.
	 */
	_free(m);
}

void
jmscott_halloc_add_callback(
	void *p,
	void (*func)(void *, void *),
	void *private_data
)
{
	struct jmscott_memory *m;
	struct jmscott_halloc_callback *cb;

	m = (struct jmscott_memory *)((char *)p - sizeof *m);

	cb = (struct jmscott_halloc_callback *)malloc(sizeof *cb);
	cb->func = func;
	cb->private_data = private_data;
	cb->next = 0;

	if (m->callback_head) {
		struct jmscott_halloc_callback *cb2 = m->callback_head;

		/*
		 *  Find the callback tail.
		 */
		while (cb2->next)
			cb2 = cb2->next;
		cb2->next = cb;
	} else
		m->callback_head = cb;
}

char *
jmscott_halloc_strdup(void *parent, char *cp)
{
	char *p;

	if (cp == (char *)0) {
		errno = EINVAL;
		return (char *)0;
	}
	p = (char *)jmscott_halloc(parent, strlen(cp) + 1);
	if (p == (char *)0)
		return (char *)0;
	strcpy(p, cp);
	return p;
}

/*
 *  Resize/realloc a chunk of memory and move children of old chunk
 *  to new parent, then free parent.
 */
void *
jmscott_halloc_resize(void *p, size_t size)
{
	struct jmscott_memory *m, *newm;
	struct jmscott_memory *tmp;

	if (!p) {
		errno = EINVAL;
		return (void *)0;
	}
	/*
	 *  Realloc the block.
	 */
	m = ((struct jmscott_memory *)p - 1);

	/*
	 *  GCC issues incorrect warning about realloc() always moving.
	 *
	 * 	gcc (GCC) 12.1.1 20220507 (Red Hat 12.1.1-1)
	 *
	 *  Fool the compiler
	 */
	struct jmscott_memory *gcc_bug = ((struct jmscott_memory *)p - 1);;
	newm = (struct jmscott_memory *)realloc(m, sizeof *m + size);
	if (!newm)
		return (void *)0;

	/*
	 *  Block didn't move, so just return.
	 */
	if (newm == gcc_bug)
		return (void *)(gcc_bug + 1);
	/*
	 *  Block moved, so point kin to new incarnation.
	 */
	for (tmp = newm->child_head;  tmp;  tmp = tmp->next)
		tmp->parent = newm;
	/*
	 *  Block moved, point parents and siblings to new block.
	 *  Notice that we can't use values from m, since it was freed,
	 *  so borrow the values copied into newm.  Hope the compiler
	 *  can handle this.
	 */
	if (newm->next)
		newm->next->previous = newm;
	else if (newm->parent)
		newm->parent->child_tail = newm;
	if (newm->previous)
		newm->previous->next = newm;
	else if (newm->parent)
		newm->parent->child_head = newm;
	return (void *)(newm + 1);
}
