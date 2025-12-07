#include "marksdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QAbstractItemView>

MarksDialog::MarksDialog(QSqlDatabase &database, QWidget *parent)
    : QDialog(parent), db(database)
{
    setWindowTitle("📊 Manage Student Marks");
    resize(800, 600);
    setupUI();
    loadStudentList();
}

void MarksDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *title = new QLabel("📊 Marks Management");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; padding: 10px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);
    
    // Student selection
    QGroupBox *selectionBox = new QGroupBox("Select Student");
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionBox);
    
    selectionLayout->addWidget(new QLabel("Student:"));
    studentCombo = new QComboBox();
    studentCombo->setMinimumWidth(300);
    selectionLayout->addWidget(studentCombo);
    
    QPushButton *loadBtn = new QPushButton("Load Marks");
    loadBtn->setStyleSheet("background-color: #3498db; color: white; padding: 8px 15px;");
    selectionLayout->addWidget(loadBtn);
    
    mainLayout->addWidget(selectionBox);
    
    // Add marks area
    QGroupBox *addBox = new QGroupBox("Add New Marks");
    QFormLayout *addLayout = new QFormLayout(addBox);
    
    subjectEdit = new QLineEdit();
    subjectEdit->setPlaceholderText("e.g., Data Structures, DBMS");
    addLayout->addRow("Subject:", subjectEdit);
    
    QHBoxLayout *marksLayout = new QHBoxLayout();
    marksSpin = new QSpinBox();
    marksSpin->setRange(0, 100);
    marksSpin->setSuffix(" marks");
    marksLayout->addWidget(marksSpin);
    
    marksLayout->addWidget(new QLabel("out of"));
    
    maxMarksSpin = new QSpinBox();
    maxMarksSpin->setRange(1, 100);
    maxMarksSpin->setValue(100);
    maxMarksSpin->setSuffix(" marks");
    marksLayout->addWidget(maxMarksSpin);
    
    addLayout->addRow("Marks:", marksLayout);
    
    examTypeCombo = new QComboBox();
    examTypeCombo->addItems({"Mid-Term", "End-Term", "Assignment", "Quiz", "Project"});
    addLayout->addRow("Exam Type:", examTypeCombo);
    
    addBtn = new QPushButton("➕ Add Marks");
    addBtn->setStyleSheet("background-color: #27ae60; color: white; padding: 10px; font-weight: bold;");
    addLayout->addRow("", addBtn);
    
    mainLayout->addWidget(addBox);
    
    // Marks table
    marksTable = new QTableWidget(0, 6);
    marksTable->setHorizontalHeaderLabels({
        "ID", "Subject", "Marks", "Max Marks", "Percentage", "Exam Type"
    });
    marksTable->horizontalHeader()->setStretchLastSection(true);
    marksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    marksTable->setAlternatingRowColors(true);
    marksTable->setColumnHidden(0, true);
    mainLayout->addWidget(marksTable);
    
    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    
    deleteBtn = new QPushButton("🗑️ Delete Selected");
    deleteBtn->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px;");
    actionLayout->addWidget(deleteBtn);
    
    calculateBtn = new QPushButton("🧮 Calculate CGPA");
    calculateBtn->setStyleSheet("background-color: #f39c12; color: white; padding: 8px;");
    actionLayout->addWidget(calculateBtn);
    
    actionLayout->addStretch();
    
    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px;");
    actionLayout->addWidget(closeBtn);
    
    mainLayout->addLayout(actionLayout);
    
    connect(loadBtn, &QPushButton::clicked, this, &MarksDialog::loadStudentMarks);
    connect(addBtn, &QPushButton::clicked, this, &MarksDialog::addMarks);
    connect(deleteBtn, &QPushButton::clicked, this, &MarksDialog::deleteMarks);
    connect(calculateBtn, &QPushButton::clicked, this, &MarksDialog::calculateCGPA);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void MarksDialog::loadStudentList() {
    studentCombo->clear();
    
    QSqlQuery query(db);
    query.exec("SELECT roll_no, name FROM students ORDER BY roll_no");
    
    while (query.next()) {
        QString rollNo = query.value(0).toString();
        QString name = query.value(1).toString();
        studentCombo->addItem(rollNo + " - " + name, rollNo);
    }
}

