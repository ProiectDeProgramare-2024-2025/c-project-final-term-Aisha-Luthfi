#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define NOTES 50
#define NOTES_LENGTH 500

typedef struct {
    int day, month, year, hour, minute;
    char note[NOTES_LENGTH];
} Note;

Note notes[NOTES];
int noteCount = 0;

void saveNotesToFile() {
    FILE *file = fopen("notes.txt", "w");  
    if (file == NULL) {
        printf(COLOR_RED "Error saving notes!\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    for (int i = 0; i < noteCount; i++) {
        fprintf(file, "%02d %02d %04d %02d %02d|%s", 
                notes[i].day, notes[i].month, notes[i].year, 
                notes[i].hour, notes[i].minute, notes[i].note);
    }
    
    fclose(file);
}

void loadNotesFromFile() {
    FILE *file = fopen("notes.txt", "r");  
    if (file == NULL) {
        printf(COLOR_YELLOW "No existing notes found. Starting fresh...\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    noteCount = 0;
    while (fscanf(file, "%d %d %d %d %d|", 
                  &notes[noteCount].day, &notes[noteCount].month, &notes[noteCount].year, 
                  &notes[noteCount].hour, &notes[noteCount].minute) == 5) {
        fgets(notes[noteCount].note, NOTES_LENGTH, file);
        noteCount++;
    }
    
    fclose(file);
}

void clearConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int validateDate(int day, int month, int year) {
    if (year < 0000 || year > 2030) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return (day <= 30);
    } else if (month == 2) {
        int leap = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
        return (day <= (28 + leap));
    }
    return 1;
}

int validateTime(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
}

void addNote() {
    clearConsole();
    if (noteCount >= NOTES) {
        printf(COLOR_RED "Note storage full!\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    printf(COLOR_CYAN "Enter date and time (DD MM YYYY HH MM): " COLOR_RESET);
    scanf("%d %d %d %d %d", &notes[noteCount].day, &notes[noteCount].month, 
          &notes[noteCount].year, &notes[noteCount].hour, &notes[noteCount].minute);
    
    if (!validateDate(notes[noteCount].day, notes[noteCount].month, notes[noteCount].year) ||
        !validateTime(notes[noteCount].hour, notes[noteCount].minute)) {
        printf(COLOR_RED "Invalid date or time! Note not added.\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        getchar(); // Second getchar() to consume the newline
        return;
    }
    
    getchar();
    printf(COLOR_CYAN "Enter note of the day: " COLOR_RESET);
    fgets(notes[noteCount].note, NOTES_LENGTH, stdin);
    noteCount++;
    printf(COLOR_GREEN "Note added!\n" COLOR_RESET);
    saveNotesToFile();
    printf("Press Enter to continue...");
    getchar();
}

void viewNotes() {
    clearConsole();
    if (noteCount == 0) {
        printf(COLOR_YELLOW "No notes available.\n" COLOR_RESET);
    } else {
        printf(COLOR_MAGENTA "All Notes:\n" COLOR_RESET);
        for (int i = 0; i < noteCount; i++) {
            printf(COLOR_BLUE "%02d-%02d-%04d %02d:%02d" COLOR_RESET " - %s", 
                   notes[i].day, notes[i].month, notes[i].year, 
                   notes[i].hour, notes[i].minute, notes[i].note);
        }
    }
    printf("\nPress Enter to continue...");
    getchar();
}

void viewNotesByDate() {
    clearConsole();
    int day, month, year;
    printf(COLOR_CYAN "Enter the date of the note you want to view (DD MM YYYY): " COLOR_RESET);
    scanf("%d %d %d", &day, &month, &year);
    getchar();
    
    if (!validateDate(day, month, year)) {
        printf(COLOR_RED "Invalid date!\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    int found = 0;
    printf(COLOR_MAGENTA "Notes for %02d-%02d-%04d:\n" COLOR_RESET, day, month, year);
    for (int i = 0; i < noteCount; i++) {
        if (notes[i].day == day && notes[i].month == month && notes[i].year == year) {
            printf(COLOR_BLUE "%02d:%02d" COLOR_RESET " - %s", notes[i].hour, notes[i].minute, notes[i].note);
            found = 1;
        }
    }
    if (!found) 
        printf(COLOR_YELLOW "No notes found for this date.\n" COLOR_RESET);

    printf("\nPress Enter to continue...");
    getchar();
}

void editNote() {
    clearConsole();
    int day, month, year;
    printf(COLOR_CYAN "Enter date of the note you want to edit (DD MM YYYY): " COLOR_RESET);
    scanf("%d %d %d", &day, &month, &year);
    getchar();
    
    if (!validateDate(day, month, year)) {
        printf(COLOR_RED "Invalid date!\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    for (int i = 0; i < noteCount; i++) {
        if (notes[i].day == day && notes[i].month == month && notes[i].year == year) {
            printf(COLOR_MAGENTA "Editing: " COLOR_BLUE "%02d:%02d" COLOR_RESET " - %s", 
                  notes[i].hour, notes[i].minute, notes[i].note);
            printf(COLOR_CYAN "Do you want to edit the date and time? (y/n): " COLOR_RESET);
            char choice;
            scanf("%c", &choice);
            getchar();
            if (choice == 'y' || choice == 'Y') {
                printf(COLOR_CYAN "Enter new date and time (DD MM YYYY HH MM): " COLOR_RESET);
                scanf("%d %d %d %d %d", &notes[i].day, &notes[i].month, &notes[i].year, 
                      &notes[i].hour, &notes[i].minute);
                getchar();
                
                if (!validateDate(notes[i].day, notes[i].month, notes[i].year) ||
                    !validateTime(notes[i].hour, notes[i].minute)) {
                    printf(COLOR_RED "Invalid date or time! Changes not saved.\n" COLOR_RESET);
                    printf("Press Enter to continue...");
                    getchar();
                    return;
                }
            }
        
            printf(COLOR_CYAN "Enter new note: " COLOR_RESET);
            fgets(notes[i].note, NOTES_LENGTH, stdin);
            printf(COLOR_GREEN "Note updated successfully!\n" COLOR_RESET);
            saveNotesToFile();
            printf("Press Enter to continue...");
            getchar();
            return;
        }
    }
    printf(COLOR_YELLOW "No matching note found.\n" COLOR_RESET);
    printf("Press Enter to continue...");
    getchar();
}

void deleteNote() {
    clearConsole();
    int day, month, year;
    printf(COLOR_CYAN "Enter date of the note you want to delete (DD MM YYYY): " COLOR_RESET);
    scanf("%d %d %d", &day, &month, &year);
    getchar();
    
    if (!validateDate(day, month, year)) {
        printf(COLOR_RED "Invalid date!\n" COLOR_RESET);
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    for (int i = 0; i < noteCount; i++) {
        if (notes[i].day == day && notes[i].month == month && notes[i].year == year) {
            for (int j = i; j < noteCount - 1; j++) {
                notes[j] = notes[j + 1];
            }
            noteCount--;
            printf(COLOR_GREEN "Note deleted!\n" COLOR_RESET);
            saveNotesToFile();
            printf("Press Enter to continue...");
            getchar();
            return;
        }
    }
    printf(COLOR_YELLOW "No matching note found.\n" COLOR_RESET);
    printf("Press Enter to continue...");
    getchar();
}

int main() {
    loadNotesFromFile();
    int choice;
    while (1) {
        clearConsole();
        printf(COLOR_MAGENTA "\nNotes Application\n" COLOR_RESET);
        printf(COLOR_CYAN "1. Add Note\n2. View Notes\n3. View Notes By Date\n4. Edit Note\n5. Delete Note\n6. Exit\n" COLOR_RESET);
        printf(COLOR_CYAN "Enter your choice: " COLOR_RESET);
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: addNote(); break;
            case 2: viewNotes(); break;
            case 3: viewNotesByDate(); break;
            case 4: editNote(); break;
            case 5: deleteNote(); break;
            case 6: 
                clearConsole();
                exit(0);
            default: 
                printf(COLOR_RED "Invalid choice! Try again.\n" COLOR_RESET);
                printf("Press Enter to continue...");
                getchar();
        }
    }
    return 0;
}