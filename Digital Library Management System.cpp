#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#endif

using namespace std;

struct BorrowRecord {
    long long studentID;
};

struct Book {
    string id;
    string title;
    string author;
    string category;          // Department-wise category
    int pageCount;
    int totalCopies;
    int availableCopies;
    vector<long long> borrowedBy;
};

struct StudentAccount {
    long long studentID;
    string password;
    string name;
    string department;
    int section;
    int intake;
};

// Global Data Structures
vector<Book> books;
vector<long long> registeredStudentIDs;
string adminPassword = "admin123";
vector<StudentAccount> studentAccounts;

// File Persistence
const string DATA_FILE = "library_data.txt";

// ---------- Small Helper Functions ----------

bool isStrongPassword(const string& password) {
    if (password.length() < 8) return false;

    bool hasLetter = false;
    bool hasNumber = false;
    bool hasSpecial = false;

    for (char ch : password) {
        if (isalpha(static_cast<unsigned char>(ch))) hasLetter = true;
        else if (isdigit(static_cast<unsigned char>(ch))) hasNumber = true;
        else hasSpecial = true;
    }

    return hasLetter && hasNumber && hasSpecial;
}

string getPassword(const string& prompt) {
    cout << prompt;

#ifdef _WIN32
    string password;
    char ch;

    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if (ch != '\n') {
            password += ch;
            cout << '*';
        }
    }

    cout << '\n';
    return password;
#else
    // Fallback for non-Windows systems.
    string password;
    cin >> password;
    cout << string(password.length(), '*') << '\n';
    return password;
#endif
}

StudentAccount* findStudent(long long studentID) {
    for (auto& student : studentAccounts) {
        if (student.studentID == studentID) {
            return &student;
        }
    }
    return nullptr;
}

bool bookAlreadyExists(const string& bookID) {
    for (const auto& book : books) {
        if (book.id == bookID) return true;
    }
    return false;
}

void showPasswordRule() {
    cout << "Password must contain:\n";
    cout << "- At least 8 characters\n";
    cout << "- At least one alphabet\n";
    cout << "- At least one number\n";
    cout << "- At least one special character\n";
}

// ---------- File Persistence ----------

void saveData() {
    ofstream out(DATA_FILE);
    if (!out) return;

    out << "VERSION 2\n";
    out << "ADMIN " << adminPassword << "\n";

    out << "STUDENTS " << studentAccounts.size() << "\n";
    for (const auto& student : studentAccounts) {
        out << student.studentID << "|"
            << student.password << "|"
            << student.name << "|"
            << student.department << "|"
            << student.section << "|"
            << student.intake << "\n";
    }

    out << "REGISTERED " << registeredStudentIDs.size() << "\n";
    for (long long id : registeredStudentIDs) {
        out << id << "\n";
    }

    out << "BOOKS " << books.size() << "\n";
    for (const auto& book : books) {
        out << book.id << "\n";
        out << book.title << "\n";
        out << book.author << "\n";
        out << book.category << "\n";
        out << book.pageCount << "\n";
        out << book.totalCopies << " " << book.availableCopies << "\n";

        out << book.borrowedBy.size();
        for (long long id : book.borrowedBy) {
            out << " " << id;
        }
        out << "\n";
    }
}

bool loadData() {
    ifstream in(DATA_FILE);
    if (!in) return false;

    string tag;
    bool newFormat = false;

    if (!(in >> tag)) return false;

    if (tag == "VERSION") {
        int version;
        in >> version;
        newFormat = (version >= 2);

        if (!(in >> tag) || tag != "ADMIN") return false;
    }
    else if (tag != "ADMIN") {
        return false;
    }

    in >> adminPassword;

    size_t count;
    string line;

    if (!(in >> tag) || tag != "STUDENTS") return false;
    in >> count;
    getline(in, line);

    studentAccounts.clear();

    for (size_t i = 0; i < count; ++i) {
        getline(in, line);

        StudentAccount s;
        s.name = "Not Set";
        s.department = "CSE";
        s.section = 0;
        s.intake = 0;

        if (newFormat) {
            // id|password|name|department|section|intake
            vector<string> parts;
            string part;
            stringstream ss(line);

            while (getline(ss, part, '|')) {
                parts.push_back(part);
            }

            if (parts.size() >= 6) {
                s.studentID = stoll(parts[0]);
                s.password = parts[1];
                s.name = parts[2];
                s.department = parts[3];
                s.section = stoi(parts[4]);
                s.intake = stoi(parts[5]);
            }
        }
        else {
            // Old format: ID PASSWORD
            stringstream ss(line);
            ss >> s.studentID >> s.password;
        }

        studentAccounts.push_back(s);
    }

    if (!(in >> tag) || tag != "REGISTERED") return false;
    in >> count;

    registeredStudentIDs.clear();

    for (size_t i = 0; i < count; ++i) {
        long long id;
        in >> id;
        registeredStudentIDs.push_back(id);
    }

    if (!(in >> tag) || tag != "BOOKS") return false;
    in >> count;
    getline(in, line);

    books.clear();

    for (size_t i = 0; i < count; ++i) {
        Book book;
        book.category = "CSE";
        book.pageCount = 0;

        getline(in, book.id);
        getline(in, book.title);
        getline(in, book.author);

        if (newFormat) {
            getline(in, book.category);
            getline(in, line);
            book.pageCount = stoi(line);

            in >> book.totalCopies >> book.availableCopies;
        }
        else {
            // Old format: totalCopies availableCopies
            in >> book.totalCopies >> book.availableCopies;
        }

        size_t borrowedCount;
        in >> borrowedCount;

        book.borrowedBy.clear();

        for (size_t j = 0; j < borrowedCount; ++j) {
            long long id;
            in >> id;
            book.borrowedBy.push_back(id);
        }

        getline(in, line);
        books.push_back(book);
    }

    return true;
}

