/*
 * GRADE DATABASE (Barebones Edition)
 *
 * Features: Add, View, Edit, Locate, Delete, Save, Load.
 * Constraints: No input validation, Case-sensitive, No duplicate checks.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// === Configuration =======================================================
#define CLASS_SIZE 5
#define NAME_SIZE 50
#define DB_FILE "database.txt"

// === Global Storage ======================================================
char student[CLASS_SIZE][NAME_SIZE];
int grades[CLASS_SIZE][3];
int id_array[CLASS_SIZE];
int last = -1; // -1 means empty

// === Prototypes ==========================================================
int menu(void);
int find_index(void);
void add_data(void);
void view_data(void);
void edit_data(void);
void locate_data(void);
void delete_data(void);
void save_database(void);
void load_database(void);

// === Main Loop ===========================================================
int main(void)
{
    int isRunning = 1;
    
    // Optional: Auto-load on start for convenience
    load_database(); 
    system("pause");
    system("cls");

    while (isRunning) {
        switch (menu()) {
            case 1: add_data();    break;
            case 2: view_data();   break;
            case 3: edit_data();   break;
            case 4: locate_data(); break;
            case 5: delete_data(); break;
            case 6: save_database(); break;
            case 7: load_database(); break;
            case 8: isRunning = 0; break;
            default: printf("Invalid choice.\n"); break;
        }
        if (isRunning) {
            system("pause");
            system("cls");
        }
    }
    return 0;
}

// === Helper: Simple Locator ==============================================
int find_index(void) {
    if (last == -1) return -1;
    
    int choice;
    printf("Search by: [1] ID [2] Name: ");
    scanf("%d", &choice);

    if (choice == 1) {
        int id;
        printf("Enter ID: ");
        scanf("%d", &id);
        for (int i = 0; i <= last; i++) {
            if (id_array[i] == id) return i;
        }
    } else {
        char name[NAME_SIZE];
        printf("Enter Name: ");
        scanf(" %49[^\n]", name);
        for (int i = 0; i <= last; i++) {
            // Raw strcmp: Case-sensitive!
            if (strcmp(student[i], name) == 0) return i;
        }
    }
    return -1;
}

// === Core Functions ======================================================

int menu(void) {
    int option;
    printf("=== BAREBONES DATABASE ===\n");
    printf("[1] Add\n[2] View\n[3] Edit\n[4] Locate\n[5] Delete\n[6] Save\n[7] Load\n[8] Exit\nChoice: ");
    scanf("%d", &option);
    return option;
}

void add_data(void) {
    if (last == CLASS_SIZE - 1) {
        printf("Database full.\n");
        return;
    }
    
    last++;
    id_array[last] = last + 1;

    printf("Enter Name: ");
    scanf(" %49[^\n]", student[last]);

    printf("Enter 3 Grades (Math Sci Eng): ");
    // No validation loop. Assumes user inputs numbers correctly.
    scanf("%d %d %d", &grades[last][0], &grades[last][1], &grades[last][2]);

    printf("Added.\n");
}

void view_data(void) {
    if (last == -1) { printf("Empty.\n"); return; }

    printf("ID | Name             | M  | S  | E  | Avg\n");
    for (int i = 0; i <= last; i++) {
        float avg = (grades[i][0] + grades[i][1] + grades[i][2]) / 3.0f;
        printf("%2d | %-16s | %-2d | %-2d | %-2d | %.2f\n",
               id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2], avg);
    }
}

void edit_data(void) {
    int idx = find_index();
    if (idx == -1) { printf("Not found.\n"); return; }

    printf("Editing %s. Enter new grades (Math Sci Eng): ", student[idx]);
    scanf("%d %d %d", &grades[idx][0], &grades[idx][1], &grades[idx][2]);
    printf("Updated.\n");
}

void locate_data(void) {
    int idx = find_index();
    if (idx == -1) { printf("Not found.\n"); return; }

    float avg = (grades[idx][0] + grades[idx][1] + grades[idx][2]) / 3.0f;
    printf("FOUND: ID %d | %s | Avg: %.2f\n", id_array[idx], student[idx], avg);
}

void delete_data(void) {
    int idx = find_index();
    if (idx == -1) { printf("Not found.\n"); return; }

    // Shift everything down
    for (int i = idx; i < last; i++) {
        id_array[i] = id_array[i + 1];
        strcpy(student[i], student[i + 1]);
        grades[i][0] = grades[i + 1][0];
        grades[i][1] = grades[i + 1][1];
        grades[i][2] = grades[i + 1][2];
    }
    last--;

    // Re-ID logic (Optional, but keeps things neat)
    for (int i = 0; i <= last; i++) id_array[i] = i + 1;
    
    printf("Deleted.\n");
}

void save_database(void) {
    FILE *fp = fopen(DB_FILE, "w");
    if (!fp) { printf("Error saving.\n"); return; }

    for (int i = 0; i <= last; i++) {
        // Simple CSV format
        fprintf(fp, "%d,%s,%d,%d,%d\n", 
                id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2]);
    }
    fclose(fp);
    printf("Saved.\n");
}

void load_database(void) {
    FILE *fp = fopen(DB_FILE, "r");
    if (!fp) { printf("No file found.\n"); return; }

    last = -1;
    int id, g1, g2, g3;
    char name[NAME_SIZE];

    // Blindly trust the file content
    while (fscanf(fp, "%d,%[^,],%d,%d,%d\n", &id, name, &g1, &g2, &g3) == 5) {
        last++;
        id_array[last] = id;
        strcpy(student[last], name);
        grades[last][0] = g1;
        grades[last][1] = g2;
        grades[last][2] = g3;
    }
    fclose(fp);
    printf("Loaded.\n");
}