#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_AUTO 512
//#define PROBLEMA 6809
#define PROBLEMA 308
long i=0;

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

//----------------------------RB---------------------------------

typedef struct nodo {
    Stazione stazione;
    struct nodo *left;
    struct nodo *right;
    struct nodo *p;
    int isRed;
    int color;
    struct nodo *path_predecessor;
} Nodo;

typedef struct autostrada {
    Nodo *root;
    Nodo *nil;
} Tree;

//--

typedef struct check{
    int bh;
    int isrb;
} Check;

Check* controllaSottoAl(Tree *t, Nodo* n, int bh){
    Check *c = malloc(sizeof(Check));
    if(n==t->nil){
        c->bh = bh;
        c->isrb = 1;
        return c;
    }
    if(n->isRed && (n->left->isRed || n->right->isRed)){
        c->bh = bh;
        c->isrb = 0;
        return c;
    }
    if(!n->isRed)
        bh = bh+1;
    Check *l = controllaSottoAl(t, n->left, bh);
    Check *r = controllaSottoAl(t, n->right, bh);
    if(l->isrb==0 || r->isrb==0 || l->bh!=r->bh){
        c->bh = bh;
        c->isrb = 0;
        return c;
    }
    c->bh = l->bh;
    c->isrb = 1;
    return c; 
}

int controllaRB(Tree *t){
    if(t->root == t->nil)
        return 1;
    if(t->root->isRed)
        return 0;
    Check *c = controllaSottoAl(t, t->root, 0);
    return c->isrb;
}

//--

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
    /*if(i>=PROBLEMA){
        printf(" ");
    }*/
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

//-----------------------------BST--------------------------------

//#include "bstprinter_ottimizzato.h"

Nodo* bst_newnode(Tree *t, Stazione s){
    Nodo *new = malloc(sizeof(Nodo));
    new->stazione = s;
    new->left = t->nil;
    new->right = t->nil;
    new->p = t->nil;
    return new;
}

Nodo* bst_search(Tree *t, Nodo *n, int distanza){
    //if(i>686){
    //    printf(" ");
    //}
    if(n==t->nil || distanza == n->stazione.distanza)
        return n;
    if(distanza < n->stazione.distanza)
        return bst_search(t, n->left, distanza);
    else
        return bst_search(t, n->right, distanza);
}

int bst_insert(Tree *autostrada, Stazione s){
    Nodo *new = bst_newnode(autostrada, s);
    Nodo *cur = autostrada->root;
    Nodo *par = autostrada->nil;

    while(cur!=autostrada->nil){
        par = cur;
        if(new->stazione.distanza == cur->stazione.distanza)
            return 0;
        if(new->stazione.distanza < cur->stazione.distanza)
            cur = cur->left;
        else
            cur = cur->right;
    }
    new->p = par;
    if(par == autostrada->nil)
        autostrada->root = new;
    else if(new->stazione.distanza < par->stazione.distanza)
        par->left = new;
    else if(new->stazione.distanza > par->stazione.distanza)
        par->right = new;

    new->left = autostrada->nil;
    new->right = autostrada->nil;
    new->isRed = 1;
    RB_insert_fixup(autostrada, new);
    return 1;
}

Nodo* bst_minimum(Tree *t, Nodo* n){
    while(n->left != t->nil){
        n = n->left;
    }
    return n;
}

Nodo* bst_maximum(Tree *t, Nodo* n){
    while(n->right != t->nil){
        n = n->right;
    }
    return n;
}

Nodo* bst_successor(Tree *t, Nodo* n){
    if(n->right != t->nil)
        return bst_minimum(t, n->right);
    Nodo* par = n->p;
    while(par!=t->nil && n==par->right){
        n = par;
        par = par->p;
    }
    return par;
}

Nodo* bst_predecessor(Tree *t, Nodo* n){
    if(n->left != t->nil)
        return bst_maximum(t, n->left);
    Nodo* par = n->p;
    while(par!=t->nil && n==par->left){
        n = par;
        par = par->p;
    }
    return par;
}

/*Nodo* bst_find_furthest(Nodo* n){
    int autonomia = heap_getmax(n->stazione.parco_auto);
    Nodo *tmp = n, *prec = NULL;
    while(tmp!=NULL && tmp->stazione.distanza >= n->stazione.distanza-autonomia){
        prec = tmp;
        tmp = bst_predecessor(tmp);
    }
    return prec;
}*/

void bst_print_preorder(Tree *t, Nodo* n){
    if(n!=t->nil){
        if(n->isRed)
            printf("R");
        else
            printf("B");
        printf("%d ", n->stazione.distanza);
        bst_print_preorder(t, n->left);
        bst_print_preorder(t, n->right);
    }
}

