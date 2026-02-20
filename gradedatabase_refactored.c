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
#define CLASS_SIZE 10
#define NAME_SIZE 50

// === Global storage ======================================================
char student[CLASS_SIZE][NAME_SIZE];
int grades[CLASS_SIZE][3];
int id_array[CLASS_SIZE];
int last = -1; // Index of the last used record (-1 = empty)

// === Function declarations (what each part does) ==========================
int main(); // main hub for action, calls menu functions
void get_valid_grades(int *g1, int *g2, int *g3);              // refactored version of grade validation to reduce code duplication for add and edit record
int strings_match_ignore_case(const char *s1, const char *s2); // makes sure that it finds record by name regardless of string case (upper/lower)
int find_student_index(void);                     			   // New Locator, deprecated original locator function so action functions call this internally
int is_duplicate_name(char *new_name);             		       // checks if name entered is duplicate
void log_action(char *message);								   // makes logs to all actions made in the program
void sort_by_name(void);                                       // sort the database by name using bubblesort

// === Menu Functions =======================================================
int menu(); //displays the Menu UI
int modify_menu();
int add_data(char name[NAME_SIZE], int g1, int g2, int g3);
	int edit_data(void);
	int delete_data(void);
int view_data(void);
int locate_data(void);
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

        case 1:  
            modify_menu();
            system("cls");
            break;

        case 2:  /* View all records */
            system("cls");
            view_data();
            system("pause");
            system("cls");
            break;

        case 3: /* Locate */
            system("cls");
            locate_data(); // No arguments needed now
            system("pause");
            system("cls");
            break;

        case 4: /* Save */
            system("cls");
            save_database();
            system("pause");
            system("cls");
            break;

        case 5: /* Load */
            system("cls");
            load_database();
            system("pause");
            system("cls");
            break;

        case 6: /* Exit */
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

void log_action(char *message) {
    FILE *fp = fopen("audit_log.txt", "a"); // opens auditl log file
    if (fp != NULL) {
        fprintf(fp, "LOG: %s\n", message); // uses the message sent by the function as the string input
        fclose(fp);
    }
}

void sort_by_name(void) {
    // Bubble Sort Algorithm
    for (int i = 0; i <= last - 1; i++) {
        for (int j = 0; j <= last - i - 1; j++) {
            
            // If Student J comes AFTER Student J+1 alphabetically... SWAP THEM!
            // (e.g., if student[j] is "Zebra" and student[j+1] is "Apple")
            if (strcmp(student[j], student[j + 1]) > 0) {
                
                // 1. Swap the Names using a temporary string
                char temp_name[NAME_SIZE];
                strcpy(temp_name, student[j]);
                strcpy(student[j], student[j + 1]);
                strcpy(student[j + 1], temp_name);

                // 2. Swap the Grades (Math, Sci, Eng) using a loop
                for (int k = 0; k < 3; k++) {
                    int temp_grade = grades[j][k];
                    grades[j][k] = grades[j + 1][k];
                    grades[j + 1][k] = temp_grade;
                }

                // 3. optional Swap the IDs (So the student keeps their original ID number)
                //int temp_id = id_array[j];
                //id_array[j] = id_array[j + 1];
                //id_array[j + 1] = temp_id;
            }
        }
    }
}

// menu: show choices and read user selection, returns choice 1..4, or -1 on invalid input
int menu(void)
{
    int option;

    printf("=== STUDENT DATABASE ===\n");
    printf("[1] Modify Database\n[2] View Database\n[3] Locate Student\n[4] Save Database\n[5] Load Database\n[6] Exit\n\nSelect option: ");
	
    /* If scanf fails (user typed a letter), clear the input line and return -1 */
	option = validinput();

    return option;
}

int modify_menu(){
	int option;
	int g1, g2, g3, id_number;
    char name[NAME_SIZE];

	while (1){
        system("cls");
        printf("=== MODIFY DATABASE ===\n");
        printf("[1] Add Record\n[2] Edit Record\n[3] Remove Record\n[4] Return to main menu\n\nSelect option: ");
        /* If scanf fails (user typed a letter), clear the input line and return -1 */
        option = validinput();
        switch (option)
        {
        case 1: /* Add student */
            system("cls");  // clear screen for neatness on Windows

            printf("Input student name: ");
            // Read up to 49 chars and stop at newline to avoid overflow
            scanf(" %49[^\n]", name);

            if (is_duplicate_name(name)) // if true, stop Do not ask for grades, do not add.
            {
                printf("Error: Student '%s' already exists in the database.\n", name);
                system("pause");
                system("cls");
                break; 
            }
            
            // DELEGATED: Call the new function to get grades
            get_valid_grades(&g1, &g2, &g3);
            
            add_data(name, g1, g2, g3); // call the function that adds data to the database
            system("pause");
            system("cls");
            break;
        
        case 2: /* edit record */
            system("cls");
            edit_data(); // No arguments needed now
            system("pause");
            system("cls");   
            break;
        
        case 3: /* delete record */
            system("cls");
            delete_data(); // No arguments needed now
            system("pause");
            system("cls");
            break;
        
        case 4:
            return 0;
            break;
        
        default:
            printf("Invalid input, please try again\n");
            system("pause");
            system("cls");
            break;
        }
	}


    return option;
}

