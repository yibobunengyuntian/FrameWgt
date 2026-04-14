#include "framewgt.h"

FrameWgt::FrameWgt(QWidget *centerWidget)
{
    m_pCenter_widget = centerWidget;
    initialize();
}

FrameWgt::~FrameWgt()
{

}

void FrameWgt::setTitleBarColor(const QColor &color)
{
    m_pTitleBar->setBackgroundColor(color);
}

void FrameWgt::setTitleTextColor(const QColor &color)
{
    m_pTitleBar->setTextColor(color);
}

void FrameWgt::setTitleText(const QString &text)
{
    m_pTitleBar->setTitleText(text);
}

void FrameWgt::setTitleIcon(const QString &path)
{
    m_pTitleBar->setTitleIcon(path);
}

void FrameWgt::setTitleIcon(const QPixmap &icon)
{
    m_pTitleBar->setTitleIcon(icon);
}

void FrameWgt::setBackgroundColor(const QColor &color)
{
    m_bgColor = color;
    if(this->isMaximized() || this->isFullScreen())
    {
        updateRadius(0);
    }
    else
    {
        updateRadius(m_radius);
    }
    updateSize();
}

void FrameWgt::setRadius(const uint &r)
{
    m_radius = r;
    updateRadius(m_radius);
    updateSize();
}

void FrameWgt::setShadowColor(const QColor &color)
{
    m_shadowColor = color;
    update();
}

void FrameWgt::setBlurRadius(const uint &r)
{
    m_blurRadius = r;
    if(this->isMaximized() || this->isFullScreen())
    {
        m_pGridLayout->setContentsMargins(0, 0, 0, 0);
        return;
    }
    else
    {
        // 给出阴影绘制区域
        m_pGridLayout->setContentsMargins(m_blurRadius, m_blurRadius, m_blurRadius, m_blurRadius);
    }
}

void FrameWgt::setHiddenMin(const bool &is)
{
    m_pTitleBar->setHiddenMin(is);
}

void FrameWgt::setHiddenMax(const bool &is)
{
    m_pTitleBar->setHiddenMax(is);
}

void FrameWgt::setHiddenTitleBar(const bool &is)
{
    m_pTitleBar->setHidden(is);
    updateRadius(m_radius);
    updateSize();
}

void FrameWgt::setTitleBarHeight(const uint &h)
{
    m_pTitleBar->setHeight(h);
    updateSize();
}

void FrameWgt::setMinIcon(const QIcon &icon)
{
    m_pTitleBar->setMinIcon(icon);
}

void FrameWgt::setMaxIcon(const QIcon &icon)
{
    m_pTitleBar->setMaxIcon(icon);
}

void FrameWgt::setCloseIcon(const QIcon &icon)
{
    m_pTitleBar->setCloseIcon(icon);
}

void FrameWgt::setTitleTextFont(const QFont &font)
{
    m_pTitleBar->setTitleTextFont(font);
}

void FrameWgt::addWidgetToTitleBar(QWidget *w)
{
    m_pTitleBar->addWidget(w);
}

void FrameWgt::addLayoutToTitleBar(QLayout *layout)
{
    m_pTitleBar->addLayout(layout);
}

void FrameWgt::initialize()
{
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags()); // 隐藏默认边框
    this->setAttribute(Qt::WA_TranslucentBackground, true); // 设置透明背景
    setMouseTracking(true); // 启用鼠标追踪
    // this->resize(400, 300);
    m_pBorder = new QWidget;
    m_pBorder->setCursor(Qt::ArrowCursor);
    m_pTitleBar = new TitleBar(m_pBorder);
    QVBoxLayout *pCenterLayout = new QVBoxLayout;
    pCenterLayout->setContentsMargins(0, 0,0 ,0);
    pCenterLayout->setSpacing(0);
    pCenterLayout->setAlignment(Qt::AlignTop);
    pCenterLayout->addWidget(m_pTitleBar);

    if(m_pCenter_widget){
        this->setTitleText(m_pCenter_widget->windowTitle());
        this->setTitleIcon(m_pCenter_widget->windowIcon().pixmap(64, 64));
        pCenterLayout->addWidget(m_pCenter_widget);
        pCenterLayout->setStretch(1, 1);
        m_pBorder->setCursor(m_pCenter_widget->cursor());
    }
    m_pBorder->setLayout(pCenterLayout);
    m_pGridLayout = new QGridLayout;
    m_pGridLayout->setContentsMargins(m_blurRadius, m_blurRadius, m_blurRadius, m_blurRadius); // 给边框阴影预留位置
    m_pGridLayout->addWidget(m_pBorder);
    this->setLayout(m_pGridLayout);

    setTitleBarColor(Qt::white);
    setRadius(m_radius);

    connect(m_pTitleBar, SIGNAL(closed()), this, SLOT(close()));
}

