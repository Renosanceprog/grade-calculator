#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define NAME_SIZE 50
#define PREFIX_SIZE 10
#define SAVE_DIR "./save files/"

// === Structs & Globals ===
typedef struct Node {
    char *UID; // Reverted back to dynamic string pointer
    char name[NAME_SIZE];
    int grades[3];
    struct Node *next;
} StudentRecord;

StudentRecord *head = NULL;
int studentCount = 0;
char idPrefix[PREFIX_SIZE] = "ID"; // Default prefix fallback

// === Helper Functions ===
void get_valid_grades(int *g1, int *g2, int *g3) {
    do {
        printf("Enter 3 grades (0-100): ");
        scanf("%d %d %d", g1, g2, g3);
    } while (*g1 < 0 || *g1 > 100 || *g2 < 0 || *g2 > 100 || *g3 < 0 || *g3 > 100);
}

// Generates the dynamic UID string (e.g., "CS-001")
char *generate_UID(void) {
    studentCount++;
    int len = snprintf(NULL, 0, "%s-%03d", idPrefix, studentCount);
    char *new_id = (char *)malloc(len + 1);
    sprintf(new_id, "%s-%03d", idPrefix, studentCount);
    return new_id;
}

// Now searches using string comparison
StudentRecord *find_student(char *search_id) {
    StudentRecord *walker = head;
    while (walker != NULL) {
        // Using standard strcmp. Note: this is case-sensitive!
        if (strcmp(walker->UID, search_id) == 0) return walker;
        walker = walker->next;
    }
    return NULL;
}

StudentRecord *locatePrevNode(int g1, int g2, int g3) {
    int new_sum = g1 + g2 + g3;
    StudentRecord *walker = head;
    StudentRecord *prev = NULL;

    while (walker != NULL) {
        int walker_sum = walker->grades[0] + walker->grades[1] + walker->grades[2];
        if (walker_sum < new_sum) break; 
        prev = walker;
        walker = walker->next;
    }
    return prev;
}

void detach_node(StudentRecord *target) {
    if (head == target) {
        head = target->next;
    } else {
        StudentRecord *prev = head;
        while (prev->next != target) prev = prev->next;
        prev->next = target->next;
    }
}

// IMPORTANT: Must free the UID string before freeing the node!
void free_database() {
    StudentRecord *walker = head;
    while (walker) {
        StudentRecord *temp = walker;
        walker = walker->next;
        free(temp->UID); 
        free(temp);
    }
    head = NULL;
}

void list_save_files() {
    DIR *d = opendir(SAVE_DIR);
    struct dirent *dir;
    if (d) {
        printf("\n--- Available Files in %s ---\n", SAVE_DIR);
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".txt")) printf(" - %s\n", dir->d_name);
        }
        closedir(d);
        printf("-----------------------------------\n");
    } else {
        printf("\nDirectory '%s' not found. Please create it.\n", SAVE_DIR);
    }
}

// === CRUD Functions ===
void add_data() {
    char name[NAME_SIZE];
    int g1, g2, g3;

    printf("Input student name: ");
    scanf(" %49[^\n]", name);
    get_valid_grades(&g1, &g2, &g3);

    StudentRecord *prev = locatePrevNode(g1, g2, g3);
    StudentRecord *new_node = (StudentRecord *)malloc(sizeof(StudentRecord));
    
    new_node->UID = generate_UID(); // Allocate and set the string ID
    strcpy(new_node->name, name);
    new_node->grades[0] = g1; new_node->grades[1] = g2; new_node->grades[2] = g3;

    if (prev == NULL) {
        new_node->next = head;
        head = new_node;
    } else {
        new_node->next = prev->next;
        prev->next = new_node;
    }
    printf("Record added with ID: %s\n", new_node->UID);
}

void view_database() {
    if (!head) { printf("Database is empty.\n"); return; }
    
    printf("\nID       | Name       | Math | Sci | Eng \n");
    printf("-----------------------------------------\n");
    StudentRecord *walker = head;
    while (walker) {
        // Formatted to accommodate string IDs
        printf("%-8s | %-10s | %-4d | %-3d | %-3d\n", 
               walker->UID, walker->name, walker->grades[0], walker->grades[1], walker->grades[2]);
        walker = walker->next;
    }
    printf("\n");
}

