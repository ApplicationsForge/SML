#include "spindelcontrolwidget.h"

SpindelControlWidget::SpindelControlWidget(QString spindelLabel, QString spindelName, size_t rotationsUpperBound, size_t rotationsLowerBound, size_t currentRotations, bool enable, QWidget *parent) :
    QWidget(parent),
    m_spindelLabel(spindelLabel),
    m_spindelName(spindelName),
    m_rotationsUpperBound(rotationsUpperBound),
    m_rotationsLowerBound(rotationsLowerBound),
    m_currentRotations(currentRotations),
    m_enable(enable)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);
    this->layout()->setContentsMargins(0, 5, 0, 5);

    QFrame* container = new QFrame(this);
    container->setLayout(new QVBoxLayout(container));
    container->layout()->setSpacing(0);
    container->layout()->setContentsMargins(0, 0, 0, 0);

    QFrame* topFrame = new QFrame(container);
    topFrame->setLayout(new QHBoxLayout(topFrame));
    topFrame->layout()->setSpacing(0);
    topFrame->layout()->setContentsMargins(0, 0, 0, 0);
        m_titleLabel = new QLabel(m_spindelLabel, container);
        m_titleLabel->setStyleSheet("font-weight: bold; border: 1px solid transparent; background-color: transparent;");
        topFrame->layout()->addWidget(m_titleLabel);

        m_rotationsLCDNumber = new QLCDNumber(container);
        m_rotationsLCDNumber->display(QString::number(m_currentRotations));
        m_rotationsLCDNumber->setSegmentStyle(QLCDNumber::Flat);
        m_rotationsLCDNumber->setFrameShape(QFrame::NoFrame);
        topFrame->layout()->addWidget(m_rotationsLCDNumber);
    container->layout()->addWidget(topFrame);

    m_rotationsSlider = new QSlider(Qt::Horizontal, container);
    m_rotationsSlider->setMaximum(m_rotationsUpperBound);
    m_rotationsSlider->setMinimum(m_rotationsLowerBound);
    m_rotationsSlider->setValue(m_currentRotations);
    connect(m_rotationsSlider, SIGNAL(valueChanged(int)), this, SLOT(onRotationsSliderValueChanged(int)));
    container->layout()->addWidget(m_rotationsSlider);


    m_switchPushButton = new QPushButton(container);
    m_switchPushButton->setObjectName("switchPushButton");
    if(!m_enable)
    {
        m_switchPushButton->setText("Включить");
    }
    else
    {
        m_switchPushButton->setText("Выключить");
    }
    connect(m_switchPushButton, SIGNAL(clicked(bool)), this, SLOT(onSwitchSpindelClicked()));
    container->layout()->addWidget(m_switchPushButton);

    m_warmingUpPushButton = new QPushButton("Прогреть", container);
    container->layout()->addWidget(m_warmingUpPushButton);

    this->layout()->addWidget(container);
}

void SpindelControlWidget::onSwitchSpindelClicked()
{
    if(!m_enable)
    {
        emit switchOn(m_spindelName, m_currentRotations);
    }
    else
    {
        emit switchOff(m_spindelName);
    }
    m_enable = !m_enable;
}

void SpindelControlWidget::onWarmingSpindelUpClicked()
{
    emit switchOn(m_spindelName, m_rotationsLowerBound);
    updateControls(true, m_currentRotations);
}

void SpindelControlWidget::onRotationsSliderValueChanged(int value)
{
    m_currentRotations = value;
    m_rotationsLCDNumber->display(QString::number(m_currentRotations));
}

void SpindelControlWidget::updateControls(bool enable, size_t rotations)
{
    m_enable = enable;
    m_currentRotations = rotations;
    if(!m_enable)
    {
        //m_switchPushButton->setStyleSheet("");
        m_switchPushButton->setText("Включить");
        m_rotationsSlider->setEnabled(true);
    }
    else
    {
        //m_switchPushButton->setStyleSheet("background-color:#b22222; color: #fff;");
        m_switchPushButton->setText("Выключить");
        m_rotationsSlider->setEnabled(false);
        m_rotationsSlider->setValue(m_currentRotations);
        m_rotationsLCDNumber->display(QString::number(m_currentRotations));
    }
}
