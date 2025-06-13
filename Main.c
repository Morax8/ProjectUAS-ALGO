#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#define MAX_HEAP_SIZE 200
typedef struct BSTNode
{
    char name[100];
    char code[50];
    float price;
    struct BSTNode *left, *right;
} BSTNode;

typedef struct
{
    char name[100];
    char code[50];
    float price;
} HeapItem;

typedef struct
{
    HeapItem items[MAX_HEAP_SIZE];
    int size;
} MinHeap;

typedef struct
{
    char name[100];
    char code[50];
    float price;
} Product;

Product productList[200]; // max 200 produk
int productCount = 0;

// BST FUNCTIONS
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
    printf("Name: %-35s | Code: %-10s | Price: Rp %.0f\n", root->name, root->code, root->price);
    inOrderTraversal(root->right);
}

// HEAP FUNCTIONS
void initHeap(MinHeap *heap)
{
    heap->size = 0;
}

void swap(HeapItem *a, HeapItem *b)
{
    HeapItem temp = *a;
    *a = *b;
    *b = temp;
}

void insertHeap(MinHeap *heap, char *name, char *code, float price)
{
    if (heap->size >= MAX_HEAP_SIZE)
    {
        printf("Heap penuh!\n");
        return;
    }

    int i = heap->size++;
    strcpy(heap->items[i].name, name);
    strcpy(heap->items[i].code, code);
    heap->items[i].price = price;

    // Percolate up
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (heap->items[i].price < heap->items[parent].price)
        {
            swap(&heap->items[i], &heap->items[parent]);
            i = parent;
        }
        else
        {
            break;
        }
    }
}

void heapifyDown(MinHeap *heap, int index)
{
    int smallest = index;
    int left = index * 2 + 1;
    int right = index * 2 + 2;

    if (left < heap->size && heap->items[left].price < heap->items[smallest].price)
        smallest = left;
    if (right < heap->size && heap->items[right].price < heap->items[smallest].price)
        smallest = right;

    if (smallest != index)
    {
        swap(&heap->items[index], &heap->items[smallest]);
        heapifyDown(heap, smallest);
    }
}

void deleteMin(MinHeap *heap)
{
    if (heap->size == 0)
    {
        printf("Heap kosong!\n");
        return;
    }

    printf("Menghapus produk termurah: %s - Rp %.0f\n", heap->items[0].name, heap->items[0].price);

    heap->items[0] = heap->items[--heap->size];
    heapifyDown(heap, 0);
}

void deleteMinSilent(MinHeap *heap)
{
    if (heap->size == 0)
        return;

    heap->items[0] = heap->items[--heap->size];
    heapifyDown(heap, 0);
}

void bstToHeap(BSTNode *root, MinHeap *heap)
{
    if (!root)
        return;
    insertHeap(heap, root->name, root->code, root->price);
    bstToHeap(root->left, heap);
    bstToHeap(root->right, heap);
}

void printHeapAscending(MinHeap *originalHeap)
{
    // Copy heap biar data aslinya nggak berubah
    MinHeap tempHeap;
    tempHeap.size = originalHeap->size;
    for (int i = 0; i < originalHeap->size; i++)
    {
        tempHeap.items[i] = originalHeap->items[i];
    }

    // Header tabel
    printf("\n%-4s| %-35s | %-10s | %-13s\n", "No", "Nama Produk", "Kode", "Harga");
    printf("-------------------------------------------------------------------------------\n");

    int no = 1;
    while (tempHeap.size > 0)
    {
        printf("%-4d| %-35s | %-10s | Rp %-10.0f\n", no++,
               tempHeap.items[0].name, tempHeap.items[0].code, tempHeap.items[0].price);
        deleteMinSilent(&tempHeap);
    }
}

// Sorting
void bstToArray(BSTNode *root, Product arr[], int *idx)
{
    if (!root)
        return;
    bstToArray(root->left, arr, idx);
    strcpy(arr[*idx].name, root->name);
    strcpy(arr[*idx].code, root->code);
    arr[*idx].price = root->price;
    (*idx)++;
    bstToArray(root->right, arr, idx);
}

