#include "ElastixQtGui.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <iostream>

ElastixQtGui::ElastixQtGui(QWidget* parent) :
    QWidget(parent),
    process(nullptr)
{
    QVBoxLayout* mainLayout = new QVBoxLayout;

    int buttonWidth = 200;
    int initialWidth = 600;

    // Set an initial width for the GUI
    setMinimumWidth(initialWidth);

    // Reference image
    QHBoxLayout* referenceImageLayout = new QHBoxLayout;
    referenceImageBtn = new QPushButton("Select Reference Image");
    referenceImageBtn->setFixedWidth(buttonWidth);
    referenceImageLabel = new QLabel("No reference image selected");
    referenceImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    referenceImageLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    referenceImageLayout->addWidget(referenceImageBtn);
    referenceImageLayout->addWidget(referenceImageLabel);
    mainLayout->addLayout(referenceImageLayout);

    // Moving image
    QHBoxLayout* movingImageLayout = new QHBoxLayout;
    movingImageBtn = new QPushButton("Select Moving Image");
    movingImageBtn->setFixedWidth(buttonWidth);
    movingImageLabel = new QLabel("No moving image selected");
    movingImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    movingImageLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    movingImageLayout->addWidget(movingImageBtn);
    movingImageLayout->addWidget(movingImageLabel);
    mainLayout->addLayout(movingImageLayout);

    // Output folder
    QHBoxLayout* outputFolderLayout = new QHBoxLayout;
    outputFolderBtn = new QPushButton("Select Output Folder");
    outputFolderBtn->setFixedWidth(buttonWidth);
    outputFolderLabel = new QLabel("No output folder selected");
    outputFolderLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    outputFolderLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    outputFolderLayout->addWidget(outputFolderBtn);
    outputFolderLayout->addWidget(outputFolderLabel);
    mainLayout->addLayout(outputFolderLayout);

    // Parameter file
    QHBoxLayout* parameterFileLayout = new QHBoxLayout;
    parameterFileComboBox = new QComboBox();
    parameterFileLabel = new QLabel("Parameter File:");
    parameterFileLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    parameterFileComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Populate the QComboBox with .txt files in the "parameter-files" folder
    QDir dir(QCoreApplication::applicationDirPath() + "/parameter-files");
    dir.setNameFilters(QStringList("*.txt"));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    QFileInfoList fileInfoList = dir.entryInfoList();
    if (fileInfoList.isEmpty()) {
        QMessageBox::critical(this, "Error", "No parameter files found in the 'parameter-files' folder. Exiting.");
        QTimer::singleShot(0, QApplication::instance(), &QApplication::quit);
    }
    else {
        for (const QFileInfo& fileInfo : fileInfoList) {
            parameterFileComboBox->addItem(fileInfo.fileName(), fileInfo.filePath());
        }
    }

    parameterFileLayout->addWidget(parameterFileLabel);
    parameterFileLayout->addWidget(parameterFileComboBox);
    mainLayout->addLayout(parameterFileLayout);


    // Register button
    registerBtn = new QPushButton("Register");
    mainLayout->addWidget(registerBtn);

    // Output display
    outputDisplay = new QTextEdit();
    outputDisplay->setReadOnly(true);
    outputDisplay->setWordWrapMode(QTextOption::NoWrap);
    outputDisplay->setStyleSheet("QTextEdit { background-color: black; color: white; }");
    mainLayout->addWidget(outputDisplay);

    setLayout(mainLayout);

    setWindowTitle("Elastix Registration GUI");

    connect(referenceImageBtn, &QPushButton::clicked, this, &ElastixQtGui::selectReferenceImage);
    connect(movingImageBtn, &QPushButton::clicked, this, &ElastixQtGui::selectMovingImage);
    connect(outputFolderBtn, &QPushButton::clicked, this, &ElastixQtGui::selectOutputFolder);
    connect(registerBtn, &QPushButton::clicked, this, &ElastixQtGui::registerImages);
}

ElastixQtGui::~ElastixQtGui() {
    if (process) {
        if (process->state() != QProcess::NotRunning) {
            process->terminate();
            process->waitForFinished();
        }
        delete process;
    }
}

void ElastixQtGui::selectReferenceImage() {
    referenceImagePath = QFileDialog::getOpenFileName(this, "Select Reference Image", "", "Images (*.nii *.nii.gz)");
    if (!referenceImagePath.isEmpty()) {
        referenceImageLabel->setText(referenceImagePath);
    }
}

void ElastixQtGui::selectMovingImage() {
    movingImagePath = QFileDialog::getOpenFileName(this, tr("Select Moving Image"), "", tr("Image Files (*.nii *.nii.gz)"));
    if (!movingImagePath.isEmpty()) {
        movingImageLabel->setText(movingImagePath);
        QFileInfo fileInfo(movingImagePath);
        outputFolderPath = fileInfo.absolutePath();
        outputFolderLabel->setText(outputFolderPath);
    }
}

void ElastixQtGui::selectOutputFolder() {
    outputFolderPath = QFileDialog::getExistingDirectory(this, "Select Output Folder");
    if (!outputFolderPath.isEmpty()) {
        outputFolderLabel->setText(outputFolderPath);
    }
}

void ElastixQtGui::registerImages() {
    if (referenceImagePath.isEmpty() || movingImagePath.isEmpty() || outputFolderPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all the required fields.");
        return;
    }

    QString parameterFilePath = parameterFileComboBox->currentData().toString();

    QStringList arguments;
    arguments << "-f" << referenceImagePath
        << "-m" << movingImagePath
        << "-out" << outputFolderPath
        << "-p" << parameterFilePath;

    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, this, &ElastixQtGui::handleReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError, this, &ElastixQtGui::handleReadyReadStandardError);
    connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
        this, &ElastixQtGui::handleFinished);

    process->start("elastix.exe", arguments);
}

void ElastixQtGui::handleReadyReadStandardOutput() {
    QProcess* process = qobject_cast<QProcess*>(sender());
    if (process) {
        QByteArray output = process->readAllStandardOutput();
        if (!output.isEmpty()) {
            outputDisplay->append(QString::fromStdString(output.toStdString()));
        }
    }
}

void ElastixQtGui::handleReadyReadStandardError() {
    QProcess* process = qobject_cast<QProcess*>(sender());
    if (process) {
        QByteArray errorOutput = process->readAllStandardError();
        if (!errorOutput.isEmpty()) {
            outputDisplay->append(QString::fromStdString(errorOutput.toStdString()));
        }
    }
}

void ElastixQtGui::handleFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            QMessageBox::information(this, "Success", "Image registration completed successfully.");
        }
        else {
            QMessageBox::critical(this, "Error", QString("Image registration failed with exit code %1.").arg(exitCode));
        }
    }
    else {
        QMessageBox::critical(this, "Error", "Image registration process crashed.");
    }

    if (process) {
        delete process;
        process = nullptr;
    }
}
