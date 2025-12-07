#include "srmswindow.h"
#include "marksdialog.h"
#include "attendancedialog.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QCryptographicHash>
#include <QAbstractItemView>

#include <QSqlQuery>
#include <QSqlError>

// =========================================
// Constructor / Destructor
// =========================================

SRMSWindow::SRMSWindow(QWidget *parent)
    : QMainWindow(parent),
      stackedWidget(new QStackedWidget(this)),
      loginPage(nullptr),
      teacherPage(nullptr),
      studentPage(nullptr),
      studentModel(nullptr),
      logoutButtonTeacher(nullptr),
      logoutButtonStudent(nullptr),
      currentRole(UserRole::Teacher)   // default, will be overwritten on login
{
    setWindowTitle("Student Record Management System");
    resize(1000, 600);

    initDatabase();
    setupLoginUI();
    setupTeacherUI();
    setupStudentUI();

    setCentralWidget(stackedWidget);
    stackedWidget->setCurrentWidget(loginPage);
}

SRMSWindow::~SRMSWindow()
{
}

// =========================================
// Database Setup
// =========================================

void SRMSWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("srms.db");

    if (!db.open()) {
        QMessageBox::critical(this, "DB Error", "Could not open database:\n" + db.lastError().text());
        return;
    }

    QSqlQuery q;

    // Users table: user_id = roll_no for students, anything for teachers
    q.exec("CREATE TABLE IF NOT EXISTS users ("
           " user_id TEXT PRIMARY KEY,"
           " username TEXT UNIQUE,"
           " password TEXT NOT NULL,"
           " role TEXT NOT NULL,"         // 'TEACHER' or 'STUDENT'
           " email TEXT)");

    // Students table
    q.exec("CREATE TABLE IF NOT EXISTS students ("
           " roll_no TEXT PRIMARY KEY,"
           " name TEXT,"
           " email TEXT,"
           " branch TEXT,"
           " year INTEGER,"
           " gender TEXT,"
           " cgpa REAL DEFAULT 0.0)");

    // Marks table
    q.exec("CREATE TABLE IF NOT EXISTS marks ("
           " mark_id INTEGER PRIMARY KEY AUTOINCREMENT,"
           " roll_no TEXT,"
           " subject TEXT,"
           " marks INTEGER,"
           " max_marks INTEGER,"
           " exam_type TEXT)");

    // Attendance table
    q.exec("CREATE TABLE IF NOT EXISTS attendance ("
           " attendance_id INTEGER PRIMARY KEY AUTOINCREMENT,"
           " roll_no TEXT,"
           " subject TEXT,"
           " status TEXT)");

    // Default admin teacher if not exists
    QString hashed = QString(
        QCryptographicHash::hash("admin123", QCryptographicHash::Sha256).toHex());

    q.prepare("SELECT COUNT(*) FROM users WHERE username='admin'");
    q.exec();
    if (q.next() && q.value(0).toInt() == 0) {
        QSqlQuery ins;
        ins.prepare("INSERT INTO users (user_id, username, password, role, email) "
                    "VALUES ('ADMIN', 'admin', ?, 'TEACHER', 'admin@example.com')");
        ins.addBindValue(hashed);
        ins.exec();
    }
}

// =========================================
// Login UI
// =========================================

void SRMSWindow::setupLoginUI()
{
    loginPage = new QWidget;
    QVBoxLayout *main = new QVBoxLayout(loginPage);
    main->setAlignment(Qt::AlignCenter);

    QGroupBox *box = new QGroupBox("Login");
    box->setMaximumWidth(400);
    QFormLayout *form = new QFormLayout(box);

    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Username");

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginBtn = new QPushButton("Login");
    QPushButton *registerBtn = new QPushButton("Create Account");

    loginInfoLabel = new QLabel("Default teacher:\n"
                                "username: admin\npassword: admin123");
    loginInfoLabel->setStyleSheet("color: gray; font-size: 11px;");

    form->addRow("Username:", usernameEdit);
    form->addRow("Password:", passwordEdit);
    form->addRow(loginBtn);
    form->addRow(registerBtn);
    form->addRow(loginInfoLabel);

    main->addWidget(box);

    connect(loginBtn, &QPushButton::clicked, this, &SRMSWindow::onLogin);
    connect(registerBtn, &QPushButton::clicked, this, &SRMSWindow::onRegister);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &SRMSWindow::onLogin);

    stackedWidget->addWidget(loginPage);
}

