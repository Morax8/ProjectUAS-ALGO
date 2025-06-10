#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


typedef struct BSTNode
{
    char name[100];
    char code[50];
    float price;
    struct BSTNode *left, *right;
} BSTNode;


BSTNode *createNode(char *name, char *code, float price)
{
    BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));
    strcpy(newNode->name, name);
    strcpy(newNode->code, code);
    newNode->price = price;
    newNode->left = newNode->right = NULL;
    return newNode;
}

BSTNode *insertBST(BSTNode *root, char *name, char *code, float price)
{
    if (!root)
        return createNode(name, code, price);

    if (strcmp(name, root->name) < 0)
        root->left = insertBST(root->left, name, code, price);
    else if (strcmp(name, root->name) > 0)
        root->right = insertBST(root->right, name, code, price);

    return root;
}

BSTNode *searchBST(BSTNode *root, char *name)
{
    if (!root || strcmp(name, root->name) == 0)
        return root;

    if (strcmp(name, root->name) < 0)
        return searchBST(root->left, name);
    else
        return searchBST(root->right, name);
}

BSTNode *findMin(BSTNode *root)
{
    while (root && root->left)
        root = root->left;
    return root;
}

BSTNode *deleteBST(BSTNode *root, char *name)
{
    if (!root)
        return NULL;

    if (strcmp(name, root->name) < 0)
        root->left = deleteBST(root->left, name);
    else if (strcmp(name, root->name) > 0)
        root->right = deleteBST(root->right, name);
    else
    {
        // 1. Leaf
        if (!root->left && !root->right)
        {
            free(root);
            return NULL;
        }
        // 2. One child
        else if (!root->left || !root->right)
        {
            BSTNode *temp = root->left ? root->left : root->right;
            free(root);
            return temp;
        }
        // 3. Two children
        else
        {
            BSTNode *succ = findMin(root->right);
            strcpy(root->name, succ->name);
            strcpy(root->code, succ->code);
            root->price = succ->price;
            root->right = deleteBST(root->right, succ->name);
        }
    }
    return root;
}

BSTNode *loadAllProducts()
{
    const char *files[] = {
        "Data/Drinks.txt", "Data/Snacks.txt", "Data/Foods.txt",
        "Data/FrozenFood.txt", "Data/FruitsVeggies.txt",
        "Data/CleaningSupplies.txt", "Data/Groceries.txt"};
    int numFiles = sizeof(files) / sizeof(files[0]);

    BSTNode *root = NULL;
    char name[100], code[50];
    float price;

    for (int i = 0; i < numFiles; i++)
    {
        FILE *f = fopen(files[i], "r");
        if (!f)
            continue;

        while (fscanf(f, " %[^#]#%[^#]#%f\n", name, code, &price) == 3)
        {
            root = insertBST(root, name, code, price);
        }
        fclose(f);
    }
    return root;
}

void inOrderTraversal(BSTNode *root)
{
    if (!root)
        return;
    inOrderTraversal(root->left);
    printf("Name: %-25s | Code: %-10s | Price: Rp %.0f\n", root->name, root->code, root->price);
    inOrderTraversal(root->right);
}

int main()
{
    BSTNode *root = loadAllProducts();
    int choice;
    char name[100];

    while (1)
    {
        printf("\n=== AEON Product BST ===\n");
        printf("1. Lihat semua produk (in-order)\n");
        printf("2. Cari produk\n");
        printf("3. Hapus produk\n");
        printf("4. Keluar\n");
        printf("5. Tambah produk baru\n");
        printf("Pilih menu: ");
        scanf("%d", &choice);
        getchar(); // Buat handle newline

        switch (choice)
        {
        case 1:
            printf("\n=== Semua Produk ===\n");
            inOrderTraversal(root);
            break;

        case 2:
            printf("Masukkan nama produk yang ingin dicari: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            BSTNode *found = searchBST(root, name);
            if (found)
                printf("✅ Ketemu: %s | %s | Rp %.0f\n", found->name, found->code, found->price);
            else
                printf("❌ Produk tidak ditemukan.\n");
            break;

        case 3:
            printf("Masukkan nama produk yang ingin dihapus: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            root = deleteBST(root, name);
            printf("Jika produk ada, sekarang sudah dihapus.\n");
            break;

        case 4:
            printf("Keluar dari program...\n");
            return 0;

        case 5:
            {
                char newName[100], newCode[50];
                float newPrice;

                printf("Masukkan nama produk baru: ");
                fgets(newName, sizeof(newName), stdin);
                newName[strcspn(newName, "\n")] = 0;

                printf("Masukkan kode produk baru: ");
                fgets(newCode, sizeof(newCode), stdin);
                newCode[strcspn(newCode, "\n")] = 0;

                printf("Masukkan harga produk baru: ");
                scanf("%f", &newPrice);
                getchar();

                root = insertBST(root, newName, newCode, newPrice);
                printf("Produk baru berhasil ditambah!\n");
            }
            break;

        default:
            printf("Pilihan tidak valid!\n");
            break;
        }
    }
    return 0;
}

