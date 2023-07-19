#ifndef QSIMPLESLIDERNAVIGATION_H
#define QSIMPLESLIDERNAVIGATION_H
#include <QTabWidget>
#include <QVariantAnimation>

class TabBarAnimation;
class QSimpleSliderNavigation : public QTabWidget
{
    Q_OBJECT

public:
    QSimpleSliderNavigation(QWidget *parent = Q_NULLPTR);
    ~QSimpleSliderNavigation();
    void setAnimationCurrentValue(int value);
protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *o, QEvent *e);
private:
    void startAnimation(int beginX,int endX,int duration); // 先只考虑横向的
private:
    TabBarAnimation *animation;
    int m_animationX;
};

#endif // QSIMPLESLIDERNAVIGATION_H
