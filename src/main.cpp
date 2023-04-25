#include <QApplication>
#include "ElastixQtGui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ElastixQtGui elastixQtGui;
    elastixQtGui.show();

    return app.exec();
}
