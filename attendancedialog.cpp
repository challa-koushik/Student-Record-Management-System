#include "attendancedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCheckBox>
#include <QAbstractItemView>

AttendanceDialog::AttendanceDialog(QSqlDatabase &database, QWidget *parent)
    : QDialog(parent), db(database)
{
    setWindowTitle("📅 Manage Attendance");
    resize(900, 700);
    setupUI();
    loadSubjects();
}

void AttendanceDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *title = new QLabel("📅 Attendance Management");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; padding: 10px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);
    
    // Filter section
    QGroupBox *filterBox = new QGroupBox("Select Class & Subject");
    QGridLayout *filterLayout = new QGridLayout(filterBox);
    
    filterLayout->addWidget(new QLabel("Branch:"), 0, 0);
    branchCombo = new QComboBox();
    branchCombo->addItems({"All", "CSE", "ECE", "EEE", "MECH", "CIVIL", "IT"});
    filterLayout->addWidget(branchCombo, 0, 1);
    
    filterLayout->addWidget(new QLabel("Year:"), 0, 2);
    yearCombo = new QComboBox();
    yearCombo->addItems({"All", "1", "2", "3", "4"});
    filterLayout->addWidget(yearCombo, 0, 3);
    
    filterLayout->addWidget(new QLabel("Subject:"), 1, 0);
    subjectCombo = new QComboBox();
    subjectCombo->setEditable(true);
    filterLayout->addWidget(subjectCombo, 1, 1);
    
    loadBtn = new QPushButton("📋 Load Students");
    loadBtn->setStyleSheet("background-color: #3498db; color: white; padding: 10px; font-weight: bold;");
    filterLayout->addWidget(loadBtn, 1, 2, 1, 2);
    
    mainLayout->addWidget(filterBox);
    
    // Quick actions
    QHBoxLayout *quickLayout = new QHBoxLayout();
    
    allPresentBtn = new QPushButton("✅ Mark All Present");
    allPresentBtn->setStyleSheet("background-color: #27ae60; color: white; padding: 8px;");
    quickLayout->addWidget(allPresentBtn);
    
    allAbsentBtn = new QPushButton("❌ Mark All Absent");
    allAbsentBtn->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px;");
    quickLayout->addWidget(allAbsentBtn);
    
    quickLayout->addStretch();
    mainLayout->addLayout(quickLayout);
    
    // Student table
    studentTable = new QTableWidget(0, 6);
    studentTable->setHorizontalHeaderLabels({
        "Roll No", "Name", "Branch", "Year", "Present", "Status"
    });
    studentTable->horizontalHeader()->setStretchLastSection(true);
    studentTable->setAlternatingRowColors(true);
    studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(studentTable);
    
    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    
    saveBtn = new QPushButton("💾 Save Attendance");
    saveBtn->setStyleSheet("background-color: #27ae60; color: white; padding: 10px; font-weight: bold;");
    actionLayout->addWidget(saveBtn);
    
    statsBtn = new QPushButton("📊 View Statistics");
    statsBtn->setStyleSheet("background-color: #f39c12; color: white; padding: 10px;");
    actionLayout->addWidget(statsBtn);
    
    actionLayout->addStretch();
    
    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setStyleSheet("background-color: #95a5a6; color: white; padding: 10px;");
    actionLayout->addWidget(closeBtn);
    
    mainLayout->addLayout(actionLayout);
    
    connect(loadBtn, &QPushButton::clicked, this, &AttendanceDialog::loadStudents);
    connect(saveBtn, &QPushButton::clicked, this, &AttendanceDialog::markAttendance);
    connect(allPresentBtn, &QPushButton::clicked, this, &AttendanceDialog::markAllPresent);
    connect(allAbsentBtn, &QPushButton::clicked, this, &AttendanceDialog::markAllAbsent);
    connect(statsBtn, &QPushButton::clicked, this, &AttendanceDialog::calculateAttendanceStats);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void AttendanceDialog::loadSubjects() {
    subjectCombo->addItems({
        "Mathematics", "Physics", "Chemistry",
        "Data Structures", "Algorithms", "DBMS",
        "Operating Systems", "Computer Networks",
        "Software Engineering", "Web Technologies"
    });
}