int bst_delete(Tree *autostrada, Nodo* n){
    /*if(i>PROBLEMA){
        printf("\n");
        printf("\n");
        bst_print_preorder(autostrada, n);
        //TreePrinter(*autostrada);
        printf("\n");
    }*/
    if(n==autostrada->nil)
        return 0;

    Nodo *todel, *repl;
    if(n->left==autostrada->nil || n->right==autostrada->nil)
        todel = n;
    else
        todel = bst_successor(autostrada, n);
    if(todel->left!=autostrada->nil)
        repl = todel->left;
    else
        repl = todel->right;
    //if(repl!=autostrada->nil)
    repl->p = todel->p;
    if(todel->p==autostrada->nil)
        autostrada->root = repl;
    else if(todel==todel->p->left)
        todel->p->left = repl;
    else
        todel->p->right = repl;

    if(todel!=n)
        n->stazione = todel->stazione;
    
    if(!todel->isRed)
        RB_delete_fixup(autostrada, repl);

    free(todel);
    return 1;
}

void bst_print_inorder(Tree *t, Nodo* n){
    if(n!=t->nil){
        bst_print_inorder(t, n->left);
        //print_stazione(n->stazione);
        printf("%d ", n->stazione.distanza);
        bst_print_inorder(t, n->right);
    }
}

void bst_print_inorder_count(Tree *t, Nodo* n, int *count){
    if(n!=t->nil){
        *count = *count+1;
        bst_print_inorder_count(t, n->left, count);
        //print_stazione(n->stazione);
        printf("%d ", n->stazione.distanza);
        bst_print_inorder_count(t, n->right, count);
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

void scanf_veloce_numero(int* n){
    *n=0;
    int c=getchar_unlocked();
    while( c!=' ' && c!=EOF && c!='\n' && c!='\r'){
        *n=*n*10+c-48;
        c=getchar_unlocked();
    }
}

int aggiungi_stazione(Tree *autostrada){
    Stazione s;
    scanf_veloce_numero(&s.distanza);
    scanf_veloce_numero(&s.parco_auto.size);
    for(int i=0; i<s.parco_auto.size; i++){
        scanf_veloce_numero(&s.parco_auto.vet[i]);
        heap_check_priority(&s.parco_auto, i);
    }
    //print_stazione(s);
    return bst_insert(autostrada, s);
}

int demolisci_stazione(Tree *autostrada){
    int distanza;
    scanf_veloce_numero(&distanza);
    return bst_delete(autostrada, bst_search(autostrada, autostrada->root, distanza));
}

int aggiungi_auto(Tree *autostrada){
    int distanza, autonomia;
    scanf_veloce_numero(&distanza);
    scanf_veloce_numero(&autonomia);
    Nodo* n = bst_search(autostrada, autostrada->root, distanza);
    if(n==autostrada->nil)
        return 0;
    return heap_insert(&n->stazione.parco_auto, autonomia);
}

int rottama_auto(Tree *autostrada){
    int distanza, autonomia;
    scanf_veloce_numero(&distanza);
    scanf_veloce_numero(&autonomia);
    Nodo* n = bst_search(autostrada, autostrada->root, distanza);
    if(n==autostrada->nil)
        return 0;
    return heap_delete(&n->stazione.parco_auto, autonomia);
}

//----------------------------PIANIFICA PERCORSO---------------------------------

void reset(Tree *t, Nodo *n){
    if(n!=t->nil){
        n->color = 0;
        n->path_predecessor = NULL;
        reset(t, n->left);
        reset(t, n->right);
    }
}

// idea iniziale
/*int pianifica_percorso_avanti(Tree *autostrada, Nodo* start, int end){
    Nodo* n = start;
    int distanza = n->stazione.distanza;
    int autonomia = heap_getmax(n->stazione.parco_auto);

    if(start->stazione.distanza == end){
        return 1;
    }

    while(distanza+autonomia < end){
        if(autonomia<0)
            return 0;
        n = bst_successor(autostrada, n);
        if(n==autostrada->nil || n->stazione.distanza == end)
            return 0;
        distanza = n->stazione.distanza;
        autonomia = heap_getmax(n->stazione.parco_auto);
    }
    if(pianifica_percorso_avanti(autostrada, start, n->stazione.distanza)){
        printf("%d ", n->stazione.distanza);
        return 1;
    }
    return 0;
}*/

// visita in ampiezza da start
Nodo* pianifica_percorso_avanti(Tree *autostrada, Nodo* start, int end){
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
        if(q.tail!=NULL)
            succ = bst_successor(autostrada, q.tail->val);
        else
            succ = bst_successor(autostrada, n);
        autonomia = heap_getmax(n->stazione.parco_auto);
        if(autonomia<0)
            return NULL;
        while(succ!=autostrada->nil && succ->stazione.distanza <= n->stazione.distanza+autonomia){
            if(succ->color==0){
                succ->path_predecessor = n;
                succ->color=1;
                enqueue(&q, succ);
            }
            succ = bst_successor(autostrada, succ);
        }
        n->color=2;
    }
    return NULL;
}

// visita in ampiezza da end
Nodo* pianifica_percorso_indietro(Tree *autostrada, Nodo* end, int start){
    end->color=1;
    Nodo *n, *succ;
    Coda q;
    int autonomia;

    init(&q);
    enqueue(&q, end);
    while(!is_empty(&q)){
        n = dequeue(&q);
        if(n->stazione.distanza==start){
            clean(&q);
            return n;
        }
        if(q.tail!=NULL)
            succ = bst_successor(autostrada, q.tail->val);
        else
            succ = bst_successor(autostrada, n);
        while(succ!=autostrada->nil && succ->stazione.distanza <= start){
            autonomia = heap_getmax(succ->stazione.parco_auto);
            if(autonomia<0)
                return NULL;
            if(succ->color==0 && n->stazione.distanza+autonomia >= succ->stazione.distanza){
                succ->path_predecessor = n;
                succ->color=1;
                enqueue(&q, succ);
            }
            succ = bst_successor(autostrada, succ);
        }
        n->color=2;
    }
    return NULL;
}

// visita in ampiezza da start con passo piu lontano e torno indietro
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

// visita in ampiezza da start
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
    scanf_veloce_numero(&start);
    scanf_veloce_numero(&end);
    if(start==end){
        printf("%d\n", start);
        return 1;
    }
    else if(start<end){
        /*if( pianifica_percorso_avanti(&autostrada, bst_search(&autostrada, autostrada.root, start), end)){
            printf("%d\n", end);
            return 1;
        }*/
        reset(&autostrada, autostrada.root);
        if( (n=pianifica_percorso_avanti(&autostrada, bst_search(&autostrada, autostrada.root, start), end)) != NULL){
            post_backtrace(n);
            printf("\n");
            return 1;
        }
    } 
    else {
        reset(&autostrada, autostrada.root);
        if( (n=pianifica_percorso_indietro(&autostrada, bst_search(&autostrada, autostrada.root, end), start)) != NULL){
            pre_backtrace(n);
            printf("\n");
            return 1;
        }
    }
    return 0;
}