// =========================================
// Teacher UI
// =========================================

void SRMSWindow::setupTeacherUI()
{
    teacherPage = new QWidget;
    QVBoxLayout *main = new QVBoxLayout(teacherPage);

    // Header
    teacherHeaderLabel = new QLabel("Teacher Portal");
    teacherHeaderLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    main->addWidget(teacherHeaderLabel);

    // Search row
    QHBoxLayout *searchRow = new QHBoxLayout;
    searchBox = new QLineEdit;
    searchBox->setPlaceholderText("Search by roll no / name / email");

    QPushButton *btnSearch = new QPushButton("Search");
    QPushButton *btnReset  = new QPushButton("Reset");

    connect(btnSearch, &QPushButton::clicked, this, &SRMSWindow::onSearch);
    connect(btnReset,  &QPushButton::clicked, this, &SRMSWindow::onResetSearch);

    searchRow->addWidget(searchBox);
    searchRow->addWidget(btnSearch);
    searchRow->addWidget(btnReset);
    searchRow->addStretch();

    main->addLayout(searchRow);

    // Buttons row
    QHBoxLayout *btns = new QHBoxLayout;

    QPushButton *addBtn = new QPushButton("Add Student");
    QPushButton *editBtn = new QPushButton("Edit Student");
    QPushButton *delBtn = new QPushButton("Delete Student");
    QPushButton *markBtn = new QPushButton("Manage Marks");
    QPushButton *attBtn  = new QPushButton("Manage Attendance");

    logoutButtonTeacher = new QPushButton("Logout");
    logoutButtonTeacher->setStyleSheet("background-color:#d9534f; color:white; padding:6px;");

    connect(addBtn,  &QPushButton::clicked, this, &SRMSWindow::onAddStudent);
    connect(editBtn, &QPushButton::clicked, this, &SRMSWindow::onEditStudent);
    connect(delBtn,  &QPushButton::clicked, this, &SRMSWindow::onDeleteStudent);
    connect(markBtn, &QPushButton::clicked, this, &SRMSWindow::onManageMarks);
    connect(attBtn,  &QPushButton::clicked, this, &SRMSWindow::onManageAttendance);
    connect(logoutButtonTeacher, &QPushButton::clicked, this, &SRMSWindow::onLogout);

    btns->addWidget(addBtn);
    btns->addWidget(editBtn);
    btns->addWidget(delBtn);
    btns->addWidget(markBtn);
    btns->addWidget(attBtn);
    btns->addStretch();
    btns->addWidget(logoutButtonTeacher);

    main->addLayout(btns);

    // Student table
    studentTable = new QTableView;
    studentModel = new QSqlTableModel(this, db);
    studentModel->setTable("students");
    studentModel->select();

    studentModel->setHeaderData(0, Qt::Horizontal, "Roll No");
    studentModel->setHeaderData(1, Qt::Horizontal, "Name");
    studentModel->setHeaderData(2, Qt::Horizontal, "Email");
    studentModel->setHeaderData(3, Qt::Horizontal, "Branch");
    studentModel->setHeaderData(4, Qt::Horizontal, "Year");
    studentModel->setHeaderData(5, Qt::Horizontal, "Gender");
    studentModel->setHeaderData(6, Qt::Horizontal, "CGPA");

    studentTable->setModel(studentModel);
    studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    studentTable->setSelectionMode(QAbstractItemView::SingleSelection);
    studentTable->horizontalHeader()->setStretchLastSection(true);

    main->addWidget(studentTable);

    stackedWidget->addWidget(teacherPage);
}

