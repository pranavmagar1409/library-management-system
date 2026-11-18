/* Library Management System - C version (Linked List + Queue + Array) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_USERS 100
#define MAX_ISSUE 3

typedef struct QNode { int uid; struct QNode *next; } QNode;
typedef struct { QNode *front, *rear; } Queue;
typedef struct Book { int id; char title[80], author[60]; int issuedTo; Queue q; struct Book *next; } Book;
typedef struct { int id; char name[50]; int issued[MAX_ISSUE]; int cnt; } User;

Book *available = NULL, *issued = NULL;
User users[MAX_USERS];
int userCount = 0;

/* ---- helpers ---- */
void lower(const char *s, char *d) { int i = 0; for (; s[i]; i++) d[i] = tolower((unsigned char)s[i]); d[i] = 0; }
int readInt(const char *p) { char b[64]; printf("%s", p); if (!fgets(b, sizeof b, stdin)) exit(0); return atoi(b); }
void readLine(const char *p, char *d, int n) { printf("%s", p); if (!fgets(d, n, stdin)) exit(0); d[strcspn(d, "\n")] = 0; }

/* ---- queue ---- */
void enqueue(Queue *q, int uid) {
    QNode *n = malloc(sizeof(QNode)); n->uid = uid; n->next = NULL;
    if (!q->rear) q->front = q->rear = n; else { q->rear->next = n; q->rear = n; }
}
int dequeue(Queue *q) {
    if (!q->front) return -1;
    QNode *t = q->front; int u = t->uid;
    q->front = t->next; if (!q->front) q->rear = NULL;
    free(t); return u;
}
int qContains(Queue *q, int uid) { for (QNode *p = q->front; p; p = p->next) if (p->uid == uid) return 1; return 0; }
int qSize(Queue *q) { int c = 0; for (QNode *p = q->front; p; p = p->next) c++; return c; }

/* ---- linked list ---- */
void pushBack(Book **head, Book *b) {
    b->next = NULL;
    if (!*head) { *head = b; return; }
    Book *p = *head; while (p->next) p = p->next; p->next = b;
}
Book *findIn(Book *h, int id) { for (; h; h = h->next) if (h->id == id) return h; return NULL; }
Book *unlinkBook(Book **head, int id) {
    Book *prev = NULL, *cur = *head;
    while (cur && cur->id != id) { prev = cur; cur = cur->next; }
    if (!cur) return NULL;
    if (prev) prev->next = cur->next; else *head = cur->next;
    cur->next = NULL; return cur;
}
Book *findBook(int id) { Book *b = findIn(available, id); return b ? b : findIn(issued, id); }

/* ---- users (array) ---- */
User *findUser(int id) { for (int i = 0; i < userCount; i++) if (users[i].id == id) return &users[i]; return NULL; }
void giveToUser(User *u, Book *b) { u->issued[u->cnt++] = b->id; b->issuedTo = u->id; }
void takeFromUser(User *u, int bid) {
    for (int i = 0; i < u->cnt; i++) if (u->issued[i] == bid) { u->issued[i] = u->issued[--u->cnt]; return; }
}

void printBook(Book *b) {
    printf("  [%d] %s by %s", b->id, b->title, b->author);
    if (b->issuedTo != -1) printf("  (issued to user %d)", b->issuedTo);
    if (b->q.front) { printf("  Queue: "); for (QNode *p = b->q.front; p; p = p->next) printf("%d%s", p->uid, p->next ? " -> " : ""); }
    printf("\n");
}

