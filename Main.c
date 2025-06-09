#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Added for boolean support
#include <ctype.h>

typedef struct User
{
    char username[50];
    char password[50];
    int point;
} User;

typedef struct Cart
{
    char name[50];
    char code[20];
    float price;
    int quantity;
    struct Cart *prev, *next;
} Cart;

// Function declarations
// Aditya Zianur Rahman S - 00000128409
void displayProducts(const char *categoryFile);
void viewProducts();
void menu(char *username, int point);
bool login();
bool regist();
int searchProductByName(char *productName, char *codeOut, float *priceOut);

// Rafael Lesmana - 00000134095
void updateUserPoints(char *username, int points);
void checkout(char *username, int *point, Cart **head, Cart **tail);

// Dhimas Alkautsar Putra Alif Viano - 00000134095
void emptyCart(Cart **head, Cart **tail);
void deleteItemInCart(Cart *head, Cart *tail, int pos);
void viewCart(Cart *head);

// Michaell Garets Kon - 00000088779
void addToCart(Cart **cartHead);

// ini untuk perhitungan rank
const char *getRank(int point)
{
    if (point >= 1 && point <= 1000)
        return "Bronze"; // diskon 2%
    else if (point >= 1001 && point <= 5000)
        return "Silver"; // diskon 3%
    else if (point >= 5001 && point <= 10000)
        return "Gold"; // diskon 4%
    else if (point > 10000)
        return "Platinum"; // diskon 5%
    else
        return "Unknown Rank";
}

// ini untuk perhitungan seberapa jauh lagi ke rank selanjutnya
int getPointsToNextRank(int point)
{
    if (point >= 1 && point <= 1000)
    {
        return 1001 - point;
    }
    else if (point >= 1001 && point <= 5000)
    {
        return 5001 - point;
    }
    else if (point >= 5001 && point <= 10000)
    {
        return 10001 - point;
    }
    else
    {
        return -1; // udah max rank atau invalid
    }
}

// fungsi ini untuk mencari produk berdasarkan nama yang di input user
int searchProductByName(char *productName, char *codeOut, float *priceOut)
{
    const char *filePaths[] = {
        "Data/Drinks.txt", "Data/Snacks.txt", "Data/Foods.txt",
        "Data/FrozenFood.txt", "Data/FruitsVeggies.txt",
        "Data/CleaningSupplies.txt", "Data/Groceries.txt"};
    int numFiles = sizeof(filePaths) / sizeof(filePaths[0]);

    for (int i = 0; i < numFiles; i++)
    {
        FILE *fp = fopen(filePaths[i], "r");
        if (fp == NULL)
        {
            printf("Gagal buka file: %s\n", filePaths[i]);
            continue;
        }

        char line[200];
        while (fgets(line, sizeof(line), fp))
        {
            char name[100], code[30];
            float price;

            if (sscanf(line, " %[^\n#]#%[^#]#%f", name, code, &price) == 3)
            {
                // biar case-insensitive, ubah ke huruf kecil semua dulu
                char nameLower[100], inputLower[100];
                strcpy(nameLower, name);
                strcpy(inputLower, productName);

                for (int j = 0; nameLower[j]; j++)
                    nameLower[j] = tolower(nameLower[j]);
                for (int j = 0; inputLower[j]; j++)
                    inputLower[j] = tolower(inputLower[j]);

                if (strstr(nameLower, inputLower) != NULL)
                {
                    strcpy(codeOut, code);
                    *priceOut = price;
                    fclose(fp);
                    return 1; // ketemu
                }
            }
        }

        fclose(fp);
    }

    return 0;
}

