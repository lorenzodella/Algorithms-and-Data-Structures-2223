#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_AUTO 512

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

//-----------------------------BST--------------------------------

typedef struct nodo {
    Stazione stazione;
    struct nodo *left;
    struct nodo *right;
    struct nodo *p;
    int color;
    struct nodo *path_predecessor;
} Nodo;

typedef struct autostrada {
    Nodo *root;
} Tree;

//#include "bstprinter.h"

Nodo* bst_newnode(Stazione s){
    Nodo *new = malloc(sizeof(Nodo));
    new->stazione = s;
    new->left = NULL;
    new->right = NULL;
    new->p = NULL;
    return new;
}

Nodo* bst_search(Nodo *n, int distanza){
    if(n==NULL || distanza == n->stazione.distanza)
        return n;
    if(distanza < n->stazione.distanza)
        return bst_search(n->left, distanza);
    else
        return bst_search(n->right, distanza);
}

int bst_insert(Tree *autostrada, Stazione s){
    Nodo *new = bst_newnode(s);
    Nodo *cur = autostrada->root;
    Nodo *par = NULL;

    while(cur!=NULL){
        par = cur;
        if(new->stazione.distanza == cur->stazione.distanza)
            return 0;
        if(new->stazione.distanza < cur->stazione.distanza)
            cur = cur->left;
        else
            cur = cur->right;
    }
    new->p = par;
    if(par == NULL)
        autostrada->root = new;
    else if(new->stazione.distanza < par->stazione.distanza)
        par->left = new;
    else if(new->stazione.distanza > par->stazione.distanza)
        par->right = new;

    return 1;
}

Nodo* bst_minimum(Nodo* n){
    while(n->left != NULL){
        n = n->left;
    }
    return n;
}

Nodo* bst_maximum(Nodo* n){
    while(n->right != NULL){
        n = n->right;
    }
    return n;
}

Nodo* bst_successor(Nodo* n){
    if(n->right != NULL)
        return bst_minimum(n->right);
    Nodo* par = n->p;
    while(par!=NULL && n==par->right){
        n = par;
        par = par->p;
    }
    return par;
}

Nodo* bst_predecessor(Nodo* n){
    if(n->left != NULL)
        return bst_maximum(n->left);
    Nodo* par = n->p;
    while(par!=NULL && n==par->left){
        n = par;
        par = par->p;
    }
    return par;
}

Nodo* bst_find_furthest(Nodo* n){
    int autonomia = heap_getmax(n->stazione.parco_auto);
    Nodo *tmp = n, *prec = NULL;
    while(tmp!=NULL && tmp->stazione.distanza >= n->stazione.distanza-autonomia){
        prec = tmp;
        tmp = bst_predecessor(tmp);
    }
    return prec;
}

int bst_delete(Tree *autostrada, Nodo* n){
    if(n==NULL)
        return 0;

    Nodo *todel, *repl;
    if(n->left==NULL || n->right==NULL)
        todel = n;
    else
        todel = bst_successor(n);
    if(todel->left!=NULL)
        repl = todel->left;
    else
        repl = todel->right;
    if(repl!=NULL)
        repl->p = todel->p;
    if(repl!=NULL && repl->p==NULL)
        autostrada->root = repl;
    else if(todel->p!=NULL){
        if(todel==todel->p->left)
            todel->p->left = repl;
        else
            todel->p->right = repl;
    }
    else{
        autostrada->root=NULL;
    }
    if(todel!=n)
        n->stazione = todel->stazione;
    
    free(todel);
    return 1;
}

void bst_print_inorder(Nodo* n){
    if(n!=NULL){
        bst_print_inorder(n->left);
        print_stazione(n->stazione);
        bst_print_inorder(n->right);
    }
}

//----------------------------CODA---------------------------------

typedef struct elem {
    Nodo* val;
    struct elem *next;
    struct elem *prev;
} Elem;

typedef struct coda {
    Elem *tail;
    Elem *head;
} Coda;

void init(Coda *p){
    p->head = NULL;
    p->tail = NULL;
}

int is_empty(Coda *p){
    return (p->tail==NULL && p->head==NULL);
}

void enqueue(Coda *p, Nodo *n){
    Elem *e = malloc(sizeof(Elem));
    e->val = n;
    e->next = p->tail;
    e->prev = NULL;
    p->tail = e;
    if(e->next==NULL)
        p->head = p->tail;
    else
        e->next->prev = e;
}

