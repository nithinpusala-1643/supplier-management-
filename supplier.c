#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SUPPLIERS 1000
#define NAME_LEN 100
#define PHONE_LEN 20
#define EMAIL_LEN 100
#define ADDR_LEN 200
#define ITEM_LEN 200
#define DATAFILE "suppliers.dat"

typedef struct {
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
    char address[ADDR_LEN];
    char items[ITEM_LEN]; // items supplied / description
    int active; // 1 = exists, 0 = deleted
} Supplier;

Supplier suppliers[MAX_SUPPLIERS];
int supplier_count = 0;

/* Utility: trim newline from strings read by fgets */
void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len == 0) return;
    if (s[len - 1] == '\n') s[len - 1] = '\0';
}

/* Utility: flush remaining input (for single-char or numeric inputs) */
void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* Load suppliers from binary file into suppliers[] */
void load_data() {
    FILE *fp = fopen(DATAFILE, "rb");
    if (!fp) {
        // No file yet -- this is okay
        supplier_count = 0;
        return;
    }
    // Try reading entire array; file contains only the actual count followed by records
    fread(&supplier_count, sizeof(int), 1, fp);
    if (supplier_count < 0 || supplier_count > MAX_SUPPLIERS) {
        // corrupt or unexpected, reset
        supplier_count = 0;
        fclose(fp);
        return;
    }
    fread(suppliers, sizeof(Supplier), supplier_count, fp);
    fclose(fp);
}

/* Save suppliers[] to binary file */
void save_data() {
    FILE *fp = fopen(DATAFILE, "wb");
    if (!fp) {
        printf("Error: Unable to write to data file '%s'\n", DATAFILE);
        return;
    }
    fwrite(&supplier_count, sizeof(int), 1, fp);
    fwrite(suppliers, sizeof(Supplier), supplier_count, fp);
    fclose(fp);
}

/* Find supplier index by ID; return index or -1 if not found */
int find_supplier_by_id(int id) {
    for (int i = 0; i < supplier_count; ++i) {
        if (suppliers[i].active && suppliers[i].id == id) return i;
    }
    return -1;
}

/* Generate a new unique ID (max existing id + 1) */
int generate_new_id() {
    int maxid = 0;
    for (int i = 0; i < supplier_count; ++i) {
        if (suppliers[i].active && suppliers[i].id > maxid) maxid = suppliers[i].id;
    }
    return maxid + 1;
}

/* Add a new supplier */
void add_supplier() {
    if (supplier_count >= MAX_SUPPLIERS) {
        printf("Cannot add more suppliers (limit reached).\n");
        return;
    }

    Supplier s;
    s.active = 1;
    s.id = generate_new_id();

    printf("Adding new supplier (auto ID = %d)\n", s.id);
    printf("Enter name: ");
    fgets(s.name, NAME_LEN, stdin); trim_newline(s.name);

    printf("Enter phone: ");
    fgets(s.phone, PHONE_LEN, stdin); trim_newline(s.phone);

    printf("Enter email: ");
    fgets(s.email, EMAIL_LEN, stdin); trim_newline(s.email);

    printf("Enter address: ");
    fgets(s.address, ADDR_LEN, stdin); trim_newline(s.address);

    printf("Enter items supplied / notes: ");
    fgets(s.items, ITEM_LEN, stdin); trim_newline(s.items);

    suppliers[supplier_count++] = s;
    save_data();
    printf("Supplier added successfully with ID %d.\n", s.id);
}

/* Display a single supplier */
void display_supplier(const Supplier *s) {
    if (!s->active) return;
    printf("ID: %d\n", s->id);
    printf("Name: %s\n", s->name);
    printf("Phone: %s\n", s->phone);
    printf("Email: %s\n", s->email);
    printf("Address: %s\n", s->address);
    printf("Items/Notes: %s\n", s->items);
    printf("--------------------------------------------------\n");
}

/* Display all suppliers */
void display_all_suppliers() {
    int found = 0;
    printf("\n--- All Suppliers ---\n");
    for (int i = 0; i < supplier_count; ++i) {
        if (suppliers[i].active) {
            display_supplier(&suppliers[i]);
            found = 1;
        }
    }
    if (!found) printf("No suppliers found.\n");
}

/* Search supplier by ID and show details */
void search_supplier() {
    int id;
    printf("Enter supplier ID to search: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        flush_input();
        return;
    }
    flush_input();
    int idx = find_supplier_by_id(id);
    if (idx == -1) {
        printf("Supplier with ID %d not found.\n", id);
    } else {
        printf("Supplier details:\n");
        display_supplier(&suppliers[idx]);
    }
}

