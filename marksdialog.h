#ifndef MARKSDIALOG_H
#define MARKSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QPushButton>
#include <QSqlDatabase>

class MarksDialog : public QDialog {
    Q_OBJECT

public:
    explicit MarksDialog(QSqlDatabase &database, QWidget *parent = nullptr);

private slots:
    void loadStudentMarks();
    void addMarks();
    void deleteMarks();
    void calculateCGPA();
    void refreshTable();

private:
    QSqlDatabase &db;
    
    // UI Components
    QComboBox *studentCombo;
    QTableWidget *marksTable;
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QPushButton *calculateBtn;
    
    // For adding marks
    QLineEdit *subjectEdit;
    QSpinBox *marksSpin;
    QSpinBox *maxMarksSpin;
    QComboBox *examTypeCombo;
    
    void setupUI();
    void loadStudentList();
    double calculateStudentCGPA(const QString &rollNo);
};

#endif // MARKSDIALOG_H