// =========================================
// Student UI
// =========================================

void SRMSWindow::setupStudentUI()
{
    studentPage = new QWidget;
    QVBoxLayout *main = new QVBoxLayout(studentPage);

    studentHeaderLabel = new QLabel("Student Portal");
    studentHeaderLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    main->addWidget(studentHeaderLabel);

    studentDetailsLabel = new QLabel("Details will appear here.");
    studentDetailsLabel->setStyleSheet("font-size: 14px;");
    main->addWidget(studentDetailsLabel);

    // Marks table
    QLabel *marksLabel = new QLabel("Marks:");
    marksLabel->setStyleSheet("font-weight: bold;");
    main->addWidget(marksLabel);

    studentMarksTable = new QTableWidget;
    studentMarksTable->setColumnCount(4);
    studentMarksTable->setHorizontalHeaderLabels(
        {"Subject", "Marks", "Max Marks", "Percent"});
    studentMarksTable->horizontalHeader()->setStretchLastSection(true);
    main->addWidget(studentMarksTable);

    // Attendance table
    QLabel *attLabel = new QLabel("Attendance:");
    attLabel->setStyleSheet("font-weight: bold;");
    main->addWidget(attLabel);

    studentAttendanceTable = new QTableWidget;
    studentAttendanceTable->setColumnCount(2);
    studentAttendanceTable->setHorizontalHeaderLabels({"Subject", "Status"});
    studentAttendanceTable->horizontalHeader()->setStretchLastSection(true);
    main->addWidget(studentAttendanceTable);

    // Logout button
    logoutButtonStudent = new QPushButton("Logout");
    logoutButtonStudent->setStyleSheet("background-color:#d9534f; color:white; padding:6px;");
    connect(logoutButtonStudent, &QPushButton::clicked, this, &SRMSWindow::onLogout);
    main->addWidget(logoutButtonStudent);

    stackedWidget->addWidget(studentPage);
}

// =========================================
// Register
// =========================================