// ---------- Initial Data ----------

void initializeStudentIDs() {
    for (long long id = 20254103082LL; id <= 20254103150LL; ++id) {
        registeredStudentIDs.push_back(id);
    }
}

void initializeBooks() {
    books = {
        {"B001", "Introduction to Algorithms", "T. Cormen et al.", "CSE", 1312, 10, 10, {}},
        {"B002", "The C++ Programming Language", "Bjarne Stroustrup", "CSE", 1376, 10, 10, {}},
        {"B003", "Data Structures and Algorithms", "A. Aho", "CSE", 700, 10, 10, {}},
        {"B004", "Operating System Concepts", "Silberschatz", "CSE", 944, 10, 10, {}},
        {"B005", "Database System Concepts", "A. Silberschatz", "CSE", 1376, 10, 10, {}},
        {"B006", "Computer Networking", "Kurose & Ross", "CSE", 864, 10, 10, {}},
        {"B007", "Discrete Mathematics", "K. Rosen", "CSE", 1072, 10, 10, {}},
        {"B008", "Digital Logic Design", "M. Morris Mano", "EEE", 800, 10, 10, {}},
        {"B009", "Artificial Intelligence: A Modern Approach", "Russell & Norvig", "CSE", 1152, 10, 10, {}},
        {"B010", "Clean Code", "Robert C. Martin", "CSE", 464, 10, 10, {}}
    };
}

// ---------- Student Registration / Login Helpers ----------

bool isValidStudentID(long long studentID) {
    return find(registeredStudentIDs.begin(),
                registeredStudentIDs.end(),
                studentID) != registeredStudentIDs.end();
}

bool isStudentRegistered(long long studentID) {
    return findStudent(studentID) != nullptr;
}

void studentRegistration() {
    long long studentID;
    string password, confirmPassword;

    cout << "\n--- NEW STUDENT REGISTRATION ---\n";

    cout << "Enter your University Student ID: ";
    cin >> studentID;

    if (!isValidStudentID(studentID)) {
        cout << "Error: This Student ID is not registered in the university records.\n";
        return;
    }

    if (isStudentRegistered(studentID)) {
        cout << "Error: An account already exists for this Student ID.\n";
        return;
    }

    cin.ignore();
    string name;
    cout << "Enter your Name: ";
    getline(cin, name);

    cout << "Enter Department (e.g., CSE): ";
    string department;
    cin >> department;

    cout << "Enter Section: ";
    int section;
    cin >> section;

    cout << "Enter Intake: ";
    int intake;
    cin >> intake;

    showPasswordRule();

    password = getPassword("Create Password: ");
    if (!isStrongPassword(password)) {
        cout << "Error: Password is not strong enough.\n";
        return;
    }

    confirmPassword = getPassword("Confirm Password: ");

    if (password != confirmPassword) {
        cout << "Error: Passwords do not match.\n";
        return;
    }

    studentAccounts.push_back({
        studentID, password, name, department, section, intake
    });

    saveData();

    cout << "Registration Successful!\n";
    cout << "You can now login using your Student ID and Password.\n";
}

// ---------- Book Display ----------

