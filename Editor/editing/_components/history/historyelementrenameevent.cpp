#include "historyelementrenameevent.h"
#include <editing/_scenes/level/lvl_scene.h>

#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_events_box.h>

HistoryElementRenameEvent::HistoryElementRenameEvent(const int array_id, const QString &oldName, const QString &newName, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_oldName(oldName),
    m_newName(newName)
{}

HistoryElementRenameEvent::~HistoryElementRenameEvent()
{}

QString HistoryElementRenameEvent::getHistoryName()
{
    return tr("Rename Event");
}

void HistoryElementRenameEvent::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    for(int i = 0; i < lvlScene->LvlData->events.size(); i++){
        if(lvlScene->LvlData->events[i].array_id == (unsigned int)m_array_id){
            lvlScene->LvlData->events[i].name = m_oldName;
        }
    }

    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlEvents->ModifyEvent(m_newName, m_oldName);
    MainWinConnect::pMainWin->dock_LvlEvents->setEventsBox();
    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(false);
}

void HistoryElementRenameEvent::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


    for(int i = 0; i < lvlScene->LvlData->events.size(); i++){
        if(lvlScene->LvlData->events[i].array_id == (unsigned int)m_array_id){
            lvlScene->LvlData->events[i].name = m_newName;
        }
    }

    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlEvents->ModifyEvent(m_oldName, m_newName);
    MainWinConnect::pMainWin->dock_LvlEvents->setEventsBox();
    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(false);
}
