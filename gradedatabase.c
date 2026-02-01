/*
 * GRADE DATABASE
 *
 * Simple student records manager for a small class.
 * - Store student names and three grades each (Math, Sci, Eng).
 * - Add students, view all records, and delete by ID.
 */

// Standard libraries
#include <stdio.h>   // input / output functions
#include <string.h>  // string handling (strcpy)
#include <unistd.h>  // used for system() on some platforms (optional)

// === Configuration =======================================================
// Maximum number of students the database can hold and maximum name length
#define CLASS_SIZE 5
#define NAME_SIZE 50

// === Global storage (simple, easy to inspect) ============================
// Students' names: student[i] holds the name for record i
char student[CLASS_SIZE][NAME_SIZE];

// grades[i][0..2] holds the three grades (Math, Sci, Eng) for record i
int grades[CLASS_SIZE][3];

// Index of the last used record. -1 means "no records yet" (empty database)
int last = -1;

// id_array[i] holds the displayed ID for record i (keeps IDs easy to manage)
int id_array[CLASS_SIZE];

// === Function declarations (what each part does) ==========================
int menu(void);
int add_data(char name[NAME_SIZE], int g1, int g2, int g3);
int view_data(void);
int delete_data(int id_number);

/*
 * main: program loop
 * - Shows a menu, performs actions until the user chooses to exit
 * - Input is minimally validated to avoid crashes from bad input
 */
int main(void)
{
    int isRunning = 1;
    int g1, g2, g3, id_number;
    char name[NAME_SIZE];

    while (isRunning) {
        switch (menu()) {

        case 1:  /* Add student */
            system("cls");  // clear screen for neatness on Windows

            printf("Input student name: ");
            /* Read up to 49 chars and stop at newline to avoid overflow */
            scanf(" %49[^\n]", name);

            /* Read three grades, re-prompt on bad input or out-of-range values */
            {
                int valid = 0;    /* flag: grades are valid */
                int ret2;

                do {
                    printf("Enter 3 grades: ");

                    /* scanf returns the number of successful conversions */
                    ret2 = scanf("%d %d %d", &g1, &g2, &g3);

                    if (ret2 != 3) {
                        printf("Invalid input. Please enter three integer grades.\n");
                        int c; while ((c = getchar()) != '\n' && c != EOF); /* clear bad input */
                        continue; /* try again */
                    }

                    /* check grades are in 0..100 */
                    if (g1 >= 0 && g1 <= 100 && g2 >= 0 && g2 <= 100 && g3 >= 0 && g3 <= 100) {
                        valid = 1;
                    } else {
                        printf("Invalid grades. Please try again.\n");
                        int c; while ((c = getchar()) != '\n' && c != EOF); /* clear rest */
                    }
                } while (!valid);
            }

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

        case 3:  /* Delete a record by ID */
            system("cls");
            view_data();
            printf("input ID # to be deleted: ");

            /* Validate delete input so letters don't crash the program */
            if (scanf("%d", &id_number) != 1) {
                printf("Invalid ID input.\n");
                int c; while ((c = getchar()) != '\n' && c != EOF); /* clear rest */
                system("pause");
                system("cls");
                break;
            }

            delete_data(id_number);
            view_data();
            system("pause");
            system("cls");
            break;

        case 4:  /* Exit */
            printf("Thank you for using the program.\n");
            isRunning = 0;
            break;

        default: /* invalid menu choice */
            printf("Invalid input, please try again\n");
            system("pause");
            system("cls");
            break;
        }
    }

    return 0;
}

/*
 * menu: show choices and read user selection
 * - returns choice 1..4, or -1 on invalid input
 */
int menu(void)
{
    int option;
    int ret;

    printf("=== STUDENT DATABASE ===\n");
    printf("[1] Add Student\n[2] View Database\n[3] Remove Student\n[4] Exit\n\nSelect option: ");

    ret = scanf("%d", &option);

    /* If scanf fails (user typed a letter), clear the input line and return -1 */
    if (ret != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }

    return option;
}

/*
 * add_data: insert a new student record at the end
 * - name: student's name (string)
 * - g1,g2,g3: three integer grades (0..100)
 *
 * Returns 0 on success, prints message and returns 0 on failure.
 */
int add_data(char name[NAME_SIZE], int g1, int g2, int g3)
{
    if (last == CLASS_SIZE - 1) {
        printf("Database is full, please delete existing data first.\n");
        return 0;
    }

    last++;                         /* move to next free slot */
    id_array[last] = last + 1;      /* simple user-friendly ID */
    strcpy(student[last], name);    /* copy name into storage */
    grades[last][0] = g1;           /* Math */
    grades[last][1] = g2;           /* Sci  */
    grades[last][2] = g3;           /* Eng  */

    return 0;
}

/*
 * view_data: print all stored student records in a table
 * - prints ID, Name, each grade, average, and remark (Passed/Failed)
 */
int view_data(void)
{
    if (last == -1) {
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
               average, (average >= 75.0f) ? "Passed" : "Failed");
        printf("-----------------------------------------------------------------------\n");
    }

    return 0;
}

/*
 * delete_data: remove a record by its ID and shift following records down
 * - id_number: the displayed ID (1-based) to remove
 *
 * Behavior:
 * - If database is empty, a message is printed.
 * - If ID is not found, a message is printed and nothing changes.
 * - After deletion, IDs are reassigned to stay consecutive (1,2,...).
 */
int delete_data(int id_number)
{
    if (last == -1) {
        printf("Database is empty, please enter data first.\n");
        return 0;
    }

    /* find the record matching the ID */
    int todelete = -1;
    for (int i = 0; i <= last; i++) {
        if (id_array[i] == id_number) {
            todelete = i;
            break;
        }
    }

    if (todelete == -1) {
        printf("ID %d not found.\n", id_number);
        return 0;
    }

    /* shift entries down to fill the removed slot */
    for (int i = todelete; i < last; i++) {
        id_array[i] = id_array[i + 1];
        strcpy(student[i], student[i + 1]);
        grades[i][0] = grades[i + 1][0];
        grades[i][1] = grades[i + 1][1];
        grades[i][2] = grades[i + 1][2];
    }

    last--; /* one fewer record now */

    /* reassign IDs so they stay simple and consecutive */
    for (int i = 0; i <= last; i++) {
        id_array[i] = i + 1;
    }

    printf("ID %d was deleted from the database.\n", id_number);
    return 0;
}
