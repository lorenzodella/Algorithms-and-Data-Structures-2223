#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_AUTO 512

//-----------------------------SCANF--------------------------------

void scanf_veloce_numero(int* n){
    *n=0;
    int c=getchar_unlocked();
    while( c!=' ' && c!=EOF && c!='\n' && c!='\r'){
        *n=*n*10+c-48;
        c=getchar_unlocked();
    }
}

int scanf_veloce(char* s){
    int i=0;
    char c=getchar_unlocked();
    while( c!=' ' && c!=EOF){
        if(c!='\n' && c!='\r'){
            s[i]=c;
            i++;
        }
        c=getchar_unlocked();
    }
    s[i]='\0';
    return i;
}

//-----------------------------MAXHEAP--------------------------------

typedef struct heap {
    int vet[MAX_AUTO];
    int size;
} Heap;

void swap(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int heap_parent(int i){
    return (i+1)/2 - 1;
}
int heap_left(int i){
    return (i+1)*2 - 1;
}
int heap_right(int i){
    return (i+1)*2;
}

void max_heapify(Heap *heap, int i){
    int l = heap_left(i);
    int r = heap_right(i);
    int max;
    if(l<heap->size && heap->vet[l] > heap->vet[i])
        max = l;
    else
        max = i;
    if(r<heap->size && heap->vet[r] > heap->vet[max])
        max = r;
    if(max!=i){
        swap(&heap->vet[i], &heap->vet[max]);
        max_heapify(heap, max);
    }
}

void heap_check_priority(Heap *heap, int i){
    int p = heap_parent(i);
    if(p>=0 && heap->vet[i] > heap->vet[p]){
        swap(&heap->vet[i], &heap->vet[p]);
        heap_check_priority(heap, p);
    }
}

int heap_insert(Heap *heap, int val){
    if(heap->size==MAX_AUTO)
        return 0;
    heap->vet[heap->size]=val;
    heap_check_priority(heap, heap->size);
    heap->size++;
    return 1;
}

int heap_delete(Heap *heap, int val){
    int i=0;
    while(i<heap->size && heap->vet[i]!=val)
        i++;
    if(i<heap->size && heap->vet[i]==val){
        heap->size--;
        heap->vet[i] = heap->vet[heap->size];
        max_heapify(heap, i);
        return 1;
    }
    return 0;
}

int heap_getmax(Heap heap){
    if(heap.size>0)
        return heap.vet[0];
    else
        return -1;
}

void heap_print(Heap heap){
    for(int i=0; i<heap.size; i++){
        printf("%d ", heap.vet[i]);
    }
}

//-----------------------------STAZIONI--------------------------------

typedef struct stazione {
    int distanza;
    Heap parco_auto;
} Stazione;

void print_stazione(Stazione s){
    printf("distanza: %d\n", s.distanza);
    printf("num auto: %d\n", s.parco_auto.size);
    printf("parco auto:");
    heap_print(s.parco_auto);
    printf("\n");
}

//----------------------------RBTREE---------------------------------

typedef struct nodo {
    Stazione stazione;
    struct nodo *left;
    struct nodo *right;
    struct nodo *p;
    int isRed;
    struct nodo *path_predecessor;
} Nodo;

typedef struct tree {
    Nodo *root;
    Nodo *nil;
} Tree;

void RB_init(Tree *t){
    t->nil = malloc(sizeof(Nodo));
    t->nil->left = t->nil;
    t->nil->right = t->nil;
    t->nil->p = t->nil;
    t->nil->isRed = 0;
    t->nil->path_predecessor = NULL;
    t->root = t->nil;
}

void left_rotate(Tree *t, Nodo* x){
    Nodo *y = x->right;
    x->right = y->left;
    if(y->left!=t->nil)
        y->left->p = x;
    y->p = x->p;
    if(x->p==t->nil)
        t->root = y;
    else if(x==x->p->left)
        x->p->left = y;
    else
        x->p->right = y;
    y->left = x;
    x->p = y;
}

void right_rotate(Tree *t, Nodo* x){
    Nodo *y = x->left;
    x->left = y->right;
    if(y->right!=t->nil)
        y->right->p = x;
    y->p = x->p;
    if(x->p==t->nil)
        t->root = y;
    else if(x==x->p->right)
        x->p->right = y;
    else
        x->p->left = y;
    y->right = x;
    x->p = y;
}

void RB_insert_fixup(Tree *t, Nodo* z){
    Nodo *y, *x = t->nil;
    if(z == t->root)
        t->root->isRed = 0;
    else
        x = z->p;
    if(x->isRed){
        if(x == x->p->left){
            y = x->p->right;
            if(y->isRed){
                x->isRed = 0;
                y->isRed = 0;
                x->p->isRed = 1;
                RB_insert_fixup(t, x->p);
            }
            else {
                if(z == x->right){
                    z = x;
                    left_rotate(t, z);
                    x = z->p;
                }
                x->isRed = 0;
                x->p->isRed = 1;
                right_rotate(t, x->p);
            }
        }
        else {
            y = x->p->left;
            if(y->isRed){
                x->isRed = 0;
                y->isRed = 0;
                x->p->isRed = 1;
                RB_insert_fixup(t, x->p);
            }
            else {
                if(z == x->left){
                    z = x;
                    right_rotate(t, z);
                    x = z->p;
                }
                x->isRed = 0;
                x->p->isRed = 1;
                left_rotate(t, x->p);
            }
        }
    }
}

void RB_delete_fixup(Tree *t, Nodo* x){
    Nodo *w;
    if(x->isRed || x->p == t->nil)
        x->isRed = 0;
    else if(x == x->p->left){
        w = x->p->right;
        if(w->isRed){
            w->isRed = 0;
            x->p->isRed = 1;
            left_rotate(t, x->p);
            w = x->p->right;
        }
        if(!w->left->isRed && !w->right->isRed){
            w->isRed = 1;
            RB_delete_fixup(t, x->p);
        }
        else {
            if(!w->right->isRed){
                w->left->isRed = 0;
                w->isRed = 1;
                right_rotate(t, w);
                w = x->p->right;
            }
            w->isRed = x->p->isRed;
            x->p->isRed = 0;
            w->right->isRed = 0;
            left_rotate(t, x->p);
        }
    }
    else{
        w = x->p->left;
        if(w->isRed){
            w->isRed = 0;
            x->p->isRed = 1;
            right_rotate(t, x->p);
            w = x->p->left;
        }
        if(!w->left->isRed && !w->right->isRed){
            w->isRed = 1;
            RB_delete_fixup(t, x->p);
        }
        else {
            if(!w->left->isRed){
                w->right->isRed = 0;
                w->isRed = 1;
                left_rotate(t, w);
                w = x->p->left;
            }
            w->isRed = x->p->isRed;
            x->p->isRed = 0;
            w->left->isRed = 0;
            right_rotate(t, x->p);
        }
    }
}

Nodo* RB_newnode(Tree *t, Stazione s){
    Nodo *new = malloc(sizeof(Nodo));
    new->stazione = s;
    new->left = t->nil;
    new->right = t->nil;
    new->p = t->nil;
    return new;
}

Nodo* RB_search(Tree *t, Nodo *n, int distanza){
    if(n==t->nil || distanza == n->stazione.distanza)
        return n;
    if(distanza < n->stazione.distanza)
        return RB_search(t, n->left, distanza);
    else
        return RB_search(t, n->right, distanza);
}

Nodo* RB_minimum(Tree *t, Nodo* n){
    while(n->left != t->nil){
        n = n->left;
    }
    return n;
}

Nodo* RB_maximum(Tree *t, Nodo* n){
    while(n->right != t->nil){
        n = n->right;
    }
    return n;
}

Nodo* RB_successor(Tree *t, Nodo* n){
    if(n->right != t->nil)
        return RB_minimum(t, n->right);
    Nodo* par = n->p;
    while(par!=t->nil && n==par->right){
        n = par;
        par = par->p;
    }
    return par;
}

Nodo* RB_predecessor(Tree *t, Nodo* n){
    if(n->left != t->nil)
        return RB_maximum(t, n->left);
    Nodo* par = n->p;
    while(par!=t->nil && n==par->left){
        n = par;
        par = par->p;
    }
    return par;
}

int RB_insert(Tree *t, Stazione s){
    Nodo *new = RB_newnode(t, s);
    Nodo *cur = t->root;
    Nodo *par = t->nil;

    while(cur!=t->nil){
        par = cur;
        if(new->stazione.distanza == cur->stazione.distanza)
            return 0;
        if(new->stazione.distanza < cur->stazione.distanza)
            cur = cur->left;
        else
            cur = cur->right;
    }
    new->p = par;
    if(par == t->nil)
        t->root = new;
    else if(new->stazione.distanza < par->stazione.distanza)
        par->left = new;
    else if(new->stazione.distanza > par->stazione.distanza)
        par->right = new;

    new->left = t->nil;
    new->right = t->nil;
    new->isRed = 1;
    RB_insert_fixup(t, new);
    return 1;
}

int RB_delete(Tree *t, Nodo* n){
    if(n==t->nil)
        return 0;

    Nodo *todel, *repl;
    if(n->left==t->nil || n->right==t->nil)
        todel = n;
    else
        todel = RB_successor(t, n);
    if(todel->left!=t->nil)
        repl = todel->left;
    else
        repl = todel->right;
    repl->p = todel->p;
    if(todel->p==t->nil)
        t->root = repl;
    else if(todel==todel->p->left)
        todel->p->left = repl;
    else
        todel->p->right = repl;

    if(todel!=n)
        n->stazione = todel->stazione;
    
    if(!todel->isRed)
        RB_delete_fixup(t, repl);

    free(todel);
    return 1;
}

void RB_print_preorder(Tree *t, Nodo* n){
    if(n!=t->nil){
        if(n->isRed)
            printf("R");
        else
            printf("B");
        printf("%d ", n->stazione.distanza);
        RB_print_preorder(t, n->left);
        RB_print_preorder(t, n->right);
    }
}

void RB_print_inorder(Tree *t, Nodo* n){
    if(n!=t->nil){
        RB_print_inorder(t, n->left);
        printf("%d ", n->stazione.distanza);
        RB_print_inorder(t, n->right);
    }
}

void RB_print_inorder_count(Tree *t, Nodo* n, int *count){
    if(n!=t->nil){
        *count = *count+1;
        RB_print_inorder_count(t, n->left, count);
        printf("%d ", n->stazione.distanza);
        RB_print_inorder_count(t, n->right, count);
    }
}

//----------------------------CODA---------------------------------

typedef struct c_elem {
    Nodo* val;
    struct c_elem *next;
    struct c_elem *prev;
} CodaElem;

typedef struct coda {
    CodaElem *tail;
    CodaElem *head;
} Coda;

void coda_init(Coda *q){
    q->head = NULL;
    q->tail = NULL;
}

int coda_is_empty(Coda *q){
    return (q->tail==NULL && q->head==NULL);
}

void enqueue(Coda *q, Nodo *n){
    CodaElem *e = malloc(sizeof(CodaElem));
    e->val = n;
    e->next = q->tail;
    e->prev = NULL;
    q->tail = e;
    if(e->next==NULL)
        q->head = q->tail;
    else
        e->next->prev = e;
}

Nodo* dequeue(Coda *q){
    CodaElem *e = q->head;
    q->head = q->head->prev;
    if(q->head!=NULL)
        q->head->next = NULL;
    else
        q->tail = NULL;
    Nodo *n = e->val;
    free(e);
    return n;
}

void clean(Coda *q){
    while(!coda_is_empty(q)){
        dequeue(q);
    }
}

//----------------------------PILA---------------------------------

typedef struct p_elem {
    Nodo *val;
    struct p_elem *next;
} PilaElem;

typedef struct pila {
    PilaElem *top;
} Pila;

void pila_init(Pila *p){
    p->top = NULL;
}

int pila_is_empty(Pila *p){
    return p->top == NULL;
}

void push(Pila *p, Nodo *n){
    PilaElem *e = malloc(sizeof(PilaElem));
    e->val = n;
    e->next = p->top;
    p->top = e;
}

Nodo* pop(Pila *p){
    if(pila_is_empty(p))
        return NULL;
    PilaElem *e = p->top;
    p->top = p->top->next;
    Nodo *n = e->val;
    free(e);
    return n;
}

//----------------------------AUTOSTRADA---------------------------------

int aggiungi_stazione(Tree *autostrada){
    Stazione s;
    scanf_veloce_numero(&s.distanza);
    scanf_veloce_numero(&s.parco_auto.size);
    for(int i=0; i<s.parco_auto.size; i++){
        scanf_veloce_numero(&s.parco_auto.vet[i]);
        heap_check_priority(&s.parco_auto, i);
    }
    return RB_insert(autostrada, s);
}

int demolisci_stazione(Tree *autostrada){
    int distanza;
    scanf_veloce_numero(&distanza);
    return RB_delete(autostrada, RB_search(autostrada, autostrada->root, distanza));
}

int aggiungi_auto(Tree *autostrada){
    int distanza, autonomia;
    scanf_veloce_numero(&distanza);
    scanf_veloce_numero(&autonomia);
    Nodo* n = RB_search(autostrada, autostrada->root, distanza);
    if(n==autostrada->nil)
        return 0;
    return heap_insert(&n->stazione.parco_auto, autonomia);
}

int rottama_auto(Tree *autostrada){
    int distanza, autonomia;
    scanf_veloce_numero(&distanza);
    scanf_veloce_numero(&autonomia);
    Nodo* n = RB_search(autostrada, autostrada->root, distanza);
    if(n==autostrada->nil)
        return 0;
    return heap_delete(&n->stazione.parco_auto, autonomia);
}

//----------------------------PIANIFICA PERCORSO---------------------------------

void reset(Tree *t, Nodo *n){
    if(n!=t->nil){
        n->path_predecessor = NULL;
        reset(t, n->left);
        reset(t, n->right);
    }
}

// visita in ampiezza da start
Nodo* pianifica_percorso_avanti(Tree *autostrada, Nodo* start, int end){
    Nodo *n, *succ;
    Coda q;
    int autonomia;

    coda_init(&q);
    enqueue(&q, start);
    while(!coda_is_empty(&q)){
        n = dequeue(&q);
        if(n->stazione.distanza==end)
            return n;

        if(q.tail!=NULL)
            succ = RB_successor(autostrada, q.tail->val);
        else
            succ = RB_successor(autostrada, n);
        autonomia = heap_getmax(n->stazione.parco_auto);
        if(autonomia<0)
            return NULL;
        while(succ!=autostrada->nil && succ->stazione.distanza <= n->stazione.distanza+autonomia){
            succ->path_predecessor = n;
            enqueue(&q, succ);
            succ = RB_successor(autostrada, succ);
        }
    }
    return NULL;
}

// visita in ampiezza da start con pila
Nodo* pianifica_percorso_indietro(Tree *autostrada, Nodo* start, int end){
    Nodo *n, *prec;
    Pila p, tmp;
    int autonomia;

    pila_init(&p);
    pila_init(&tmp);

    push(&p, start);
    while(!pila_is_empty(&p)){
        n = pop(&p);
        if(n->stazione.distanza==end)
            return n;
        
        if(tmp.top!=NULL)
            prec = RB_predecessor(autostrada, tmp.top->val);
        else
            prec = RB_predecessor(autostrada, n);
        autonomia = heap_getmax(n->stazione.parco_auto);
        if(autonomia<0)
            return NULL;
        while(prec!=autostrada->nil && n->stazione.distanza-autonomia <= prec->stazione.distanza){
            prec->path_predecessor = n;
            push(&tmp, prec);
            prec = RB_predecessor(autostrada, prec);
        }

        if(pila_is_empty(&p)){
            p.top = tmp.top;
            pila_init(&tmp);
        }
    }
    return NULL;
}

void backtrace(Nodo *n){
    if(n!=NULL){
        backtrace(n->path_predecessor);
        if(n->path_predecessor!=NULL)
            printf(" ");
        printf("%d", n->stazione.distanza);
    }
}

int pianifica_percorso(Tree autostrada){
    Nodo *n;
    int start, end;
    scanf_veloce_numero(&start);
    scanf_veloce_numero(&end);
    if(start==end){
        printf("%d\n", start);
        return 1;
    }
    else if(start<end){
        reset(&autostrada, autostrada.root);
        if( (n=pianifica_percorso_avanti(&autostrada, RB_search(&autostrada, autostrada.root, start), end)) != NULL){
            backtrace(n);
            printf("\n");
            return 1;
        }
    } 
    else {
        reset(&autostrada, autostrada.root);
        if( (n=pianifica_percorso_indietro(&autostrada, RB_search(&autostrada, autostrada.root, start), end)) != NULL){
            backtrace(n);
            printf("\n");
            return 1;
        }
    }
    return 0;
}

//----------------------------MAIN---------------------------------

int main(){
    Tree autostrada;
    RB_init(&autostrada);

    char *command = malloc(sizeof(char)*20);
    while(scanf_veloce(command)>0){
        if(strcmp(command, "aggiungi-stazione")==0){
            if(aggiungi_stazione(&autostrada))
                printf("aggiunta\n");
            else
                printf("non aggiunta\n");
        }
        else if(strcmp(command, "demolisci-stazione")==0){
            if(demolisci_stazione(&autostrada))
                printf("demolita\n");
            else
                printf("non demolita\n");
        }
        else if(strcmp(command, "aggiungi-auto")==0){
            if(aggiungi_auto(&autostrada))
                printf("aggiunta\n");
            else
                printf("non aggiunta\n");
        }
        else if(strcmp(command, "rottama-auto")==0){
            if(rottama_auto(&autostrada))
                printf("rottamata\n");
            else
                printf("non rottamata\n");
        }
        else if(strcmp(command, "pianifica-percorso")==0){
            if(!pianifica_percorso(autostrada))
                printf("nessun percorso\n");
        }
    }
} 