/* ---- operations ---- */
void addBook(int id, const char *t, const char *a) {
    if (findBook(id)) { printf("  Book ID already exists.\n"); return; }
    Book *b = calloc(1, sizeof(Book));
    b->id = id; strncpy(b->title, t, 79); strncpy(b->author, a, 59); b->issuedTo = -1;
    pushBack(&available, b); printf("  Book added.\n");
}
void showList(Book *h, const char *title) {
    printf("\n--- %s ---\n", title);
    if (!h) printf("  (none)\n");
    for (; h; h = h->next) printBook(h);
}
void searchBook(const char *key) {
    char k[100], t[100], a[100]; int found = 0; lower(key, k);
    printf("\n--- Search results for \"%s\" ---\n", key);
    for (int pass = 0; pass < 2; pass++)
        for (Book *p = pass == 0 ? available : issued; p; p = p->next) {
            lower(p->title, t); lower(p->author, a);
            if (strstr(t, k) || strstr(a, k)) { printBook(p); printf("      Status: %s\n", pass == 0 ? "Available" : "Issued"); found = 1; }
        }
    if (!found) printf("  No book found.\n");
}
void addUser(int id, const char *name) {
    if (userCount == MAX_USERS) { printf("  User array full.\n"); return; }
    if (findUser(id)) { printf("  User ID already exists.\n"); return; }
    users[userCount].id = id; strncpy(users[userCount].name, name, 49); users[userCount].cnt = 0; userCount++;
    printf("  User added.\n");
}
void searchUser(int id) {
    User *u = findUser(id);
    if (!u) { printf("  User not found.\n"); return; }
    printf("  User ID: %d | Name: %s | Issued: ", u->id, u->name);
    if (!u->cnt) printf("none");
    for (int i = 0; i < u->cnt; i++) { Book *b = findBook(u->issued[i]); printf("[%d] %s%s", u->issued[i], b ? b->title : "?", i + 1 < u->cnt ? ", " : ""); }
    printf("\n");
}
void updateUser(int id, const char *n) {
    User *u = findUser(id);
    if (!u) { printf("  User not found.\n"); return; }
    strncpy(u->name, n, 49); printf("  User updated.\n");
}
void issueBook(int uid, int bid) {
    User *u = findUser(uid);
    if (!u) { printf("  User not found.\n"); return; }
    if (u->cnt >= MAX_ISSUE) { printf("  Limit of %d books reached.\n", MAX_ISSUE); return; }
    Book *b = unlinkBook(&available, bid);
    if (b) { giveToUser(u, b); pushBack(&issued, b); printf("  \"%s\" issued to %s.\n", b->title, u->name); return; }
    b = findIn(issued, bid);
    if (!b) { printf("  Book not found.\n"); return; }
    if (b->issuedTo == uid) printf("  You already have this book.\n");
    else printf("  Book is already issued. Use Reserve to join the queue.\n");
}
void reserveBook(int uid, int bid) {
    User *u = findUser(uid);
    if (!u) { printf("  User not found.\n"); return; }
    Book *b = findIn(issued, bid);
    if (!b) { printf(findIn(available, bid) ? "  Book is available - just issue it.\n" : "  Book not found.\n"); return; }
    if (b->issuedTo == uid) { printf("  You already hold this book.\n"); return; }
    if (qContains(&b->q, uid)) { printf("  Already in the queue.\n"); return; }
    enqueue(&b->q, uid);
    printf("  %s joined the queue (position %d).\n", u->name, qSize(&b->q));
}
void returnBook(int bid) {
    Book *b = findIn(issued, bid);
    if (!b) { printf("  This book is not currently issued.\n"); return; }
    User *prev = findUser(b->issuedTo);
    if (prev) takeFromUser(prev, bid);
    printf("  \"%s\" returned%s%s.\n", b->title, prev ? " by " : "", prev ? prev->name : "");
    b->issuedTo = -1;
    while (b->q.front) {
        User *nu = findUser(dequeue(&b->q));
        if (nu && nu->cnt < MAX_ISSUE) { giveToUser(nu, b); printf("  Reserved! Book automatically issued to %s (ID %d).\n", nu->name, nu->id); return; }
    }
    unlinkBook(&issued, bid); pushBack(&available, b);
    printf("  Book is back on the available list.\n");
}

void menu() {
    printf("\n========== LIBRARY MANAGEMENT SYSTEM ==========\n"
           " 1. Add Book              2. Show Available Books\n"
           " 3. Show Issued Books     4. Search Book (title/author)\n"
           " 5. Issue Book            6. Return Book\n"
           " 7. Reserve Book          8. Add User\n"
           " 9. Search User          10. Update User\n"
           "11. Show All Users        0. Exit\n"
           "================================================\n");
}

int main() {
    char s1[100], s2[100]; int ch, a, b;
    addBook(101, "Data Structures in C", "Reema Thareja"); addBook(102, "Let Us C", "Yashavant Kanetkar");
    addBook(103, "Database System Concepts", "Silberschatz"); addBook(104, "Operating System Concepts", "Galvin");
    addUser(1, "Amit"); addUser(2, "Neha"); addUser(3, "Rahul");
    do {
        menu(); ch = readInt("Enter choice: ");
        switch (ch) {
            case 1: a = readInt("Book ID: "); readLine("Title: ", s1, 100); readLine("Author: ", s2, 100); addBook(a, s1, s2); break;
            case 2: showList(available, "Available Books"); break;
            case 3: showList(issued, "Issued Books"); break;
            case 4: readLine("Enter title or author: ", s1, 100); searchBook(s1); break;
            case 5: a = readInt("User ID: "); b = readInt("Book ID: "); issueBook(a, b); break;
            case 6: returnBook(readInt("Book ID: ")); break;
            case 7: a = readInt("User ID: "); b = readInt("Book ID: "); reserveBook(a, b); break;
            case 8: a = readInt("User ID: "); readLine("Name: ", s1, 100); addUser(a, s1); break;
            case 9: searchUser(readInt("User ID: ")); break;
            case 10: a = readInt("User ID: "); readLine("New name: ", s1, 100); updateUser(a, s1); break;
            case 11: printf("\n--- All Users ---\n"); for (int i = 0; i < userCount; i++) searchUser(users[i].id); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("  Invalid choice.\n");
        }
    } while (ch != 0);
    return 0;
}
