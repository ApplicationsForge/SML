#include "motor_state.h"

MotorState::MotorState(int id) :
    m_id(id),
    m_isMoving(false),
    m_initialPos(0.0),
    m_targetPos(0.0),
    m_currentProgress(0)
{

}

int MotorState::id() const
{
    return m_id;
}

bool MotorState::isMoving() const
{
    return m_isMoving;
}

int MotorState::initialPos() const
{
    return m_initialPos;
}

int MotorState::targetPos() const
{
    return m_targetPos;
}

int MotorState::currentPos() const
{
    return m_initialPos + m_currentProgress;
}

void MotorState::setCurrentProgress(int currentProgress)
{
    m_currentProgress = currentProgress;
    ((m_currentProgress + m_initialPos) == m_targetPos) ?
        m_isMoving = false : m_isMoving = true;
}

int MotorState::delay() const
{
    return m_delay;
}

QtJson::JsonObject MotorState::prepareMotorCmd(int targetPos, int feedrate)
{
    m_initialPos = this->currentPos();
    m_targetPos = targetPos;
    m_delay = 1 / (feedrate + 1) + 6;
    m_isMoving = true;
    int posRelative = m_targetPos - m_initialPos;

    QtJson::JsonObject result = {};
    result["steps"] = posRelative;
    result["delay"] = m_delay;
    return result;
}
