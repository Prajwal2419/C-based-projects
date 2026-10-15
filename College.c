/*
 * Student Record Management System
 * Concepts: structures, arrays, file handling (binary), functions
 *
 * Compile:  gcc student_records.c -o student_records
 * Run:      ./student_records      (Windows: student_records.exe)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NAME_LEN     50
#define BRANCH_LEN   30
#define SUBJECTS     3
#define FILE_NAME    "students.dat"

/* ---------- Structure ---------- */
typedef struct {
    int   roll;
    char  name[NAME_LEN];
    char  branch[BRANCH_LEN];
    float marks[SUBJECTS];
    float percentage;
} Student;

/* ---------- Array of structures ---------- */
Student students[MAX_STUDENTS];
int count = 0;

/* ---------- Input helpers ---------- */
void readLine(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';   /* remove trailing newline */
}

int readInt(const char *prompt) {
    char line[64];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        if (sscanf(line, "%d", &value) == 1) return value;
        printf("  Invalid input. Please enter a whole number.\n");
    }
}

float readMarks(const char *prompt) {
    char line[64];
    float value;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        if (sscanf(line, "%f", &value) == 1 && value >= 0 && value <= 100)
            return value;
        printf("  Marks must be a number between 0 and 100.\n");
    }
}

/* ---------- Core helpers ---------- */
int findIndex(int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll) return i;
    }
    return -1;
}

void calculatePercentage(Student *s) {
    float total = 0;
    for (int i = 0; i < SUBJECTS; i++) total += s->marks[i];
    s->percentage = total / SUBJECTS;
}

void readMarksInto(Student *s) {
    for (int i = 0; i < SUBJECTS; i++) {
        char prompt[40];
        sprintf(prompt, "Enter marks for subject %d: ", i + 1);
        s->marks[i] = readMarks(prompt);
    }
    calculatePercentage(s);
}

/* ---------- File handling ---------- */
void saveToFile(void) {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) {
        printf("Error: could not open file for writing.\n");
        return;
    }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(Student), count, fp);
    fclose(fp);
}

void loadFromFile(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) return;           /* no file yet: first run */

    int n = 0;
    if (fread(&n, sizeof(int), 1, fp) == 1 && n >= 0 && n <= MAX_STUDENTS) {
        count = (int)fread(students, sizeof(Student), n, fp);
    }
    fclose(fp);
    printf("Loaded %d record(s) from %s\n", count, FILE_NAME);
}

/* ---------- Display ---------- */
void printHeader(void) {
    printf("\n%-6s %-20s %-12s %7s %7s %7s %8s\n",
           "Roll", "Name", "Branch", "Sub1", "Sub2", "Sub3", "Percent");
    printf("------------------------------------------------------------------------\n");
}

void printStudent(const Student *s) {
    printf("%-6d %-20s %-12s %7.1f %7.1f %7.1f %7.2f%%\n",
           s->roll, s->name, s->branch,
           s->marks[0], s->marks[1], s->marks[2], s->percentage);
}

/* ---------- Operations ---------- */
void addStudent(void) {
    if (count >= MAX_STUDENTS) {
        printf("Storage full! Cannot add more students.\n");
        return;
    }

    Student s;
    s.roll = readInt("Enter roll number: ");
    if (findIndex(s.roll) != -1) {
        printf("A student with roll number %d already exists.\n", s.roll);
        return;
    }

    readLine("Enter name: ", s.name, NAME_LEN);
    readLine("Enter branch: ", s.branch, BRANCH_LEN);
    readMarksInto(&s);

    students[count++] = s;
    saveToFile();
    printf("Student added successfully.\n");
}

void displayAll(void) {
    if (count == 0) {
        printf("No records found.\n");
        return;
    }
    printHeader();
    for (int i = 0; i < count; i++) printStudent(&students[i]);
}

void searchStudent(void) {
    int roll = readInt("Enter roll number to search: ");
    int idx = findIndex(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }
    printHeader();
    printStudent(&students[idx]);
}

void updateStudent(void) {
    int roll = readInt("Enter roll number to update: ");
    int idx = findIndex(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    printf("Updating record for %s (press Enter to keep current name/branch)\n",
           students[idx].name);

    char buf[NAME_LEN];
    readLine("Enter new name: ", buf, NAME_LEN);
    if (buf[0] != '\0') strcpy(students[idx].name, buf);

    char branch[BRANCH_LEN];
    readLine("Enter new branch: ", branch, BRANCH_LEN);
    if (branch[0] != '\0') strcpy(students[idx].branch, branch);

    readMarksInto(&students[idx]);

    saveToFile();
    printf("Record updated successfully.\n");
}

void deleteStudent(void) {
    int roll = readInt("Enter roll number to delete: ");
    int idx = findIndex(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    /* shift remaining records one place to the left */
    for (int i = idx; i < count - 1; i++) students[i] = students[i + 1];
    count--;

    saveToFile();
    printf("Record deleted successfully.\n");
}

void showTopper(void) {
    if (count == 0) {
        printf("No records found.\n");
        return;
    }
    int best = 0;
    for (int i = 1; i < count; i++) {
        if (students[i].percentage > students[best].percentage) best = i;
    }
    printf("\nTopper:");
    printHeader();
    printStudent(&students[best]);
}

/* ---------- Main menu ---------- */
int main(void) {
    loadFromFile();

    int choice;
    do {
        printf("\n===== STUDENT RECORD MANAGEMENT =====\n");
        printf("1. Add student\n");
        printf("2. Display all students\n");
        printf("3. Search student by roll number\n");
        printf("4. Update student\n");
        printf("5. Delete student\n");
        printf("6. Show topper\n");
        printf("0. Exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addStudent();    break;
            case 2: displayAll();    break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: showTopper();    break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}