void FrameWgt::calculateOpflag(QPoint pos)
{
    QCursor cursor = Qt::ArrowCursor;
    m_opFlag = NONE;
    if(pos.x() < m_blurRadius)
    {
        if(pos.y() < m_blurRadius + m_radius)
        {
            m_opFlag = OpFlag::TL;
            cursor = Qt::SizeFDiagCursor;
        }
        else if(pos.y() > this->height() - m_blurRadius - m_radius)
        {
            m_opFlag = OpFlag::BL;
            cursor = Qt::SizeBDiagCursor;
        }
        else
        {
            m_opFlag = OpFlag::Left;
            cursor = Qt::SizeHorCursor;
        }
    }
    else if(pos.x() > this->width() - m_blurRadius)
    {
        if(pos.y() < m_blurRadius + m_radius)
        {
            m_opFlag = OpFlag::TR;
            cursor = Qt::SizeBDiagCursor;
        }
        else if(pos.y() > this->height() - m_blurRadius - m_radius)
        {
            m_opFlag = OpFlag::BR;
            cursor = Qt::SizeFDiagCursor;
        }
        else
        {
            m_opFlag = OpFlag::Right;
            cursor = Qt::SizeHorCursor;
        }
    }
    else
    {
        if(pos.y() < m_blurRadius)
        {
            m_opFlag = OpFlag::Top;
            cursor = Qt::SizeVerCursor;
        }
        else if(pos.y() > this->height() - m_blurRadius)
        {
            m_opFlag = OpFlag::Bottom;
            cursor = Qt::SizeVerCursor;
        }
        else
        {
            m_opFlag = OpFlag::NONE;
            cursor = Qt::ArrowCursor;
        }
    }
    this->setCursor(cursor);
}

void FrameWgt::updateRadius(const uint &r)
{
    m_pBorder->setObjectName("Border");
    QString cr = QString::number(m_bgColor.red());   // 获取红色分量
    QString cg = QString::number(m_bgColor.green()); // 获取绿色分量
    QString cb = QString::number(m_bgColor.blue());  // 获取蓝色分量
    QString ca = QString::number(m_bgColor.alpha()); // 获取透明度分量
    m_pBorder->setStyleSheet(QString("QWidget#Border{border-radius: %1px;background-color: rgba(%2, %3, %4, %5);}").arg(QString::number(r), cr, cg, cb, ca));

    m_pBorder->style()->unpolish(m_pBorder);
    m_pBorder->style()->polish(m_pBorder);
    m_pTitleBar->setRadius(r);
    update();
}

void FrameWgt::updateSize()
{
    if(m_pCenter_widget == nullptr)
    {
        return;
    }

    int w = m_pCenter_widget->maximumWidth() + m_blurRadius;
    int h = m_pCenter_widget->maximumHeight() + m_blurRadius;

    if(!m_pTitleBar->isHidden())
    {
        h += m_pTitleBar->height();
    }

    this->setMaximumSize(qMin(w, QWIDGETSIZE_MAX), qMin(h, QWIDGETSIZE_MAX));

    w = m_pCenter_widget->width() + m_blurRadius;
    h = m_pCenter_widget->height() + m_blurRadius;

    if(!m_pTitleBar->isHidden())
    {
        h += m_pTitleBar->height();
    }
    this->resize(w, h);
}

void FrameWgt::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if(m_pBorder->geometry().contains(pos))
    {
        event->ignore();
        return;
    }
    if(m_opFlag != -1)
    {
        m_isOp = true;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        m_lastPos = event->globalPos();
#else
        m_lastPos = event->globalPosition();
#endif
    }
}

void FrameWgt::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if(!m_isOp)
    {
        calculateOpflag(pos);
    }

    //获取鼠标拖动的位置，
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPointF currPos = event->globalPos();
#else
    QPointF currPos = event->globalPosition();