void AttendanceDialog::loadStudents() {
    QString branch = branchCombo->currentText();
    QString year = yearCombo->currentText();
    
    studentTable->setRowCount(0);
    
    QSqlQuery query(db);
    QString sql = "SELECT roll_no, name, branch, year FROM students WHERE 1=1";
    
    if (branch != "All") {
        sql += " AND branch = '" + branch + "'";
    }
    if (year != "All") {
        sql += " AND year = " + year;
    }
    
    sql += " ORDER BY roll_no";
    
    if (query.exec(sql)) {
        while (query.next()) {
            int row = studentTable->rowCount();
            studentTable->insertRow(row);
            
            QString rollNo = query.value(0).toString();
            QString name = query.value(1).toString();
            QString studentBranch = query.value(2).toString();
            QString studentYear = query.value(3).toString();
            
            studentTable->setItem(row, 0, new QTableWidgetItem(rollNo));
            studentTable->setItem(row, 1, new QTableWidgetItem(name));
            studentTable->setItem(row, 2, new QTableWidgetItem(studentBranch));
            studentTable->setItem(row, 3, new QTableWidgetItem(studentYear));
            
            QCheckBox *checkbox = new QCheckBox();
            checkbox->setChecked(false);
            studentTable->setCellWidget(row, 4, checkbox);
            
            studentTable->setItem(row, 5, new QTableWidgetItem("Absent"));
            
            connect(checkbox, &QCheckBox::stateChanged, [this, row](int state) {
                QString status = (state == Qt::Checked) ? "Present" : "Absent";
                if (studentTable->item(row, 5)) {
                    studentTable->item(row, 5)->setText(status);
                    QColor color = (state == Qt::Checked) ? QColor(39, 174, 96, 50) : QColor(231, 76, 60, 50);
                    for (int col = 0; col < 6; col++) {
                        if (studentTable->item(row, col)) {
                            studentTable->item(row, col)->setBackground(color);
                        }
                    }
                }
            });
        }
        
        if (studentTable->rowCount() == 0) {
            QMessageBox::information(this, "No Students", "No students found for selected filters!");
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to load students: " + query.lastError().text());
    }
}

void AttendanceDialog::markAttendance() {
    if (studentTable->rowCount() == 0) {
        QMessageBox::warning(this, "No Students", "Please load students first!");
        return;
    }
    
    QString subject = subjectCombo->currentText();
    if (subject.isEmpty()) {
        QMessageBox::warning(this, "No Subject", "Please enter a subject!");
        return;
    }
    
    int savedCount = 0;
    QSqlQuery query(db);
    
    for (int row = 0; row < studentTable->rowCount(); row++) {
        QString rollNo = studentTable->item(row, 0)->text();
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(studentTable->cellWidget(row, 4));
        QString status = checkbox->isChecked() ? "Present" : "Absent";
        
        // Check if attendance exists
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT attendance_id FROM attendance "
                          "WHERE roll_no = ? AND subject = ?");
        checkQuery.addBindValue(rollNo);
        checkQuery.addBindValue(subject);
        
        if (checkQuery.exec() && checkQuery.next()) {
            // Update existing
            query.prepare("UPDATE attendance SET status = ? "
                         "WHERE roll_no = ? AND subject = ?");
            query.addBindValue(status);
            query.addBindValue(rollNo);
            query.addBindValue(subject);
        } else {
            // Insert new
            query.prepare("INSERT INTO attendance (roll_no, status, subject) "
                         "VALUES (?, ?, ?)");
            query.addBindValue(rollNo);
            query.addBindValue(status);
            query.addBindValue(subject);
        }
        
        if (query.exec()) {
            savedCount++;
        }
    }
    
    QMessageBox::information(this, "Success",
                            QString("Attendance saved for %1 students!\nSubject: %2")
                            .arg(savedCount)
                            .arg(subject));
}

