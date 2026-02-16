/*
 * GRADE DATABASE (Refactored)
 *
 * - input_grades: Handles validation for grade entry (removes duplication).
 * - find_student_index: Asks user to search by ID or Name.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for system()
#include <ctype.h> // Required for tolower()

// === Configuration =======================================================
#define CLASS_SIZE 5
#define NAME_SIZE 50

// === Global storage ======================================================
char student[CLASS_SIZE][NAME_SIZE];
int grades[CLASS_SIZE][3];
int id_array[CLASS_SIZE];
int last = -1; // Index of the last used record (-1 = empty)

// === Function Declarations ===============================================
int menu(void);
void get_valid_grades(int *g1, int *g2, int *g3); // New Helper
int strings_match_ignore_case(const char *s1, const char *s2); // lowers name
int find_student_index(void);                     // New Locator
int is_duplicate_name(char *new_name);             // checks if duplicate

// Action functions now take no arguments (they call locator internally)
int add_data(char name[NAME_SIZE], int g1, int g2, int g3);
int view_data(void);
int edit_data(void);
int locate_data(void);
int delete_data(void);
int save_database(void);
int load_database(void);

// Utils
int validinput(void);

// === Main Loop ===========================================================
int main(void)
{
    int isRunning = 1;
    int g1, g2, g3;
    char name[NAME_SIZE];

    while (isRunning) {
        switch (menu()) {

        case 1:  /* Add student */
            system("cls");
            
            if (last == CLASS_SIZE - 1) {
                printf("Database is full...\n");
                // ... (break)
            }

            printf("Input student name: ");
            scanf(" %49[^\n]", name);

            // --- NEW CODE STARTS HERE ---
            
            // Check for duplicates immediately
            if (is_duplicate_name(name)) {
                printf("Error: Student '%s' already exists in the database.\n", name);
                system("pause");
                system("cls");
                break; // Stop! Do not ask for grades, do not add.
            }

            // DELEGATED: Call the new function to get grades
            get_valid_grades(&g1, &g2, &g3);

            add_data(name, g1, g2, g3);
            system("pause");
            system("cls");
            break;

        case 2:  /* View all records */
            system("cls");
            view_data();
            system("pause");
            system("cls");
            break;

        case 3: /* Edit */
            system("cls");
            edit_data(); // No arguments needed now
            system("pause");
            system("cls");            
            break;

        case 4: /* Locate */
            system("cls");
            locate_data(); // No arguments needed now
            system("pause");
            system("cls");
            break;

        case 5:  /* Delete */
            system("cls");
            view_data();
            delete_data(); // No arguments needed now
            system("pause");
            system("cls");
            break;

        case 6: /* Save */
            system("cls");
            save_database();
            system("pause");
            system("cls");
            break;

        case 7: /* Load */
            system("cls");
            load_database();
            system("pause");
            system("cls");
            break;

        case 8: /* Exit */
            printf("Thank you for using the program.\n");
            isRunning = 0;
            break;

        default:
            printf("Invalid input, please try again\n");
            system("pause");
            system("cls");
            break;
        }
    }

    return 0;
}

// === Helper Functions ====================================================

/*
 * get_valid_grades: Prompts user for 3 grades and validates range (0-100).
 * Uses pointers to return multiple values.
 */
void get_valid_grades(int *g1, int *g2, int *g3) {
    int valid = 0;
    do {
        printf("Enter 3 grades (Math Sci Eng): ");
        
        if (scanf("%d %d %d", g1, g2, g3) != 3) {
            printf("Invalid input. Please enter numbers only.\n");
            int c; while ((c = getchar()) != '\n' && c != EOF); // Flush buffer
            continue;
        }

        if (*g1 >= 0 && *g1 <= 100 && *g2 >= 0 && *g2 <= 100 && *g3 >= 0 && *g3 <= 100) {
            valid = 1;
        } else {
            printf("Grades must be between 0 and 100.\n");
        }
    } while (!valid);
}

/*
 * Compare two strings ignoring case.
 * Returns 1 if they match, 0 if they don't.
 */
