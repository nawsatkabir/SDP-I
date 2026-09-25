# 📚 Digital Library Management System

A **console-based Digital Library Management System** developed using **C++** as a university Software Development Project.

The system is designed to manage students, books, borrowing/returning activities, and administrative operations in an organized way.

---

## 🚀 Features

### 👨‍🎓 Student Features

* 🔐 Student Login & Authentication
* 📝 New Student Registration
* 📚 View Available Books
* 🏫 Browse Books by Department
* 🔍 Search Books by Department
* 📖 Borrow Books
* ↩️ Return Books
* 📋 View Borrowed Books
* 👤 View Student Information
* 🔑 Change Student Password
* 🚪 Logout

### 👨‍💼 Admin Features

* 🔐 Admin Authentication
* 📚 View All Books
* ➕ Add New Books
* 🗑️ Remove Books
* 👨‍🎓 Add Student IDs
* ❌ Remove Student IDs
* 📋 View Registered Student IDs
* 🔑 Change Admin Password

---

## 🛠️ Technologies Used

* **Language:** C++
* **Programming Concepts:** OOP, Functions, Structures, Vectors, File Handling
* **Interface:** Console / Command Line
* **IDE:** Visual Studio Code / Code::Blocks / Dev-C++

---

## 📂 Project Structure

```text
SDP-I/
│
├── Digital Library Management System.cpp
└── README.md
```

---

## ⚙️ How to Run

### 1. Clone the repository

```bash
git clone https://github.com/nawsatkabir/SDP-I.git
```

### 2. Open the project

Open the project folder in **Visual Studio Code** or any C++ IDE.

### 3. Compile the program

Using g++:

```bash
g++ "Digital Library Management System.cpp" -o library
```

### 4. Run

```bash
./library
```

On Windows:

```bash
library.exe
```

---

## 🔑 Main System Flow

```text
        ┌─────────────────────────────┐
        │   Digital Library System   │
        └──────────────┬──────────────┘
                       │
          ┌────────────┼────────────┐
          │            │            │
          ▼            ▼            ▼
     Student Login  Registration  Admin Panel
          │            │            │
          ▼            ▼            ▼
     Student Menu   Create Account  Manage Library
          │
     ┌────┼────────┬──────────┐
     │    │        │          │
     ▼    ▼        ▼          ▼
   Books Borrow   Return   Student Info
```

---

## 📖 Book Management

The system stores information such as:

* Book ID
* Book Title
* Author
* Department / Category
* Number of Pages
* Total Copies
* Available Copies
* Borrowing Information

Students can view books according to their department or browse books from all departments.

---

## 💾 Data Management

The system supports saving and loading application data so that important information can be preserved between program executions.

Student accounts, books, registered student IDs, and system settings are managed by the program.

---

## 🎓 Project Purpose

This project was developed as part of a **Software Development Project (SDP)** to practice C++ programming and software development concepts.

It helped us understand:

* Object-oriented programming
* Data structures
* Functions and modular programming
* File handling
* User authentication
* Menu-driven applications
* Basic software design
* Problem solving

---

## 🔮 Future Improvements

Possible future improvements include:

* 🌐 Web-based interface
* 🗄️ Database integration
* 📱 Mobile application
* 🔒 Password encryption
* 📊 Library statistics and reports
* 🔎 Advanced book search
* ⏰ Due-date and fine management
* 👨‍💼 More advanced admin controls

---

## 👨‍💻 Developer

**Nawsat Kabir**

🎓 CSE Student — Bangladesh University of Business and Technology (BUBT)

GitHub: [@nawsatkabir](https://github.com/nawsatkabir)

---

## ⭐ Support

If you find this project useful, consider giving the repository a ⭐.

---

<div align="center">

### 📚 Digital Library Management System

**Built with C++ | Software Development Project**

</div>
