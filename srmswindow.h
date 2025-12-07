#ifndef SRMSWINDOW_H
#define SRMSWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QTableWidget>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

enum class UserRole {
    Teacher,
    Student
};

class SRMSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SRMSWindow(QWidget *parent = nullptr);
    ~SRMSWindow();

private slots:
    // Auth
    void onRegister();
    void onLogin();
    void onLogout();

    // Teacher: student management
    void onAddStudent();
    void onEditStudent();
    void onDeleteStudent();

    // Teacher: marks & attendance
    void onManageMarks();
    void onManageAttendance();

    // Teacher: search
    void onSearch();
    void onResetSearch();

private:
    // Database
    QSqlDatabase db;
    void initDatabase();

    // Shared
    QStackedWidget *stackedWidget;

    // Login page
    QWidget   *loginPage;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLabel    *loginInfoLabel;

    // Teacher page
    QWidget        *teacherPage;
    QLabel         *teacherHeaderLabel;
    QLineEdit      *searchBox;
    QTableView     *studentTable;
    QSqlTableModel *studentModel;
    QPushButton    *logoutButtonTeacher;

    // Student page
    QWidget        *studentPage;
    QLabel         *studentHeaderLabel;
    QLabel         *studentDetailsLabel;
    QTableWidget   *studentMarksTable;
    QTableWidget   *studentAttendanceTable;
    QPushButton    *logoutButtonStudent;

    // Current user info
    QString  currentUsername;
    QString  currentRollNo;   // for students (roll_no == user_id)
    UserRole currentRole;

    // UI setup
    void setupLoginUI();
    void setupTeacherUI();
    void setupStudentUI();

    // Helpers
    bool validateLogin(const QString &username,
                       const QString &password,
                       QString &outRollNo,
                       UserRole &outRole);

    void loadStudentRecords();
    void loadStudentMarks(const QString &rollNo);
    void loadStudentAttendance(const QString &rollNo);

    void showStudentDialog(bool isEdit = false);
};

#endif