// fungsi ini untuk checkout, menghitung total harga, dan mengurangi point user
// serta mengupdate file user.txt untuk point user
void checkout(char *username, int *point, Cart **head, Cart **tail)
{
    if (*head == NULL)
    {
        printf("Your cart is empty!\n");
        return;
    }

    float totalPrice = 0.0;
    int earnedPoints = 0;
    Cart *current = *head;

    // Display receipt header
    printf("\n======================================\n");
    printf("              CHECKOUT               \n");
    printf("======================================\n");
    printf("Username: %s\n", username);
    printf("Current Points: %d\n", *point);
    printf("--------------------------------------\n");
    printf("%-20s %-10s %-10s %-10s\n", "Product", "Price", "Qty", "Subtotal");
    printf("--------------------------------------\n");

    // Calculate total price and display items
    while (current != NULL)
    {
        float subtotal = current->price * current->quantity;
        totalPrice += subtotal;
        printf("%-20s Rp %-8.0f %-10d Rp %-8.0f\n",
               current->name, current->price, current->quantity, subtotal);
        current = current->next;
    }

    // Calculate earned points (1 point for every 10,000 Rupiah)
    earnedPoints = (int)(totalPrice / 10000);

    // discount based on rank points
    if (*point < 1000)
    {
        totalPrice *= 0.98; // 2% discount
        printf("You are a Bronze member. You get a 2%% discount.\n");
    }
    else if (*point < 5001) // ini nangkep 1000 - 5000
    {
        totalPrice *= 0.97; // 3% discount
        printf("You are a Silver member. You get a 3%% discount.\n");
    }
    else if (*point < 10000) // ini nangkep 5001 - 9999
    {
        totalPrice *= 0.96; // 4% discount
        printf("You are a Gold member. You get a 4%% discount.\n");
    }
    else // 10000 ke atas
    {
        totalPrice *= 0.95; // 5% discount
        printf("You are a Platinum member. You get a 5%% discount.\n");
    }

    // Ask if user wants to use points
    int usePoints = 0;
    if (*point > 0)
    {
        int maxPointsUsable = *point < (int)(totalPrice / 1000) ? *point : (int)(totalPrice / 1000);
        printf("\nYou have %d points. Each point is worth Rp 1,000.\n", *point);
        printf("You can use up to %d points for this transaction.\n", maxPointsUsable);
        printf("How many points do you want to use? (0 for none): ");
        scanf("%d", &usePoints);

        if (usePoints < 0 || usePoints > maxPointsUsable)
        {
            printf("Invalid points amount. Using 0 points instead.\n");
            usePoints = 0;
        }
    }

    // Calculate discount from points
    float pointDiscount = usePoints * 1000.0;
    float finalPrice = totalPrice - pointDiscount;
    if (finalPrice < 0)
    {
        finalPrice = 0; // Prevent negative final price
    }

    // Display summary
    printf("--------------------------------------\n");
    printf("%-30s Rp %-8.0f\n", "Total Price:", totalPrice);
    if (usePoints > 0)
    {
        printf("%-30s Rp %-8.0f\n", "Points Discount:", pointDiscount);
        printf("%-30s Rp %-8.0f\n", "Final Price:", finalPrice);
    }
    printf("%-30s %d\n", "Points Earned:", earnedPoints);
    printf("%-30s %d\n", "Remaining Points:", *point - usePoints + earnedPoints);
    printf("======================================\n");

    // Payment process
    float payment;
    while (1)
    {
        printf("Enter payment amount (Rp): ");
        scanf("%f", &payment);

        if (payment >= finalPrice)
        {
            printf("Payment successful!\n");
            printf("Change: Rp %.0f\n", payment - finalPrice);
            break;
        }
        else
        {
            printf("Insufficient payment amount. Please enter at least Rp %.0f\n", finalPrice);
        }
    }

    // Update user points in the file
    *point = *point - usePoints + earnedPoints;
    updateUserPoints(username, *point);

    // Clear cart
    emptyCart(head, tail);
    printf("\nThank you for shopping at AEON Store!\n");
}

// fungsi ini untuk update point user di file user.txt
void updateUserPoints(char *username, int points)
{
    FILE *users = fopen("Data/User.txt", "r");
    FILE *temp = fopen("Data/Temp.txt", "w");

    if (users == NULL || temp == NULL)
    {
        printf("Error opening files for point update.\n");
        return;
    }

    User user;
    while (fscanf(users, " %[^#]#%[^#]#%d\n", user.username, user.password, &user.point) != EOF)
    {
        if (strcmp(username, user.username) == 0)
        {
            fprintf(temp, "%s#%s#%d\n", user.username, user.password, points);
        }
        else
        {
            fprintf(temp, "%s#%s#%d\n", user.username, user.password, user.point);
        }
    }

    fclose(users);
    fclose(temp);

    remove("Data/User.txt");
    rename("Data/Temp.txt", "Data/User.txt");
}

