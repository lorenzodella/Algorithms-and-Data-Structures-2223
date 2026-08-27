#include <math.h>

int height(Tree *t, Nodo *root) {
    if (root == t->nil)
        return 0;
    return fmax(height(t, root->left), height(t, root->right)) + 1;
}
 
int getcol(int h) {
    if (h == 1)
        return 1;
    return getcol(h - 1) + getcol(h - 1) + 1;
}
 
void printTree(Tree *t, int **M, Nodo *root, int col, int row, int height) {
    if (root == t->nil)
        return;
    M[row][col] = root->stazione.distanza;
    if(root->isRed)
        M[row][col]*=-1;
    printTree(t, M, root->left, col - pow(2, height - 2), row + 1, height - 1);
    printTree(t, M, root->right, col + pow(2, height - 2), row + 1, height - 1);
}
 
void TreePrinter(Tree tree) {
    int h = height(&tree, tree.root);
    int col = getcol(h);
    int **M = calloc(sizeof(int*), h);
    for (int i = 0; i < h; i++) {
        M[i] = calloc(sizeof(int), col);
    }
    printTree(&tree, M, tree.root, col / 2, 0, h);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < col; j++) {
            if (M[i][j] == 0)
                printf("  ");
            else
                printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}