/* Modify supplier details */
void modify_supplier() {
    int id;
    printf("Enter supplier ID to modify: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        flush_input();
        return;
    }
    flush_input();

    int idx = find_supplier_by_id(id);
    if (idx == -1) {
        printf("Supplier with ID %d not found.\n", id);
        return;
    }

    Supplier *s = &suppliers[idx];
    printf("Modifying supplier (ID %d). Leave blank to keep current value.\n", s->id);

    char buf[ITEM_LEN];

    printf("Current name: %s\nNew name: ", s->name);
    fgets(buf, NAME_LEN, stdin); trim_newline(buf);
    if (strlen(buf) > 0) strncpy(s->name, buf, NAME_LEN);

    printf("Current phone: %s\nNew phone: ", s->phone);
    fgets(buf, PHONE_LEN, stdin); trim_newline(buf);
    if (strlen(buf) > 0) strncpy(s->phone, buf, PHONE_LEN);

    printf("Current email: %s\nNew email: ", s->email);
    fgets(buf, EMAIL_LEN, stdin); trim_newline(buf);
    if (strlen(buf) > 0) strncpy(s->email, buf, EMAIL_LEN);

    printf("Current address: %s\nNew address: ", s->address);
    fgets(buf, ADDR_LEN, stdin); trim_newline(buf);
    if (strlen(buf) > 0) strncpy(s->address, buf, ADDR_LEN);

    printf("Current items/notes: %s\nNew items/notes: ", s->items);
    fgets(buf, ITEM_LEN, stdin); trim_newline(buf);
    if (strlen(buf) > 0) strncpy(s->items, buf, ITEM_LEN);

    save_data();
    printf("Supplier (ID %d) updated.\n", s->id);
}

/* Delete supplier (soft delete by marking active=0) */
void delete_supplier() {
    int id;
    printf("Enter supplier ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        flush_input();
        return;
    }
    flush_input();

    int idx = find_supplier_by_id(id);
    if (idx == -1) {
        printf("Supplier with ID %d not found.\n", id);
        return;
    }

    printf("Are you sure you want to delete supplier '%s' (ID %d)? (y/n): ", suppliers[idx].name, id);
    int ch = getchar();
    flush_input();
    if (ch == 'y' || ch == 'Y') {
        suppliers[idx].active = 0;
        save_data();
        printf("Supplier deleted.\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}

/* Sort suppliers by name (case-insensitive), keeping deleted entries out */
int name_cmp(const void *a, const void *b) {
    const Supplier *sa = (const Supplier *)a;
    const Supplier *sb = (const Supplier *)b;
    // Keep inactive suppliers at the end
    if (sa->active != sb->active) return (sb->active - sa->active);
    // case-insensitive compare
    char na[NAME_LEN], nb[NAME_LEN];
    strncpy(na, sa->name, NAME_LEN); na[NAME_LEN-1] = '\0';
    strncpy(nb, sb->name, NAME_LEN); nb[NAME_LEN-1] = '\0';
    for (int i = 0; na[i]; ++i) na[i] = tolower((unsigned char)na[i]);
    for (int i = 0; nb[i]; ++i) nb[i] = tolower((unsigned char)nb[i]);
    return strcmp(na, nb);
}

void sort_suppliers_by_name() {
    if (supplier_count <= 1) {
        printf("Not enough suppliers to sort.\n");
        return;
    }
    qsort(suppliers, supplier_count, sizeof(Supplier), name_cmp);
    save_data();
    printf("Suppliers sorted by name.\n");
}

/* Print menu */
void print_menu() {
    printf("\n==== Supplier Management ====\n");
    printf("1. Add supplier\n");
    printf("2. Display all suppliers\n");
    printf("3. Search supplier by ID\n");
    printf("4. Modify supplier\n");
    printf("5. Delete supplier\n");
    printf("6. Sort suppliers by name\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
}

/* Program entry */
int main() {
    load_data();
    int choice;

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            flush_input();
            continue;
        }
        flush_input();

        switch (choice) {
            case 1: add_supplier(); break;
            case 2: display_all_suppliers(); break;
            case 3: search_supplier(); break;
            case 4: modify_supplier(); break;
            case 5: delete_supplier(); break;
            case 6: sort_suppliers_by_name(); break;
            case 7:
                printf("Exiting. Goodbye!\n");
                save_data();
                exit(0);
                break;
            default:
                printf("Invalid choice. Please select 1-7.\n");
        }
    }
    return 0;
}