//----------------------------MAIN---------------------------------

int scanf_veloce(char* s){
    int i=0;
    char c=getchar_unlocked();
    while( c!=' ' && c!=EOF){
        if(c!='\n' && c!='\r'){
            s[i]=c;
            i++;
        }c=getchar_unlocked();
    }
    s[i]='\0';
    return i;
}

int main(){
    //int count, check;

    Tree autostrada;
    autostrada.nil = malloc(sizeof(Nodo));
    autostrada.nil->left = autostrada.nil;
    autostrada.nil->right = autostrada.nil;
    autostrada.nil->p = autostrada.nil;
    autostrada.nil->isRed = 0;
    autostrada.nil->color = 0;
    autostrada.nil->path_predecessor = NULL;
    autostrada.root = autostrada.nil;

    char *command = malloc(sizeof(char)*20);
    while(scanf_veloce(command)>0){
        i=i+1;
        //printf("%li\t", i);
        //if(i==686){
        //    bst_print_inorder(&autostrada, autostrada.root);
        //    printf("\n");
        //}
        if(strcmp(command, "aggiungi-stazione")==0){
            if(aggiungi_stazione(&autostrada)){
                printf("aggiunta\n");
                /*if(i>=PROBLEMA){
                    count = check = 0;
                    bst_print_inorder_count(&autostrada, autostrada.root, &count);
                    printf("\n\t%d", count);
                    check = controllaRB(&autostrada);
                    printf("\t%d", check);
                    if(check==0){
                        printf(" ");
                    }
                    printf("\n");
                }*/
            }else
                printf("non aggiunta\n");
        }
        else if(strcmp(command, "demolisci-stazione")==0){
            if(demolisci_stazione(&autostrada)){
                printf("demolita\n");
                /*if(i>=PROBLEMA){
                    count = check = 0;
                    bst_print_inorder_count(&autostrada, autostrada.root, &count);
                    printf("\n\t%d", count);
                    check = controllaRB(&autostrada);
                    printf("\t%d", check);
                    if(check==0){
                        printf(" ");
                    }
                    printf("\n");
                }*/
            }else
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