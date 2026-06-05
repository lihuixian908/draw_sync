#include "FancyButton.h"

#include <QPainter>
#include <QStyleOptionButton>
#include <QPainterPath>

FancyButton::FancyButton(QWidget *parent)
    : QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFlat(true); // 去掉原生立体边框，让自绘效果干净一点
}

// 设置圆角并刷新
void FancyButton::setRadius(int r)
{
    m_radius = r;
    update();
}

// 可选：以后你想换别的主题色可以用这个
void FancyButton::setBaseColor(const QColor &c)
{
    m_baseColor = c;
    update();
}

void FancyButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1. 拿到按钮当前状态（是否 hover / pressed / disabled）
    QStyleOptionButton option;
    initStyleOption(&option);

    // 2. 基础绘制区域，稍微缩一下避免出界
    QRect rect = this->rect().adjusted(1, 1, -1, -1);

    // 3. 圆角裁剪路径
    QPainterPath clipPath;
    clipPath.addRoundedRect(rect, m_radius, m_radius);

    // 4. 按状态从主题色派生不同颜色
    QColor base = m_baseColor;              // #99cff6
    QColor bg   = base;                     // 背景
    QColor border     = base.darker(115);   // 稍微深一点的边框
    QColor bottomLine = base.darker(130);   // 底部更深一条线

    if (!(option.state & QStyle::State_Enabled)) {
        // Disabled: 整体变浅一点
        bg         = base.darker(110);
        border     = base.darker(130);
        bottomLine = base.darker(120);
    } else if (option.state & QStyle::State_Sunken) {
        // 按下：比原色稍微深一点
        bg = base.darker(110);
    } else if (option.state & QStyle::State_MouseOver) {
        // Hover：比原色稍微亮一点
        bg = base.lighter(110);
    } else {
        // Normal：直接用主题色
        bg = base;
    }

    // 如果是“选中”状态（checkable 按钮），再略微加深一点
    if (option.state & QStyle::State_On) {
        bg = base.darker(115);
    }

    // 5. 画背景 + 边框
    painter.save();
    painter.setClipPath(clipPath);

    painter.setBrush(bg);
    painter.setPen(border);
    painter.drawPath(clipPath);

    // 6. Hover 时叠加一层淡淡的高光蒙版
    if (option.state & QStyle::State_MouseOver) {
        QColor hoverMask(255, 255, 255, 30); // 白色半透明
        painter.setPen(Qt::NoPen);
        painter.setBrush(hoverMask);
        painter.drawPath(clipPath);
    }

    // 7. 底部一条深线，增加一点立体感
    painter.setPen(bottomLine);
    painter.drawLine(rect.bottomLeft(), rect.bottomRight());

    painter.restore();

    // 8. 最后交给 Qt 画文字和图标
    QStyleOptionButton labelOption(option);
    // 文本区域缩小一点，避免贴到圆角
    labelOption.rect = rect.adjusted(4, 2, -4, -4);
    style()->drawControl(QStyle::CE_PushButtonLabel, &labelOption, &painter, this);
}
