/* Library Management System - Java version (Linked List + Queue + Array) */
import java.util.Scanner;

public class LibraryManagementSystem {
    static final int MAX_USERS = 100, MAX_ISSUE = 3;

    static class QNode { int uid; QNode next; QNode(int u) { uid = u; } }

    static class Queue {                       // linked-list queue (FIFO)
        QNode front, rear;
        boolean isEmpty() { return front == null; }
        void enqueue(int uid) {
            QNode n = new QNode(uid);
            if (rear == null) front = rear = n; else { rear.next = n; rear = n; }
        }
        int dequeue() {
            if (front == null) return -1;
            int u = front.uid; front = front.next;
            if (front == null) rear = null;
            return u;
        }
        boolean contains(int uid) { for (QNode p = front; p != null; p = p.next) if (p.uid == uid) return true; return false; }
        int size() { int c = 0; for (QNode p = front; p != null; p = p.next) c++; return c; }
        String show() {
            StringBuilder sb = new StringBuilder();
            for (QNode p = front; p != null; p = p.next) sb.append(p.uid).append(p.next != null ? " -> " : "");
            return sb.toString();
        }
    }

    static class Book {                        // linked-list node
        int id, issuedTo = -1; String title, author; Queue q = new Queue(); Book next;
        Book(int i, String t, String a) { id = i; title = t; author = a; }
    }

    static class User {                        // stored in array
        int id, cnt = 0; String name; int[] issued = new int[MAX_ISSUE];
        User(int i, String n) { id = i; name = n; }
    }

    static class BookList {
        Book head;
        void pushBack(Book b) {
            b.next = null;
            if (head == null) { head = b; return; }
            Book p = head; while (p.next != null) p = p.next; p.next = b;
        }
        Book find(int id) { for (Book p = head; p != null; p = p.next) if (p.id == id) return p; return null; }
        Book remove(int id) {
            Book prev = null, cur = head;
            while (cur != null && cur.id != id) { prev = cur; cur = cur.next; }
            if (cur == null) return null;
            if (prev != null) prev.next = cur.next; else head = cur.next;
            cur.next = null; return cur;
        }
    }

    static BookList available = new BookList(), issued = new BookList();
    static User[] users = new User[MAX_USERS];
    static int userCount = 0;

    static Book findBook(int id) { Book b = available.find(id); return b != null ? b : issued.find(id); }
    static User findUser(int id) { for (int i = 0; i < userCount; i++) if (users[i].id == id) return users[i]; return null; }
    static void give(User u, Book b) { u.issued[u.cnt++] = b.id; b.issuedTo = u.id; }
    static void take(User u, int bid) {
        for (int i = 0; i < u.cnt; i++) if (u.issued[i] == bid) { u.issued[i] = u.issued[--u.cnt]; return; }
    }
    static void print(Book b) {
        System.out.print("  [" + b.id + "] " + b.title + " by " + b.author);
        if (b.issuedTo != -1) System.out.print("  (issued to user " + b.issuedTo + ")");
        if (!b.q.isEmpty()) System.out.print("  Queue: " + b.q.show());
        System.out.println();
    }