void SRMSWindow::onRegister()
{
    QStringList roles = {"Teacher", "Student"};
    bool ok = false;

    QString roleText = QInputDialog::getItem(
        this, "Role", "Create account as:", roles, 0, false, &ok);
    if (!ok || roleText.isEmpty())
        return;

    QString username = QInputDialog::getText(
        this, "Username", "Enter username:", QLineEdit::Normal, "", &ok);
    if (!ok || username.trimmed().isEmpty())
        return;

    QString password = QInputDialog::getText(
        this, "Password", "Enter password:", QLineEdit::Normal, "", &ok);
    if (!ok || password.isEmpty())
        return;

    QString email = QInputDialog::getText(
        this, "Email", "Enter email:", QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QString userId;
    QString rollNo;

    if (roleText == "Student") {
        // collect student details
        rollNo = QInputDialog::getText(
            this, "Roll No", "Student Roll No:", QLineEdit::Normal, "", &ok);
        if (!ok || rollNo.trimmed().isEmpty())
            return;

        QString name = QInputDialog::getText(
            this, "Name", "Student Name:", QLineEdit::Normal, "", &ok);
        if (!ok || name.trimmed().isEmpty())
            return;

        QString branch = QInputDialog::getText(
            this, "Branch", "Branch (e.g., CSE):", QLineEdit::Normal, "CSE", &ok);
        if (!ok)
            return;

        QString yearStr = QInputDialog::getItem(
            this, "Year", "Year:", {"1", "2", "3", "4"}, 0, false, &ok);
        if (!ok)
            return;

        QString gender = QInputDialog::getItem(
            this, "Gender", "Gender:", {"Male", "Female", "Other"}, 0, false, &ok);
        if (!ok)
            return;

        int year = yearStr.toInt();

        // insert/update into students table
        QSqlQuery qs(db);
        qs.prepare("INSERT OR REPLACE INTO students "
                   "(roll_no, name, email, branch, year, gender) "
                   "VALUES (?, ?, ?, ?, ?, ?)");
        qs.addBindValue(rollNo.trimmed());
        qs.addBindValue(name.trimmed());
        qs.addBindValue(email.trimmed());
        qs.addBindValue(branch.trimmed());
        qs.addBindValue(year);
        qs.addBindValue(gender.trimmed());

        if (!qs.exec()) {
            QMessageBox::critical(this, "Error",
                                  "Failed to save student record:\n" + qs.lastError().text());
            return;
        }

        userId = rollNo.trimmed(); // user_id = roll_no for students
    } else {
        // Teacher
        userId = username.trimmed().toUpper();
    }

    QString hashed = QString(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());

    QString dbRole = (roleText == "Teacher") ? "TEACHER" : "STUDENT";

    QSqlQuery q(db);
    q.prepare("INSERT INTO users (user_id, username, password, role, email) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(userId);
    q.addBindValue(username.trimmed());
    q.addBindValue(hashed);
    q.addBindValue(dbRole);
    q.addBindValue(email.trimmed());

    if (!q.exec()) {
        QMessageBox::critical(this, "Registration Failed",
                              "Failed to create account:\n" + q.lastError().text());
        return;
    }

    QString msg = QString(
        "Account created!\n\nRole: %1\nUsername: %2\nUser ID: %3\n\n"
        "Use these credentials on login screen.")
                      .arg(roleText)
                      .arg(username)
                      .arg(userId);

    QMessageBox::information(this, "Success", msg);

    if (studentModel)
        studentModel->select();
}

// =========================================
// Login / Logout
// =========================================

void SRMSWindow::onLogin()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Failed",
                             "Please enter both username and password.");
        return;
    }

    QString rollNo;
    UserRole role;
    if (!validateLogin(username, password, rollNo, role)) {
        QMessageBox::warning(this, "Login Failed",
                             "Invalid username or password.");
        return;
    }

    currentUsername = username;
    currentRole = role;
    currentRollNo.clear();
    if (role == UserRole::Student)
        currentRollNo = rollNo;

    if (role == UserRole::Teacher) {
        teacherHeaderLabel->setText(
            QString("Teacher Portal - %1").arg(currentUsername));
        loadStudentRecords();
        stackedWidget->setCurrentWidget(teacherPage);
    } else {
        // Student view
        studentHeaderLabel->setText(
            QString("Student Portal - %1").arg(rollNo));

        // load details
        QSqlQuery qs(db);
        qs.prepare("SELECT name, email, branch, year, gender, cgpa "
                   "FROM students WHERE roll_no=?");
        qs.addBindValue(rollNo);
        if (qs.exec() && qs.next()) {
            QString details = QString(
                                  "Name: %1\nEmail: %2\nBranch: %3\nYear: %4\nGender: %5\nCGPA: %6")
                                  .arg(qs.value(0).toString())
                                  .arg(qs.value(1).toString())
                                  .arg(qs.value(2).toString())
                                  .arg(qs.value(3).toInt())
                                  .arg(qs.value(4).toString())
                                  .arg(qs.value(5).toDouble());
            studentDetailsLabel->setText(details);
        } else {
            studentDetailsLabel->setText("No student record found for this roll number.");
        }

        loadStudentMarks(rollNo);
        loadStudentAttendance(rollNo);

        stackedWidget->setCurrentWidget(studentPage);
    }
}

bool SRMSWindow::validateLogin(const QString &username,
                               const QString &password,
                               QString &outRollNo,
                               UserRole &outRole)
{
    QString hashed = QString(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());

    QSqlQuery q(db);
    q.prepare("SELECT user_id, role FROM users "
              "WHERE username=? AND password=?");
    q.addBindValue(username.trimmed());
    q.addBindValue(hashed);

    if (!q.exec() || !q.next())
        return false;

    QString userId = q.value(0).toString();
    QString role = q.value(1).toString();

    if (role == "TEACHER") {
        outRole = UserRole::Teacher;
        outRollNo.clear();
        return true;
    } else if (role == "STUDENT") {
        outRole = UserRole::Student;
        outRollNo = userId;  // user_id == roll_no
        return true;
    }

    return false;
}

