# SRMS 
The Student Record Management System (SRMS) is a GUI-based desktop app built using Qt, C++, SQLite, and CMake.
It helps teachers maintain student data and allows students to log in and view their own academic details.

This system includes two major portals:

👨‍🏫 Teacher Portal: Full control over managing records

🎓 Student Portal: Secure view-only dashboard


### Login Screen
```
BEFORE:                         AFTER:
┌─────────────────┐            ┌─────────────────┐
│ Username: _____ │            │ Username: _____ │
│ Password: _____ │            │ Password: _____ │
│ Role: [▼]       │            │                 │  ← REMOVED
│ [LOGIN]         │            │ [LOGIN]         │
│ [Create Account]│            │ [Create Student]│
└─────────────────┘            └─────────────────┘
```

### Teacher Portal
```
BEFORE:                                  AFTER:
┌────────────────────────────────┐      ┌────────────────────────────┐
│ Search: [______] [🔍] [Clear]  │      │                            │ ← REMOVED
│ Branch: [▼] Year: [▼]          │      │                            │ ← REMOVED
│ ───────────────────────────────│      │ ──────────────────────────│
│ [Add] [Edit] [Delete]          │      │ [Add] [Edit] [Delete]      │
│ [Marks] [Attendance]           │      │ [Marks] [Attendance]       │
│ [Import CSV] [Export CSV]      │      │                            │ ← REMOVED
│ ───────────────────────────────│      │ ──────────────────────────│
│ Roll│Name│Email│Branch│Year│   │      │ Roll│Name│Branch│Year│CGPA│
│ Phone│Admission│CGPA           │      │                            │
└────────────────────────────────┘      └────────────────────────────┘
                                         Cleaner, fewer columns!
```

### Student Portal
```
NEW MINIMAL DESIGN:
┌─────────────────────────────────────┐
│         Student Portal              │
│                           [Logout]  │
├─────────────────────────────────────┤
│ ┌─ My Information ─────────────┐    │
│ │ Name: Koushik Challa          │   │
│ │ Roll No: AP24110120           │   │
│ │ Email: koushik@srmap.edu.in   │   │
│ │ Branch: CSE                   │   │
│ │ Year: 2                       │   │
│ │ Gender: Male                  │   │
│ │ CGPA: 8.5                     │   │
│ └───────────────────────────────┘   │
│                                     │
│ 📊 My Marks                         │
│ ┌───────────────────────────────┐   │
│ │ Subject │ Marks │ Max │ %     │   │
│ │─────────┼───────┼─────┼───────│   │
│ │ DS      │ 85    │ 100 │ 85%   │   │
│ │ DBMS    │ 90    │ 100 │ 90%   │   │
│ └───────────────────────────────┘   │
│                                     │
│ 📅 My Attendance                    │
│ ┌───────────────────────────────┐   │
│ │ Subject      │ Status         │   │
│ │──────────────┼────────────────│   │
│ │ DS           │ Present        │   │  ← Green
│ │ DBMS         │ Absent         │   │  ← Red
│ └───────────────────────────────┘   │
└─────────────────────────────────────┘
Simple, clean, no clutter!
```

---

## 🚀 How to Build

### Step 1: Replace Files
Make sure you have these 8 files:
1. `main.cpp`
2. `srmswindow.h`
3. `srmswindow.cpp` 
4. `marksdialog.h` 
5. `marksdialog.cpp` 
6. `attendancedialog.h` 
7. `attendancedialog.cpp` 
8. `CMakeLists.txt` 

### Step 2: Clean Build
```bash
rm -rf build srms.db  # Remove old database too!
mkdir build && cd build
cmake ..
make
./srms-enhanced
```

---

## 📝 Testing Guide

### Test 1: Teacher Login
```
1. Username: admin
2. Password: admin123
3. Click LOGIN
Will show Teacher Portal with student table
```

### Test 2: Create Student Account
```
1. Click "Create Student Account"
2. Fill all details:
   - Roll No: AP001
   - Username: john
   - Password: john123
   - Name: John Doe
   - Email: john@test.com
   - Branch: CSE
   - Year: 2
   - Gender: Male
3. Click OK through all dialogs
✅ Account created message appears
```

### Test 3: Student Login
```
1. Username: john
2. Password: john123
3. Click LOGIN
✅ Should show Student Portal with:
   - Personal info card
   - Empty marks table (no marks yet)
   - Empty attendance table (no attendance yet)
```

### Test 4: Teacher Adds Marks
```
1. Login as teacher
2. Click "Manage Marks"
3. Select student: AP001 - John Doe
4. Add marks:
   - Subject: Data Structures
   - Marks: 85 / 100
   - Exam Type: Mid-Term
5. Click Add
6. Click Calculate CGPA
✅ CGPA calculated and updated
```

### Test 5: Teacher Marks Attendance
```
1. Click "Manage Attendance"
2. Branch: CSE, Year: 2
3. Subject: Data Structures
4. Click Load Students
5. Check John's checkbox (mark present)
6. Click Save
✅ Attendance saved
```

### Test 6: Student Views Data
```
1. Logout teacher
2. Login as john/john123
✅ Should now see:
   - CGPA: 8.5 (or calculated value)
   - Marks table with Data Structures entry
   - Attendance table showing Present
```

---

## 💡 What Makes It Better

### For Students:
1. **Clean Interface** - No confusing options
2. **All Info Visible** - Credentials displayed clearly
3. **Easy to Understand** - Just marks and attendance
4. **No Clutter** - Removed unnecessary features

### For Teachers:
1. **Focused Tools** - Only essential buttons
2. **Quick Access** - Direct student management
3. **Cleaner Table** - Fewer columns, easier to read
4. **Simplified Forms** - Less data entry

### For Developers:
1. **Simpler Code** - Less complexity
2. **Easier Maintenance** - Fewer features to maintain
3. **Clear Data Model** - Straightforward schema
4. **Better Performance** - Less data to process

---

srms/
│── src/
│   ├── srmswindow.h
│   ├── srmswindow.cpp
│   ├── marksdialog.h
│   ├── marksdialog.cpp
│   ├── attendancedialog.h
│   ├── attendancedialog.cpp
│   └── main.cpp
│
│── CMakeLists.txt
│── README.md
│── srms.db (auto created)
└── build/  (generated)

---
To run:
mkdir -p build
cd build
cmake ..
make -j4
./srms
---
## 📊 Final Statistics

- Best use for enterprises
- Teachers can maintain record
- Can act as an ERP
- Mainting more number of students is easier
- Simple to Use

---


**Your SRMS is now cleaner, simpler, and student-friendly!** 🎉