// add_data: insert a new student record at the end, takes name (string with spaces), then g1,g2,g3 (three integer grades values 0-100), returns 0 on success and prints error message and returns 0 for full database
int add_data(char name[NAME_SIZE], int g1, int g2, int g3){
	
    if (last == CLASS_SIZE - 1) { //if the value of last equates to the class_size (accounting for last counting on 0) will report the user that the database is full
        printf("Database is full, please delete existing data first.\n");
        return 0;
    }

    last++;                         // move to next free slot
    id_array[last] = last + 1;      // simple user-friendly ID
    strcpy(student[last], name);    // copy name into storage
    grades[last][0] = g1;           // Math
    grades[last][1] = g2;           // Sci
    grades[last][2] = g3;           // Eng
    
    char log_msg[100];
    sprintf(log_msg, "Added student %s", student[last]);
    log_action(log_msg);

    sort_by_name();
    printf("Student added successfully.\n");
    return 0;
}

// view_data: print all stored student records in a table - prints ID, Name, each grade, average, and remark (Passed/Failed)
int view_data(void)
{
    if (last == -1) { // checks if database is empty
        printf("Database is empty, please enter data first.\n");
        return -1;
    }

    float average;

    printf("--- STUDENT RECORDS ---\n");
    printf("-----------------------------------------------------------------------\n");
    printf("ID | Name             | Math | Sci | Eng | Average | Remarks\n");
    printf("-----------------------------------------------------------------------\n");

    for (size_t i = 0; i <= last; i++) {
        average = (grades[i][0] + grades[i][1] + grades[i][2]) / 3.0f;
        printf("%2d | %-16.16s | %-4d | %-3d | %-3d | %-7.2f | %s\n",
               id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2],
               average, (average >= 75.0f) ? "Passed" : "Failed"); // uses a ternary operator for remarks, checks if average is greater than or equal to 75 then prints remark
    }
    printf("-----------------------------------------------------------------------\n");
    return 0;
}

// edit_data: locates a record by its ID or name then prompts you to edit 3 grades

// Behavior:
// - If database is empty, a message is printed.
// - If record is not found, a message is printed.
int edit_data(void)
{
    if (last == -1) {
        printf("Database is empty.\n");
        return 0;
    }

    // 1. Find the index using the new locator
    int target_index = find_student_index();

    if (target_index ==-1){
        printf("Student not found.\n");
        return 0;
    }
	view_data();
    printf("Editing record for: %s (ID: %d)\n", student[target_index], id_array[target_index]);

    get_valid_grades(&grades[target_index][0], &grades[target_index][1], &grades[target_index][2]);
    
    char log_msg[100];
    sprintf(log_msg, "Modified student %s's grades", student[last]);
    log_action(log_msg);

    printf("Record updated.\n");
    view_data();
}

// locate_data: locates a record by its ID or name

// Behavior:
// - If database is empty, a message is printed.
// - If record is not found, a message is printed.
int locate_data()
{
    if (last == -1) {
        printf("Database is empty.\n");
        return 0;
    }
    int target_index = find_student_index();

    if (target_index ==-1){
        printf("Student not found.\n");
        return 0;
    }

    // 2. Show single result
	printf("Found %s at (ID: %d)\n", student[target_index], id_array[target_index]);
    printf("-----------------------------------------------------------------------\n");
    printf("ID | Name             | Math | Sci | Eng | Average | Remarks\n");
    printf("-----------------------------------------------------------------------\n");
    
    float avg = (grades[target_index][0] + grades[target_index][1] + grades[target_index][2]) / 3.0f;
    printf("%2d | %-16.16s | %-4d | %-3d | %-3d | %-7.2f | %s\n",
            id_array[target_index], student[target_index], grades[target_index][0], grades[target_index][1], grades[target_index][2],
            avg, (avg >= 75.0f) ? "Passed" : "Failed");
    printf("-----------------------------------------------------------------------\n");

}

// delete_data: remove a record by its ID and shift following records down

// Behavior:
// - If database is empty, a message is printed.
// - If ID is not found, a message is printed and nothing changes.
// - After deletion, IDs are reassigned to stay consecutive (1,2,...).
int delete_data()
{
    if (last == -1) {
        printf("Database is empty.\n");
        return 0;
    }

    view_data();
    // 1. find index
    int target_index = find_student_index();

    if (target_index ==-1){
        printf("Student not found.\n");
        return 0;
    }
	printf("Deleting %s (ID: %d)...\n", student[target_index], id_array[target_index]);

    char log_msg[100];
    sprintf(log_msg, "Deleted student %s", student[last]);
    log_action(log_msg);

    // 2. Shift Logic
    for (int i = target_index; i < last; i++) {
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
    view_data();
}

//load_database: saves data from arrays into "database.txt" 
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
        // Since names have spaces, we will use a comma ',' as a delimiter.
        fprintf(fp, "%d,%s,%d,%d,%d\n", id_array[i], student[i], grades[i][0], grades[i][1], grades[i][2]);
    }

    fclose(fp); // Always close the file taught by sir renegado
    printf("Database saved successfully to 'database.txt'.\n");
    return 1;
}


//load_database: Reads data from "database.txt" into arrays
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

    sort_by_name();
    printf("Database loaded! %d records found.\n", last + 1);
    return 1;
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
