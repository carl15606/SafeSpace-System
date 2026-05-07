#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CATEGORIES 5

/* ARRAY: Bullying Categories */
char categories[MAX_CATEGORIES][30] = {
    "Physical",
    "Verbal",
    "Cyberbullying",
    "Social Exclusion",
    "Threats"
};

/* LINKED LIST: Reports */
typedef struct Report {
    int reportID;
    char category[30];
    char location[50];
    char description[200];
    int urgency;
    char status[30];
    struct Report *next;
} Report;

Report *head = NULL;
int nextID = 1;

/* STACK: Undo Deleted Reports */
typedef struct StackNode {
    Report data;
    struct StackNode *next;
} StackNode;

StackNode *undoStack = NULL;

/* QUEUE: Case Processing */
typedef struct QueueNode {
    Report *reportPtr;
    struct QueueNode *next;
} QueueNode;

QueueNode *front = NULL;
QueueNode *rear = NULL;

/* FUNCTION DECLARATIONS */
void submitReport();
void viewReports();
void searchReport();
void deleteReport();
void undoDelete();
void enqueueReport(Report *newReport);
void removeFromQueue(int reportID);
void sortQueue();
void processNextCase();
void showCategories();
void checkReportStatus();
void markReportCompleted();

int adminLogin();
void userMenu();
void adminMenu();

/* SHOW CATEGORIES */
void showCategories() {
    int i;

    printf("\n===== Bullying Categories =====\n");

    for (i = 0; i < MAX_CATEGORIES; i++) {
        printf("%d. %s\n", i + 1, categories[i]);
    }
}

/* SUBMIT REPORT - USER */
void submitReport() {
    Report *newNode = (Report*)malloc(sizeof(Report));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newNode->reportID = nextID++;
    strcpy(newNode->status, "Pending");

    showCategories();

    printf("\nEnter category: ");
    scanf(" %[^\n]", newNode->category);

    printf("Enter location: ");
    scanf(" %[^\n]", newNode->location);

    printf("Describe incident: ");
    scanf(" %[^\n]", newNode->description);

    printf("Enter urgency (1-10): ");
    scanf("%d", &newNode->urgency);

    newNode->next = head;
    head = newNode;

    enqueueReport(newNode);

    printf("\nReport submitted successfully.");
    printf("\nYour Report ID is: %d", newNode->reportID);
    printf("\nStatus: %s\n", newNode->status);
}

/* VIEW REPORTS - ADMIN */
void viewReports() {
    Report *temp = head;

    if (temp == NULL) {
        printf("No reports found.\n");
        return;
    }

    printf("\n===== All Reports =====\n");

    while (temp != NULL) {
        printf("\nReport ID: %d", temp->reportID);
        printf("\nCategory: %s", temp->category);
        printf("\nLocation: %s", temp->location);
        printf("\nDescription: %s", temp->description);
        printf("\nUrgency: %d", temp->urgency);
        printf("\nStatus: %s\n", temp->status);

        temp = temp->next;
    }
}

/* LINEAR SEARCH - ADMIN */
void searchReport() {
    int id;
    Report *temp = head;

    printf("Enter Report ID to search: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->reportID == id) {
            printf("\n===== Report Found =====");
            printf("\nReport ID: %d", temp->reportID);
            printf("\nCategory: %s", temp->category);
            printf("\nLocation: %s", temp->location);
            printf("\nDescription: %s", temp->description);
            printf("\nUrgency: %d", temp->urgency);
            printf("\nStatus: %s\n", temp->status);
            return;
        }

        temp = temp->next;
    }

    printf("Report not found.\n");
}

/* CHECK REPORT STATUS - USER */
void checkReportStatus() {
    int id;
    Report *temp = head;

    printf("Enter your Report ID: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->reportID == id) {
            printf("\n===== Report Status =====");
            printf("\nReport ID: %d", temp->reportID);
            printf("\nCategory: %s", temp->category);
            printf("\nUrgency: %d", temp->urgency);
            printf("\nStatus: %s\n", temp->status);

            if (strcmp(temp->status, "Pending") == 0) {
                printf("Your report is still waiting to be processed.\n");
            } else if (strcmp(temp->status, "Processing") == 0) {
                printf("Your report is currently being handled by the admin.\n");
            } else if (strcmp(temp->status, "Completed") == 0) {
                printf("Your report has been processed and completed.\n");
            }

            return;
        }

        temp = temp->next;
    }

    printf("Report not found.\n");
}

/* DELETE REPORT + STACK - ADMIN */
void deleteReport() {
    int id;
    Report *temp = head;
    Report *prev = NULL;

    printf("Enter Report ID to delete: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->reportID == id) {
            StackNode *deleted = (StackNode*)malloc(sizeof(StackNode));

            if (deleted == NULL) {
                printf("Memory allocation failed.\n");
                return;
            }

            deleted->data = *temp;
            deleted->next = undoStack;
            undoStack = deleted;

            if (prev == NULL) {
                head = temp->next;
            } else {
                prev->next = temp->next;
            }

            removeFromQueue(id);

            free(temp);

            printf("Report deleted and saved to undo stack.\n");
            return;
        }

        prev = temp;
        temp = temp->next;
    }

    printf("Report not found.\n");
}

