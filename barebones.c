#include <stdio.h>
#include <string.h>

#define CLASS_SIZE 5
#define NAME_SIZE 50

// Global Data
char names[CLASS_SIZE][NAME_SIZE];
int grades[CLASS_SIZE][3]; // Math, Sci, Eng
int count = 0;

// Functions
void add_student();
void view_students();
void delete_student();

int main(void) {
    int choice;
    
    while (1) {
        printf("\n1. Add  2. View  3. Delete  4. Exit\nSelect: ");
        scanf(" %d", &choice);

        switch (choice) {
            case 1: add_student();    break;
            case 2: view_students();  break;
            case 3: delete_student(); break;
            case 4: return 0;
        }
    }
}

void add_student() {
    if (count >= CLASS_SIZE) return;

    printf("Enter Name: ");
    scanf(" %s", names[count]); // Assumes 1 word name for simplicity

    printf("Enter 3 Grades: ");
    scanf("%d %d %d", &grades[count][0], &grades[count][1], &grades[count][2]);

    count++;
}

void view_students() {
    printf("\nID | Name           | Avg\n");
    for (int i = 0; i < count; i++) {
        float avg = (grades[i][0] + grades[i][1] + grades[i][2]) / 3.0f;
        printf("%d  | %-14s | %.2f\n", i + 1, names[i], avg);
    }
}

void delete_student() {
    int id;
    printf("Enter ID to delete: ");
    scanf("%d", &id);

    int index = id - 1; // Convert ID (1-based) to Index (0-based)

    // Shift data left to fill the gap
    for (int i = index; i < count - 1; i++) {
        strcpy(names[i], names[i+1]);
        grades[i][0] = grades[i+1][0];
        grades[i][1] = grades[i+1][1];
        grades[i][2] = grades[i+1][2];
    }
    
    count--;
}