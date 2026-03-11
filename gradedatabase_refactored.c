/*
 * GRADE DATABASE (Linked List Refactor)
 *
 * - get_valid_grades: Handles validation for grade entry (prevents duplication).
 * - find_student: Asks user to search by permanent ID (UID) or Name. Returns a 
 * direct memory pointer to the node instead of an array index.
 * - locatePrevNode: The "Wingman" traversal that finds the correct insertion 
 * gap to keep the linked list sorted by grade average.
 * - edit/delete_data: Safely bridges pointers around a target node to cleanly
 * detach it from the chain without memory leaks.
 * - save/load_database: Handles persistent CSV storage and executes safe 
 * list demolition (freeing memory) before loading new data.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <ctype.h> 

// === Configuration =======================================================
// Set the max number of name and prefix length
#define NAME_SIZE 50
#define PREFIX_SIZE 10

// === Global storage (Structs) ============================================
// Using a struct to keep all student info together (id, name, and 3 grades)
// Makes it way easier to work with complete student records instead of separate arrays
typedef struct Node{
    char *UID;
    char name[NAME_SIZE];
    int grades[3];
    struct Node *next_student;
} StudentRecord;

// linked list of student records - our "database"
StudentRecord *head = NULL;

// Tracks the index of the last student added (starts at 0 for empty database)
int studentCount = 0;
char idPrefix[PREFIX_SIZE];

// === Function declarations ===============================================
int main();
int validinput(void);
void get_valid_grades(int *g1, int *g2, int *g3);              
int strings_match_ignore_case(const char *s1, const char *s2); 
StudentRecord *find_student(void);                                  
int is_duplicate_name(char *new_name);                         
void log_action(char *message);                                
StudentRecord *locatePrevNode(char name[], int g1, int g2, int g3);
int print_Student(StudentRecord *node_Student);
char* generate_UID(void);

int persistence_menu();
    int new_database(void);
    int save_database(void);
    int load_database(void);
int menu(void);
int modify_menu(void);
    int add_data(char name[NAME_SIZE], int g1, int g2, int g3);
    int edit_data(void);
    int delete_data(void);
int view_database(void);

// === Main Loop ===========================================================
// Main menu system - keeps running until user exits
int main(void) {
    int isRunning = 1;
    if (persistence_menu() == -1){ printf("Thank you for using the program.\n"); return 0;}
    while (isRunning) {
        // Display menu and handle user choices
        switch (menu()) {
        case 1:  modify_menu(); system("cls"); break;
        case 2:  system("cls"); view_database(); system("pause"); system("cls"); break;
        case 3:  system("cls"); print_Student(find_student()); system("pause"); system("cls"); break;
        case 4:  system("cls"); if (persistence_menu() == -1){ system("cls"); printf("Thank you for using the program.\n"); isRunning = 0;} break;
        default: printf("Invalid input.\n"); system("pause"); system("cls"); break;
        }
    }
    return 0;
}

// === Helper Functions ====================================================

// Helper function to safely get an integer from user
// Keeps asking until we get valid integer input
int validinput(void){
    int valid = 0, input;
    do {
        // Try to read an integer
        if (scanf(" %d", &input) != 1) {
            printf("Invalid input. Enter value: ");
            // Clear the input buffer if user entered non-numeric garbage
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        valid = 1;  // Got good input, exit loop
    } while (!valid);
    return input;
}

// Helper function to get grades from user
// Keeps asking until we get valid input (0-100 for each grade)
// Using pointers so we can return multiple values back to caller
void get_valid_grades(int *g1, int *g2, int *g3) {
    int valid = 0;
    do {
        printf("Enter 3 grades (Math Sci Eng): ");
        // Check if we got exactly 3 numbers
        if (scanf("%d %d %d", g1, g2, g3) != 3) {
            printf("Invalid input. Please enter numbers only.\n");
            // Clear the input buffer if user entered garbage
            int c; while ((c = getchar()) != '\n' && c != EOF); 
            continue;
        }
        // Validate that all grades are in valid range
        if (*g1 >= 0 && *g1 <= 100 && *g2 >= 0 && *g2 <= 100 && *g3 >= 0 && *g3 <= 100) {
            valid = 1;  // Good input, exit loop
        } else {
            printf("Grades must be between 0 and 100.\n");
        }
    } while (!valid);
}

// Compare two strings ignoring uppercase/lowercase differences
// Returns 1 if they match, 0 if they don't
int strings_match_ignore_case(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        // Convert both characters to lowercase before comparing
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) return 0; 
        s1++; s2++;
    }
    return *s1 == *s2; 
}

// Search for a student by ID or Name
// Returns the index where we found them, or -1 if not found
StudentRecord *find_student(void) {
    int choice;
    printf("Locate by:\n[1] Index\n[2] ID Number\n[3] Name\nSelect: ");
    choice = validinput();
    StudentRecord *walker = head;
    view_database();
    // Search by ID number
    switch (choice)
    {
    case 1:
        printf("Enter index: ");
        int index, walkerIndex = 1;
        index = validinput();
        // Loop through all students until we find the matching ID
        while (walker != NULL) {
            if (index == walkerIndex) return walker;
            walker = walker->next_student;
            walkerIndex++;
        }
        break;
    case 2:
        printf("Enter ID (%s-###): ", idPrefix);
        char search_id[PREFIX_SIZE];
        scanf(" %9[^\n]", search_id);
        // Loop through all students until we find the matching ID
        while (walker != NULL) {
            if (strings_match_ignore_case(walker->UID, search_id)) return walker;
            walker = walker->next_student;
        }
        break;
    case 3:
        char search_name[NAME_SIZE];
        printf("Enter Name: ");
        scanf(" %49[^\n]", search_name);
        // Loop through all students checking for name match (case doesn't matter)
        while (walker != NULL) {
            if (strings_match_ignore_case(walker->name, search_name)) return walker;
            walker = walker->next_student;
        }
        break;
    default:
        printf("Choose a valid option.\n");
        break;
    }
    return NULL; 
}

// Check if a student name already exists in the database
// Returns 1 if duplicate found, 0 if name is new
int is_duplicate_name(char *new_name) {
    StudentRecord *walker = head;
    while (walker != NULL) {
        if (strings_match_ignore_case(walker->name, new_name)) return 1;
        walker = walker->next_student;
    }    
    return 0; 
}

void log_action(char *message) {
    FILE *fp = fopen("audit_log.txt", "a"); 
    if (fp != NULL) {
        fprintf(fp, "LOG: %s\n", message); 
        fclose(fp);
    }
}


StudentRecord *locatePrevNode(char name[], int g1, int g2, int g3) {
    int new_sum = g1 + g2 + g3;  
    StudentRecord *walker = head;
    StudentRecord *prev = NULL;

    while (walker != NULL) {
        int walker_sum = walker->grades[0] + walker->grades[1] + walker->grades[2];
        

        if (walker_sum > new_sum) {

        } 
        else if (walker_sum == new_sum && stricmp(walker->name, name) < 0) {

        } 
        else {
            break;
        }
        

        prev = walker;
        walker = walker->next_student;
    }
    
    return prev;
}

// Display a single student's information
int print_Student(StudentRecord *node_Student) {
    if (studentCount == 0) { printf("Database is empty.\n"); return 0; }
    // Find the student
    if (node_Student == NULL){ printf("Student not found.\n"); return 0; }

    printf("-----------------------------------------------------------------------\n");
    printf("ID      | Name             | Math | Sci | Eng | Average | Remarks\n");
    printf("-----------------------------------------------------------------------\n");
    
    // Calculate and display average
    float avg = (node_Student->grades[0] + node_Student->grades[1] + node_Student->grades[2]) / 3.0f;
    printf("%7.7s | %-16.16s | %-4d | %-3d | %-3d | %-7.2f | %s\n",
            node_Student->UID, node_Student->name, node_Student->grades[0], node_Student->grades[1], node_Student->grades[2],
            avg, (avg >= 75.0f) ? "Passed" : "Failed");
    printf("-----------------------------------------------------------------------\n");
    return 1;
}

char* generate_UID(void)
{
    studentCount++;
    int len = snprintf(NULL, 0, "%s-%03d",idPrefix, studentCount);
    char *new_id = (char *)malloc(len + 1);
    sprintf(new_id, "%s-%03d",idPrefix, studentCount);
    return new_id;
}

// === PERSISTENCE FUNCTIONS (NEW, SAVE AND LOAD) ======================================
int persistence_menu()
{
    printf("=== STUDENT DATABASE (START) ===\n");
    printf("[1] New Database\n[2] Save Database\n[3] Load Database\n[4] Exit Program\n\nSelect option: ");
    int option = validinput();
    switch (option) {
    case 1: system("cls"); printf("Enter Class Prefix (max of 9 characters):"); scanf("%s", idPrefix); new_database(); printf("Created New Database\n"); system("pause"); system("cls"); break;
    case 2: system("cls"); save_database(); system("pause"); system("cls"); break;
    case 3: system("cls"); load_database(); system("pause"); system("cls"); break;
    case 4: return -1;
    default: printf("Invalid input.\n"); system("pause"); system("cls"); break;
    }
    return 0;
}

// Resets database
int new_database(void)
{
    StudentRecord *free_walker = head;
    while (free_walker)
    {
        StudentRecord *next = free_walker->next_student;
        free(free_walker);
        free_walker = next;
    }
    head = NULL;
    studentCount = 0;
    return 0;
}

// Save all student records to a file (comma-separated format)
int save_database(void) {
    if (studentCount == 0) { printf("Database is empty. Nothing to save.\n"); return 0; }
    // Open file for writing (creates new or overwrites existing)
    FILE *fp = fopen("database.txt", "w"); 
    if (fp == NULL) { printf("Error: Could not create file.\n"); return 0; }

    // Write each student's data in CSV format
    StudentRecord *walker = head;
    while (walker != NULL)
    {
        fprintf(fp, "%s,%s,%d,%d,%d\n", walker->UID, walker->name, walker->grades[0], walker->grades[1], walker->grades[2]);
        walker = walker->next_student;
    }
    fclose(fp); 
    printf("Database saved successfully to 'database.txt'.\n");
    return 1;
}

// Load student records from the saved file
int load_database(void) {
    FILE *fp = fopen("database.txt", "r"); 
    if (fp == NULL) { printf("No saved database found.\n"); return 0; }

    // Resets the database
    new_database();

    int g1, g2, g3;
    char temp_id[PREFIX_SIZE]; // Temporary scratchpad for the UID
    char name[NAME_SIZE];

    printf("Loading data...\n");

    // FIX 1: Use %[^,] for the ID so it stops exactly at the comma
    while (fscanf(fp, "%[^,],%[^,],%d,%d,%d\n", temp_id, name, &g1, &g2, &g3) == 5) {
        
        // --- APP STATE RESTORATION ---
        // Find the hyphen in the loaded ID (e.g., "CS-005")
        char *hyphen_ptr = strchr(temp_id, '-');
        if (hyphen_ptr != NULL) {
            // 1. Extract the Prefix (Copy everything before the hyphen)
            int prefix_length = hyphen_ptr - temp_id; 
            strncpy(idPrefix, temp_id, prefix_length);
            idPrefix[prefix_length] = '\0'; // Manually close the string
            
            // 2. Extract the Number (Convert everything after the hyphen to an int)
            // hyphen_ptr + 1 skips the '-' and points directly at "005"
            int loaded_number = atoi(hyphen_ptr + 1);
            
            // Only update studentCount if this number is the highest we've seen
            if (loaded_number > studentCount) {
                studentCount = loaded_number;
            }
        }
        // -----------------------------

        StudentRecord *prev_student = locatePrevNode(name, g1, g2, g3);
        StudentRecord *new_student = (StudentRecord *)malloc(sizeof(StudentRecord));

        // FIX 2: Malloc the space for the dynamic string BEFORE copying
        new_student->UID = (char *)malloc(strlen(temp_id) + 1);
        strcpy(new_student->UID, temp_id);

        // Pack the rest of the backpack
        strcpy(new_student->name, name);
        new_student->grades[0] = g1;
        new_student->grades[1] = g2;
        new_student->grades[2] = g3;

        // Link the new node
        if (prev_student == NULL) {
            new_student->next_student = head;
            head = new_student;
        } 
        else {
            new_student->next_student = prev_student->next_student;
            prev_student->next_student = new_student;
        }
    }
    fclose(fp);

    // Provide a cool debug print to prove it synced correctly!
    printf("Database loaded! Next new student will be #%d using prefix '%s'.\n", studentCount + 1, idPrefix);
    return 1;
}


// === MAIN PROGRAM MENU FUNCTIONS ======================================
int menu(void) {
    printf("=== STUDENT DATABASE (Linked Lists) ===\n");
    printf("[1] Modify Database\n[2] View Database\n[3] Locate Student\n[4] Database Options\nSelect option: ");
    return validinput();
}

int modify_menu() {
    int option, g1, g2, g3;
    char name[NAME_SIZE];

    while (1) {
        system("cls");
        printf("=== MODIFY DATABASE ===\n");
        printf("[1] Add Record\n[2] Edit Record\n[3] Remove Record\n[4] Return to main menu\n\nSelect option: ");
        option = validinput();
        switch (option) {
        case 1: 
            system("cls");  
            printf("Input student name: ");
            scanf(" %49[^\n]", name);
            if (is_duplicate_name(name)) {
                printf("Error: Student '%s' already exists in the database.\n", name);
                system("pause"); system("cls"); break; 
            }
            get_valid_grades(&g1, &g2, &g3);
            add_data(name, g1, g2, g3); 
            system("pause"); system("cls"); break;
        case 2: system("cls"); edit_data(); system("pause"); system("cls"); break;
        case 3: system("cls"); delete_data(); system("pause"); system("cls"); break;
        case 4: return 0;
        default: printf("Invalid input.\n"); system("pause"); system("cls"); break;
        }
    }
    return option;
}

// === CORE CRUD (Create, read, update and delete) FUNCTIONS REFACTORED ======================================

// Add a new student to the database, keeping it sorted by average
int add_data(char name[NAME_SIZE], int g1, int g2, int g3) {
    // Find the correct position to insert this student
    StudentRecord *prev_student = locatePrevNode(name, g1, g2, g3);
    StudentRecord *new_student = (StudentRecord *)malloc(sizeof(StudentRecord));

    // Insert the new student's data at the correct position
    new_student->UID = generate_UID();
    strcpy(new_student->name, name);
    new_student->grades[0] = g1;
    new_student->grades[1] = g2;
    new_student->grades[2] = g3;
    
    //link the new node
    if (prev_student == NULL) {
        // EDGE CASE: List is empty, OR this student has the highest grade in the class!
        // They become the brand new head of the list.
        new_student->next_student = head;
        head = new_student;
    } 
    else {
        // NORMAL CASE: Drop them right into the gap
        new_student->next_student = prev_student->next_student; // New guy points to the guy after
        prev_student->next_student = new_student;               // Guy before points to the new guy
    }

    // Log this action to the audit file for record-keeping
    char log_msg[100];
    sprintf(log_msg, "Added student %s", new_student->name);
    log_action(log_msg);
    
    print_Student(new_student);
    return 0;
}

// Display all students in a formatted table showing their grades and average
int view_database(void) {
    if (studentCount == 0) {  // Check if database has any records
        printf("Database is empty, please enter data first.\n");
        return -1;
    }
    printf("--- STUDENT RECORDS ---\n");
    printf("-----------------------------------------------------------------------\n");
    printf("Index | ID      | Name             | Math | Sci | Eng | Average | Remarks\n");
    printf("-----------------------------------------------------------------------\n");

    // Print each student's record with their average and pass/fail status
    StudentRecord *walker = head;
    int index = 1;
    while (walker != NULL) {
        float avg = (walker->grades[0] + walker->grades[1] + walker->grades[2]) / 3.0f;
        printf("%5d | %7.7s | %-16.16s | %-4d | %-3d | %-3d | %-7.2f | %s\n",
               index, walker->UID, walker->name, walker->grades[0], walker->grades[1], walker->grades[2],
               avg, (avg >= 75.0f) ? "Passed" : "Failed");
        walker = walker->next_student;
        index++;
    }
    printf("-----------------------------------------------------------------------\n");
    return 0;
}

// Edit a student's grades (removes from old position and re-sorts)
int edit_data(void) {
    if (studentCount == 0) { printf("Database is empty.\n"); return 0; }
    // Find which student to edit
    StudentRecord *target_Student = find_student();
    if (target_Student == NULL){ printf("Student not found.\n"); return 0; }
    
    printf("Editing record for: %s (ID: %s)\n", target_Student->name, target_Student->UID);
    
    int new_g1, new_g2, new_g3;
    get_valid_grades(&new_g1, &new_g2, &new_g3);
    
    // PHASE 1a: Remove the old record of the student
    if (target_Student == head) head = target_Student->next_student;
    else
    {
        StudentRecord *prev = head;
        while (prev->next_student != target_Student)
        {
            prev = prev->next_student;
        }
        prev->next_student = target_Student->next_student;
    }
    
    // Save the new data
    target_Student->grades[0] = new_g1;
    target_Student->grades[1] = new_g2;
    target_Student->grades[2] = new_g3;

    StudentRecord *new_Position = locatePrevNode(target_Student->name, new_g1, new_g2, new_g3);
    // PHASE 1b: append the records on each side of the node to each other
    if (new_Position == NULL) {
        // EDGE CASE: List is empty, OR this student has the highest grade in the class!
        // They become the brand new head of the list.
        target_Student->next_student = head;
        head = target_Student;
    } 
    else {
        // NORMAL CASE: Drop them right into the gap
        target_Student->next_student = new_Position->next_student; // New guy points to the guy after
        new_Position->next_student = target_Student;               // Guy before points to the new guy
    }

    // Log the modification
    char log_msg[100];
    sprintf(log_msg, "Modified student %s's grades", target_Student->name);
    log_action(log_msg);

    print_Student(target_Student);
    return 1;
}

// Remove a student from the database
int delete_data() {
    if (studentCount == 0) { printf("Database is empty.\n"); return 0; }
    // Find which student to edit
    StudentRecord *target_Student = find_student();
    if (target_Student == NULL){ printf("Student not found.\n"); return 0; }
    
    printf("Deleting %s (ID: %d)...\n", target_Student->name, target_Student->UID);
    
    //Remove the old record of the student
    if (target_Student == head) head = target_Student->next_student;
    else
    {
        StudentRecord *prev = head;
        while (prev->next_student != target_Student)
        {
            prev = prev->next_student;
        }
        prev->next_student = target_Student->next_student;
    }

    // Log the deletion
    char log_msg[100];
    sprintf(log_msg, "Deleted student %s", target_Student->name);
    log_action(log_msg);

    free(target_Student);

    printf("Record deleted successfully.\n");
    view_database();
    return 1;
}