// tambah item ke cart
void addToCart(Cart **cartHead)
{
    char productName[50], productCode[20];
    float productPrice;
    int quantity;

    printf("\nEnter product name: ");
    scanf(" %[^\n]", productName);

    if (!searchProductByName(productName, productCode, &productPrice))
    {
        printf("Product not found!\n");
        return;
    }

    printf("Enter quantity: ");
    scanf("%d", &quantity);

    if (productPrice <= 0 || quantity <= 0)
    {
        printf("Invalid price or quantity. Please try again.\n");
        return;
    }

    // Check if the product is already in the cart
    Cart *temp = *cartHead;
    while (temp != NULL)
    {
        // The segfault occurs here if temp contains invalid memory
        if (strcmp(temp->code, productCode) == 0)
        {
            temp->quantity += quantity;
            printf("Product quantity updated in cart.\n");
            return;
        }
        temp = temp->next;
    }

    // Add new item to cart
    Cart *newItem = (Cart *)malloc(sizeof(Cart));
    if (newItem == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    strcpy(newItem->name, productName);
    strcpy(newItem->code, productCode);
    newItem->price = productPrice;
    newItem->quantity = quantity;
    newItem->next = NULL;
    newItem->prev = NULL;

    if (*cartHead == NULL)
    {
        *cartHead = newItem;
    }
    else
    {
        Cart *temp = *cartHead;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newItem;
        newItem->prev = temp;
    }

    printf("Product added to cart successfully!\n");
}

// liat isi cart
void viewCart(Cart *head)
{
    if (head == NULL)
    {
        printf("\nCart is empty!\n");
        return;
    }

    Cart *temp = head;
    int index = 1;
    float total = 0;

    printf("\nItems in Cart:\n");
    printf("╔════╦══════════════════════╦════════════╦═══════╦════════════╦════════════╗\n");
    printf("║ No ║ Product Name         ║ Code       ║ Qty   ║ Price/pcs  ║ Total      ║\n");
    printf("╠════╬══════════════════════╬════════════╬═══════╬════════════╬════════════╣\n");

    while (temp != NULL)
    {
        float subtotal = temp->price * temp->quantity;
        printf("║ %-2d ║ %-20s ║ %-10s ║ %-5d ║ Rp %-7.0f ║ Rp %-7.0f ║\n",
               index++, temp->name, temp->code, temp->quantity, temp->price, subtotal);
        total += subtotal;
        temp = temp->next;
    }

    printf("╚════╩══════════════════════╩════════════╩═══════╩════════════╩════════════╝\n");
    printf("Total: Rp %.0f\n", total);
}

// hapus item dari cart by index
void deleteItemInCart(Cart *head, Cart *tail, int pos)
{
    if (head == NULL)
    {
        printf("Cart is already empty.\n");
        return;
    }

    Cart *temp = head;
    int index = 1;

    while (temp != NULL && index < pos)
    {
        temp = temp->next;
        index++;
    }

    if (temp == NULL)
    {
        printf("Invalid item number.\n");
        return;
    }

    if (temp == head)
        head = head->next;
    if (temp == tail)
        tail = temp->prev;

    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    free(temp);
    printf("Item removed from cart.\n");
}

// kosongkan cart
void emptyCart(Cart **head, Cart **tail)

{
    Cart *temp = *head;
    while (temp != NULL)
    {
        Cart *toDelete = temp;
        temp = temp->next;
        free(toDelete);
    }
    *head = *tail = NULL;
    printf("Cart has been emptied.\n");
}

// Fungsi ini digunakan untuk menampilkan produk berdasarkan kategori
// dan format outputnya
void displayProducts(const char *categoryFile)
{
    FILE *file = fopen(categoryFile, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", categoryFile);
        return;
    }

    char name[100], code[100];
    float price;

    printf("\nAvailable Products:\n");
    printf("╔═══════════════════════╦═════════════╦══════════════╗\n");
    printf("║ %-21s ║ %-11s ║ %-12s ║\n", "Product Name", "Code", "Price");
    printf("╠═══════════════════════╬═════════════╬══════════════╣\n");

    while (fscanf(file, " %[^#]#%[^#]#%f\n", name, code, &price) != EOF)
    {
        printf("║ %-21s ║ %-11s ║ Rp %-9.0f ║\n", name, code, price);
    }

    printf("╚═══════════════════════╩═════════════╩══════════════╝\n");
    fclose(file);
}

// Fungsi ini untuk menampilkan kategori produk yang ada
// dan meminta user untuk memilih kategori
void viewProducts()
{
    int subChoice;
    const char *categories[] = {
        "Data/Drinks.txt", "Data/Snacks.txt", "Data/Foods.txt",
        "Data/FrozenFood.txt", "Data/FruitsVeggies.txt",
        "Data/CleaningSupplies.txt", "Data/Groceries.txt"};

    printf("\nProduct Categories:\n");
    printf("1. Drinks\n2. Snacks\n3. Foods\n4. Frozen Food\n");
    printf("5. Fruits & Vegetables\n6. Cleaning Supplies\n7. Groceries\n");
    printf("8. Back to Menu\n");

    printf("Select a category: ");
    scanf("%d", &subChoice);

    if (subChoice >= 1 && subChoice <= 7)
    {
        displayProducts(categories[subChoice - 1]);
    }
    else if (subChoice == 8)
    {
        printf("Returning to main menu...\n");
    }
    else
    {
        printf("Invalid selection!\n");
    }
}

// fungsi ini untuk menampilkan menu utama setelah login
void menu(char *username, int point)
{
    Cart *head = NULL, *tail = NULL; // taro diatas menu()

    int choice;
    while (1)
    {
        printf("\n===============================\n");
        printf("         AEON STORE MENU       \n");
        printf("==================================\n");
        printf("Welcome %s!\n", username);
        const char *rank = getRank(point);
        int pointsToNextRank = getPointsToNextRank(point);

        printf("Points: %d - %s Rank\n", point, rank);
        if (pointsToNextRank != -1)
        {
            printf("Points needed to next rank: %d\n", pointsToNextRank);
        }
        else
        {
            printf("You are at the highest rank!\n");
        }
        printf("==================================\n");
        printf("1. View Products\n2. Add to Cart\n3. View Cart\n");
        printf("4. Checkout\n5. Logout\n===========================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            viewProducts();
            break;
        case 2:
            addToCart(&head); // Call the addToCart function
            break;
        case 3:
            while (1)
            {
                viewCart(head);
                printf("\n1. Delete item\n2. Empty cart\n0. Back\nChoice: ");
                int sub;
                scanf("%d", &sub);
                if (sub == 1)
                {
                    int delIndex;
                    printf("Enter item number to delete: ");
                    scanf("%d", &delIndex);
                    deleteItemInCart(head, tail, delIndex);
                }
                else if (sub == 2)
                {
                    emptyCart(&head, &tail);
                    break;
                }
                else if (sub == 0)
                {
                    break;
                }
                else
                {
                    printf("Invalid option.\n");
                }
            }
            break;
        case 4:
            checkout(username, &point, &head, &tail);
            break;
        case 5:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }
    }
}

// fungsi ini untuk login dan mengecek username dan password di file user.txt
// jika berhasil login, maka akan memanggil menu() dan mengirimkan username dan point user
bool login()
{
    User user;
    char inputUser[50], inputPass[50];
    bool loginSuccess = false;
    FILE *users = fopen("Data/User.txt", "r");
    if (users == NULL)
    {
        printf("Error opening file.\n");
        return false;
    }

    printf("===========================\n");
    printf("           Login           \n");
    printf("===========================\n");

    printf("Enter username: ");
    scanf("%s", inputUser);
    printf("Enter password: ");
    scanf("%s", inputPass);

    while (fscanf(users, " %[^#]#%[^#]#%d", user.username, user.password, &user.point) != EOF)
    {
        if (strcmp(inputUser, user.username) == 0 && strcmp(inputPass, user.password) == 0)
        {
            printf("Login successful!\n");
            loginSuccess = true;
            break;
        }
    }

    fclose(users);

    if (loginSuccess)
    {
        menu(user.username, user.point);
        return true;
    }
    else
    {
        printf("Invalid username or password.\n");
        return false;
    }
}

// fungsi ini untuk registrasi user baru dan menyimpan data ke file user.txt
// jika berhasil registrasi, maka akan memanggil login() dan mengirimkan username dan point user
// jika gagal registrasi, maka akan mengulang lagi
bool regist()
{
    User user;
    char existingUsername[50], existingPassword[50];
    int existingPoint;
    bool usernameTaken = false;

    FILE *users = fopen("Data/User.txt", "r"); // Buka untuk dibaca dulu
    if (users == NULL)
    {
        printf("Error opening file.\n");
        return false;
    }

    printf("===========================\n");
    printf("Register \n");
    printf("===========================\n");
    printf("Enter username: ");
    scanf("%s", user.username);

    // Cek apakah username sudah dipakai
    while (fscanf(users, "%[^#]#%[^#]#%d\n", existingUsername, existingPassword, &existingPoint) == 3)
    {
        if (strcmp(existingUsername, user.username) == 0)
        {
            usernameTaken = true;
            break;
        }
    }
    fclose(users); // Tutup dulu file bacaan

    if (usernameTaken)
    {
        printf("Username already exists. Please try a different one.\n");
        return false;
    }

    printf("Enter password: ");
    scanf("%s", user.password);
    user.point = 0;

    // Lanjut nulis user baru
    users = fopen("Data/User.txt", "a");
    if (users == NULL)
    {
        printf("Error opening file for writing.\n");
        return false;
    }

    fprintf(users, "%s#%s#%d\n", user.username, user.password, user.point);
    fclose(users);
    printf("Registration successful!\n");
    printf("Please login with your new account.\n");
    return true;
}

// fungsi utama
// ini untuk menampilkan menu awal dan meminta user untuk login atau registrasi
int main()
{
    while (1)
    {
        printf("=====================================");
        printf("\n      Welcome to AEON Store        \n");
        printf("=====================================\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Exit\n");
        printf("=====================================\n");
        printf("Please select an option: ");
        int option;
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            printf("Login selected.\n");
            login();
            break;
        case 2:
            printf("Register selected.\n");
            if (regist())
            {
                // If registration is successful, proceed to login
                printf("Redirecting to login...\n");
                login();
            }
            break;
        case 3:
            printf("Exiting...\n");
            return 0;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }
    }
}