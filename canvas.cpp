#include "canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
    , m_currentColor(Qt::black)
    , m_currentWidth(2)
    , m_currentEraser(false)
    , m_drawing(false)
    , m_isMyTurn(false)
    , m_isDrawer(false)
    , m_roundActive(false)
    , m_samplingDistance(3)
{
    setAttribute(Qt::WA_StaticContents); //优化重绘
    setMouseTracking(false); //只有按下时才在move中收到该事件
    setMinimumSize(600, 400);
}

QSize Canvas::minimumSizeHint() const{
    return QSize(600, 400);
}

//绘制工具

void Canvas::setColor(const QColor &c){
    m_currentColor = c;
    m_currentEraser = false; //选择颜色的时候自动退出橡皮擦
}

void Canvas::setWidth(int w){
    if (w < 1) w = 1;
    m_currentWidth = w;
}

void Canvas::setEraser(bool on){
    m_currentEraser = on;
}

void Canvas::clearCanvas(){
    m_strokes.clear();
    update();
    qDebug() << "Canvas::clearCanvas - 清空画布成功";
}

void Canvas::undoLastStroke(){
    if(!m_strokes.isEmpty()){
        m_strokes.removeLast();
        update();
        qDebug() << "Canvas::undoLastStroke - 撤回成功，剩余笔画数:" << m_strokes.size();
    } else {
        qDebug() << "Canvas::undoLastStroke - 没有笔画可撤回";
    }
}

//回合和权限控制

void Canvas::setRoundActive(bool active){
    m_roundActive = active;
    qDebug() << "Canvas::setRoundActive:" << active;
    update();
}

void Canvas::setDrawer(bool isDrawer){
    m_isDrawer = isDrawer;
    qDebug() << "Canvas::setDrawer:" << isDrawer;
}

void Canvas::setMyTurn(bool myTurn){
    m_isMyTurn = myTurn;
    qDebug() << "Canvas::setMyTurn:" << myTurn;
}

bool Canvas::canDraw() const {
    bool can = m_roundActive && m_isMyTurn && m_isDrawer;
    qDebug() << "Canvas::canDraw() - roundActive:" << m_roundActive
             << "isMyTurn:" << m_isMyTurn
             << "isDrawer:" << m_isDrawer
             << "result:" << can;
    return can;
}

//鼠标绘制事件

void Canvas::mousePressEvent(QMouseEvent *event)
{
    // 检查绘制权限
    if (!canDraw()) {
        qDebug() << "Canvas::mousePressEvent - 没有绘制权限";
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_currentStroke = Stroke{};
        m_currentStroke.color = m_currentColor;
        m_currentStroke.width = m_currentWidth;
        m_currentStroke.eraser = m_currentEraser;

        QPoint pt = event -> pos();
        m_lastPoint = pt;
        m_currentStroke.points.append(pt);
        m_strokes.append(m_currentStroke);

        qDebug() << "Canvas::mousePressEvent - 开始绘制, 颜色:" << m_currentColor
                 << "宽度:" << m_currentWidth << "橡皮擦:" << m_currentEraser;

        emit strokeBegan(m_currentStroke);
        update();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    // 检查绘制权限
    if (!canDraw()) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (m_drawing && (event->buttons() & Qt::LeftButton)) {
        if (m_strokes.isEmpty()) {
            return;
        }
        QPoint pt = event->pos();

        if((pt - m_lastPoint).manhattanLength() < m_samplingDistance){
            return;
        }

        m_lastPoint = pt;
        m_strokes.last().points.append(pt);

        emit strokePointAdded(pt);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    // 检查绘制权限
    if (!canDraw()) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton  && m_drawing) {
        m_drawing = false;
        emit strokeEnded();
        qDebug() << "Canvas::mouseReleaseEvent - 结束绘制";
    }
    QWidget::mouseReleaseEvent(event);
}

//绘制

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), Qt::white);

    // 每一条笔画分别画
    for (const Stroke &s : m_strokes) {
        if (s.points.size() < 2) continue;

        QColor penColor = s.eraser ? Qt::white : s.color;
        QPen pen(penColor, s.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);

        for (int i = 1; i < s.points.size(); ++i){
            p.drawLine(s.points[i - 1], s.points[i]);
        }
    }

    // 在不能绘制时显示提示
    if (!canDraw()) {
        p.setPen(Qt::gray);
        p.setFont(QFont("Arial", 12));
        QString statusText;

        if (!m_roundActive) {
            statusText = "等待回合开始...";
        } else if (!m_isMyTurn) {
            statusText = "不是你的回合";
        } else if (!m_isDrawer) {
            statusText = "你不是画手";
        }

        if (!statusText.isEmpty()) {
            p.drawText(rect(), Qt::AlignCenter, statusText);
        }
    }
}

//同步其他人的绘制

void Canvas::remoteStorkeBegin(const QColor &color, int width, bool eraser){
    qDebug() << "Canvas::remoteStorkeBegin - 颜色:" << color << "宽度:" << width << "橡皮擦:" << eraser;
    Stroke s;
    s.color = color;
    s.width = width;
    s.eraser = eraser;
    m_strokes.append(s);
    m_drawing = true;
    m_lastPoint = QPoint();
}

void Canvas::remoteStrokePoint(const QPoint &pt){
    if (m_strokes.isEmpty()) {
        qDebug() << "Canvas::remoteStrokePoint - 没有笔画数据";
        return;
    }

    if (!m_lastPoint.isNull() && (pt - m_lastPoint).manhattanLength() < m_samplingDistance)
        return;

    m_lastPoint = pt;
    m_strokes.last().points.append(pt);
    qDebug() << "Canvas::remoteStrokePoint - 添加点:" << pt;
    update();
}

void Canvas::remoteStrokeEnd(){
    qDebug() << "Canvas::remoteStrokeEnd";
    m_drawing = false;
}

void Canvas::remoteUndoLastStroke(){
    qDebug() << "Canvas::remoteUndoLastStroke - 远程撤回请求";
    if(!m_strokes.isEmpty()){
        m_strokes.removeLast();
        update();
        qDebug() << "Canvas::remoteUndoLastStroke - 远程撤回成功，剩余笔画数:" << m_strokes.size();
    } else {
        qDebug() << "Canvas::remoteUndoLastStroke - 没有笔画可撤回";
    }
}

void Canvas::remoteClearCanvas(){
    qDebug() << "Canvas::remoteClearCanvas - 远程清空画布请求";
    m_strokes.clear();
    update();
    qDebug() << "Canvas::remoteClearCanvas - 远程清空画布成功";
}