void AttendanceDialog::viewAttendance() {
    QString branch = branchCombo->currentText();
    QString year = yearCombo->currentText();
    QString subject = subjectCombo->currentText();
    
    studentTable->setRowCount(0);
    
    QSqlQuery query(db);
    QString sql = "SELECT s.roll_no, s.name, s.branch, s.year, "
                  "COALESCE(a.status, 'Not Marked') as status "
                  "FROM students s "
                  "LEFT JOIN attendance a ON s.roll_no = a.roll_no "
                  "AND a.subject = ? "
                  "WHERE 1=1";
    
    if (branch != "All") sql += " AND s.branch = '" + branch + "'";
    if (year != "All") sql += " AND s.year = " + year;
    sql += " ORDER BY s.roll_no";
    
    query.prepare(sql);
    query.addBindValue(subject);
    
    if (query.exec()) {
        while (query.next()) {
            int row = studentTable->rowCount();
            studentTable->insertRow(row);
            
            QString rollNo = query.value(0).toString();
            QString name = query.value(1).toString();
            QString studentBranch = query.value(2).toString();
            QString studentYear = query.value(3).toString();
            QString status = query.value(4).toString();
            
            studentTable->setItem(row, 0, new QTableWidgetItem(rollNo));
            studentTable->setItem(row, 1, new QTableWidgetItem(name));
            studentTable->setItem(row, 2, new QTableWidgetItem(studentBranch));
            studentTable->setItem(row, 3, new QTableWidgetItem(studentYear));
            
            QCheckBox *checkbox = new QCheckBox();
            checkbox->setChecked(status == "Present");
            studentTable->setCellWidget(row, 4, checkbox);
            
            studentTable->setItem(row, 5, new QTableWidgetItem(status));
            
            QColor color;
            if (status == "Present") color = QColor(39, 174, 96, 50);
            else if (status == "Absent") color = QColor(231, 76, 60, 50);
            else color = QColor(255, 255, 255);
            
            for (int col = 0; col < 6; col++) {
                if (studentTable->item(row, col)) {
                    studentTable->item(row, col)->setBackground(color);
                }
            }
        }
    }
}

void AttendanceDialog::markAllPresent() {
    for (int row = 0; row < studentTable->rowCount(); row++) {
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(studentTable->cellWidget(row, 4));
        if (checkbox) {
            checkbox->setChecked(true);
        }
    }
}

void AttendanceDialog::markAllAbsent() {
    for (int row = 0; row < studentTable->rowCount(); row++) {
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(studentTable->cellWidget(row, 4));
        if (checkbox) {
            checkbox->setChecked(false);
        }
    }
}

void AttendanceDialog::calculateAttendanceStats() {
    QString subject = subjectCombo->currentText();
    
    if (subject.isEmpty()) {
        QMessageBox::warning(this, "No Subject", "Please select a subject!");
        return;
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT s.roll_no, s.name, a.status "
                  "FROM students s "
                  "LEFT JOIN attendance a ON s.roll_no = a.roll_no AND a.subject = ? "
                  "ORDER BY s.roll_no");
    query.addBindValue(subject);
    
    QString stats = "📊 Attendance Statistics for " + subject + "\n\n";
    stats += "Roll No\t\tName\t\t\tStatus\n";
    stats += "─────────────────────────────────────────────\n";
    
    int presentCount = 0, absentCount = 0, notMarkedCount = 0;
    
    if (query.exec()) {
        while (query.next()) {
            QString rollNo = query.value(0).toString();
            QString name = query.value(1).toString();
            QString status = query.value(2).toString();
            
            if (status.isEmpty()) {
                status = "Not Marked";
                notMarkedCount++;
            } else if (status == "Present") {
                presentCount++;
            } else {
                absentCount++;
            }
            
            stats += QString("%1\t%2\t\t%3\n")
                    .arg(rollNo, -12)
                    .arg(name, -20)
                    .arg(status);
        }
    }
    
    int total = presentCount + absentCount + notMarkedCount;
    double presentPercentage = (total > 0) ? (presentCount * 100.0 / total) : 0;
    
    stats += "\n─────────────────────────────────────────────\n";
    stats += QString("Total Students: %1\n").arg(total);
    stats += QString("Present: %1\n").arg(presentCount);
    stats += QString("Absent: %1\n").arg(absentCount);
    stats += QString("Not Marked: %1\n").arg(notMarkedCount);
    stats += QString("Overall Attendance: %1%").arg(presentPercentage, 0, 'f', 1);
    
    QMessageBox::information(this, "Attendance Statistics", stats);
}