void SRMSWindow::onLogout()
{
    currentUsername.clear();
    currentRollNo.clear();

    usernameEdit->clear();
    passwordEdit->clear();

    stackedWidget->setCurrentWidget(loginPage);

    QMessageBox::information(this, "Logout", "You have been logged out successfully.");
}

// =========================================
// Teacher: student records
// =========================================

void SRMSWindow::loadStudentRecords()
{
    if (!studentModel)
        return;

    studentModel->setTable("students");
    studentModel->select();
}

void SRMSWindow::onSearch()
{
    if (!studentModel)
        return;

    QString text = searchBox->text().trimmed();
    if (text.isEmpty()) {
        onResetSearch();
        return;
    }

    QString filter =
        QString("roll_no LIKE '%%1%' OR name LIKE '%%1%' OR email LIKE '%%1%'")
            .arg(text.replace("'", "''"));

    studentModel->setFilter(filter);
    studentModel->select();
}

void SRMSWindow::onResetSearch()
{
    if (!studentModel)
        return;

    searchBox->clear();
    studentModel->setFilter("");
    studentModel->select();
}

void SRMSWindow::onAddStudent()
{
    showStudentDialog(false);
}

void SRMSWindow::onEditStudent()
{
    QModelIndexList rows = studentTable->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        QMessageBox::warning(this, "Edit Student", "Select a student first.");
        return;
    }

    showStudentDialog(true);
}

void SRMSWindow::onDeleteStudent()
{
    QModelIndexList rows = studentTable->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        QMessageBox::warning(this, "Delete Student", "Select a student first.");
        return;
    }

    int row = rows.first().row();
    QString rollNo = studentModel->index(row, 0).data().toString();

    if (QMessageBox::question(this, "Confirm",
                              "Delete student " + rollNo + "?") != QMessageBox::Yes)
        return;

    QSqlQuery q(db);
    q.prepare("DELETE FROM students WHERE roll_no=?");
    q.addBindValue(rollNo);
    q.exec();

    // Also delete user account for that roll_no
    q.prepare("DELETE FROM users WHERE user_id=?");
    q.addBindValue(rollNo);
    q.exec();

    studentModel->select();
}

