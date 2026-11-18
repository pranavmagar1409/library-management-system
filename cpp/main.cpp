/*
 * Library Management System  -  MCA Sem 1 Mini Project (Data Structures)
 *
 * Data structures used:
 *   1. Singly Linked List : available books list + issued books list
 *   2. Queue (linked)      : reservation queue attached to every book
 *   3. Array               : user records (ID, name, issued books)
 */
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
using namespace std;

const int MAX_USERS = 100;       // size of the user array
const int MAX_ISSUE = 3;         // max books one user can hold

// ---------- helpers ----------
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}
int readInt(const string &prompt) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x) { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return x; }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid number, try again.\n";
    }
}
string readLine(const string &prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// ---------- Queue (reservation) ----------
struct QNode {
    int userId;
    QNode *next;
    QNode(int u) : userId(u), next(nullptr) {}
};

class Queue {
    QNode *front, *rear;
public:
    Queue() : front(nullptr), rear(nullptr) {}
    bool isEmpty() const { return front == nullptr; }
    void enqueue(int uid) {
        QNode *n = new QNode(uid);
        if (!rear) front = rear = n;
        else { rear->next = n; rear = n; }
    }
    int dequeue() {                       // returns -1 if empty
        if (!front) return -1;
        QNode *t = front;
        int uid = t->userId;
        front = front->next;
        if (!front) rear = nullptr;
        delete t;
        return uid;
    }
    bool contains(int uid) const {
        for (QNode *p = front; p; p = p->next)
            if (p->userId == uid) return true;
        return false;
    }
    int size() const {
        int c = 0;
        for (QNode *p = front; p; p = p->next) c++;
        return c;
    }
    void display() const {
        for (QNode *p = front; p; p = p->next) cout << p->userId << (p->next ? " -> " : "");
    }
    ~Queue() { while (!isEmpty()) dequeue(); }
};

// ---------- Book (linked list node) ----------
struct Book {
    int id;
    string title, author;
    int issuedTo;          // user id, -1 if available
    Queue reservations;
    Book *next;
    Book(int i, string t, string a) : id(i), title(t), author(a), issuedTo(-1), next(nullptr) {}
};

// ---------- User (stored in array) ----------
struct User {
    int id;
    string name;
    int issued[MAX_ISSUE];
    int issuedCount;
    User() : id(-1), issuedCount(0) {}
};

// ---------- Library ----------
class Library {
    Book *available;       // linked list of books on the shelf
    Book *issued;          // linked list of books currently issued
    User users[MAX_USERS]; // array of users
    int userCount;

    // list helpers
    void pushBack(Book *&head, Book *b) {
        b->next = nullptr;
        if (!head) { head = b; return; }
        Book *p = head;
        while (p->next) p = p->next;
        p->next = b;
    }
    Book *findIn(Book *head, int id) {
        for (Book *p = head; p; p = p->next) if (p->id == id) return p;
        return nullptr;
    }
    Book *unlink(Book *&head, int id) {   // remove node from list, return it
        Book *prev = nullptr, *cur = head;
        while (cur && cur->id != id) { prev = cur; cur = cur->next; }
        if (!cur) return nullptr;
        if (prev) prev->next = cur->next; else head = cur->next;
        cur->next = nullptr;
        return cur;
    }
    Book *findBook(int id) {
        Book *b = findIn(available, id);
        return b ? b : findIn(issued, id);
    }
    User *findUser(int id) {
        for (int i = 0; i < userCount; i++) if (users[i].id == id) return &users[i];
        return nullptr;
    }
    void giveToUser(User *u, Book *b) {
        u->issued[u->issuedCount++] = b->id;
        b->issuedTo = u->id;
    }
    void takeFromUser(User *u, int bookId) {
        for (int i = 0; i < u->issuedCount; i++)
            if (u->issued[i] == bookId) {
                u->issued[i] = u->issued[--u->issuedCount];
                return;
            }
    }
    void printBook(Book *b) {
        cout << "  [" << b->id << "] " << b->title << " by " << b->author;
        if (b->issuedTo != -1) cout << "  (issued to user " << b->issuedTo << ")";
        if (!b->reservations.isEmpty()) { cout << "  Queue: "; b->reservations.display(); }
        cout << "\n";
    }

public:
    Library() : available(nullptr), issued(nullptr), userCount(0) {}

    // ----- books -----
    void addBook(int id, string t, string a) {
        if (findBook(id)) { cout << "  Book ID already exists.\n"; return; }
        pushBack(available, new Book(id, t, a));
        cout << "  Book added.\n";
    }
    void showAvailable() {
        cout << "\n--- Available Books ---\n";
        if (!available) cout << "  (none)\n";
        for (Book *p = available; p; p = p->next) printBook(p);
    }
    void showIssued() {
        cout << "\n--- Issued Books ---\n";
        if (!issued) cout << "  (none)\n";
        for (Book *p = issued; p; p = p->next) printBook(p);
    }
    // Search by title or author using linked list traversal
    void searchBook(const string &key) {
        string k = toLower(key);
        bool found = false;
        cout << "\n--- Search results for \"" << key << "\" ---\n";
        for (int pass = 0; pass < 2; pass++) {
            for (Book *p = pass == 0 ? available : issued; p; p = p->next)
                if (toLower(p->title).find(k) != string::npos ||
                    toLower(p->author).find(k) != string::npos) {
                    printBook(p);
                    cout << "      Status: " << (pass == 0 ? "Available" : "Issued") << "\n";
                    found = true;
                }
        }
        if (!found) cout << "  No book found.\n";
    }

    // ----- users -----
    void addUser(int id, string name) {
        if (userCount == MAX_USERS) { cout << "  User array full.\n"; return; }
        if (findUser(id)) { cout << "  User ID already exists.\n"; return; }
        users[userCount].id = id;
        users[userCount].name = name;
        userCount++;
        cout << "  User added.\n";
    }
    void searchUser(int id) {
        User *u = findUser(id);
        if (!u) { cout << "  User not found.\n"; return; }
        cout << "  User ID: " << u->id << " | Name: " << u->name << " | Issued: ";
        if (u->issuedCount == 0) cout << "none";
        for (int i = 0; i < u->issuedCount; i++) {
            Book *b = findBook(u->issued[i]);
            cout << "[" << u->issued[i] << "] " << (b ? b->title : "?") << (i + 1 < u->issuedCount ? ", " : "");
        }
        cout << "\n";
    }
    void updateUser(int id, string newName) {
        User *u = findUser(id);
        if (!u) { cout << "  User not found.\n"; return; }
        u->name = newName;
        cout << "  User updated.\n";
    }
    void showUsers() {
        cout << "\n--- All Users ---\n";
        if (!userCount) cout << "  (none)\n";
        for (int i = 0; i < userCount; i++) searchUser(users[i].id);
    }

    // ----- issue / return / reserve -----
    void issueBook(int uid, int bid) {
        User *u = findUser(uid);
        if (!u) { cout << "  User not found.\n"; return; }
        if (u->issuedCount >= MAX_ISSUE) { cout << "  Limit of " << MAX_ISSUE << " books reached.\n"; return; }
        Book *b = unlink(available, bid);
        if (b) {                                   // available -> remove from available list
            giveToUser(u, b);
            pushBack(issued, b);
            cout << "  \"" << b->title << "\" issued to " << u->name << ".\n";
            return;
        }
        b = findIn(issued, bid);
        if (!b) { cout << "  Book not found.\n"; return; }
        if (b->issuedTo == uid) { cout << "  You already have this book.\n"; return; }
        cout << "  Book is already issued. Use Reserve to join the queue.\n";
    }

    void reserveBook(int uid, int bid) {
        User *u = findUser(uid);
        if (!u) { cout << "  User not found.\n"; return; }
        Book *b = findIn(issued, bid);
        if (!b) {
            cout << (findIn(available, bid) ? "  Book is available - just issue it.\n" : "  Book not found.\n");
            return;
        }
        if (b->issuedTo == uid) { cout << "  You already hold this book.\n"; return; }
        if (b->reservations.contains(uid)) { cout << "  Already in the queue.\n"; return; }
        b->reservations.enqueue(uid);
        cout << "  " << u->name << " joined the queue (position " << b->reservations.size() << ").\n";
    }

    void returnBook(int bid) {
        Book *b = findIn(issued, bid);
        if (!b) { cout << "  This book is not currently issued.\n"; return; }
        User *prev = findUser(b->issuedTo);
        if (prev) takeFromUser(prev, bid);
        cout << "  \"" << b->title << "\" returned" << (prev ? " by " + prev->name : "") << ".\n";
        b->issuedTo = -1;

        // give it to the next person in the reservation queue
        while (!b->reservations.isEmpty()) {
            int nextId = b->reservations.dequeue();
            User *nu = findUser(nextId);
            if (nu && nu->issuedCount < MAX_ISSUE) {
                giveToUser(nu, b);                 // stays in issued list
                cout << "  Reserved! Book automatically issued to " << nu->name << " (ID " << nu->id << ").\n";
                return;
            }
        }
        // nobody waiting -> move to available list
        unlink(issued, bid);
        pushBack(available, b);
        cout << "  Book is back on the available list.\n";
    }

    ~Library() {
        for (int pass = 0; pass < 2; pass++) {
            Book *p = pass == 0 ? available : issued;
            while (p) { Book *t = p; p = p->next; delete t; }
        }
    }
};

// ---------- menu ----------
void menu() {
    cout << "\n========== LIBRARY MANAGEMENT SYSTEM ==========\n"
         << " 1. Add Book              2. Show Available Books\n"
         << " 3. Show Issued Books     4. Search Book (title/author)\n"
         << " 5. Issue Book            6. Return Book\n"
         << " 7. Reserve Book          8. Add User\n"
         << " 9. Search User          10. Update User\n"
         << "11. Show All Users        0. Exit\n"
         << "================================================\n";
}

int main() {
    Library lib;
    // sample data so the demo works immediately
    cout << "Loading sample data...\n";
    lib.addBook(101, "Data Structures in C", "Reema Thareja");
    lib.addBook(102, "Let Us C", "Yashavant Kanetkar");
    lib.addBook(103, "Database System Concepts", "Silberschatz");
    lib.addBook(104, "Operating System Concepts", "Galvin");
    lib.addUser(1, "Amit");
    lib.addUser(2, "Neha");
    lib.addUser(3, "Rahul");

    int ch;
    do {
        menu();
        ch = readInt("Enter choice: ");
        switch (ch) {
            case 1: { int id = readInt("Book ID: "); string t = readLine("Title: "), a = readLine("Author: "); lib.addBook(id, t, a); break; }
            case 2: lib.showAvailable(); break;
            case 3: lib.showIssued(); break;
            case 4: lib.searchBook(readLine("Enter title or author: ")); break;
            case 5: { int u = readInt("User ID: "), b = readInt("Book ID: "); lib.issueBook(u, b); break; }
            case 6: lib.returnBook(readInt("Book ID: ")); break;
            case 7: { int u = readInt("User ID: "), b = readInt("Book ID: "); lib.reserveBook(u, b); break; }
            case 8: { int id = readInt("User ID: "); string n = readLine("Name: "); lib.addUser(id, n); break; }
            case 9: lib.searchUser(readInt("User ID: ")); break;
            case 10: { int id = readInt("User ID: "); string n = readLine("New name: "); lib.updateUser(id, n); break; }
            case 11: lib.showUsers(); break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "  Invalid choice.\n";
        }
    } while (ch != 0);
    return 0;
}