    static void addBook(int id, String t, String a) {
        if (findBook(id) != null) { System.out.println("  Book ID already exists."); return; }
        available.pushBack(new Book(id, t, a)); System.out.println("  Book added.");
    }
    static void showList(BookList l, String name) {
        System.out.println("\n--- " + name + " ---");
        if (l.head == null) System.out.println("  (none)");
        for (Book p = l.head; p != null; p = p.next) print(p);
    }
    static void searchBook(String key) {
        String k = key.toLowerCase(); boolean found = false;
        System.out.println("\n--- Search results for \"" + key + "\" ---");
        for (int pass = 0; pass < 2; pass++)
            for (Book p = (pass == 0 ? available : issued).head; p != null; p = p.next)
                if (p.title.toLowerCase().contains(k) || p.author.toLowerCase().contains(k)) {
                    print(p); System.out.println("      Status: " + (pass == 0 ? "Available" : "Issued")); found = true;
                }
        if (!found) System.out.println("  No book found.");
    }
    static void addUser(int id, String name) {
        if (userCount == MAX_USERS) { System.out.println("  User array full."); return; }
        if (findUser(id) != null) { System.out.println("  User ID already exists."); return; }
        users[userCount++] = new User(id, name); System.out.println("  User added.");
    }
    static void showUser(int id) {
        User u = findUser(id);
        if (u == null) { System.out.println("  User not found."); return; }
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < u.cnt; i++) sb.append("[").append(u.issued[i]).append("] ").append(findBook(u.issued[i]).title).append(i + 1 < u.cnt ? ", " : "");
        System.out.println("  User ID: " + u.id + " | Name: " + u.name + " | Issued: " + (u.cnt == 0 ? "none" : sb));
    }
    static void updateUser(int id, String n) {
        User u = findUser(id);
        if (u == null) { System.out.println("  User not found."); return; }
        u.name = n; System.out.println("  User updated.");
    }
    static void issueBook(int uid, int bid) {
        User u = findUser(uid);
        if (u == null) { System.out.println("  User not found."); return; }
        if (u.cnt >= MAX_ISSUE) { System.out.println("  Limit of " + MAX_ISSUE + " books reached."); return; }
        Book b = available.remove(bid);
        if (b != null) { give(u, b); issued.pushBack(b); System.out.println("  \"" + b.title + "\" issued to " + u.name + "."); return; }
        b = issued.find(bid);
        if (b == null) System.out.println("  Book not found.");
        else if (b.issuedTo == uid) System.out.println("  You already have this book.");
        else System.out.println("  Book is already issued. Use Reserve to join the queue.");
    }
    static void reserveBook(int uid, int bid) {
        User u = findUser(uid);
        if (u == null) { System.out.println("  User not found."); return; }
        Book b = issued.find(bid);
        if (b == null) { System.out.println(available.find(bid) != null ? "  Book is available - just issue it." : "  Book not found."); return; }
        if (b.issuedTo == uid) { System.out.println("  You already hold this book."); return; }
        if (b.q.contains(uid)) { System.out.println("  Already in the queue."); return; }
        b.q.enqueue(uid);
        System.out.println("  " + u.name + " joined the queue (position " + b.q.size() + ").");
    }
    static void returnBook(int bid) {
        Book b = issued.find(bid);
        if (b == null) { System.out.println("  This book is not currently issued."); return; }
        User prev = findUser(b.issuedTo);
        if (prev != null) take(prev, bid);
        System.out.println("  \"" + b.title + "\" returned" + (prev != null ? " by " + prev.name : "") + ".");
        b.issuedTo = -1;
        while (!b.q.isEmpty()) {
            User nu = findUser(b.q.dequeue());
            if (nu != null && nu.cnt < MAX_ISSUE) {
                give(nu, b);
                System.out.println("  Reserved! Book automatically issued to " + nu.name + " (ID " + nu.id + ")."); return;
            }
        }
        issued.remove(bid); available.pushBack(b);
        System.out.println("  Book is back on the available list.");
    }

    static Scanner sc = new Scanner(System.in);
    static int readInt(String p) {
        while (true) {
            System.out.print(p);
            try { return Integer.parseInt(sc.nextLine().trim()); }
            catch (NumberFormatException e) { System.out.println("  Invalid number, try again."); }
        }
    }
    static String readLine(String p) { System.out.print(p); return sc.nextLine(); }

    public static void main(String[] args) {
        addBook(101, "Data Structures in C", "Reema Thareja"); addBook(102, "Let Us C", "Yashavant Kanetkar");
        addBook(103, "Database System Concepts", "Silberschatz"); addBook(104, "Operating System Concepts", "Galvin");
        addUser(1, "Amit"); addUser(2, "Neha"); addUser(3, "Rahul");
        int ch;
        do {
            System.out.println("\n========== LIBRARY MANAGEMENT SYSTEM ==========\n"
                + " 1. Add Book              2. Show Available Books\n"
                + " 3. Show Issued Books     4. Search Book (title/author)\n"
                + " 5. Issue Book            6. Return Book\n"
                + " 7. Reserve Book          8. Add User\n"
                + " 9. Search User          10. Update User\n"
                + "11. Show All Users        0. Exit\n"
                + "================================================");
            ch = readInt("Enter choice: ");
            switch (ch) {
                case 1: { int id = readInt("Book ID: "); String t = readLine("Title: "), a = readLine("Author: "); addBook(id, t, a); break; }
                case 2: showList(available, "Available Books"); break;
                case 3: showList(issued, "Issued Books"); break;
                case 4: searchBook(readLine("Enter title or author: ")); break;
                case 5: { int u = readInt("User ID: "), b = readInt("Book ID: "); issueBook(u, b); break; }
                case 6: returnBook(readInt("Book ID: ")); break;
                case 7: { int u = readInt("User ID: "), b = readInt("Book ID: "); reserveBook(u, b); break; }
                case 8: { int id = readInt("User ID: "); addUser(id, readLine("Name: ")); break; }
                case 9: showUser(readInt("User ID: ")); break;
                case 10: { int id = readInt("User ID: "); updateUser(id, readLine("New name: ")); break; }
                case 11: System.out.println("\n--- All Users ---"); for (int i = 0; i < userCount; i++) showUser(users[i].id); break;
                case 0: System.out.println("Goodbye!"); break;
                default: System.out.println("  Invalid choice.");
            }
        } while (ch != 0);
    }
}
