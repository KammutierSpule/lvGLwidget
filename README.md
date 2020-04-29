# lvGLwidget
QT openGL widget to use [LittlevGL](https://github.com/littlevgl/lvgl)

Implements a QOpenGLWidget based class to integrate LittlevGL into a QT application.

# Demo project

On the folder example/ there is a QTcreator project (example.pro) that runs one of LittlevGL demo.


# Instructions to use widget

## Add source code to your project

- Add LittlevGL source code to your QTproject
- Add the lvGLwidget/ source files to your QTproject
- Configure the .pro file to your needs, example:
```
DEFINES += LV_CONF_INCLUDE_SIMPLE
DEFINES += LV_EX_CONF_INCLUDE_SIMPLE

INCLUDEPATH += "../" \
               "../lvgl/" \
               "../lvgl/src" \
               "../lvGLwidget/"
```

## Using form designer
- Add an OpenGL widget (QOpenGLWidget) to your form.
- Select the new widget and Promote widget to "LVGLwidget"
