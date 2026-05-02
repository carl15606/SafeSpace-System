#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CATEGORIES 5

/* ==========================
   ARRAY: Bullying Categories
   ========================== */
char categories[MAX_CATEGORIES][30] = {
    "Physical",
    "Verbal",
    "Cyberbullying",
    "Social Exclusion",
    "Threats"
};

/* ==========================
   LINKED LIST: Reports
   ========================== */
typedef struct Report {
    int reportID;
    char category[30];
    char location[50];
    char description[200];
    int urgency;
    struct Report *next;
} Report;

Report *head = NULL;
int nextID = 1;

/* ==========================
   STACK: Undo Deleted Reports
   ========================== */
typedef struct StackNode {
    Report data;
    struct StackNode *next;
} StackNode;

StackNode *undoStack = NULL;

/* ==========================
   QUEUE: Case Processing
   ========================== */
typedef struct QueueNode {
    Report *reportPtr;
    struct QueueNode *next;
} QueueNode;

QueueNode *front = NULL;
QueueNode *rear = NULL;

/* ==========================
   FUNCTION DECLARATIONS
   ========================== */
void submitReport();
void viewReports();
void searchReport();
void deleteReport();
void undoDelete();
void enqueueReport(Report *newReport);
void sortQueue();
void processNextCase();
void showCategories();

/* ==========================
   SUBMIT REPORT
   ========================== */
void submitReport() {
    Report *newNode = (Report*)malloc(sizeof(Report));

    newNode->reportID = nextID++;

    printf("Enter category: ");
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

    printf("\nReport submitted successfully.\n");
}

/* ==========================
   VIEW REPORTS
   ========================== */
void viewReports() {
    Report *temp = head;

    if (temp == NULL) {
        printf("No reports found.\n");
        return;
    }

    while (temp != NULL) {
        printf("\nReport ID: %d", temp->reportID);
        printf("\nCategory: %s", temp->category);
        printf("\nLocation: %s", temp->location);
        printf("\nDescription: %s", temp->description);
        printf("\nUrgency: %d\n", temp->urgency);

        temp = temp->next;
    }
}

/* ==========================
   LINEAR SEARCH
   ========================== */
void searchReport() {
    int id;
    Report *temp = head;

    printf("Enter Report ID to search: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->reportID == id) {
            printf("\nReport Found:");
            printf("\nCategory: %s", temp->category);
            printf("\nLocation: %s", temp->location);
            printf("\nDescription: %s", temp->description);
            printf("\nUrgency: %d\n", temp->urgency);
            return;
        }
        temp = temp->next;
    }

    printf("Report not found.\n");
}

/* ==========================
   DELETE REPORT + STACK
   ========================== */
void deleteReport() {
    int id;
    Report *temp = head, *prev = NULL;

    printf("Enter Report ID to delete: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->reportID == id) {

            StackNode *deleted = (StackNode*)malloc(sizeof(StackNode));
            deleted->data = *temp;
            deleted->next = undoStack;
            undoStack = deleted;

            if (prev == NULL)
                head = temp->next;
            else
                prev->next = temp->next;

            free(temp);
            printf("Report deleted.\n");
            return;
        }

        prev = temp;
        temp = temp->next;
    }

    printf("Report not found.\n");
}

/* ==========================
   UNDO DELETE
   ========================== */
void undoDelete() {
    if (undoStack == NULL) {
        printf("Nothing to undo.\n");
        return;
    }

    StackNode *top = undoStack;
    undoStack = undoStack->next;

    Report *restored = (Report*)malloc(sizeof(Report));
    *restored = top->data;

    restored->next = head;
    head = restored;

    enqueueReport(restored);

    free(top);

    printf("Deleted report restored.\n");
}

/* ==========================
   QUEUE OPERATIONS
   ========================== */
void enqueueReport(Report *newReport) {
    QueueNode *newNode = (QueueNode*)malloc(sizeof(QueueNode));
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

/* ==========================
   BUBBLE SORT BY URGENCY
   ========================== */
void sortQueue() {
    if (front == NULL) return;

    QueueNode *i, *j;
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

/* ==========================
   PROCESS NEXT CASE
   ========================== */
void processNextCase() {
    if (front == NULL) {
        printf("No pending cases.\n");
        return;
    }

    QueueNode *temp = front;

    printf("\nProcessing Report ID: %d", temp->reportPtr->reportID);
    printf("\nCategory: %s", temp->reportPtr->category);
    printf("\nUrgency: %d\n", temp->reportPtr->urgency);

    front = front->next;
    if (front == NULL)
        rear = NULL;

    free(temp);
}

/* ==========================
   SHOW CATEGORIES
   ========================== */
void showCategories() {
    int i;
    printf("\nBullying Categories:\n");
    for (i = 0; i < MAX_CATEGORIES; i++) {
        printf("%d. %s\n", i + 1, categories[i]);
    }
}

/* ==========================
   MAIN MENU
   ========================== */
int main() {
    int choice;

    do {
        printf("\n===== SafeSpace =====\n");
        printf("1. Submit Report\n");
        printf("2. View Reports\n");
        printf("3. Search Report\n");
        printf("4. Delete Report\n");
        printf("5. Undo Delete\n");
        printf("6. Process Next Case\n");
        printf("7. Show Categories\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: submitReport(); break;
            case 2: viewReports(); break;
            case 3: searchReport(); break;
            case 4: deleteReport(); break;
            case 5: undoDelete(); break;
            case 6: processNextCase(); break;
            case 7: showCategories(); break;
            case 8: printf("Exiting SafeSpace...\n"); break;
            default: printf("Invalid choice.\n");
        }

    } while(choice != 8);

    return 0;
}
