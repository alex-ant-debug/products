QT += widgets
QT += sql
QT += network

FORMS       = mainwindow.ui \
    checkform.ui \
    loginWindow.ui
HEADERS     = mainwindow.h \
              checkform.h \
              loginwindow.h \
              treeitem.h \
              treemodel.h
RESOURCES   =
SOURCES     = mainwindow.cpp \
              checkform.cpp \
              loginwindow.cpp \
              treeitem.cpp \
              treemodel.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/editabletreemodel
INSTALLS += target
