#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAX_HEAP_SIZE 100

typedef struct {
    char name[100];
    char code[50];
    float price;
} HeapItem;

typedef struct {
    HeapItem items[MAX_HEAP_SIZE];
    int size;
} MinHeap;

void initHeap(MinHeap *heap) {
    heap->size = 0;
}


void swap(HeapItem *a, HeapItem *b) {
    HeapItem temp = *a;
    *a = *b;
    *b = temp;
}

void insertHeap(MinHeap *heap, char *name, char *code, float price) {
    if (heap->size >= MAX_HEAP_SIZE) {
        printf("Heap penuh!\n");
        return;
    }

    int i = heap->size++;
    strcpy(heap->items[i].name, name);
    strcpy(heap->items[i].code, code);
    heap->items[i].price = price;

    // Percolate up
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->items[i].price < heap->items[parent].price) {
            swap(&heap->items[i], &heap->items[parent]);
            i = parent;
        } else {
            break;
        }
    }
}

void heapifyDown(MinHeap *heap, int index) {
    int smallest = index;
    int left = index * 2 + 1;
    int right = index * 2 + 2;

    if (left < heap->size && heap->items[left].price < heap->items[smallest].price)
        smallest = left;
    if (right < heap->size && heap->items[right].price < heap->items[smallest].price)
        smallest = right;

    if (smallest != index) {
        swap(&heap->items[index], &heap->items[smallest]);
        heapifyDown(heap, smallest);
    }
}

void deleteMin(MinHeap *heap) {
    if (heap->size == 0) {
        printf("Heap kosong!\n");
        return;
    }

    printf("Menghapus produk termurah: %s - Rp %.0f\n", heap->items[0].name, heap->items[0].price);

    heap->items[0] = heap->items[--heap->size];
    heapifyDown(heap, 0);
}

void bstToHeap(BSTNode *root, MinHeap *heap) {
    if (!root) return;
    insertHeap(heap, root->name, root->code, root->price);
    bstToHeap(root->left, heap);
    bstToHeap(root->right, heap);
}


int main() {
    BSTNode *root = loadAllProducts(); // pakai yang udah lo buat
    MinHeap heap;
    initHeap(&heap);

    bstToHeap(root, &heap);
    printf("Heap berhasil dibuat dari BST.\n");

    int choice;
    while (1) {
        printf("\n=== MIN HEAP MENU ===\n");
        printf("1. Lihat semua produk (heap order)\n");
        printf("2. Hapus produk termurah\n");
        printf("3. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Isi Heap (harga termurah dulu):\n");
                for (int i = 0; i < heap.size; i++)
                    printf("%d. %s | %s | Rp %.0f\n", i + 1,
                           heap.items[i].name, heap.items[i].code, heap.items[i].price);
                break;
            case 2:
                deleteMin(&heap);
                break;
            case 3:
                return 0;
            default:
                printf("Pilihan tidak valid!\n");
        }
    }

    return 0;
}