void printBook(const Book& book) {
    cout << "\nBook ID      : " << book.id;
    cout << "\nTitle        : " << book.title;
    cout << "\nAuthor       : " << book.author;
    cout << "\nDepartment   : " << book.category;
    cout << "\nPage Number  : " << book.pageCount;
    cout << "\nCopies       : " << book.availableCopies
         << "/" << book.totalCopies;
    cout << "\n";
}

void displayBooks(const string& departmentFilter = "") {
    cout << "\n==================== BOOK LIST ====================\n";

    bool found = false;

    for (const auto& book : books) {
        if (!departmentFilter.empty() &&
            book.category != departmentFilter) {
            continue;
        }

        found = true;
        printBook(book);
        cout << "----------------------------------------------------\n";
    }

    if (!found) {
        if (departmentFilter.empty())
            cout << "No books found.\n";
        else
            cout << "No books found for Department: "
                 << departmentFilter << "\n";
    }
}

void displayBooksByDepartment() {
    cout << "\n--- BOOK CATEGORY / DEPARTMENT ---\n";
    cout << "1. My Department\n";
    cout << "2. All Departments\n";
    cout << "3. Enter Department Manually\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        cout << "Showing books for your department...\n";
        // This function is called from studentMenu, so the student's
        // department will be passed through the other overload below.
    }
    else if (choice == 2) {
        displayBooks();
    }
    else if (choice == 3) {
        string department;
        cout << "Enter Department: ";
        cin >> department;
        displayBooks(department);
    }
    else {
        cout << "Invalid choice.\n";
    }
}

void displayStudentBooks(const string& studentDepartment) {
    cout << "\n--- BOOK CATEGORY ---\n";
    cout << "1. My Department (" << studentDepartment << ")\n";
    cout << "2. All Departments\n";
    cout << "3. Search by Department\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        displayBooks(studentDepartment);
    }
    else if (choice == 2) {
        displayBooks();
    }
    else if (choice == 3) {
        string department;
        cout << "Enter Department: ";
        cin >> department;
        displayBooks(department);
    }
    else {
        cout << "Invalid choice!\n";
    }
}

// ---------- Student Panel ----------

void showStudentInfo(long long studentID) {
    StudentAccount* student = findStudent(studentID);

    if (student == nullptr) {
        cout << "Student information not found.\n";
        return;
    }

    cout << "\n========== STUDENT INFORMATION ==========\n";
    cout << "Name       : " << student->name << "\n";
    cout << "Student ID : " << student->studentID << "\n";
    cout << "Department : " << student->department << "\n";
    cout << "Section    : " << student->section << "\n";
    cout << "Intake     : " << student->intake << "\n";
    cout << "=========================================\n";
}

void showBorrowedBooks(long long studentID) {
    cout << "\n========== MY BORROWED BOOKS ==========" << "\n";

    bool found = false;

    for (const auto& book : books) {
        // Count how many copies of this book the student has borrowed.
        int borrowedCopies = count(book.borrowedBy.begin(),
                                   book.borrowedBy.end(),
                                   studentID);

        if (borrowedCopies > 0) {
            found = true;

            cout << "\nBook ID        : " << book.id;
            cout << "\nTitle          : " << book.title;
            cout << "\nAuthor         : " << book.author;
            cout << "\nDepartment     : " << book.category;
            cout << "\nPage Number    : " << book.pageCount;
            cout << "\nBorrowed Copies: " << borrowedCopies << "\n";
            cout << "----------------------------------------\n";
        }
    }

    if (!found) {
        cout << "You have not borrowed any books.\n";
    }
}

void changeStudentPassword(long long studentID) {
    StudentAccount* student = findStudent(studentID);

    if (student == nullptr) {
        cout << "Student account not found.\n";
        return;
    }

    cout << "\n--- CHANGE STUDENT PASSWORD ---\n";

    string oldPassword = getPassword("Enter Current Password: ");

    if (oldPassword != student->password) {
        cout << "Error: Incorrect current password.\n";
        return;
    }

    showPasswordRule();

    string newPassword = getPassword("Enter New Password: ");

    if (!isStrongPassword(newPassword)) {
        cout << "Error: New password does not meet the requirements.\n";
        return;
    }

    string confirmPassword = getPassword("Confirm New Password: ");

    if (newPassword != confirmPassword) {
        cout << "Error: Passwords do not match.\n";
        return;
    }

    student->password = newPassword;
    saveData();

    cout << "Student password changed successfully!\n";
}