/* UNDO DELETE - ADMIN */
void undoDelete() {
    if (undoStack == NULL) {
        printf("Nothing to undo.\n");
        return;
    }

    StackNode *top = undoStack;
    undoStack = undoStack->next;

    Report *restored = (Report*)malloc(sizeof(Report));

    if (restored == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    *restored = top->data;

    restored->next = head;
    head = restored;

    if (strcmp(restored->status, "Completed") != 0) {
        enqueueReport(restored);
    }

    free(top);

    printf("Deleted report restored.\n");
}

/* ENQUEUE REPORT - QUEUE */
void enqueueReport(Report *newReport) {
    QueueNode *newNode = (QueueNode*)malloc(sizeof(QueueNode));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newNode->reportPtr = newReport;
    newNode->next = NULL;

    if (rear == NULL) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }

    sortQueue();
}

/* REMOVE FROM QUEUE */
void removeFromQueue(int reportID) {
    QueueNode *temp = front;
    QueueNode *prev = NULL;

    while (temp != NULL) {
        if (temp->reportPtr->reportID == reportID) {
            if (prev == NULL) {
                front = temp->next;
            } else {
                prev->next = temp->next;
            }

            if (temp == rear) {
                rear = prev;
            }

            free(temp);
            return;
        }

        prev = temp;
        temp = temp->next;
    }
}

/* BUBBLE SORT BY URGENCY */
void sortQueue() {
    if (front == NULL) {
        return;
    }

    QueueNode *i;
    QueueNode *j;

    for (i = front; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->reportPtr->urgency < j->reportPtr->urgency) {
                Report *temp = i->reportPtr;
                i->reportPtr = j->reportPtr;
                j->reportPtr = temp;
            }
        }
    }
}

/* PROCESS NEXT CASE - ADMIN */
void processNextCase() {
    if (front == NULL) {
        printf("No pending cases.\n");
        return;
    }

    QueueNode *temp = front;

    strcpy(temp->reportPtr->status, "Processing");

    printf("\n===== Processing Next Case =====");
    printf("\nReport ID: %d", temp->reportPtr->reportID);
    printf("\nCategory: %s", temp->reportPtr->category);
    printf("\nLocation: %s", temp->reportPtr->location);
    printf("\nDescription: %s", temp->reportPtr->description);
    printf("\nUrgency: %d", temp->reportPtr->urgency);
    printf("\nStatus: %s\n", temp->reportPtr->status);

    front = front->next;

    if (front == NULL) {
        rear = NULL;
    }

    free(temp);

    printf("\nCase is now marked as Processing.\n");
    printf("Admin may mark this report as Completed after handling it.\n");
}

/* MARK REPORT COMPLETED - ADMIN */
void markReportCompleted() {
    int id;
    Report *temp = head;

    printf("Enter Report ID to mark as completed: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->reportID == id) {
            strcpy(temp->status, "Completed");
            removeFromQueue(id);

            printf("\nReport ID %d has been marked as Completed.\n", temp->reportID);
            return;
        }

        temp = temp->next;
    }

    printf("Report not found.\n");
}

/* ADMIN LOGIN */
int adminLogin() {
    char username[30];
    char password[30];

    printf("\n===== Admin Login =====\n");

    printf("Enter username: ");
    scanf("%s", username);

    printf("Enter password: ");
    scanf("%s", password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0) {
        printf("\nAdmin login successful.\n");
        return 1;
    } else {
        printf("\nInvalid username or password.\n");
        return 0;
    }
}

/* USER MENU */
void userMenu() {
    int choice;

    do {
        printf("\n===== User Menu =====\n");
        printf("1. Submit Report\n");
        printf("2. Show Categories\n");
        printf("3. Check Report Status\n");
        printf("4. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                submitReport();
                break;

            case 2:
                showCategories();
                break;

            case 3:
                checkReportStatus();
                break;

            case 4:
                printf("Returning to main menu...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while(choice != 4);
}

/* ADMIN MENU */
void adminMenu() {
    int choice;

    do {
        printf("\n===== Admin Menu =====\n");
        printf("1. View Reports\n");
        printf("2. Search Report\n");
        printf("3. Delete Report\n");
        printf("4. Undo Delete\n");
        printf("5. Process Next Case\n");
        printf("6. Mark Report Completed\n");
        printf("7. Show Categories\n");
        printf("8. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                viewReports();
                break;

            case 2:
                searchReport();
                break;

            case 3:
                deleteReport();
                break;

            case 4:
                undoDelete();
                break;

            case 5:
                processNextCase();
                break;

            case 6:
                markReportCompleted();
                break;

            case 7:
                showCategories();
                break;

            case 8:
                printf("Returning to main menu...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while(choice != 8);
}

/* MAIN MENU */
int main() {
    int choice;

    do {
        printf("\n===== SafeSpace System =====\n");
        printf("1. User\n");
        printf("2. Admin\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                userMenu();
                break;

            case 2:
                if (adminLogin()) {
                    adminMenu();
                }
                break;

            case 3:
                printf("Exiting SafeSpace System...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while(choice != 3);

    return 0;
}
