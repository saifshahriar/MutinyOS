#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_TASKS 100
#define MAX_LEN 256
typedef struct {
    char text[MAX_LEN];
    int completed;
} Task;
Task tasks[MAX_TASKS];
int task_count = 0;
void show_tasks() {
    printf("\n--- Todo List ---\n");
    if (task_count == 0) {
        printf("No tasks yet.\n");
        return;
    }
    for (int i = 0; i < task_count; i++) {
        printf("%d. [%c] %s\n", i + 1, tasks[i].completed ? 'x' : ' ', tasks[i].text);
    }
}
void add_task() {
    if (task_count >= MAX_TASKS) {
        printf("Task list full!\n");
        return;
    }
    printf("Enter task description: ");
    getchar(); // Clear newline from input buffer
    fgets(tasks[task_count].text, MAX_LEN, stdin);
    tasks[task_count].text[strcspn(tasks[task_count].text, "\n")] = 0; // Remove newline
    tasks[task_count].completed = 0;
    task_count++;
    printf("Task added!\n");
}
void complete_task() {
    int num;
    printf("Enter task number to mark as done: ");
    scanf("%d", &num);
    if (num < 1 || num > task_count) {
        printf("Invalid task number.\n");
        return;
    }
    tasks[num - 1].completed = 1;
    printf("Task marked as completed.\n");
}
void delete_task() {
    int num;
    printf("Enter task number to delete: ");
    scanf("%d", &num);
    if (num < 1 || num > task_count) {
        printf("Invalid task number.\n");
        return;
    }
    for (int i = num - 1; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;
    printf("Task deleted.\n");
}
int main() {
    int choice;
    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. View tasks\n");
        printf("2. Add task\n");
        printf("3. Mark task as done\n");
        printf("4. Delete task\n");
        printf("5. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: show_tasks(); break;
            case 2: add_task(); break;
            case 3: complete_task(); break;
            case 4: delete_task(); break;
            case 5: printf("Goodbye!\n"); return 0;
            default: printf("Invalid option.\n"); break;
        }
    }
}
























void save_tasks(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Failed to save tasks.\n");
        return;
    }
    for (int i = 0; i < task_count; i++) {
        fprintf(f, "%d|%s\n", tasks[i].completed, tasks[i].text);
    }
    fclose(f);
}

void load_tasks(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return; // No file yet

    while (!feof(f)) {
        char line[MAX_LEN + 10];
        if (!fgets(line, sizeof(line), f)) break;

        if (strlen(line) < 3) continue;

        line[strcspn(line, "\n")] = 0;

        int done;
        char text[MAX_LEN];
        sscanf(line, "%d|%[^\n]", &done, text);
        strcpy(tasks[task_count].text, text);
        tasks[task_count].completed = done;
        task_count++;
    }
    fclose(f);
}

