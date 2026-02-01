# Student Grade Database

A lightweight, command-line C program designed to manage student records for a small class. This application allows users to store student names, record grades for three subjects (Math, Science, English), and calculate averages automatically.

## 📋 Features

* **Add Student Records:** Input a student's name and grades for three subjects.
* *Includes input validation to ensure grades are between 0-100.*


* **View Database:** Display a formatted table of all students, including:
* Calculated Grade Average.
* Pass/Fail remarks (Passing grade: 75.0).


* **Delete Records:** Remove a student by their ID number.
* *Automatically re-indexes remaining students to keep IDs consecutive.*


* **Capacity Management:** Prevents overflow by respecting a defined class limit (default: 5 students).

## 🛠️ Getting Started

### Prerequisites

* A C compiler (GCC, Clang, or MSVC).
* **Operating System:** This program is currently optimized for **Windows** (uses `cls` and `pause` commands).
* *Linux/macOS users: See the [Portability Note](https://www.google.com/search?q=%23-portability-note) below.*



### Compilation

Open your terminal or command prompt and run:

```bash
gcc grade_database.c -o grade_db

```

### Running the Program

```bash
./grade_db

```

## 🎮 Usage Guide

Upon launching, you will be presented with the main menu:

1. **Add Student:**
* Enter the student's name (max 49 characters).
* Enter three integer grades separated by spaces (e.g., `85 90 88`).


2. **View Database:**
* Shows the "Student Records" table.
* Check here to find a Student's **ID** number.


3. **Remove Student:**
* Enter the **ID** number found in the "View Database" screen to delete that record.


4. **Exit:**
* Closes the application.
* *Note: All data is stored in RAM and will be lost upon exiting.*



## ⚙️ Configuration

You can adjust the database limits by modifying the preprocessor constants at the top of the source file:

```c
// Change the maximum number of students
#define CLASS_SIZE 5

// Change the maximum length of a student's name
#define NAME_SIZE 50

```

## 🐧 Portability Note (Linux/macOS)

This code uses Windows-specific system commands (`cls` and `pause`). To compile on Linux or macOS, modify the following lines in the source code:

1. **Replace** `system("cls");` with `system("clear");`
2. **Replace** `system("pause");` with `getchar();` (or remove it entirely).

## 📝 License

This project is open-source and available for educational use.
