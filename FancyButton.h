#ifndef FANCYBUTTON_H
#define FANCYBUTTON_H

#include <QPushButton>
#include <QColor>

class FancyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit FancyButton(QWidget *parent = nullptr);

    // 圆角半径
    void setRadius(int r);
    int radius() const { return m_radius; }

    // 主题色（可选，不调用就默认 #99cff6）
    void setBaseColor(const QColor &c);
    QColor baseColor() const { return m_baseColor; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int   m_radius    = 10;                 // 默认圆角半径
    QColor m_baseColor = QColor("#c9e6fa"); // 默认主题色：淡蓝色
};

#endif // FANCYBUTTON_H