int strings_match_ignore_case(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 0; // Characters differ
        }
        s1++;
        s2++;
    }
    // If we reached the end, check if BOTH strings ended at the same time
    return *s1 == *s2; 
}

/*
 * find_student_index: The "Smart Locator"
 * 1. Asks user to search by ID or Name.
 * 2. Searches the array.
 * 3. Returns the array INDEX of the found student, or -1 if not found.
 */
int find_student_index(void) {
    if (last == -1) return -1; // Empty DB

    int choice;
    printf("Locate by:\n[1] ID Number\n[2] Name\nSelect: ");
    choice = validinput();

    if (choice == 1) {
        // --- Search by ID ---
        printf("Enter ID #: ");
        int search_id = validinput();
        
        for (int i = 0; i <= last; i++) {
            if (id_array[i] == search_id) {
                return i; // Found it at index i
            }
        }
    } 
    else if (choice == 2) {
        // --- Search by Name ---
        char search_name[NAME_SIZE];
        printf("Enter Name: ");
        scanf(" %49[^\n]", search_name);

        for (int i = 0; i <= last; i++) {
            // UPDATED LINE: Use the new helper function
            if (strings_match_ignore_case(student[i], search_name)) {
                return i; // Found it at index i
            }
        }
    }
    else {
        printf("Invalid search mode.\n");
        return -1;
    }

    return -1; // Not found
}

/*
 * Checks if a name already exists in the database.
 * Returns 1 if duplicate found, 0 if unique.
 */
int is_duplicate_name(char *new_name) {
    for (int i = 0; i <= last; i++) {
        // Use our case-insensitive helper from before
        if (strings_match_ignore_case(student[i], new_name)) {
            return 1; // Found a match!
        }
    }
    return 0; // No match found
}

int menu(void)
{
    int option;
    printf("=== STUDENT DATABASE ===\n");
    printf("[1] Add Student\n[2] View Database\n[3] Edit Database\n[4] Locate Student\n[5] Remove Student\n[6] Save Database\n[7] Load Database\n[8] Exit\n\nSelect option: ");
    
    if (scanf("%d", &option) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }
    return option;
}

int validinput(void){
    int valid = 0, input;
    do {
        if (scanf(" %d", &input) != 1) {
            printf("Invalid input. Enter value: ");
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        valid = 1;
    } while (!valid);
    return input;
}

// === Action Functions ====================================================

int add_data(char name[NAME_SIZE], int g1, int g2, int g3)
{
    // Check handled in main now, but safety check remains
    if (last == CLASS_SIZE - 1) return 0;

    last++;
    id_array[last] = last + 1;
    strcpy(student[last], name);
    grades[last][0] = g1;
    grades[last][1] = g2;
    grades[last][2] = g3;
    printf("Student added successfully.\n");
    return 0;
}

int view_data(void)
{
    if (last == -1) {
        printf("Database is empty.\n");
        return -1;
    }

    printf("--- STUDENT RECORDS ---\n");
    printf("-----------------------------------------------------------------------\n");
    printf("ID | Name             | Math | Sci | Eng | Average | Remarks\n");
    printf("-----------------------------------------------------------------------\n");

    for (int i = 0; i <= last; i++) {
        float avg = (grades[i][0] + grades[i][1] + grades[i][2]) / 3.0f;
        printf("%2d | %-16.16s | %-4d | %-3d | %-3d | %-7.2f | %s\n",
               id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2],
               avg, (avg >= 75.0f) ? "Passed" : "Failed");
    }
    printf("-----------------------------------------------------------------------\n");
    return 0;
}

int edit_data(void)
{
    if (last == -1) {
        printf("Database is empty.\n");
        return 0;
    }

    // 1. Find the index using the new locator
    int target_index = find_student_index();

    if (target_index == -1) {
        printf("Student not found.\n");
        return 0;
    }

    printf("Editing record for: %s (ID: %d)\n", student[target_index], id_array[target_index]);

    // 2. Use the new helper function for input
    get_valid_grades(&grades[target_index][0], 
                     &grades[target_index][1], 
                     &grades[target_index][2]);

    printf("Record updated.\n");
    return 0;
}

