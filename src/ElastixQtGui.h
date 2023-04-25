#ifndef ELASTIX_QT_GUI_H
#define ELASTIX_QT_GUI_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

 /**
  * @class   ElastixQtGui
  * 
  * @brief   The ElastixQtGui class provides a simple GUI for registering two images using Elastix.
  *
  * @details ElastixQtGui provides a simple Qt-based GUI application for calling the Elastix image
  *          registration package. The GUI allows users to select a reference image, a moving image,
  *          an output folder, and a registration parameter file.
  *
  *          The registration parameter file is selected using a QComboBox. This is populated from
  *          the `*.txt` files stored in the "parameter-files" folder. If no files are found, a
  *          warning is displayed, and the application exits.
  *
  *          When the "Register" button is selected, the elastix.exe command is called using a
  *          QProcess with the provided options. The command output is displayed within the
  *          GUI using a QTextEdit widget.
  */
class ElastixQtGui : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs an ElastixQtGui QWidget.
     * @param parent The parent QWidget, default value is nullptr.
     */
    explicit ElastixQtGui(QWidget* parent = nullptr);

    /**
     * @brief Destructor for the ElastixQtGui class.
     */
    ~ElastixQtGui();

private slots:
    /**
     * @brief Opens a file dialog to select the reference image and updates the reference image label.
     */
    void selectReferenceImage();

    /**
     * @brief Opens a file dialog to select the moving image and updates the moving image label.
     */
    void selectMovingImage();

    /**
     * @brief Opens a folder dialog to select the output folder and updates the output folder label.
     */
    void selectOutputFolder();

    /**
     * @brief Starts the image registration process using elastix.exe with the specified options.
     */
    void registerImages();

    /**
     * @brief Reads and appends the standard output of the elastix.exe process to the output display.
     */
    void handleReadyReadStandardOutput();

    /**
     * @brief Reads and appends the standard error output of the elastix.exe process to the output display.
     */
    void handleReadyReadStandardError();

    /**
     * @brief Handles the elastix.exe process completion and displays a message based on the exit status.
     * @param exitCode The exit code of the elastix.exe process.
     * @param exitStatus The exit status of the elastix.exe process.
     */
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QPushButton* referenceImageBtn;
    QPushButton* movingImageBtn;
    QPushButton* outputFolderBtn;
    QPushButton* registerBtn;

    QComboBox* parameterFileComboBox;

    QLabel* referenceImageLabel;
    QLabel* movingImageLabel;
    QLabel* outputFolderLabel;
    QLabel* parameterFileLabel;

    QTextEdit* outputDisplay;

    QString referenceImagePath;
    QString movingImagePath;
    QString outputFolderPath;

    QProcess* process;
};

#endif // ELASTIX_QT_GUI_H
