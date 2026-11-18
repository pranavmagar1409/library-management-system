"""Library Management System - Python version (Linked List + Queue + Array)"""
MAX_USERS, MAX_ISSUE = 100, 3


class QNode:
    def __init__(self, uid):
        self.uid, self.next = uid, None


class Queue:                      # linked-list queue (FIFO)
    def __init__(self):
        self.front = self.rear = None

    def is_empty(self): return self.front is None

    def enqueue(self, uid):
        n = QNode(uid)
        if self.rear is None: self.front = self.rear = n
        else: self.rear.next = n; self.rear = n

    def dequeue(self):
        if self.front is None: return -1
        uid = self.front.uid
        self.front = self.front.next
        if self.front is None: self.rear = None
        return uid

    def items(self):
        p, out = self.front, []
        while p: out.append(p.uid); p = p.next
        return out


class Book:                       # linked-list node
    def __init__(self, bid, title, author):
        self.id, self.title, self.author = bid, title, author
        self.issued_to, self.q, self.next = -1, Queue(), None


class User:
    def __init__(self, uid, name):
        self.id, self.name, self.issued = uid, name, []


class LinkedList:
    def __init__(self): self.head = None

    def push_back(self, b):
        b.next = None
        if not self.head: self.head = b; return
        p = self.head
        while p.next: p = p.next
        p.next = b

    def find(self, bid):
        p = self.head
        while p:
            if p.id == bid: return p
            p = p.next
        return None

    def remove(self, bid):
        prev, cur = None, self.head
        while cur and cur.id != bid: prev, cur = cur, cur.next
        if not cur: return None
        if prev: prev.next = cur.next
        else: self.head = cur.next
        cur.next = None
        return cur

    def __iter__(self):
        p = self.head
        while p: yield p; p = p.next


class Library:
    def __init__(self):
        self.available, self.issued = LinkedList(), LinkedList()
        self.users = []                       # array of users

    def find_book(self, bid): return self.available.find(bid) or self.issued.find(bid)
    def find_user(self, uid): return next((u for u in self.users if u.id == uid), None)

    def show_book(self, b):
        s = f"  [{b.id}] {b.title} by {b.author}"
        if b.issued_to != -1: s += f"  (issued to user {b.issued_to})"
        if not b.q.is_empty(): s += "  Queue: " + " -> ".join(map(str, b.q.items()))
        print(s)

    def add_book(self, bid, title, author):
        if self.find_book(bid): print("  Book ID already exists."); return
        self.available.push_back(Book(bid, title, author)); print("  Book added.")

    def show(self, lst, name):
        print(f"\n--- {name} ---")
        if not lst.head: print("  (none)")
        for b in lst: self.show_book(b)

    def search_book(self, key):
        k, found = key.lower(), False
        print(f'\n--- Search results for "{key}" ---')
        for status, lst in (("Available", self.available), ("Issued", self.issued)):
            for b in lst:
                if k in b.title.lower() or k in b.author.lower():
                    self.show_book(b); print(f"      Status: {status}"); found = True
        if not found: print("  No book found.")

    def add_user(self, uid, name):
        if len(self.users) >= MAX_USERS: print("  User array full."); return
        if self.find_user(uid): print("  User ID already exists."); return
        self.users.append(User(uid, name)); print("  User added.")

    def show_user(self, uid):
        u = self.find_user(uid)
        if not u: print("  User not found."); return
        names = ", ".join(f"[{i}] {self.find_book(i).title}" for i in u.issued) or "none"
        print(f"  User ID: {u.id} | Name: {u.name} | Issued: {names}")

    def update_user(self, uid, name):
        u = self.find_user(uid)
        if not u: print("  User not found."); return
        u.name = name; print("  User updated.")

    def issue_book(self, uid, bid):
        u = self.find_user(uid)
        if not u: print("  User not found."); return
        if len(u.issued) >= MAX_ISSUE: print(f"  Limit of {MAX_ISSUE} books reached."); return
        b = self.available.remove(bid)
        if b:
            u.issued.append(b.id); b.issued_to = u.id; self.issued.push_back(b)
            print(f'  "{b.title}" issued to {u.name}.'); return
        b = self.issued.find(bid)
        if not b: print("  Book not found.")
        elif b.issued_to == uid: print("  You already have this book.")
        else: print("  Book is already issued. Use Reserve to join the queue.")

    def reserve_book(self, uid, bid):
        u = self.find_user(uid)
        if not u: print("  User not found."); return
        b = self.issued.find(bid)
        if not b:
            print("  Book is available - just issue it." if self.available.find(bid) else "  Book not found."); return
        if b.issued_to == uid: print("  You already hold this book."); return
        if uid in b.q.items(): print("  Already in the queue."); return
        b.q.enqueue(uid); print(f"  {u.name} joined the queue (position {len(b.q.items())}).")

    def return_book(self, bid):
        b = self.issued.find(bid)
        if not b: print("  This book is not currently issued."); return
        prev = self.find_user(b.issued_to)
        if prev: prev.issued.remove(bid)
        print(f'  "{b.title}" returned' + (f" by {prev.name}." if prev else "."))
        b.issued_to = -1
        while not b.q.is_empty():
            nu = self.find_user(b.q.dequeue())
            if nu and len(nu.issued) < MAX_ISSUE:
                nu.issued.append(b.id); b.issued_to = nu.id
                print(f"  Reserved! Book automatically issued to {nu.name} (ID {nu.id})."); return
        self.issued.remove(bid); self.available.push_back(b)
        print("  Book is back on the available list.")


MENU = """
========== LIBRARY MANAGEMENT SYSTEM ==========
 1. Add Book              2. Show Available Books
 3. Show Issued Books     4. Search Book (title/author)
 5. Issue Book            6. Return Book
 7. Reserve Book          8. Add User
 9. Search User          10. Update User
11. Show All Users        0. Exit
================================================"""


def read_int(p):
    while True:
        try: return int(input(p))
        except ValueError: print("  Invalid number, try again.")


def main():
    lib = Library()
    for i, t, a in [(101, "Data Structures in C", "Reema Thareja"), (102, "Let Us C", "Yashavant Kanetkar"),
                    (103, "Database System Concepts", "Silberschatz"), (104, "Operating System Concepts", "Galvin")]:
        lib.add_book(i, t, a)
    for i, n in [(1, "Amit"), (2, "Neha"), (3, "Rahul")]: lib.add_user(i, n)
    while True:
        print(MENU); ch = read_int("Enter choice: ")
        if ch == 1: lib.add_book(read_int("Book ID: "), input("Title: "), input("Author: "))
        elif ch == 2: lib.show(lib.available, "Available Books")
        elif ch == 3: lib.show(lib.issued, "Issued Books")
        elif ch == 4: lib.search_book(input("Enter title or author: "))
        elif ch == 5: lib.issue_book(read_int("User ID: "), read_int("Book ID: "))
        elif ch == 6: lib.return_book(read_int("Book ID: "))
        elif ch == 7: lib.reserve_book(read_int("User ID: "), read_int("Book ID: "))
        elif ch == 8: lib.add_user(read_int("User ID: "), input("Name: "))
        elif ch == 9: lib.show_user(read_int("User ID: "))
        elif ch == 10: lib.update_user(read_int("User ID: "), input("New name: "))
        elif ch == 11:
            print("\n--- All Users ---")
            for u in lib.users: lib.show_user(u.id)
        elif ch == 0: print("Goodbye!"); break
        else: print("  Invalid choice.")


if __name__ == "__main__":
    main()