void edit_data() {
    char id[20];
    int g1, g2, g3;
    printf("Enter ID to edit (e.g., %s-001): ", idPrefix); 
    scanf("%19s", id);
    
    StudentRecord *target = find_student(id);
    if (!target) { printf("Student not found.\n"); return; }

    get_valid_grades(&g1, &g2, &g3);
    detach_node(target);

    target->grades[0] = g1; target->grades[1] = g2; target->grades[2] = g3;

    StudentRecord *prev = locatePrevNode(g1, g2, g3);
    if (prev == NULL) {
        target->next = head;
        head = target;
    } else {
        target->next = prev->next;
        prev->next = target;
    }
    printf("Record updated.\n");
}

void delete_data() {
    char id[20];
    printf("Enter ID to delete: "); scanf("%19s", id);
    
    StudentRecord *target = find_student(id);
    if (!target) { printf("Student not found.\n"); return; }

    detach_node(target);
    free(target->UID); // Free the string first
    free(target);      // Then free the node
    printf("Record deleted.\n");
}

// === Persistence Functions ===
void save_database() {
    char filename[50], filepath[100];
    printf("Enter save name (without .txt): ");
    scanf("%49s", filename);
    
    snprintf(filepath, sizeof(filepath), "%s%s.txt", SAVE_DIR, filename);

    FILE *fp = fopen(filepath, "w");
    if (!fp) { printf("Error: Could not save to %s\n", filepath); return; }

    StudentRecord *walker = head;
    while (walker) {
        // Writes string %s for UID
        fprintf(fp, "%s,%s,%d,%d,%d\n", 
            walker->UID, walker->name, walker->grades[0], walker->grades[1], walker->grades[2]);
        walker = walker->next;
    }
    fclose(fp);
    printf("Database saved to %s.\n", filepath);
}

void load_database() {
    char filename[50], filepath[100];
    
    list_save_files(); 
    
    printf("Enter file to load (without .txt): ");
    scanf("%49s", filename);
    snprintf(filepath, sizeof(filepath), "%s%s.txt", SAVE_DIR, filename);

    FILE *fp = fopen(filepath, "r");
    if (!fp) { printf("Error: %s not found.\n", filepath); return; }

    free_database(); 
    studentCount = 0;

    int g1, g2, g3;
    char temp_id[20], name[NAME_SIZE];

    // Read %[^,] for the string ID up to the comma
    while (fscanf(fp, "%[^,],%[^,],%d,%d,%d\n", temp_id, name, &g1, &g2, &g3) == 5) {
        
        // --- APP STATE RESTORATION ---
        char *hyphen_ptr = strchr(temp_id, '-');
        if (hyphen_ptr != NULL) {
            int prefix_length = hyphen_ptr - temp_id; 
            strncpy(idPrefix, temp_id, prefix_length);
            idPrefix[prefix_length] = '\0'; 
            
            int loaded_number = atoi(hyphen_ptr + 1);
            if (loaded_number > studentCount) studentCount = loaded_number;
        }

        StudentRecord *prev = locatePrevNode(g1, g2, g3);
        StudentRecord *new_node = (StudentRecord *)malloc(sizeof(StudentRecord));
        
        // Allocate and copy the exact loaded string
        new_node->UID = (char *)malloc(strlen(temp_id) + 1);
        strcpy(new_node->UID, temp_id);
        
        strcpy(new_node->name, name);
        new_node->grades[0] = g1; new_node->grades[1] = g2; new_node->grades[2] = g3;

        if (prev == NULL) {
            new_node->next = head;
            head = new_node;
        } else {
            new_node->next = prev->next;
            prev->next = new_node;
        }
    }
    fclose(fp);
    printf("Database loaded. Next ID will use prefix '%s'.\n", idPrefix);
}

// === Main Loop ===
int main() {
    int choice;
    
    // Set up the prefix once at launch
    printf("Enter Class Prefix (e.g., CS): ");
    scanf("%9s", idPrefix);

    while (1) {
        printf("\n[1] Add  [2] View  [3] Edit  [4] Delete  [5] Save  [6] Load  [7] Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) return 0; 

        switch (choice) {
            case 1: add_data(); break;
            case 2: view_database(); break;
            case 3: edit_data(); break;
            case 4: delete_data(); break;
            case 5: save_database(); break;
            case 6: load_database(); break;
            case 7: free_database(); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}