Nodo* dequeue(Coda *p){
    Elem *e = p->head;
    p->head = p->head->prev;
    if(p->head!=NULL)
        p->head->next = NULL;
    else
        p->tail = NULL;
    Nodo *n = e->val;
    free(e);
    return n;
}

void clean(Coda *p){
    while(!is_empty(p)){
        dequeue(p);
    }
}

//----------------------------AUTOSTRADA---------------------------------

int aggiungi_stazione(Tree *autostrada){
    Stazione s;
    if(scanf("%d", &s.distanza));
    if(scanf("%d", &s.parco_auto.size));
    for(int i=0; i<s.parco_auto.size; i++){
        if(scanf("%d", &s.parco_auto.vet[i]));
        heap_check_priority(&s.parco_auto, i);
    }
    //print_stazione(s);
    return bst_insert(autostrada, s);
}

int demolisci_stazione(Tree *autostrada){
    int distanza;
    if(scanf("%d", &distanza));
    return bst_delete(autostrada, bst_search(autostrada->root, distanza));
}

int aggiungi_auto(Tree *autostrada){
    int distanza, autonomia;
    if(scanf("%d", &distanza));
    if(scanf("%d", &autonomia));
    Nodo* n = bst_search(autostrada->root, distanza);
    if(n==NULL)
        return 0;
    return heap_insert(&n->stazione.parco_auto, autonomia);
}

int rottama_auto(Tree *autostrada){
    int distanza, autonomia;
    if(scanf("%d", &distanza));
    if(scanf("%d", &autonomia));
    Nodo* n = bst_search(autostrada->root, distanza);
    if(n==NULL)
        return 0;
    return heap_delete(&n->stazione.parco_auto, autonomia);
}

//----------------------------PIANIFICA PERCORSO---------------------------------

void reset(Nodo *n){
    if(n!=NULL){
        n->color = 0;
        n->path_predecessor = NULL;
        reset(n->left);
        reset(n->right);
    }
}

int pianifica_percorso_avanti(Nodo* start, int end){
    Nodo* n = start;
    int distanza = n->stazione.distanza;
    int autonomia = heap_getmax(n->stazione.parco_auto);

    if(start->stazione.distanza == end){
        return 1;
    }

    while(distanza+autonomia < end){
        if(autonomia<0)
            return 0;
        n = bst_successor(n);
        if(n==NULL || n->stazione.distanza == end)
            return 0;
        distanza = n->stazione.distanza;
        autonomia = heap_getmax(n->stazione.parco_auto);
    }
    if(pianifica_percorso_avanti(start, n->stazione.distanza)){
        printf("%d ", n->stazione.distanza);
        return 1;
    }
    return 0;
}

/*Nodo* pianifica_percorso_avanti(Nodo* start, int end){
    start->color=1;
    Nodo *n, *succ;
    Coda q;
    int autonomia;

    init(&q);
    enqueue(&q, start);
    while(!is_empty(&q)){
        n = dequeue(&q);
        if(n->stazione.distanza==end){
            clean(&q);
            return n;
        }
        succ = bst_successor(n);
        autonomia = heap_getmax(n->stazione.parco_auto);
        while(succ!=NULL && succ->stazione.distanza <= n->stazione.distanza+autonomia){
            if(succ->color==0){
                succ->path_predecessor = n;
                succ->color=1;
                enqueue(&q, succ);
            }
            succ = bst_successor(succ);
        }
        n->color=2;
    }
    return NULL;
}*/

Nodo* pianifica_percorso_indietro(Nodo* end, Nodo* start){
    end->color=1;
    Nodo *n, *succ;
    Coda q;
    int autonomia;

    init(&q);
    enqueue(&q, end);
    while(!is_empty(&q)){
        n = dequeue(&q);
        if(n->stazione.distanza==start->stazione.distanza){
            clean(&q);
            return n;
        }
        succ = bst_successor(n);
        while(succ!=NULL && succ->stazione.distanza <= start->stazione.distanza){
            autonomia = heap_getmax(succ->stazione.parco_auto);
            if(autonomia<0)
                return NULL;
            if(succ->color==0 && n->stazione.distanza+autonomia >= succ->stazione.distanza){
                succ->path_predecessor = n;
                succ->color=1;
                enqueue(&q, succ);
            }
            succ = bst_successor(succ);
        }
        n->color=2;
    }
    return NULL;
}

