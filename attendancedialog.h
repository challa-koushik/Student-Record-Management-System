#ifndef ATTENDANCEDIALOG_H
#define ATTENDANCEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QSqlDatabase>
#include <QCheckBox>

class AttendanceDialog : public QDialog {
    Q_OBJECT

public:
    explicit AttendanceDialog(QSqlDatabase &database, QWidget *parent = nullptr);

private slots:
    void loadStudents();
    void markAttendance();
    void viewAttendance();
    void markAllPresent();
    void markAllAbsent();
    void calculateAttendanceStats();

private:
    QSqlDatabase &db;
    
    // UI Components
    QComboBox *branchCombo;
    QComboBox *yearCombo;
    QComboBox *subjectCombo;
    QTableWidget *studentTable;
    
    QPushButton *loadBtn;
    QPushButton *saveBtn;
    QPushButton *viewBtn;
    QPushButton *allPresentBtn;
    QPushButton *allAbsentBtn;
    QPushButton *statsBtn;
    
    void setupUI();
    void loadSubjects();
};

#endif // ATTENDANCEDIALOG_H
