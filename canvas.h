#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QList>
#include <QVector>

// 笔画数据结构
struct Stroke {
    QColor color;
    int width;
    bool eraser;
    QVector<QPoint> points;

    Stroke() : color(Qt::black), width(2), eraser(false) {}
};

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);

    // 尺寸相关
    QSize minimumSizeHint() const override;

    // 绘制工具设置
    void setColor(const QColor &c);
    void setWidth(int w);
    void setEraser(bool on);

    // 画布操作
    void clearCanvas();
    void undoLastStroke();

    // 回合和权限控制
    void setRoundActive(bool active);
    void setDrawer(bool isDrawer);
    void setMyTurn(bool myTurn);
    bool isRoundActive() const { return m_roundActive; }

    // 远程同步绘制
    void remoteStorkeBegin(const QColor &color, int width, bool eraser);
    void remoteStrokePoint(const QPoint &pt);
    void remoteStrokeEnd();
    void remoteUndoLastStroke();
    void remoteClearCanvas();

signals:
    // 绘制事件信号
    void strokeBegan(const Stroke &s);
    void strokePointAdded(const QPoint &pt);
    void strokeEnded();

    // 状态变化信号（可选）
    void roundStateChanged(bool active);
    void drawerChanged(bool isDrawer);

protected:
    // 重写事件处理函数
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // 绘制权限检查
    bool canDraw() const;

    // 当前绘制状态
    QColor m_currentColor;
    int m_currentWidth;
    bool m_currentEraser;
    bool m_drawing;

    // 笔画数据存储
    QList<Stroke> m_strokes;
    Stroke m_currentStroke;

    // 绘制控制
    QPoint m_lastPoint;
    int m_samplingDistance = 3; // 采样距离，减少点数

    // 回合和权限控制
    bool m_isMyTurn = false;
    bool m_isDrawer = false;
    bool m_roundActive = false;
};

#endif // CANVAS_H
