#include <malloc.h>

// Estructura para cada elemento de la cola
typedef struct _q_elem {
    void *data;
    struct _q_elem *next;
} q_elem;


// Estructura para contener el inicio y final de la cola
typedef struct _queue {
    q_elem *front;
    q_elem *back;
    int size;
} *queue;


// Devuelve la cola vacia
queue queue_init(void)
{
    queue q = malloc(sizeof(struct _q));
    if (q == NULL) error("No space to allocate.");
    q->front = NULL;
    q->back = NULL;
    q->size = 0;
    return q;
}


// Destruye la cola y todo su contenido
// No se provee proteccion contra un fallo de free()
void queue_destroy(queue q)
{
    if (q->front != NULL) {
        q_elem helper;
        while (q->front.next != NULL) {
            helper = q->front.next;
            free(q->front);
            q->front = helper;
        }
        free(helper);
    }
    free(q);
}


int queue_size(queue q)
{
    return q->size;
}


// Inserta un elemento en la cola, se guarda como su 'back'
// Se termina el programa en caso de un fallo en malloc
void queue_insert(queue q, void *elem)
{
    q_elem *new = malloc(sizeof(q_elem));
    if (new == NULL) error("No space to allocate.");
    new->data = elem;
    new->next = NULL;
    if (queue_size(q) == 0) {
        q->front = new;
        q->back = new;
    }
    else {
        q->back.next = new;
        q->back = new;
    }
    q->size++;
    return;
}


/*
* Devuelve el dato guardado en el front de la cola.
* Si la cola esta vacia, se devuelve NULL.
* Notar que no se diferencia entre esto y haber guardado NULL
* en la cola (lo cual pudo haber sido un error).
*/ 
void *queue_pop(queue q)
{
    if (queue_size(q) == 0)
        return NULL;
    if (queue_size(q) == 1)
        q->back = NULL;
    q->size--;
    void *r = q->front.data;
    q->front = q->front.next;
    return r;
}