void studentMenu(long long studentID) {
    int choice;

    while (true) {
        StudentAccount* student = findStudent(studentID);
        string department = (student != nullptr) ? student->department : "CSE";

        cout << "\n--- STUDENT PANEL (ID: " << studentID << ") ---\n";
        cout << "1. View All Books / Book Category\n";
        cout << "2. Borrow Book\n";
        cout << "3. Return Book\n";
        cout << "4. My Borrowed Books\n";
        cout << "5. My Student Information\n";
        cout << "6. Change Password\n";
        cout << "7. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            displayStudentBooks(department);
        }
        else if (choice == 2) {
            displayBooks(department);

            string bookID;
            cout << "Enter Book ID to borrow: ";
            cin >> bookID;

            bool found = false;

            for (auto& book : books) {
                if (book.id == bookID) {
                    found = true;

                    if (book.availableCopies > 0) {
                        book.availableCopies--;
                        book.borrowedBy.push_back(studentID);

                        cout << "Success: Book '" << book.title
                             << "' borrowed successfully!\n";

                        cout << "Available copies remaining: "
                             << book.availableCopies << "\n";

                        saveData();
                    }
                    else {
                        cout << "Error: No copies of this book are currently available.\n";
                    }

                    break;
                }
            }

            if (!found) {
                cout << "Error: Book ID not found.\n";
            }
        }
        else if (choice == 3) {
            cout << "\n--- YOUR BORROWED BOOKS ---\n";

            vector<string> borrowedBookIDs;

            for (const auto& book : books) {
                if (find(book.borrowedBy.begin(),
                         book.borrowedBy.end(),
                         studentID) != book.borrowedBy.end()) {

                    borrowedBookIDs.push_back(book.id);

                    cout << book.id << " - " << book.title
                         << " (" << book.category << ")\n";
                }
            }

            if (borrowedBookIDs.empty()) {
                cout << "You do not have any borrowed books.\n";
                continue;
            }

            string bookID;
            cout << "Enter Book ID you want to return: ";
            cin >> bookID;

            bool found = false;

            for (auto& book : books) {
                if (book.id == bookID) {
                    found = true;

                    auto it = find(book.borrowedBy.begin(),
                                   book.borrowedBy.end(),
                                   studentID);

                    if (it != book.borrowedBy.end()) {
                        book.borrowedBy.erase(it);
                        book.availableCopies++;

                        cout << "Success: Book '" << book.title
                             << "' returned successfully!\n";

                        cout << "Available copies: "
                             << book.availableCopies << "\n";

                        saveData();
                    }
                    else {
                        cout << "Error: You did not borrow this book.\n";
                    }

                    break;
                }
            }

            if (!found) {
                cout << "Error: Book ID not found.\n";
            }
        }
        else if (choice == 4) {
            showBorrowedBooks(studentID);
        }
        else if (choice == 5) {
            showStudentInfo(studentID);
        }
        else if (choice == 6) {
            changeStudentPassword(studentID);
        }
        else if (choice == 7) {
            cout << "Logging out...\n";
            break;
        }
        else {
            cout << "Invalid choice! Try again.\n";
        }
    }
}

// ---------- Admin Panel ----------