#endif
    // 计算鼠标按下并拖动的的位移
    QPointF ptemp = currPos - m_lastPos ;
    //保存当前鼠标拖动的位置，用于下一次计算拖动位移
    m_lastPos = currPos;
    event->ignore();
    if(m_isOp)
    {
        if(this->isMaximized()) return;
        int x = 0,y = 0,w = 0,h = 0;
        // 根据拖动的那一条边框，确定拉伸还是缩小窗口。
        switch (m_opFlag) {
        // 左边框被拉伸
        case Left:
            x = this->pos().x() + ptemp.x();
            y = this->pos().y();
            w = this->size().width() - ptemp.x();
            h = this->size().height();
            if(w < this->minimumWidth())
            {
                m_lastPos.setX(this->pos().x());
            }
            break;
            // 右边框被拉伸
        case Right:
            x = this->pos().x();
            y = this->pos().y();
            w = this->size().width() + ptemp.x();
            h = this->size().height();
            if(w < this->minimumWidth())
            {
                m_lastPos.setX(this->pos().x() + this->width());
            }
            break;
            // 上边框被拉伸
        case Top:
            x = this->pos().x();
            y = this->pos().y() + ptemp.y();
            w = this->size().width() ;
            h = this->size().height() - ptemp.y();
            if(h < this->minimumHeight())
            {
                m_lastPos.setY(this->pos().y());
            }
            break;
            // 下边框被拉伸
        case Bottom:
            x = this->pos().x();
            y = this->pos().y();
            w = this->size().width() ;
            h = this->size().height() + ptemp.y();
            if(h < this->minimumHeight())
            {
                m_lastPos.setY(this->pos().y() + this->height());
            }
            break;
            //右上角被拉伸
        case TR:
            x = this->pos().x();
            y = this->pos().y() + ptemp.y();
            w = this->size().width() + ptemp.x() ;
            h = this->size().height() - ptemp.y();
            if(w < this->minimumWidth())
            {
                m_lastPos.setX(this->pos().x() + this->width());
            }
            if(h < this->minimumHeight())
            {
                m_lastPos.setY(this->pos().y());
            }
            break;
            //左上角被拉伸
        case TL:
            x = this->pos().x() + ptemp.x();
            y = this->pos().y() + ptemp.y();
            w = this->size().width() - ptemp.x() ;
            h = this->size().height() - ptemp.y();
            if(w < this->minimumWidth())
            {
                m_lastPos.setX(this->pos().x());
            }
            if(h < this->minimumHeight())
            {
                m_lastPos.setY(this->pos().y());
            }
            break;
            // 右下角被拉伸
        case BR:
            x = this->pos().x();
            y = this->pos().y();
            w = this->size().width() + ptemp.x() ;
            h = this->size().height() + ptemp.y();
            if(w < this->minimumWidth())
            {
                m_lastPos.setX(this->pos().x() + this->width());
            }
            if(h < this->minimumHeight())
            {
                m_lastPos.setY(this->pos().y() + this->height());
            }
            break;
            // 左下角被拉伸
        case BL:
            x = this->pos().x() + ptemp.x();
            y = this->pos().y();
            w = this->size().width() - ptemp.x() ;
            h = this->size().height() + ptemp.y();
            if(w < this->minimumWidth())
            {
                m_lastPos.setX(this->pos().x());
            }
            if(h < this->minimumHeight())
            {
                m_lastPos.setY(this->pos().y() + this->height());
            }
            break;
        default:
            return;
        }
        // 改变窗口的大小和位置。
        if(w < this->minimumWidth())
        {
            x = this->pos().x();
            w = this->minimumWidth();
        }
        if(h < this->minimumHeight())
        {
            y = this->pos().y();
            h = this->minimumHeight();
        }
        if(w > this->maximumWidth())
        {
            x = this->pos().x();
            w = this->maximumWidth();
        }
        if(h > this->maximumHeight())
        {
            y = this->pos().y();
            h = this->maximumHeight();
        }
        this->setGeometry(x,y,w,h);
    }
}

void FrameWgt::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_isOp = false;
}