/*Nodo* pianifica_percorso_indietro(Nodo* end, Nodo* start){
    start->color=1;
    Nodo *n, *succ;
    Coda q;

    init(&q);
    enqueue(&q, start);
    while(!is_empty(&q)){
        n = dequeue(&q);
        if(n->stazione.distanza==end->stazione.distanza)
            return n;
        succ = bst_find_furthest(n);
        while(succ!=NULL && succ!=n){
            if(succ->color==0){
                succ->path_predecessor = n;
                succ->color=1;
                enqueue(&q, succ);
            }
            succ = bst_successor(succ);
        }
        n->color=2;
    }
    return NULL;
}*/

/*Nodo* pianifica_percorso_indietro(Nodo* end, Nodo* start){
    start->color=1;
    Nodo *n, *succ;
    Coda q;
    int autonomia;

    init(&q);
    enqueue(&q, start);
    while(!is_empty(&q)){
        n = dequeue(&q);
        if(n->stazione.distanza==end->stazione.distanza)
            return n;
        succ = bst_predecessor(n);
        autonomia = heap_getmax(n->stazione.parco_auto);
        while(succ!=NULL && succ->stazione.distanza >= n->stazione.distanza-autonomia){
            if(succ->color==0){
                succ->path_predecessor = n;
                succ->color=1;
                enqueue(&q, succ);
            }
            succ = bst_predecessor(succ);
        }
        n->color=2;
    }
    return NULL;
}*/

void post_backtrace(Nodo *n){
    if(n!=NULL){
        post_backtrace(n->path_predecessor);
        if(n->path_predecessor!=NULL)
            printf(" ");
        printf("%d", n->stazione.distanza);
    }
}

void pre_backtrace(Nodo *n){
    if(n!=NULL){
        printf("%d", n->stazione.distanza);
        if(n->path_predecessor!=NULL)
            printf(" ");
        pre_backtrace(n->path_predecessor);
    }
}

int pianifica_percorso(Tree autostrada){
    Nodo *n;
    int start, end;
    if(scanf("%d", &start));
    if(scanf("%d", &end));
    if(start==end){
        printf("%d\n", start);
        return 1;
    }
    else if(start<end){
        if( pianifica_percorso_avanti(bst_search(autostrada.root, start), end)){
            printf("%d\n", end);
            return 1;
        }
        /*reset(autostrada.root);
        if( (n=pianifica_percorso_avanti(bst_search(autostrada.root, start), end)) != NULL){
            post_backtrace(n);
            printf("\n");
            return 1;
        }*/
    } 
    else {
        reset(autostrada.root);
        if( (n=pianifica_percorso_indietro(bst_search(autostrada.root, end), bst_search(autostrada.root, start))) != NULL){
            pre_backtrace(n);
            printf("\n");
            return 1;
        }
    }
    return 0;
}

//----------------------------MAIN---------------------------------

int main(){
    Tree autostrada;
    autostrada.root = NULL;

    char *command = malloc(sizeof(char)*20);
    while(scanf("%s", command)>0){
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

    /*Coda p;
    init(&p);
    Nodo *n;
    n=malloc(sizeof(Nodo));
    n->stazione.distanza = 1;
    enqueue(&p, n);
    n=malloc(sizeof(Nodo));
    n->stazione.distanza = 2;
    enqueue(&p, n);
    n=malloc(sizeof(Nodo));
    n->stazione.distanza = 3;
    enqueue(&p, n);

    printf("%d ", dequeue(&p)->stazione.distanza);
    printf("%d ", dequeue(&p)->stazione.distanza);
    printf("%d\n", dequeue(&p)->stazione.distanza);
    printf("%d\n", is_empty(&p));

    n=malloc(sizeof(Nodo));
    n->stazione.distanza = 4;
    enqueue(&p, n);
    
    printf("%d\n", is_empty(&p));

    printf("%d\n", dequeue(&p)->stazione.distanza);
    printf("%d\n", is_empty(&p));*/

    //printf("\n");
    //bst_print_inorder(autostrada.root);
    //TreePrinter(autostrada);
    //printf("\n%d\n", bst_minimum(autostrada.root)->stazione.distanza);
    //printf("\n%d\n", bst_successor(bst_minimum(autostrada.root))->stazione.distanza);
    //bst_print_inorder(autostrada.root);
    //TreePrinter(autostrada);
} 

//"args": ["<", "../tests/archivio_test_aperti/open_3.txt", ">", "../tests/archivio_test_aperti/esiti/open_3.output.txt"],