void adminMenu() {
    string password;
    password = getPassword("Enter Admin Password: ");

    if (password != adminPassword) {
        cout << "Incorrect password!\n";
        return;
    }

    int choice;

    while (true) {
        cout << "\n--- ADMIN PANEL ---\n";
        cout << "1. View All Books\n";
        cout << "2. Add New Book\n";
        cout << "3. Remove Book\n";
        cout << "4. Add New Student ID\n";
        cout << "5. Remove Student ID\n";
        cout << "6. View All Registered Student IDs\n";
        cout << "7. Change Admin Password\n";
        cout << "8. Exit to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            displayBooks();
        }
        else if (choice == 2) {
            Book newBook;

            cout << "Enter Book ID (e.g., B011): ";
            cin >> newBook.id;

            if (bookAlreadyExists(newBook.id)) {
                cout << "Error: This Book ID already exists.\n";
                continue;
            }

            cin.ignore();

            cout << "Enter Book Title: ";
            getline(cin, newBook.title);

            cout << "Enter Author Name: ";
            getline(cin, newBook.author);

            cout << "Enter Department / Category (e.g., CSE): ";
            cin >> newBook.category;

            cout << "Enter Number of Pages: ";
            cin >> newBook.pageCount;

            cout << "How many copies do you want to add: ";
            cin >> newBook.totalCopies;

            if (newBook.totalCopies < 1) {
                cout << "Error: Number of copies must be at least 1.\n";
                continue;
            }

            newBook.availableCopies = newBook.totalCopies;
            newBook.borrowedBy.clear();

            books.push_back(newBook);
            saveData();

            cout << "Success: New book added to the library!\n";
        }
        else if (choice == 3) {
            string bookID;

            cout << "\n--- REMOVE BOOK ---\n";
            displayBooks();

            cout << "Enter Book ID to remove: ";
            cin >> bookID;

            auto it = find_if(books.begin(), books.end(),
                [&](const Book& book) {
                    return book.id == bookID;
                });

            if (it == books.end()) {
                cout << "Error: Book ID not found.\n";
            }
            else if (!it->borrowedBy.empty()) {
                cout << "Error: This book cannot be removed while it is borrowed.\n";
            }
            else {
                cout << "Book '" << it->title
                     << "' removed successfully.\n";

                books.erase(it);
                saveData();
            }
        }
        else if (choice == 4) {
            long long newID;

            cout << "Enter new Student ID to add: ";
            cin >> newID;

            if (isValidStudentID(newID)) {
                cout << "Error: Student ID " << newID
                     << " is already registered.\n";
            }
            else {
                registeredStudentIDs.push_back(newID);
                saveData();

                cout << "Success: Student ID " << newID
                     << " added successfully!\n";
            }
        }
        else if (choice == 5) {
            long long removeID;

            cout << "Enter Student ID to remove: ";
            cin >> removeID;

            auto it = find(registeredStudentIDs.begin(),
                           registeredStudentIDs.end(),
                           removeID);

            if (it != registeredStudentIDs.end()) {
                registeredStudentIDs.erase(it);
                saveData();

                cout << "Success: Student ID " << removeID
                     << " has been removed successfully!\n";
            }
            else {
                cout << "Error: Student ID " << removeID
                     << " not found in the records.\n";
            }
        }
        else if (choice == 6) {
            cout << "\n--- Registered Student IDs ("
                 << registeredStudentIDs.size()
                 << " Total) ---\n";

            for (size_t i = 0; i < registeredStudentIDs.size(); ++i) {
                cout << registeredStudentIDs[i] << "  ";

                if ((i + 1) % 5 == 0)
                    cout << "\n";
            }

            cout << "\n-----------------------------------------\n";
        }
        else if (choice == 7) {
            string oldPassword, newPassword, confirmPassword;

            cout << "\n--- CHANGE ADMIN PASSWORD ---\n";

            oldPassword = getPassword("Enter Current Admin Password: ");

            if (oldPassword != adminPassword) {
                cout << "Error: Incorrect current password.\n";
                continue;
            }

            showPasswordRule();

            newPassword = getPassword("Enter New Password: ");

            if (!isStrongPassword(newPassword)) {
                cout << "Error: New password does not meet the requirements.\n";
                continue;
            }

            confirmPassword = getPassword("Confirm New Password: ");

            if (newPassword != confirmPassword) {
                cout << "Error: Passwords do not match.\n";
            }
            else {
                adminPassword = newPassword;
                saveData();

                cout << "Admin password changed successfully!\n";
            }
        }
        else if (choice == 8) {
            break;
        }
        else {
            cout << "Invalid choice! Try again.\n";
        }
    }
}

// ---------- Main Function ----------

int main() {
    if (!loadData()) {
        initializeStudentIDs();
        initializeBooks();
        saveData();
    }

    int option;
    while (true) {
        cout << "\n=============================================\n";
        cout << "   DIGITAL LIBRARY MANAGEMENT SYSTEM \n";
        cout << "=============================================\n";
        cout << "1. Student Login\n";
        cout << "2. New Student Registration\n";
        cout << "3. Admin Panel\n";
        cout << "4. Exit Program\n";
        cout << "Select Option: ";
        cin >> option;
        if (option == 1) {
            long long studentID;
            string password;

            cout << "Enter your University Student ID: ";
            cin >> studentID;

            if (!isValidStudentID(studentID)) {
                cout << "Access Denied: Student ID is not registered!\n";
                continue;
            }

            if (!isStudentRegistered(studentID)) {
                cout << "Access Denied: Please register your student account first.\n";
                continue;
            }

            password = getPassword("Enter Password: ");

            bool loginSuccessful = false;

            for (const auto& student : studentAccounts) {
                if (student.studentID == studentID &&
                    student.password == password) {
                    loginSuccessful = true;
                    break;
                }
            }

            if (loginSuccessful) {
                cout << "Login Successful!\n";
                studentMenu(studentID);
            }
            else {
                cout << "Access Denied: Incorrect password!\n";
            }
        }
        else if (option == 2) {
            studentRegistration();
        }
        else if (option == 3) {
            adminMenu();
        }
        else if (option == 4) {
            cout << "Exiting system. Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid Option! Please select again.\n";
        }
    }

    return 0;
}