int locate_data(void)
{
    if (last == -1) {
        printf("Database is empty.\n");
        return 0;
    }

    // 1. Find index
    int i = find_student_index();

    if (i == -1) {
        printf("Student not found.\n");
        return 0;
    }

    // 2. Show single result
    printf("--- RECORD FOUND ---\n");
    printf("-----------------------------------------------------------------------\n");
    printf("ID | Name             | Math | Sci | Eng | Average | Remarks\n");
    printf("-----------------------------------------------------------------------\n");
    
    float avg = (grades[i][0] + grades[i][1] + grades[i][2]) / 3.0f;
    printf("%2d | %-16.16s | %-4d | %-3d | %-3d | %-7.2f | %s\n",
            id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2],
            avg, (avg >= 75.0f) ? "Passed" : "Failed");
    printf("-----------------------------------------------------------------------\n");
    return 0;
}

int delete_data(void)
{
    if (last == -1) {
        printf("Database is empty.\n");
        return 0;
    }

    // 1. Find index
    int to_delete = find_student_index();

    if (to_delete == -1) {
        printf("Student not found.\n");
        return 0;
    }

    printf("Deleting %s (ID: %d)...\n", student[to_delete], id_array[to_delete]);

    // 2. Shift Logic
    for (int i = to_delete; i < last; i++) {
        id_array[i] = id_array[i + 1];
        strcpy(student[i], student[i + 1]);
        grades[i][0] = grades[i + 1][0];
        grades[i][1] = grades[i + 1][1];
        grades[i][2] = grades[i + 1][2];
    }

    last--;

    // 3. Re-ID Logic
    for (int i = 0; i <= last; i++) {
        id_array[i] = i + 1;
    }

    printf("Record deleted successfully.\n");
    return 0;
}

// === FILE HANDLING FUNCTIONS ===

/*
 * save_database: Writes all current data to "database.txt"
 */
int save_database(void) {
    if (last == -1) {
        printf("Database is empty. Nothing to save.\n");
        return 0;
    }

    FILE *fp = fopen("database.txt", "w"); // "w" = Write (Overwrites existing file)
    
    if (fp == NULL) {
        printf("Error: Could not create file.\n");
        return 0;
    }

    // Write the count of students first (optional, but helpful)
    // Then write each student: ID,Name,G1,G2,G3
    for (int i = 0; i <= last; i++) {
        // Format: ID Name Math Sci Eng (Space separated is easiest for scanf)
        // Note: Using a unique delimiter like | or just spaces if names have no spaces
        // Since your names might have spaces, let's use a comma ',' as a delimiter.
        fprintf(fp, "%d,%s,%d,%d,%d\n", 
                id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2]);
    }

    fclose(fp); // Always close the file!
    printf("Database saved successfully to 'database.txt'.\n");
    return 1;
}

/*
 * load_database: Reads data from "database.txt" into arrays
 */
int load_database(void) {
    FILE *fp = fopen("database.txt", "r"); // "r" = Read
    
    if (fp == NULL) {
        printf("No saved database found.\n");
        return 0;
    }

    // Clear current database in memory
    last = -1;

    // Temporary variables to hold read data
    int id, g1, g2, g3;
    char name[NAME_SIZE];

    printf("Loading data...\n");

    // Loop until the end of the file (EOF)
    // The format string must match the fprintf format EXACTLY
    // %[^,] reads everything until a comma (good for names with spaces)
    while (fscanf(fp, "%d,%[^,],%d,%d,%d\n", &id, name, &g1, &g2, &g3) == 5) {
        
        if (last >= CLASS_SIZE - 1) {
            printf("Warning: Database full. Stopped loading early.\n");
            break;
        }

        last++;
        id_array[last] = id;
        strcpy(student[last], name);
        grades[last][0] = g1;
        grades[last][1] = g2;
        grades[last][2] = g3;
    }

    fclose(fp);
    printf("Database loaded! %d records found.\n", last + 1);
    return 1;
}