void MarksDialog::loadStudentMarks() {
    if (studentCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "No Student", "Please select a student first!");
        return;
    }
    
    QString rollNo = studentCombo->currentData().toString();
    marksTable->setRowCount(0);
    
    QSqlQuery query(db);
    query.prepare("SELECT mark_id, subject, marks, max_marks, exam_type FROM marks WHERE roll_no = ?");
    query.addBindValue(rollNo);
    
    if (query.exec()) {
        while (query.next()) {
            int row = marksTable->rowCount();
            marksTable->insertRow(row);
            
            int markId = query.value(0).toInt();
            QString subject = query.value(1).toString();
            int marks = query.value(2).toInt();
            int maxMarks = query.value(3).toInt();
            QString examType = query.value(4).toString();
            
            double percentage = (marks * 100.0) / maxMarks;
            
            marksTable->setItem(row, 0, new QTableWidgetItem(QString::number(markId)));
            marksTable->setItem(row, 1, new QTableWidgetItem(subject));
            marksTable->setItem(row, 2, new QTableWidgetItem(QString::number(marks)));
            marksTable->setItem(row, 3, new QTableWidgetItem(QString::number(maxMarks)));
            marksTable->setItem(row, 4, new QTableWidgetItem(QString::number(percentage, 'f', 1) + "%"));
            marksTable->setItem(row, 5, new QTableWidgetItem(examType));
        }
    }
    
    if (marksTable->rowCount() == 0) {
        QMessageBox::information(this, "No Marks", "No marks found for this student!");
    }
}

void MarksDialog::addMarks() {
    if (studentCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "No Student", "Please select a student first!");
        return;
    }
    
    QString subject = subjectEdit->text().trimmed();
    if (subject.isEmpty()) {
        QMessageBox::warning(this, "Empty Subject", "Please enter a subject name!");
        return;
    }
    
    QString rollNo = studentCombo->currentData().toString();
    int marks = marksSpin->value();
    int maxMarks = maxMarksSpin->value();
    QString examType = examTypeCombo->currentText();
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO marks (roll_no, subject, marks, max_marks, exam_type) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(rollNo);
    query.addBindValue(subject);
    query.addBindValue(marks);
    query.addBindValue(maxMarks);
    query.addBindValue(examType);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", "Marks added successfully!");
        subjectEdit->clear();
        marksSpin->setValue(0);
        loadStudentMarks();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add marks: " + query.lastError().text());
    }
}

void MarksDialog::deleteMarks() {
    int row = marksTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a marks entry to delete!");
        return;
    }
    
    int markId = marksTable->item(row, 0)->text().toInt();
    
    int reply = QMessageBox::question(this, "Confirm Delete",
                                     "Delete this marks entry?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        QSqlQuery query(db);
        query.prepare("DELETE FROM marks WHERE mark_id = ?");
        query.addBindValue(markId);
        
        if (query.exec()) {
            QMessageBox::information(this, "Success", "Marks deleted!");
            loadStudentMarks();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete: " + query.lastError().text());
        }
    }
}

void MarksDialog::calculateCGPA() {
    if (studentCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "No Student", "Please select a student first!");
        return;
    }
    
    QString rollNo = studentCombo->currentData().toString();
    double cgpa = calculateStudentCGPA(rollNo);
    
    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE students SET cgpa = ? WHERE roll_no = ?");
    updateQuery.addBindValue(cgpa);
    updateQuery.addBindValue(rollNo);
    updateQuery.exec();
    
    QMessageBox::information(this, "CGPA Calculated",
                            QString("Student CGPA: %1 / 10.0\n\nCGPA updated in student records!").arg(cgpa, 0, 'f', 2));
}

double MarksDialog::calculateStudentCGPA(const QString &rollNo) {
    QSqlQuery query(db);
    query.prepare("SELECT marks, max_marks FROM marks WHERE roll_no = ?");
    query.addBindValue(rollNo);
    
    double totalPercentage = 0;
    int count = 0;
    
    if (query.exec()) {
        while (query.next()) {
            int marks = query.value(0).toInt();
            int maxMarks = query.value(1).toInt();
            double percentage = (marks * 100.0) / maxMarks;
            totalPercentage += percentage;
            count++;
        }
    }
    
    if (count == 0) return 0.0;
    
    double avgPercentage = totalPercentage / count;
    return avgPercentage / 10.0;
}

void MarksDialog::refreshTable() {
    loadStudentMarks();
}