void FrameWgt::paintEvent(QPaintEvent *event)
{
    if(this->isMaximized() || this->isFullScreen())
    {
        m_pGridLayout->setContentsMargins(0, 0, 0, 0);
        return;
    }
    else
    {
        // 给出阴影绘制区域
        m_pGridLayout->setContentsMargins(m_blurRadius, m_blurRadius, m_blurRadius, m_blurRadius);
    }
    int radius = m_blurRadius + m_radius;
    int width = this->width();
    int height = this->height();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); //抗锯齿
    painter.setPen(Qt::NoPen);

    QPainterPath rectPath;
    rectPath.addRect(this->rect());
    QRect borderRect(m_blurRadius, m_blurRadius, m_pBorder->width(), m_pBorder->height());
    // 创建圆角矩形的路径
    QPainterPath clipPath;
    clipPath.addRoundedRect(borderRect, m_radius, m_radius);
    // 限制绘制范围
    painter.setClipPath(rectPath - clipPath);

    //线性渐变
    QLinearGradient linearGradient;
    linearGradient.setColorAt(0, m_shadowColor);
    linearGradient.setColorAt(m_radius*1.0/radius, m_shadowColor);
    QColor color = m_shadowColor;
    color.setAlpha(0);
    linearGradient.setColorAt(1, color);

    //圆形渐变
    QRadialGradient radialGradient;
    radialGradient.setColorAt(0, m_shadowColor);
    radialGradient.setColorAt(m_radius*1.0/radius, m_shadowColor);
    radialGradient.setColorAt(1, color);

    //左上角
    radialGradient.setCenter(radius, radius); //中心点
    radialGradient.setRadius(radius); //半径
    radialGradient.setFocalPoint(radius, radius); //焦点
    painter.setBrush(radialGradient);
    QRectF rectf(0, 0, radius*2, radius*2);
    QPainterPath path;
    path.moveTo(radius, radius);//移动圆心
    path.arcTo(rectf, 90, 90);
    painter.drawPath(path);   //画路径(扇形)

    //左边
    linearGradient.setStart(radius, height/2);
    linearGradient.setFinalStop(0, height/2);
    painter.setBrush(linearGradient);
    path.clear();
    path.addRect(0, radius, radius, height - radius*2);
    painter.drawPath(path);

    //左下角
    radialGradient.setCenter(radius, height - radius); //中心点
    radialGradient.setRadius(radius); //半径
    radialGradient.setFocalPoint(radius, height - radius); //焦点
    painter.setBrush(radialGradient);
    path.clear();
    path.moveTo(radius, height - radius);//移动圆心
    rectf.setRect(0, height - radius*2, radius*2, radius*2);
    path.arcTo(rectf, 180, 90);
    painter.drawPath(path);   //画路径(扇形)

    //下边
    linearGradient.setStart(width/2, height - radius);
    linearGradient.setFinalStop(width/2, height);
    painter.setBrush(linearGradient);
    path.clear();
    path.addRect(radius, height - radius, width - radius*2, radius);
    painter.drawPath(path);

    //右下角
    radialGradient.setCenter(width - radius, height - radius); //中心点
    radialGradient.setRadius(radius); //半径
    radialGradient.setFocalPoint(width - radius, height - radius); //焦点
    painter.setBrush(radialGradient);
    path.clear();
    path.moveTo(width - radius, height - radius);//移动圆心
    rectf.setRect(width - radius*2, height - radius*2, radius*2, radius*2);
    path.arcTo(rectf, 270, 90);
    painter.drawPath(path);   //画路径(扇形)

    //右边
    linearGradient.setStart(width - radius, height/2);
    linearGradient.setFinalStop(width, height/2);
    painter.setBrush(linearGradient);
    path.clear();
    path.addRect(width - radius, radius, radius, height - radius*2);
    painter.drawPath(path);

    //右上角
    radialGradient.setCenter(width - radius, radius); //中心点
    radialGradient.setRadius(radius); //半径
    radialGradient.setFocalPoint(width - radius, radius); //焦点
    painter.setBrush(radialGradient);
    path.clear();
    path.moveTo(width - radius, radius);//移动圆心
    rectf.setRect(width - radius*2, 0, radius*2, radius*2);
    path.arcTo(rectf, 0, 90);
    painter.drawPath(path);   //画路径(扇形)

    //上边
    linearGradient.setStart(height/2, radius);
    linearGradient.setFinalStop(height/2, 0);
    painter.setBrush(linearGradient);
    path.clear();
    path.addRect(radius, 0, width - radius*2, radius);
    painter.drawPath(path);
}

void FrameWgt::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *stateChangeEvent = static_cast<QWindowStateChangeEvent *>(event);
        Qt::WindowStates oldState = stateChangeEvent->oldState();
        Qt::WindowStates newState = windowState();
        if (newState == Qt::WindowMaximized || newState == Qt::WindowFullScreen) {
            // 窗口被最大化
            updateRadius(0);
        } else if (newState == Qt::WindowMinimized) {
            // 窗口被最小化
            updateRadius(m_radius);
        } else if(oldState == Qt::WindowMaximized || oldState == Qt::WindowMinimized || oldState == Qt::WindowFullScreen)
        {
            // 窗口被还原
            updateRadius(m_radius);
        }
    }
    QWidget::changeEvent(event);
}
