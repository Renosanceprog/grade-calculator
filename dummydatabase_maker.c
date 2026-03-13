#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <ctype.h>
#include <time.h>

// Increased limits so nothing overflows!
#define NAME_SIZE 16
#define PREFIX_SIZE 50 

void generate_UID(char *id);
char generate_vowel();
char generate_consonant();
void generate_name(char *name);
int rng(int min, int max);
void generate_grade(int *grades);

char idPrefix[PREFIX_SIZE];
unsigned long long studentCount = 0;
char current_save_path[150];
char name[NAME_SIZE];
char id[100]; // Massive bucket for the ID string
int Sgrades[3];

int main()
{
    srand(time(NULL));
    
    // 1. Safe input gathering
    printf("Enter database prefix: ");
    scanf(" %49s", idPrefix); // Prevents overflow on the prefix
    
    int entries;
    printf("Enter # of entries: ");
    scanf("%d", &entries);
    
    char temp_filename[50];
    printf("Enter a name for your new save file (without .txt): ");
    scanf(" %49[^\n]", temp_filename); 
    
    sprintf(current_save_path, "./save files/%s.txt", temp_filename);

    // 2. OPEN THE FILE ONCE BEFORE THE LOOP
    FILE *fp = fopen(current_save_path, "w"); // "w" to start fresh, or "a" to append
    if (fp == NULL) {
        printf("Error: Could not create file at %s\n", current_save_path);
        return 1;
    }

    printf("Generating %d records. Please wait...\n", entries);

    // 3. THE BLAZING FAST LOOP
    for (unsigned long long i = 0; i < entries; i++)
    {
        generate_name(name);
        generate_UID(id);
        generate_grade(Sgrades);
        float ave = (Sgrades[0]+Sgrades[1]+Sgrades[2])/3.0;
        // Write directly to the open file
        fprintf(fp, "%s,%s,%d,%d,%d\n", id, name, Sgrades[0], Sgrades[1], Sgrades[2]);
        printf("%s | %s | %2d %2d %2d | %.2f | %s\n", id,name,Sgrades[0],Sgrades[1],Sgrades[2], ave, (ave > 75) ? "Passed" : "Failed");
    }
    
    // 4. CLOSE THE FILE ONCE AT THE END
    fclose(fp);

    printf("Successfully saved %llu records!\n", studentCount);
    printf("Location: %s\n", current_save_path);
    return 0;
}

int rng(int min, int max){
    return (rand() % (max - min + 1) + min);
}

void generate_UID(char *id)
{
    studentCount++;
    sprintf(id, "%s-%03llu", idPrefix, studentCount);
}

char generate_vowel(){
    const char *cons = "AEIOU";
    return cons[rng(0, 4)];
}

char generate_consonant() {
    const char *cons = "BCDFGHJKLMNPQRSTVWXYZ";
    return cons[rng(0, 20)]; 
}

void generate_name(char *name){
    name[0] = generate_consonant(); name[1] = generate_vowel(); name[2] = generate_consonant(); name[3] = generate_consonant();
    name[4] = ' ';

    if (rng(1,2) == 1) name[5] = generate_vowel(); else name[5] = generate_consonant();
    name[6] = '.';
    name[7] = ' ';

    for (int i = 8; i <= 14; i++)
    {
        if (i % 2 != 0) name[i] = generate_vowel();
        else name[i] = generate_consonant();
    }
    name[15] = '\0';
    for (int i = 0; i < NAME_SIZE; i++)
    {
        switch (i)
        {
        case 0: case 5: case 8: continue;
        default: name[i] = tolower(name[i]);
        }
    }
    return;
}

void generate_grade(int *grades){
    // Removed the sizeof trick here because when passing an array to a function, 
    // it decays to a pointer, so sizeof() gets confused. Just hardcode the 3!
    for (int i = 0; i < 3; i++)
    {
        grades[i] = rng(60,99);
    }
}