void merge(Product arr[], int left, int mid, int right)
{
    int n1 = mid - left + 1, n2 = right - mid;
    Product L[n1], R[n2];
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (strcmp(L[i].name, R[j].name) <= 0)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSortByName(Product arr[], int left, int right)
{
    if (left < right)
    {
        int mid = (left + right) / 2;
        mergeSortByName(arr, left, mid);
        mergeSortByName(arr, mid + 1, right);
        merge(arr, left, mid, right);

        // Visualisasi langkah
        printf("\n[Merge Sort Step %d-%d]\n", left, right);
        printf("%-4s | %-35s | %-10s | %-10s\n", "No", "Name", "Code", "Price");
        printf("---------------------------------------------------------------\n");
        for (int i = 0; i <= right; i++)
        {
            printf("%-4d | %-25s | %-10s | Rp %-8.0f\n",
                   i + 1, arr[i].name, arr[i].code, arr[i].price);
        }
    }
}

int partition(Product arr[], int low, int high)
{
    float pivot = arr[high].price;
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (arr[j].price <= pivot)
        {
            i++;
            Product temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Product temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

void quickSortByPrice(Product arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);

        // Visualisasi langkah
        printf("\n[Quick Sort Step %d-%d] Pivot: Rp %.0f\n", low, high, arr[pi].price);
        printf("%-4s | %-35s | %-10s | %-10s\n", "No", "Name", "Code", "Price");
        printf("---------------------------------------------------------------\n");
        for (int i = 0; i <= high; i++)
        {
            printf("%-4d | %-25s | %-10s | Rp %-8.0f\n",
                   i + 1, arr[i].name, arr[i].code, arr[i].price);
        }

        quickSortByPrice(arr, low, pi - 1);
        quickSortByPrice(arr, pi + 1, high);
    }
}

BSTNode *loadSingleCategory(const char *filename)
{
    BSTNode *root = NULL;
    char name[100], code[50];
    float price;

    FILE *f = fopen(filename, "r");
    if (!f)
    {
        printf("Gagal buka file %s\n", filename);
        return NULL;
    }

    while (fscanf(f, " %[^#]#%[^#]#%f\n", name, code, &price) == 3)
    {
        root = insertBST(root, name, code, price);
    }

    fclose(f);
    return root;
}

// // ====== BINARY SEARCH ======
void loadAllToArray(Product arr[], int *count)
{
    const char *filePaths[] = {
        "Data/Drinks.txt", "Data/Snacks.txt", "Data/Foods.txt",
        "Data/FrozenFood.txt", "Data/FruitsVeggies.txt",
        "Data/CleaningSupplies.txt", "Data/Groceries.txt"};
    char name[100], code[50];
    float price;

    *count = 0;
    for (int i = 0; i < 7; i++)
    {
        FILE *f = fopen(filePaths[i], "r");
        if (!f)
            continue;
        while (fscanf(f, " %[^#]#%[^#]#%f\n", name, code, &price) == 3)
        {
            strcpy(arr[*count].name, name);
            strcpy(arr[*count].code, code);
            arr[*count].price = price;
            (*count)++;
        }
        fclose(f);
    }
}

int substringSearchByNameAll(Product arr[], int n, const char *keyword, Product found[], int *foundCount)
{
    int step = 1;
    *foundCount = 0;

    printf("\n[SUBSTRING SEARCH NAMA: \"%s\"]\n", keyword);
    for (int i = 0; i < n; i++)
    {
        printf("Step %d: Cek \"%s\"\n", step++, arr[i].name);
        if (strstr(arr[i].name, keyword) != NULL)
        {
            found[*foundCount] = arr[i];
            (*foundCount)++;
        }
    }

    return *foundCount;
}

int substringSearchByCodeAll(Product arr[], int n, const char *keyword, Product found[], int *foundCount)
{
    int step = 1;
    *foundCount = 0;

    printf("\n[SUBSTRING SEARCH KODE: \"%s\"]\n", keyword);
    for (int i = 0; i < n; i++)
    {
        printf("Step %d: Cek \"%s\"\n", step++, arr[i].code);
        if (strstr(arr[i].code, keyword) != NULL)
        {
            found[*foundCount] = arr[i];
            (*foundCount)++;
        }
    }

    return *foundCount;
}

void freeBST(BSTNode *root)
{
    if (!root)
        return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

// MENU
void menuBST(BSTNode **root)
{
    int bstChoice;
    while (1)
    {
        printf("\n--- Soal 1: BST ---\n");
        printf("1. Lihat semua produk (in-order)\n");
        printf("2. Cari produk\n");
        printf("3. Hapus produk\n");
        printf("4. Tambah produk baru\n");
        printf("5. Kembali ke menu utama\n");
        printf("Pilih menu: ");
        scanf("%d", &bstChoice);
        getchar();

        switch (bstChoice)
        {
        case 1:
            inOrderTraversal(*root);
            break;

        case 2:
        {
            char name[100];
            printf("Masukkan nama produk: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            BSTNode *found = searchBST(*root, name);
            if (found)
                printf("✅ Ketemu: %s | %s | Rp %.0f\n", found->name, found->code, found->price);
            else
                printf("❌ Produk tidak ditemukan.\n");
            break;
        }

        case 3:
        {
            char name[100];
            printf("Masukkan nama produk: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            *root = deleteBST(*root, name);
            printf("Jika produk ada, sekarang sudah dihapus.\n");
            break;
        }

        case 4:
        {
            char newName[100], newCode[50];
            float newPrice;
            printf("Nama produk: ");
            fgets(newName, sizeof(newName), stdin);
            newName[strcspn(newName, "\n")] = 0;
            printf("Kode produk: ");
            fgets(newCode, sizeof(newCode), stdin);
            newCode[strcspn(newCode, "\n")] = 0;
            printf("Harga: ");
            scanf("%f", &newPrice);
            getchar();
            *root = insertBST(*root, newName, newCode, newPrice);
            printf("Produk ditambahkan!\n");
            break;
        }

        case 5:
            return;

        default:
            printf("Pilihan tidak valid!\n");
        }
    }
}

void menuHeap(BSTNode *root, MinHeap *heap, int *heapInitialized)
{
    int heapChoice;
    while (1)
    {
        printf("\n--- Soal 2: Binary Heap ---\n");
        printf("1. Konversi BST ke Min Heap\n");
        printf("2. Tampilkan isi Heap\n");
        printf("3. Hapus produk termurah dari Heap\n");
        printf("4. Kembali ke menu utama\n");
        printf("Pilih menu: ");
        scanf("%d", &heapChoice);
        getchar();

        switch (heapChoice)
        {
        case 1:
            initHeap(heap);
            bstToHeap(root, heap);
            *heapInitialized = 1;
            printf("Konversi BST ke Heap berhasil.\n");
            break;

        case 2:
            if (!(*heapInitialized))
            {
                printf("Heap belum dikonversi.\n");
                break;
            }
            printHeapAscending(heap);
            break;

        case 3:
            if (!(*heapInitialized))
            {
                printf("Heap belum dikonversi.\n");
                break;
            }
            deleteMin(heap);
            break;

        case 4:
            return;

        default:
            printf("Pilihan tidak valid!\n");
        }
    }
}

void menuSorting()
{
    const char *categories[] = {
        "Drinks", "Snacks", "Foods", "Frozen Food",
        "Fruits & Veggies", "Cleaning Supplies", "Groceries"};
    const char *filePaths[] = {
        "Data/Drinks.txt", "Data/Snacks.txt", "Data/Foods.txt",
        "Data/FrozenFood.txt", "Data/FruitsVeggies.txt",
        "Data/CleaningSupplies.txt", "Data/Groceries.txt"};

    while (1)
    {
        printf("\n--- PILIH KATEGORI DATA UNTUK SORTING ---\n");
        printf("0. Semua Data\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%d. %s\n", i + 1, categories[i]);
        }
        printf("8. Kembali ke menu utama\n");
        printf("Pilihan: ");

        int catChoice;
        scanf("%d", &catChoice);
        getchar();

        if (catChoice == 8)
            break;
        if (catChoice < 0 || catChoice > 7)
        {
            printf("Pilihan tidak valid!\n");
            continue;
        }

        // --- Load Data ke BST
        BSTNode *root = NULL;
        char categoryLabel[50];

        if (catChoice == 0)
        {
            root = loadAllProducts();
            strcpy(categoryLabel, "Semua Data");
        }
        else
        {
            root = loadSingleCategory(filePaths[catChoice - 1]);
            strcpy(categoryLabel, categories[catChoice - 1]);
        }

        if (!root)
        {
            printf("Gagal load data.\n");
            continue;
        }

        // --- Convert ke array
        Product arr[300];
        int count = 0;
        bstToArray(root, arr, &count);

        // --- Submenu Sorting
        while (1)
        {
            printf("\n--- SORTING %s ---\n", categoryLabel);
            printf("1. Merge Sort by Name\n");
            printf("2. Quick Sort by Price\n");
            printf("3. Kembali ke pilihan kategori\n");
            printf("Pilihan: ");

            int sortChoice;
            scanf("%d", &sortChoice);
            getchar();

            switch (sortChoice)
            {
            case 1:
                printf("\nSorting berdasarkan Nama Produk (A-Z)...\n");
                mergeSortByName(arr, 0, count - 1);
                break;

            case 2:
                printf("\nSorting berdasarkan Harga Produk (Termurah ke Termahal)...\n");
                quickSortByPrice(arr, 0, count - 1);
                break;

            case 3:
                goto kategori_ulang;

            default:
                printf("Pilihan tidak valid!\n");
            }
        }

    kategori_ulang:
        continue;
    }
}

void menuBinarySearch()
{
    const char *categories[] = {
        "Drinks", "Snacks", "Foods", "Frozen Food", "Fruits & Veggies", "Cleaning Supplies", "Groceries"};
    const char *filePaths[] = {
        "Data/Drinks.txt", "Data/Snacks.txt", "Data/Foods.txt",
        "Data/FrozenFood.txt", "Data/FruitsVeggies.txt",
        "Data/CleaningSupplies.txt", "Data/Groceries.txt"};

    Product arr[100];
    int count = 0;
    int choice;

    while (1)
    {
        printf("\n--- Binary Search Menu ---\n");
        printf("1. Cari di kategori tertentu\n");
        printf("2. Cari di semua data\n");
        printf("3. Kembali ke menu utama\n");
        printf("Pilihan: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 3)
            break;

        switch (choice)
        {
        case 1:
        {
            int cat;
            for (int i = 0; i < 7; i++)
            {
                printf("%d. %s\n", i + 1, categories[i]);
            }
            printf("Pilih kategori: ");
            scanf("%d", &cat);
            getchar();

            if (cat < 1 || cat > 7)
            {
                printf("Pilihan tidak valid!\n");
                continue;
            }

            BSTNode *root = loadSingleCategory(filePaths[cat - 1]);
            count = 0;
            bstToArray(root, arr, &count);
            break;
        }

        case 2:
            loadAllToArray(arr, &count);
            break;
        case 3:
            printf("Kembali ke menu utama...\n");
            continue;

        default:
            printf("Pilihan tidak valid!\n");
            continue;
        }

        // Sorting optional, biar data tertata
        int sortChoice;
        printf("\nSort data terlebih dahulu:\n1. By Name (merge sort)\n2. Skip sorting\nPilihan: ");
        scanf("%d", &sortChoice);
        getchar();
        if (sortChoice == 1)
            mergeSortByName(arr, 0, count - 1);

        int searchType;
        char input[100];
        printf("\nCari berdasarkan:\n1. Nama (substring)\n2. Kode (substring)\nPilihan: ");
        scanf("%d", &searchType);
        getchar();

        printf("Masukkan keyword pencarian: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        Product found[100];
        int foundCount = 0;

        if (searchType == 1)
        {
            substringSearchByNameAll(arr, count, input, found, &foundCount);
        }
        else if (searchType == 2)
        {
            substringSearchByCodeAll(arr, count, input, found, &foundCount);
        }

        if (foundCount == 0)
        {
            printf("❌ Tidak ditemukan hasil yang cocok.\n");
        }
        else
        {
            printf("\n🎯 Total ditemukan: %d produk\n", foundCount);
            printf("%-4s | %-25s | %-10s | %-10s\n", "No", "Name", "Code", "Price");
            printf("---------------------------------------------------------------\n");
            for (int i = 0; i < foundCount; i++)
            {
                printf("%-4d | %-25s | %-10s | Rp %-8.0f\n", i + 1,
                       found[i].name, found[i].code, found[i].price);
            }
        }
    }
}

// ====== MAIN ======
int main()
{
    BSTNode *root = loadAllProducts();
    MinHeap heap;
    int heapInitialized = 0;
    int mainChoice;

    while (1)
    {
        printf("\n=== AEON MENU ===\n");
        printf("1. Binary Search Tree (Soal 1)\n");
        printf("2. Binary Heap (Soal 2)\n");
        printf("3. Sorting (Soal 3)\n");
        printf("4. Binary Search (Soal 4)\n");
        printf("5. Keluar\n");
        printf("Pilih menu utama: ");
        scanf("%d", &mainChoice);
        getchar();

        switch (mainChoice)
        {
        case 1:
            menuBST(&root);
            break;
        case 2:
            menuHeap(root, &heap, &heapInitialized);
            break;
        case 3:
            menuSorting(root);
            break;
        case 4:
            menuBinarySearch();
            break;
        case 5:
            printf("Keluar dari program...\n");
            freeBST(root);
            return 0;
        default:
            printf("Pilihan tidak valid!\n");
        }
    }

    return 0;
}
