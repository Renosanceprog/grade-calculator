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
#include <dirent.h>
#include <conio.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_INVERT  "\x1b[7m"  // This highlights the selected menu item!

// === Configuration =======================================================
// Set the max number of name and prefix length
#define NAME_SIZE 50
#define PREFIX_SIZE 10
#define MAX_DATABASE 100

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
char current_save_path[NAME_SIZE] = "";

// === Function declarations ===============================================
int interactive_menu(const char *title, const char *options[], int num_options);
int get_string_or_esc(char *buffer, int max_len);
int main();
int validinput(void);
int get_valid_grades(int *g1, int *g2, int *g3);              
int strings_match_ignore_case(const char *s1, const char *s2); 
StudentRecord *find_student(void);                                  
int is_duplicate_name(char *new_name);                         
void log_action(char *message);                                
StudentRecord *locatePrevNode(char name[], int g1, int g2, int g3);
int print_Student(StudentRecord *node_Student);
void print_separator(int total_width);
char *generate_UID(void);
void choose_Database(char *directory);

int persistence_menu();
    int new_database(void);
    int save_database(void);
    int load_database(char *directory);
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
    
    // NOTE: Once we upgrade persistence_menu, it will return '4' for Exit instead of '-1'
    if (persistence_menu() == 4) { 
        printf("%sThank you for using the program.%s\n", COLOR_GREEN, COLOR_RESET); 
        return 0;
    }
    
    while (isRunning) {
        // Display menu and handle user choices
        switch (menu()) {
        case 1:  modify_menu(); break; 
        case 2:  system("cls"); view_database(); system("pause"); break;
        case 3:  system("cls"); print_Student(find_student()); system("pause"); break;
        case 4:  
            if (persistence_menu() == 4) { 
                system("cls"); 
                printf("%sThank you for using the program.%s\n", COLOR_GREEN, COLOR_RESET); 
                isRunning = 0;
            } 
            break;
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
            printf("%s[!] Invalid input. Enter value: %s", COLOR_RED, COLOR_RESET);
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
int get_valid_grades(int *g1, int *g2, int *g3) {
    int valid = 0;
    char buffer[50]; // Temporary string to hold the user's typing
    
    do {
        printf("\n%sEnter 3 grades (Math Sci Eng) [Press ESC to cancel]: %s", COLOR_CYAN, COLOR_RESET);
        
        // Read the entire line directly
        if (get_string_or_esc(buffer, 50) == -1) return -1;

        // Extract the 3 numbers from the string we just captured
        if (sscanf(buffer, "%d %d %d", g1, g2, g3) != 3) {
            printf("%s[!] Invalid input. Please enter 3 numbers separated by spaces.%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }
        
        if (*g1 >= 0 && *g1 <= 100 && *g2 >= 0 && *g2 <= 100 && *g3 >= 0 && *g3 <= 100) {
            valid = 1;  
        } else {
            printf("%s[!] Grades must be between 0 and 100.%s\n", COLOR_RED, COLOR_RESET);
        }
    } while (!valid);
    
    return 1; 
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
    const char *options[] = {
        "Search by Index",
        "Search by ID Number",
        "Search by Name"
    };
    
    // Tell the user they can press ESC at the menu layer
    int choice = interactive_menu("=== LOCATE STUDENT ===\n(Press ESC to cancel)", options, 3);
    
    // If they press ESC in the menu, return a special NULL to signal a cancellation
    if (choice == -1) return NULL; 

    system("cls"); 
    view_database(); 
    StudentRecord *walker = head;
    
    switch (choice) {
    case 1:
        printf("\n%sEnter index (Press ESC to cancel): %s", COLOR_CYAN, COLOR_RESET);
        char idx_buf[20];
        if (get_string_or_esc(idx_buf, 20) == -1) return NULL;
        
        int index = atoi(idx_buf); // Convert the string to an integer
        int walkerIndex = 1;
        
        while (walker != NULL) {
            if (index == walkerIndex) return walker;
            walker = walker->next_student;
            walkerIndex++;
        }
        break;
        
    case 2:
        printf("\n%sEnter ID [%s-###] (Press ESC to cancel): %s", COLOR_CYAN, idPrefix, COLOR_RESET);
        char search_id[PREFIX_SIZE];
        
        if (get_string_or_esc(search_id, PREFIX_SIZE) == -1) return NULL;
        
        while (walker != NULL) {
            if (strings_match_ignore_case(walker->UID, search_id)) return walker;
            walker = walker->next_student;
        }
        break;
        
    case 3:
        printf("\n%sEnter Name (Press ESC to cancel): %s", COLOR_CYAN, COLOR_RESET);
        char search_name[NAME_SIZE];
        
        if (get_string_or_esc(search_name, NAME_SIZE) == -1) return NULL;
        
        while (walker != NULL) {
            if (strings_match_ignore_case(walker->name, search_name)) return walker;
            walker = walker->next_student;
        }
        break;
    }
    
    // If we make it here, the loop finished without finding anyone.
    printf("\n%s[!] No matching student found in the database.%s\n", COLOR_RED, COLOR_RESET);
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
    if (studentCount == 0) { printf("%s[!] Database is empty.%s\n", COLOR_RED, COLOR_RESET); return 0; }
    if (node_Student == NULL) { printf("%s[!] Student not found.%s\n", COLOR_RED, COLOR_RESET); return 0; }

    int id_len = strlen(node_Student->UID) > 2 ? strlen(node_Student->UID) : 2;
    int name_len = strlen(node_Student->name) > 4 ? strlen(node_Student->name) : 4;
    
    int total_width = 41 + id_len + name_len;

    // UI UPGRADE: Cyan headers
    printf("%s", COLOR_CYAN);
    print_separator(total_width);
    printf("%-*s | %-*s | %-4s | %-3s | %-3s | %-7s | %s\n",
           id_len, "ID", name_len, "Name", "Math", "Sci", "Eng", "Average", "Remarks");
    print_separator(total_width);
    printf("%s", COLOR_RESET);
    
    float avg = (node_Student->grades[0] + node_Student->grades[1] + node_Student->grades[2]) / 3.0f;
    
    // UI UPGRADE: Dynamic Green/Red formatting for Pass/Fail
    const char *status_color = (avg >= 75.0f) ? COLOR_GREEN : COLOR_RED;
    const char *status_text  = (avg >= 75.0f) ? "Passed" : "Failed";
    
    printf("%-*s | %-*s | %-4d | %-3d | %-3d | %-7.2f | %s%s%s\n",
            id_len, node_Student->UID, 
            name_len, node_Student->name, 
            node_Student->grades[0], node_Student->grades[1], node_Student->grades[2],
            avg, status_color, status_text, COLOR_RESET);
            
    print_separator(total_width);
    return 1;
}

void print_separator(int total_width) {
    for (int i = 0; i < total_width; i++) {
        putchar('-');
    }
    putchar('\n');
}

char *generate_UID(void)
{
    studentCount++;
    int len = snprintf(NULL, 0, "%s-%03d",idPrefix, studentCount);
    char *new_id = (char *)malloc(len + 1);
    sprintf(new_id, "%s-%03d",idPrefix, studentCount);
    return new_id;
}

void choose_Database(char *directory)
{
    struct dirent *entry;
    const char *folderPath = "./save files/"; 
    
    DIR *dp = opendir(folderPath); 
    char *fileList[MAX_DATABASE];
    int fileCount = 0;

    if (dp == NULL) {
        printf("%s[!] Unable to open directory.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    // 1. Filter and list .txt files
    while ((entry = readdir(dp)) != NULL && fileCount < MAX_DATABASE) {
        if (strstr(entry->d_name, ".txt")) {
            fileList[fileCount] = strdup(entry->d_name);
            fileCount++;
        }
    }
    closedir(dp);
    
    if (fileCount == 0) {
        printf("%s[!] No .txt files found in this directory.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    // 2. UI UPGRADE: Pass the dynamic file array straight into the interactive engine!
    int choice = interactive_menu("=== AVAILABLE SAVE FILES ===", (const char **)fileList, fileCount);
    
    // 3. Construct the Full Path based on what they highlighted
    char *selectedFile = fileList[choice - 1];
    snprintf(directory, NAME_SIZE, "%s%s", folderPath, selectedFile);
    
    // 4. Clean up the strdup memory before leaving
    for (int i = 0; i < fileCount; i++) {
        free(fileList[i]);
    }
    return;
}

// === PERSISTENCE FUNCTIONS (NEW, SAVE AND LOAD) ======================================
int persistence_menu()
{
    const char *options[] = {
        "New Database",
        "Save Database",
        "Load Database",
        "Exit Program"
    };

    int option = interactive_menu("=== STUDENT DATABASE (START) ===", options, 4);
    
    switch (option) {
    case 1: 
        system("cls"); 
        printf("%sEnter Class Prefix (max of 9 characters): %s", COLOR_CYAN, COLOR_RESET); 
        // Prevent buffer overflow by limiting to 9 characters
        scanf(" %9s", idPrefix); 
        new_database(); 
        printf("\n%s[+] Created New Database!%s\n", COLOR_GREEN, COLOR_RESET); 
        system("pause"); 
        break;
    case 2: 
        system("cls"); 
        save_database(); 
        system("pause"); 
        break;
    case 3: 
        system("cls");
        char fullPath[NAME_SIZE] = {0};
        choose_Database(fullPath);
        // Only attempt to load if a file was actually selected (path is not empty)
        if (fullPath[0] != '\0') {
            load_database(fullPath); 
        }
        system("pause"); 
        break;
    case 4: 
        return 4; // Matches the exit check in your main()
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
    // Wipe the active file path (makes the string empty)
    current_save_path[0] = '\0';
    return 0;
}

// Save all student records to a file (comma-separated format)
int save_database(void) {
    if (head == NULL) {
        printf("Database is empty. Nothing to save.\n");
        return 1;
    }

    // 1. The "Save As..." Trigger
    // If the path string is completely empty, it means this is a brand new database
    if (current_save_path[0] == '\0') {
        char temp_filename[50];
        
        printf("Enter a name for your new save file (without .txt): ");
        scanf(" %49[^\n]", temp_filename); // Reads a line with spaces safely
        
        // Construct the full path and lock it into the global tracker
        snprintf(current_save_path, NAME_SIZE, "./save files/%s.txt", temp_filename);
    }

    // 2. The Auto-Save Trigger
    // Whether it was just created above, or loaded an hour ago, open the target file
    FILE *fp = fopen(current_save_path, "w");
    if (fp == NULL) {
        printf("Error: Could not save to %s\n", current_save_path);
        return 1;
    }

    printf("Saving to %s...\n", current_save_path);

    // 3. Serialize the RAM into the Text File
    StudentRecord *walker = head;
    int saved_count = 0;

    while (walker != NULL) {
        // IMPORTANT: We use the exact same format that fscanf expects to read!
        fprintf(fp, "%s,%s,%d,%d,%d\n", walker->UID, walker->name, walker->grades[0], walker->grades[1], walker->grades[2]);
        walker = walker->next_student;
        saved_count++;
    }

    fclose(fp);
    printf("Successfully saved %d records!\n", saved_count);
    return 0;
}

// Load student records from the saved file
int load_database(char *directory) {
    FILE *fp = fopen(directory, "r"); 
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
    _beep(800, 200);
    printf("Database loaded! Next new student will be #%d using prefix '%s'.\n", studentCount + 1, idPrefix);
    // Remember this file so we can auto-update it later
    strncpy(current_save_path, directory, NAME_SIZE);
    return 1;
}


// === MAIN PROGRAM MENU FUNCTIONS ======================================
int menu(void) {
    const char *options[] = {
        "Modify Database",
        "View Database",
        "Locate Student",
        "Database Options"
    };
    return interactive_menu("=== STUDENT DATABASE (Linked Lists) ===", options, 4);
}
int modify_menu() {
    int option, g1, g2, g3;
    char name[NAME_SIZE];

    const char *options[] = {
        "Add Record",
        "Edit Record",
        "Remove Record",
        "Return to main menu"
    };

    while (1) {
        option = interactive_menu("=== MODIFY DATABASE ===", options, 4);
        
        // If they press ESC (-1) or select Return (4), exit the modify menu
        if (option == -1 || option == 4) return 0; 
        
        switch (option) {
        case 1: // --- THE CONTINUOUS ADD LOOP ---
            while (1) {
                system("cls");  
                printf("%s=== ADD RECORD ===%s\n", COLOR_CYAN, COLOR_RESET);
                printf("Input student name (Press ESC to return to menu): ");
                
                // One-line input! Captures string, handles backspace, and listens for ESC
                if (get_string_or_esc(name, 50) == -1) break; 
                
                if (is_duplicate_name(name)) {
                    printf("%s[!] Error: Student '%s' already exists.%s\n", COLOR_RED, name, COLOR_RESET);
                    system("pause"); 
                    continue; 
                }
                
                if (get_valid_grades(&g1, &g2, &g3) == -1) break;
                
                add_data(name, g1, g2, g3); 
                printf("%s[+] Student added successfully!%s\n", COLOR_GREEN, COLOR_RESET);
                system("pause"); 
            }
            break;
            
        case 2: // --- THE CONTINUOUS EDIT LOOP ---
            while (1) {
                system("cls"); 
                printf("%s=== EDIT RECORD ===%s\n", COLOR_CYAN, COLOR_RESET);
                
                int status = edit_data(); 
                if (status == -1) break; // ESC was pressed inside find_student
                if (status == 0) { system("pause"); break; } // DB is empty, back to menu
                
                system("pause"); 
            }
            break;
            
        case 3: // --- THE CONTINUOUS DELETE LOOP ---
            while (1) {
                system("cls"); 
                printf("%s=== REMOVE RECORD ===%s\n", COLOR_CYAN, COLOR_RESET);
                
                int status = delete_data(); // Assuming you have a delete_data function
                if (status == -1) break; // ESC was pressed
                if (status == 0) { system("pause"); break; } // DB is empty
                
                system("pause"); 
            }
            break;
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
    if (studentCount == 0 || head == NULL) { 
        printf("%s[!] Database is empty, please enter data first.%s\n", COLOR_RED, COLOR_RESET);
        return -1;
    }

    // 1. MEASURE PASS: Find the widest ID and Name in the database
    int max_id_len = 2;   
    int max_name_len = 4; 
    
    StudentRecord *measurer = head;
    while (measurer != NULL) {
        int current_id_len = strlen(measurer->UID);
        int current_name_len = strlen(measurer->name);
        
        if (current_id_len > max_id_len) max_id_len = current_id_len;
        if (current_name_len > max_name_len) max_name_len = current_name_len;
        
        measurer = measurer->next_student;
    }

    // 2. CALCULATE TABLE WIDTH
    int total_width = 49 + max_id_len + max_name_len;

    // 3. RENDER PASS: Draw the dynamic UI with Colors
    printf("%s=== STUDENT RECORDS ===\n", COLOR_CYAN);
    print_separator(total_width);
    printf("%5s | %-*s | %-*s | %-4s | %-3s | %-3s | %-7s | %s\n", 
           "Index", max_id_len, "ID", max_name_len, "Name", "Math", "Sci", "Eng", "Average", "Remarks");
    print_separator(total_width);
    printf("%s", COLOR_RESET); // Turn off cyan for the data rows

    StudentRecord *walker = head;
    int index = 1;
    while (walker != NULL) {
        float avg = (walker->grades[0] + walker->grades[1] + walker->grades[2]) / 3.0f;
        
        // UI UPGRADE: Dynamic Green/Red formatting for Pass/Fail
        const char *status_color = (avg >= 75.0f) ? COLOR_GREEN : COLOR_RED;
        const char *status_text  = (avg >= 75.0f) ? "Passed" : "Failed";
        
        printf("%5d | %-*s | %-*s | %-4d | %-3d | %-3d | %-7.2f | %s%s%s\n",
               index, 
               max_id_len, walker->UID, 
               max_name_len, walker->name, 
               walker->grades[0], walker->grades[1], walker->grades[2],
               avg, status_color, status_text, COLOR_RESET);
               
        walker = walker->next_student;
        index++;
    }
    
    // Cyan bottom border
    printf("%s", COLOR_CYAN);
    print_separator(total_width);
    printf("%s", COLOR_RESET);
    
    return 0;
}

// Edit a student's grades (removes from old position and re-sorts)
int edit_data(void) {
    if (studentCount == 0) { 
        printf("%s[!] Database is empty.%s\n", COLOR_RED, COLOR_RESET); 
        return 0; 
    }
    
    // 1. Locate the student
    StudentRecord *target_Student = find_student();

    // 2. Handle Cancellation (ESC)
    if (target_Student == NULL) { 
        return -1; // Tell modify_menu we bailed
    }
    
    // 3. UI Prompt for Editing
    printf("\n%sEditing record for: %s (ID: %s)%s\n", COLOR_CYAN, target_Student->name, target_Student->UID, COLOR_RESET);
    
    // 4. Get New Grades
    int new_g1, new_g2, new_g3;
    if (get_valid_grades(&new_g1, &new_g2, &new_g3) == -1) {
        return -1; // Bubble the cancel signal back up
    }

    // PHASE 1a: Unlink the old record from the list
    if (target_Student == head) {
        head = target_Student->next_student;
    } else {
        StudentRecord *prev = head;
        while (prev != NULL && prev->next_student != target_Student) {
            prev = prev->next_student;
        }
        if (prev != NULL) {
            prev->next_student = target_Student->next_student;
        }
    }
    
    // 5. Update the internal data
    target_Student->grades[0] = new_g1;
    target_Student->grades[1] = new_g2;
    target_Student->grades[2] = new_g3;

    // 6. Re-insert the student in the new sorted position
    StudentRecord *new_Position = locatePrevNode(target_Student->name, new_g1, new_g2, new_g3);
    
    if (new_Position == NULL) {
        target_Student->next_student = head;
        head = target_Student;
    } 
    else {
        target_Student->next_student = new_Position->next_student; 
        new_Position->next_student = target_Student;               
    }

    // 7. Success Output
    char log_msg[100];
    sprintf(log_msg, "Modified student %s's grades", target_Student->name);
    log_action(log_msg);

    printf("\n%s[+] Record updated successfully!%s\n", COLOR_GREEN, COLOR_RESET);
    print_Student(target_Student);
    
    return 1; // NOW we return 1 at the very end!
}

// Remove a student from the database
int delete_data() {
    if (studentCount == 0) { printf("Database is empty.\n"); return 0; }
    // Find which student to edit
    StudentRecord *target_Student = find_student();
    if (target_Student == NULL){ printf("Student not found.\n"); return 0; }
    
    printf("Deleting %s (ID: %s)...\n", target_Student->name, target_Student->UID);
    
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

// The Universal Interactive Menu Engine
int interactive_menu(const char *title, const char *options[], int num_options) 
{
    int selected = 0; // Starts with the first option highlighted
    int key;

    while (1) {
        system("cls");
        printf("%s%s%s\n\n", COLOR_CYAN, title, COLOR_RESET);

        // Draw the menu options
        for (int i = 0; i < num_options; i++) {
            if (i == selected) {
                // Highlight the selected option
                printf("%s  > %s %s\n", COLOR_INVERT, options[i], COLOR_RESET);
            } else {
                // Normal option
                printf("    %s \n", options[i]);
            }
        }

        // Wait for an instant keystroke (No 'Enter' required)
        key = _getch();

        // ADD THIS: Trap the ESC key
        if (key == 27) {
            return -1; // -1 becomes our universal "Cancel" signal
        }

        // Arrow keys send TWO codes. The first is usually 224 (0xE0) or 0.
        if (key == 224 || key == 0) {
            key = _getch(); // Catch the second code
            if (key == 72) { // Up Arrow
                selected--;
                if (selected < 0) selected = num_options - 1; // Wrap around to bottom
            } 
            else if (key == 80) { // Down Arrow
                selected++;
                if (selected >= num_options) selected = 0; // Wrap around to top
            }
        } 
        else if (key == '\r') { // Enter key
            return selected + 1; // Return 1-based index (1, 2, 3...)
        }
    }
}

// The Ultimate Input Function: Handles typing, backspace, and ESC natively!
int get_string_or_esc(char *buffer, int max_len) {
    int i = 0;
    while (1) {
        int ch = _getch();
        
        // 1. Handle ESC Key
        if (ch == 27) {
            return -1; 
        }
        // 2. Handle Enter Key (\r or \n)
        else if (ch == '\r' || ch == '\n') {
            if (i > 0) { // Only accept if they actually typed something!
                buffer[i] = '\0'; // Seal the string
                printf("\n");     // Move to next line
                return 1;
            }
        }
        // 3. Handle Backspace (ASCII 8)
        else if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b"); // Visually erase the character on screen
            }
        }
        // 4. Handle regular typing (Letters, numbers, spaces)
        else if (i < max_len - 1 && ch >= 32 && ch <= 126) {
            buffer[i++] = ch;
            putchar(ch); // Echo it to the screen
        }
    }
}
