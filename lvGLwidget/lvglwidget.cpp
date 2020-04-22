#include "lvglwidget.h"
#include "lvgl_wrap.h"
#include <QOpenGLFunctions_2_0>
#include <QMouseEvent>
#include <algorithm>

LVGLwidget::LVGLwidget(QWidget * parent): QOpenGLWidget(parent)
{
    m_pPBO = NULL;
    m_timerElapsed = new QElapsedTimer();
    m_timerToRefresh = new QTimer(this);

    m_timerElapsed->start();

    connect(m_timerToRefresh, SIGNAL(timeout()), this, SLOT(update()));

    m_mousePressEvents.clear();

    setMouseTracking(true);

    LVGLWrap_Init();
}

LVGLwidget::~LVGLwidget()
{
    if(m_pPBO != NULL) {
        m_pPBO->destroy();

        delete m_pPBO;

        m_pPBO = NULL;
    }

    if(m_timerElapsed != NULL) {
        delete m_timerElapsed;

        m_timerElapsed = NULL;
    }

    if(m_timerToRefresh != NULL) {
        delete m_timerToRefresh;

        m_timerToRefresh = NULL;
    }
}

void LVGLwidget::initializeGL()
{
    QOpenGLFunctions_2_0 * f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();

    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void LVGLwidget::resizeGL(int w, int h)
{
}

void LVGLwidget::paintGL()
{
    QOpenGLFunctions_2_0 * f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();

    f->initializeOpenGLFunctions();

    if(m_pPBO == NULL) {
        m_pPBO = new QOpenGLBuffer(QOpenGLBuffer::Type::PixelUnpackBuffer);
        m_pPBO->create();
        m_pPBO->setUsagePattern(QOpenGLBuffer::UsagePattern::StreamDraw);
        m_pPBO->bind();
        m_pPBO->allocate(LV_HOR_RES_MAX * LV_VER_RES_MAX * 4);
        m_pPBO->release();
    }

    f->glClear(GL_COLOR_BUFFER_BIT);

    m_pPBO->bind();

    uint32_t * ptrPBO = (uint32_t *)m_pPBO->map(QOpenGLBuffer::WriteOnly);

    if(m_mousePressEvents.isEmpty() == false) {
        LVGLWrap_MousePressEvent(m_mousePressEvents.takeFirst());
    }

    LVGLWrap_SetMillis((uint32_t)m_timerElapsed->elapsed());

    const uint32_t recallAgainIn = LVGLWrap_Handler(ptrPBO);

    m_pPBO->unmap();

    f->glDrawPixels(LV_HOR_RES_MAX, LV_VER_RES_MAX, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    m_pPBO->release();

    m_timerToRefresh->start(std::min(recallAgainIn, (uint32_t)LV_DISP_DEF_REFR_PERIOD));
}

void LVGLwidget::mouseMoveEvent(QMouseEvent * event)
{
    LVGLWrap_MouseMoveEvent(event->x(), event->y());
    m_timerToRefresh->stop();
    update();
}

void LVGLwidget::mousePressEvent(QMouseEvent * event)
{
    m_mousePressEvents.append(event->buttons() & Qt::LeftButton);
    m_timerToRefresh->stop();
    update();
}

void LVGLwidget::mouseReleaseEvent(QMouseEvent * event)
{
    m_mousePressEvents.append(event->buttons() & Qt::LeftButton);
    m_timerToRefresh->stop();
    update();
}
