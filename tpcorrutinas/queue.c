#include <malloc.h>


typedef struct _q_elem {
    // Estructura para cada elemento de la cola
    void *data;
    struct _q_elem *next;
} q_elem;


typedef struct _queue {
    // Estructura para contener el inicio y final de la cola
    q_elem *front;
    q_elem *back;
    int size;
} *queue;


queue queue_init(void)
{
    // Devuelve la cola vacia
    queue q = malloc(sizeof(struct _q));
    if (q == NULL) error("No space to allocate.");
    q->front = NULL;
    q->back = NULL;
    q->size = 0;
    return q;
}


void queue_destroy(queue q)
{
    // Destruye la cola y todo su contenido
    // No se provee proteccion contra un fallo de free()
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


void queue_insert(queue q, void *elem)
{
    // Inserta un elemento en la cola, se guarda como su 'back'
    // Se termina el programa en caso de un fallo en malloc
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


void *queue_pop(queue q)
{
    /*
    * Devuelve el dato guardado en el front de la cola.
    * Si la cola esta vacia, se devuelve NULL.
    * Notar que no se diferencia entre esto y haber guardado NULL
    * en la cola (lo cual pudo haber sido un error).
    */ 
    if (queue_size(q) == 0)
        return NULL;
    if (queue_size(q) == 1)
        q->back = NULL;
    q->size--;
    void *r = q->front.data;
    q_elem *rem = q->front;
    q->front = q->front.next;
    free(rem);
    return r;
}

