#include "qsimpleslidernavigation.h"
#include <QStyleOptionTabWidgetFrame>
#include <QStylePainter>
#include <QMouseEvent>


struct TabBarAnimation : public QVariantAnimation {
    TabBarAnimation(QSimpleSliderNavigation* t) : tabs(t)
    {
        setEasingCurve(QEasingCurve::InOutQuad);
    }
    void updateCurrentValue(const QVariant &current) Q_DECL_OVERRIDE;
private:
    QSimpleSliderNavigation* tabs;
};

void TabBarAnimation::updateCurrentValue(const QVariant &current)
{
    if (tabs)
    {
        tabs->setAnimationCurrentValue(current.toInt());
    }
}

//这里的 QTabWidget::pane {border: none;position: relative;top:2px;} 很重要，将pane的位置下移，这样绘制的下划线才能显示
QSimpleSliderNavigation::QSimpleSliderNavigation(QWidget *parent)
    : QTabWidget(parent)
{
    setStyleSheet("QTabWidget {background-color: rgb(238, 243, 250);} \
                  QTabWidget::pane {border: none;position: relative;top:2px;} \
                  QTabWidget::tab-bar {border: none;} \
                  QTabBar::tab {color:rgb(255,255,255);border: none;min-height: 20px;min-width: 120px;padding-left:8px;padding-right:8px;padding-top:10px;padding-bottom:5px;font:20px;} \
                  QTabBar::tab:hover,QTabBar::tab:selected {color: rgb(88, 187, 228);} \
                  QTabBar::tab:!enabled {background-color: rgb(180, 180, 180);}");
    tabBar()->installEventFilter(this);
    animation = nullptr;
    m_animationX = -1;
}

QSimpleSliderNavigation::~QSimpleSliderNavigation()
{
    if (animation)
    {
        delete animation;
        animation = nullptr;
    }
}

bool QSimpleSliderNavigation::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == tabBar() && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *pMouseEvent = (QMouseEvent *)event;
        if (pMouseEvent->button() == Qt::LeftButton)
        {
            const QPoint pos = pMouseEvent->pos();
            int index = tabBar()->tabAt(pos);
            if (index >= 0)
            {
                int curIndex = tabBar()->currentIndex();
                if (index != curIndex)
                {
                    startAnimation(tabBar()->tabRect(curIndex).x(), tabBar()->tabRect(index).x(), 250);
                }
            }
        }
    }

    return false;
}

void QSimpleSliderNavigation::startAnimation(int beginX,int endX,int duration)
{
    if (!animation)
        animation = new TabBarAnimation(this);
    animation->setStartValue(beginX);
    animation->setEndValue(endX);
    animation->setDuration(duration);
    animation->start();
}

void QSimpleSliderNavigation::setAnimationCurrentValue(int value)
{
    m_animationX = value;
    update();
}

//绘制背景下划线和当前Index的下划线
void QSimpleSliderNavigation::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    int index = tabBar()->currentIndex();
    QRect rect = tabBar()->tabRect(index);
    QStyleOptionTabWidgetFrame option;
    initStyleOption(&option);
    option.lineWidth = 0;
    QStylePainter p(this);
    option.rect = style()->subElementRect(QStyle::SE_TabWidgetTabPane, &option, this);
    p.drawPrimitive(QStyle::PE_FrameTabWidget, option);
    p.fillRect(QRect(option.rect.x(), rect.y() + rect.height(),option.rect.width(),1),QColor(228,231,237));
    int x = (animation && animation->state() == QAbstractAnimation::Running) ? m_animationX : rect.x();
    p.fillRect(QRect(x, rect.y() + rect.height(), rect.width(), 2), QColor(88, 187, 228));
}