// Dialog to add / edit student basic info
void SRMSWindow::showStudentDialog(bool isEdit)
{
    QString rollNo, name, email, branch, yearStr, gender;
    bool ok;

    if (isEdit) {
        QModelIndexList rows = studentTable->selectionModel()->selectedRows();
        int row = rows.first().row();

        rollNo = studentModel->index(row, 0).data().toString();
        name   = studentModel->index(row, 1).data().toString();
        email  = studentModel->index(row, 2).data().toString();
        branch = studentModel->index(row, 3).data().toString();
        yearStr= QString::number(studentModel->index(row, 4).data().toInt());
        gender = studentModel->index(row, 5).data().toString();
    } else {
        rollNo = QInputDialog::getText(
            this, "Roll No", "Roll No:", QLineEdit::Normal, rollNo, &ok);
        if (!ok || rollNo.trimmed().isEmpty())
            return;
    }

    name = QInputDialog::getText(
        this, "Name", "Name:", QLineEdit::Normal, name, &ok);
    if (!ok || name.trimmed().isEmpty())
        return;

    email = QInputDialog::getText(
        this, "Email", "Email:", QLineEdit::Normal, email, &ok);
    if (!ok)
        return;

    branch = QInputDialog::getText(
        this, "Branch", "Branch:", QLineEdit::Normal, branch.isEmpty() ? "CSE" : branch, &ok);
    if (!ok)
        return;

    yearStr = QInputDialog::getItem(
        this, "Year", "Year:", {"1", "2", "3", "4"},
        yearStr.isEmpty() ? 0 : yearStr.toInt() - 1, false, &ok);
    if (!ok)
        return;

    gender = QInputDialog::getItem(
        this, "Gender", "Gender:", {"Male", "Female", "Other"},
        gender.isEmpty() ? 0 : 0, false, &ok);
    if (!ok)
        return;

    int year = yearStr.toInt();

    QSqlQuery q(db);
    if (isEdit) {
        q.prepare("UPDATE students SET name=?, email=?, branch=?, year=?, gender=? "
                  "WHERE roll_no=?");
        q.addBindValue(name.trimmed());
        q.addBindValue(email.trimmed());
        q.addBindValue(branch.trimmed());
        q.addBindValue(year);
        q.addBindValue(gender.trimmed());
        q.addBindValue(rollNo.trimmed());
    } else {
        q.prepare("INSERT INTO students (roll_no, name, email, branch, year, gender) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
        q.addBindValue(rollNo.trimmed());
        q.addBindValue(name.trimmed());
        q.addBindValue(email.trimmed());
        q.addBindValue(branch.trimmed());
        q.addBindValue(year);
        q.addBindValue(gender.trimmed());
    }

    if (!q.exec()) {
        QMessageBox::critical(this, "Error",
                              "Failed to save student:\n" + q.lastError().text());
    } else {
        studentModel->select();
        QMessageBox::information(this, "Success", "Student saved.");
    }
}

// =========================================
// Marks & Attendance (Teacher opens dialogs)
// =========================================

void SRMSWindow::onManageMarks()
{
    MarksDialog dialog(db, this);
    dialog.exec();
    studentModel->select(); // refresh CGPA if changed
}

void SRMSWindow::onManageAttendance()
{
    AttendanceDialog dialog(db, this);
    dialog.exec();
}

// =========================================
// Student view: load marks & attendance
// =========================================

void SRMSWindow::loadStudentMarks(const QString &rollNo)
{
    studentMarksTable->setRowCount(0);

    QSqlQuery q(db);
    q.prepare("SELECT subject, marks, max_marks FROM marks WHERE roll_no=?");
    q.addBindValue(rollNo);
    if (!q.exec())
        return;

    while (q.next()) {
        int row = studentMarksTable->rowCount();
        studentMarksTable->insertRow(row);

        QString subject = q.value(0).toString();
        int marks = q.value(1).toInt();
        int maxMarks = q.value(2).toInt();
        double percent = maxMarks > 0 ? (marks * 100.0 / maxMarks) : 0.0;

        studentMarksTable->setItem(row, 0, new QTableWidgetItem(subject));
        studentMarksTable->setItem(row, 1, new QTableWidgetItem(QString::number(marks)));
        studentMarksTable->setItem(row, 2, new QTableWidgetItem(QString::number(maxMarks)));
        studentMarksTable->setItem(row, 3, new QTableWidgetItem(QString::number(percent, 'f', 1) + "%"));
    }

    if (studentMarksTable->rowCount() == 0) {
        studentMarksTable->insertRow(0);
        studentMarksTable->setItem(0, 0, new QTableWidgetItem("No marks entered yet"));
        studentMarksTable->setSpan(0, 0, 1, 4);
    }
}

void SRMSWindow::loadStudentAttendance(const QString &rollNo)
{
    studentAttendanceTable->setRowCount(0);

    QSqlQuery q(db);
    q.prepare("SELECT subject, status FROM attendance WHERE roll_no=?");
    q.addBindValue(rollNo);
    if (!q.exec())
        return;

    while (q.next()) {
        int row = studentAttendanceTable->rowCount();
        studentAttendanceTable->insertRow(row);

        QString subject = q.value(0).toString();
        QString status = q.value(1).toString();

        studentAttendanceTable->setItem(row, 0, new QTableWidgetItem(subject));
        studentAttendanceTable->setItem(row, 1, new QTableWidgetItem(status));
    }

    if (studentAttendanceTable->rowCount() == 0) {
        studentAttendanceTable->insertRow(0);
        studentAttendanceTable->setItem(0, 0, new QTableWidgetItem("No attendance marked yet"));
        studentAttendanceTable->setSpan(0, 0, 1, 2);
    }
}
