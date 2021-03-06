/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDir>

#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>
#include <graphics/window.h>
#include <gui/pge_msgbox.h>
#include <gui/pge_menubox.h>
#include <gui/pge_textinputbox.h>
#include <settings/global_settings.h>
#include <data_configs/config_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <gui/pge_questionbox.h>

#include "../scene_title.h"

void TitleScene::processMenu()
{
    if(doExit) return;

    //Waiter in process
    switch(_currentMenu)
    {
        case menu_playlevel_wait:
        case menu_playbattle_wait:
        case menu_playepisode_wait:
            if(filefind_finished) {
                switch(_currentMenu)
                {
                    case menu_playepisode_wait: setMenu(menu_playepisode); return;
                    case menu_playlevel_wait: setMenu(menu_playlevel); return;
                    case menu_playbattle_wait: setMenu(menu_playbattle); return;
                    default: break;
                }
                return;
            }
            if(menu.isAccepted()) { menu.resetState(); }
            return;
            break;
        default:
            break;
    }

    if(!menu.isSelected()) return;

    if(menu.isAccepted())
    {
        menustates[_currentMenu].first = menu.currentItemI();
        menustates[_currentMenu].second = menu.offset();

        QString value = menu.currentItem().value;
        switch(_currentMenu)
        {
            case menu_main:
                if(value=="game1p")
                {
                    numOfPlayers=1;
                    menuChain.push(_currentMenu);
                    setMenu(menu_playepisode_wait);
                }
                else
                if(value=="game2p")
                {
                    numOfPlayers=2;
                    menuChain.push(_currentMenu);
                    setMenu(menu_playepisode_wait);
                }
                else
                if(value=="playlevel")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_playlevel_wait);
                }
                else
                if(value=="Options")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_options);
                }
                else
                if(value=="Exit")
                {
                    ret = ANSWER_EXIT;
                    fader.setNull();
                    doExit=true;
                }
                else
                {
                    PGE_MsgBox msgBox(this, QString("Sorry, is not implemented yet..."),
                                      PGE_MsgBox::msg_warn);
                    fader.setRatio(0.5);
                    PGE_Window::setCursorVisibly(true);
                    msgBox.exec();
                    PGE_Window::setCursorVisibly(false);
                    fader.setNull();
                    menu.resetState();
                }
            break;
            case menu_playepisode:
                {
                    if(value=="noworlds")
                    {
                        //do nothing!
                        menu.resetState();
                    }
                    else
                    {
                        result_episode.worldfile = value;
                        result_episode.character = 0;
                        result_episode.savefile = "save1.savx";
                        if(numOfPlayers>1)
                            ret = ANSWER_PLAYEPISODE_2P;
                        else
                            ret = ANSWER_PLAYEPISODE;
                        fader.setFade(10, 1.0f, 0.06f);
                        doExit=true;
                        menu.resetState();
                    }
                }
            break;
            case menu_playlevel:
                if(value=="nolevel")
                {
                    //do nothing!
                    menu.resetState();
                }
                else
                {
                    result_level.levelfile = value;
                    ret = ANSWER_PLAYLEVEL;
                    fader.setFade(10, 1.0f, 0.06f);
                    doExit=true;
                    menu.resetState();
                }
            break;
            case menu_playlevel_wait:
            case menu_playbattle_wait:
            case menu_playepisode_wait:
                {
                    menu.resetState();
                }
            break;
            case menu_options:
                if(value=="tests")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_tests);
                }
                else
                if(value=="testboxes")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_testboxes);
                }
                else
                if(value=="controls")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_controls);
                }
                else
                if(value=="videosetup")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_videosettings);
                }
                else
                {
                    PGE_MsgBox msgBox(this, QString("Dummy"),
                                      PGE_MsgBox::msg_warn);
                    msgBox.exec();
                    menu.resetState();
                }
            break;
            case menu_controls:
                if(value=="control_plr1")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_controls_plr1);
                }
                else
                if(value=="control_plr2")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_controls_plr2);
                }
            break;
            case menu_controls_plr1:

            break;
            case menu_controls_plr2:

            break;
            case menu_tests:
                if(value=="credits")
                {
                    ret = ANSWER_CREDITS;
                    doExit=true;
                }
                else
                if(value=="loading")
                {
                    ret = ANSWER_LOADING;
                    doExit=true;
                }
                else
                if(value=="gameover")
                {
                    ret = ANSWER_GAMEOVER;
                    doExit=true;
                }
            break;
            case menu_testboxes:
                if(value=="messagebox")
                {
                    PGE_MsgBox msg(this, "This is a small message box without texture\nЭто маленкая коробочка-сообщение без текстуры", PGE_BoxBase::msg_info_light);
                    msg.exec();

                    PGE_MsgBox msg2(this, "This is a small message box with texture\nЭто маленкая коробочка-сообщение с текстурой", PGE_BoxBase::msg_info, PGE_Point(-1,-1),
                                    ConfigManager::setup_message_box.box_padding,
                                    ConfigManager::setup_message_box.sprite);
                    msg2.exec();
                    menu.resetState();
                }
                else
                if(value=="menubox")
                {
                    PGE_MenuBox menubox(this, "Select something", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                                         ConfigManager::setup_menu_box.box_padding,
                                         ConfigManager::setup_message_box.sprite);
                    QStringList items;
                    items<<"Menuitem 1";
                    items<<"Menuitem 2";
                    items<<"Menuitem 3";
                    items<<"Menuitem 4";
                    items<<"Menuitem 5";
                    items<<"Menuitem 6";
                    items<<"Menuitem 7";
                    items<<"Menuitem 8";
                    items<<"Menuitem 9";
                    items<<"Menuitem 10";
                    items<<"Menuitem 11";
                    menubox.addMenuItems(items);
                    menubox.setRejectSnd(obj_sound_role::MenuPause);
                    menubox.setMaxMenuItems(5);
                    menubox.exec();

                    if(menubox.answer()>=0)
                    {
                        PGE_MsgBox msg(this, "Your answer is:\n"+items[menubox.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1,-1),
                                ConfigManager::setup_message_box.box_padding,
                                ConfigManager::setup_message_box.sprite);
                        msg.exec();
                    }

                    menu.resetState();
                }
                else
                if(value=="inputbox")
                {
                    PGE_TextInputBox text(this, "Type a text", PGE_BoxBase::msg_info_light,
                                         PGE_Point(-1,-1),
                                          ConfigManager::setup_message_box.box_padding,
                                          ConfigManager::setup_message_box.sprite);
                    text.exec();

                    PGE_MsgBox msg(this, "Typed a text:\n"+text.inputText(), PGE_BoxBase::msg_info_light);
                    msg.exec();
                    menu.resetState();
                }
                else if (value=="questionbox")
                {
                    PGE_QuestionBox hor(this, "AHHHH?", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                                        ConfigManager::setup_menu_box.box_padding,
                                        ConfigManager::setup_message_box.sprite);
                    QStringList items;
                    items<<"One";
                    items<<"Two";
                    items<<"Three";
                    items<<"Four";
                    items<<"Five";
                    items<<"Six";
                    items<<"Seven";
                    hor.addMenuItems(items);
                    hor.setRejectSnd(obj_sound_role::BlockSmashed);
                    hor.exec();
                    if(hor.answer()>=0)
                    {
                        PGE_MsgBox msg(this, "Answer on so dumb question is:\n"+items[hor.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1,-1),
                                ConfigManager::setup_message_box.box_padding,
                                ConfigManager::setup_message_box.sprite);
                        msg.exec();
                    }
                    menu.resetState();
                }
            break;
            case menu_dummy_and_big:
                menu.resetState();
                break;
        default:
            break;

        }
    }
    else
    {
        switch(_currentMenu)
        {
            case menu_main:
                menu.reset();
                menu.setCurrentItem(4);
            break;
            case menu_options:
            AppSettings.apply();
            AppSettings.save();
            PGE_Audio::playSoundByRole(obj_sound_role::Bonus1up);
        default:
            if(menu.isKeyGrabbing())
                menu.reset();
            else
            if(menuChain.size()>0)
            {
                menu.reset();
                setMenu((CurrentMenu)menuChain.pop());
            }
            break;
        }
    }
}


void TitleScene::setMenu(TitleScene::CurrentMenu _menu)
{
    if(_menu<menuFirst) return;
    if(_menu>menuLast) return;

    _currentMenu=_menu;
    menu.clear();
    menu.setTextLenLimit(22);
    switch(_menu)
    {
        case menu_main:
            menu.setPos(300, 350);
            menu.setItemsNumber(5);
            menu.addMenuItem("game1p", "1 Player Game");
            menu.addMenuItem("game2p", "2 Player Game");
            menu.addMenuItem("playlevel", "Play level");
            menu.addMenuItem("Options", "Options");
            menu.addMenuItem("Exit", "Exit");
        break;
            case menu_options:
                menu.setPos(260,284);
                menu.setItemsNumber(9);
                menu.addMenuItem("tests", "Test of screens");
                menu.addMenuItem("testboxes", "Test of message boxes");
                menu.addMenuItem("controls", "Player controlling");
                menu.addMenuItem("videosetup", "Video settings");
                menu.addIntMenuItem(&AppSettings.volume_music, 0, 128, "vlm_music", "Music volume", false,
                                    []()->void{ PGE_MusPlayer::MUS_changeVolume(AppSettings.volume_music); });
                menu.addIntMenuItem(&AppSettings.volume_sound, 0, 128, "vlm_sound", "Sound volume", false);
                menu.addBoolMenuItem(&AppSettings.fullScreen, "full_screen", "Full Screen mode",
                                     []()->void{ PGE_Window::setFullScreen(AppSettings.fullScreen); }
                                     );
            break;
                case menu_tests:
                    menu.setPos(300, 350);
                    menu.setItemsNumber(5);
                    menu.addMenuItem("credits", "Credits");
                    menu.addMenuItem("loading", "Loading screen");
                    menu.addMenuItem("gameover", "Game over screen");
                break;
                case menu_testboxes:
                    menu.setPos(300, 350);
                    menu.setItemsNumber(5);
                    menu.addMenuItem("messagebox", "Message box");
                    menu.addMenuItem("menubox", "Menu box");
                    menu.addMenuItem("inputbox", "Text Input box");
                    menu.addMenuItem("questionbox", "Question box");
                break;
                    case menu_videosettings:
                        menu.setPos(300, 350);
                        menu.setItemsNumber(5);
                        menu.addBoolMenuItem(&AppSettings.showDebugInfo, "dbg_flag", "Show debug info");
                        menu.addBoolMenuItem(&AppSettings.frameSkip, "frame_skip", "Enable frame-skip");
                        menu.addBoolMenuItem(&AppSettings.vsync, "vsync", "Enable V-Sync",
                                                 [this]()->void{
                                                     PGE_Window::vsync=AppSettings.vsync;
                                                     PGE_Window::toggleVSync(AppSettings.vsync);
                                                     AppSettings.timeOfFrame=PGE_Window::TimeOfFrame;
                                                 }
                                             );
                        //menu.addIntMenuItem(&AppSettings.MaxFPS, 65, 1000, "max_fps", "Max FPS");
                        menu.addIntMenuItem(&AppSettings.timeOfFrame, 2, 16, "phys_step", "Frame time (ms.)", false,
                                            [this]()->void{
                                                if(!PGE_Window::vsync)
                                                {
                                                    PGE_Window::TicksPerSecond=1000.0f/AppSettings.timeOfFrame;
                                                    PGE_Window::TimeOfFrame=AppSettings.timeOfFrame;
                                                    AppSettings.TicksPerSecond=1000.0f/AppSettings.timeOfFrame;
                                                    //PGE_Window::TicksPerSecond =AppSettings.TicksPerSecond;
                                                    this->updateTickValue();
                                                } else {
                                                    AppSettings.timeOfFrame=PGE_Window::TimeOfFrame;
                                                }
                                            }
                                            );
                    break;
                    case menu_controls:
                        menu.setPos(300, 350);
                        menu.setItemsNumber(5);
                        menu.addMenuItem("control_plr1", "Player 1 controls");
                        menu.addMenuItem("control_plr2", "Player 2 controls");
                    break;
                        case menu_controls_plr1:
                        case menu_controls_plr2:
                        {

                        KeyMap *mp_p;
                        int *mct_p=0;
                        SDL_Joystick* jdev=NULL;
                        std::function<void()> ctrlSwitch;

                        if(_menu==menu_controls_plr1)
                        {
                            ctrlSwitch = [this]()->void{
                                setMenu(menu_controls_plr1);
                                };
                            mct_p = &AppSettings.player1_controller;
                            if((*mct_p>=0)&&(*mct_p<AppSettings.player1_joysticks.size()))
                            {
                                if(*mct_p<AppSettings.joysticks.size())
                                    jdev        = AppSettings.joysticks[*mct_p];
                                mp_p         = &AppSettings.player1_joysticks[*mct_p];
                            }
                            else
                                mp_p = &AppSettings.player1_keyboard;
                        }
                        else{
                            ctrlSwitch = [this]()->void{
                                setMenu(menu_controls_plr2);
                                };
                            mct_p  = &AppSettings.player2_controller;
                            if((*mct_p>=0)&&(*mct_p<AppSettings.player2_joysticks.size()))
                            {
                                if(*mct_p<AppSettings.joysticks.size())
                                    jdev        = AppSettings.joysticks[*mct_p];
                                mp_p = &AppSettings.player2_joysticks[*mct_p];
                            }
                            else
                                mp_p = &AppSettings.player2_keyboard;
                        }

                            menu.setPos(300, 216);
                            menu.setItemsNumber(11);
                            QList<NamedIntItem> ctrls;
                            NamedIntItem controller;
                            controller.value=-1;
                            controller.label="Keyboard";
                            ctrls.push_back(controller);
                            for(int i=0;i<AppSettings.joysticks.size();i++)
                            {
                                controller.value=i;
                                controller.label=QString("Joystick: %1").arg(SDL_JoystickName(AppSettings.joysticks[i]));
                                ctrls.push_back(controller);
                            }
                            menu.addNamedIntMenuItem(mct_p, ctrls, "ctrl_type", "Input:", true, ctrlSwitch);
                            menu.setItemWidth(300);
                            menu.setValueOffset(150);
                            menu.addKeyGrabMenuItem(&mp_p->left, "key1",        "Left.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->right, "key2",       "Right........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->up, "key3",          "Up...........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->down, "key4",        "Down.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->jump, "key5",        "Jump.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->jump_alt, "key6",    "Alt-Jump....", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->run, "key7",         "Run..........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->run_alt, "key8",     "Alt-Run.....", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->drop, "key9",        "Drop.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->start, "key10",      "Start........", jdev);
                            menu.setValueOffset(210);
                        }
                        break;
        case menu_playepisode_wait:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                menu.addMenuItem("waitinginprocess", "Please wait...");
                filefind_finished=false;
                filefind_folder=ConfigManager::dirs.worlds;
                filefind_thread=SDL_CreateThread( findEpisodes, "EpisodeFinderThread", NULL);
            }
            break;
        case menu_playepisode:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                //Build list of episodes
                for(int i=0;i<filefind_found_files.size();i++)
                    menu.addMenuItem(filefind_found_files[i].first, filefind_found_files[i].second);
                menu.sort();
            }
        break;
        case menu_playlevel_wait:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                menu.addMenuItem("waitinginprocess", "Please wait...");
                filefind_finished=false;
                filefind_folder=ConfigManager::dirs.worlds;
                filefind_thread=SDL_CreateThread( findLevels, "LevelFinderThread", NULL);
            }
        break;
        case menu_playlevel:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                //Build list of episodes
                for(int i=0;i<filefind_found_files.size();i++)
                    menu.addMenuItem(filefind_found_files[i].first, filefind_found_files[i].second);
                menu.sort();
            }
        break;
    default:
        break;
    }
    PGE_Rect menuBox = menu.rect();
    menu.setPos(PGE_Window::Width/2-menuBox.width()/2, menuBox.y());
    qDebug()<<"Menuitem ID: "<<menustates[_menu].first << ", scrolling offset: "<<menustates[_menu].second;
    menu.setCurrentItem(menustates[_menu].first);
    menu.setOffset(menustates[_menu].second);
}




int TitleScene::findEpisodes( void* )
{
    filefind_found_files.clear();
    QDir worlddir(filefind_folder);
    QStringList filter;
    filter << "*.wld" << "*.wldx";
    QStringList files;
    QStringList folders = worlddir.entryList(QDir::Dirs);

    foreach(QString folder, folders)
    {
        QString path = filefind_folder+folder;
        QDir episodedir(path);
        QStringList worlds = episodedir.entryList(filter);
        foreach(QString world, worlds)
        {
            files << filefind_folder+folder+"/"+world;
        }
    }

    if(files.isEmpty())
    {
        QPair<QString,QString > file;
        file.first = "noworlds";
        file.second = "<episodes not found>";
        filefind_found_files.push_back(file);
    }
    else
    {
        foreach(QString filename, files)
        {
            WorldData world = FileFormats::OpenWorldFileHeader(filename);
            if(world.ReadFileValid)
            {
                QString title = world.EpisodeTitle;
                QPair<QString,QString > file;
                file.first = filename;
                file.second = (title.isEmpty()?QFileInfo(filename).fileName():title);
                filefind_found_files.push_back(file);
            }
        }
    }
    filefind_finished=true;
    return 0;
}

int TitleScene::findLevels( void* )
{
    //Build list of casual levels
    QDir leveldir(filefind_folder);
    QStringList filter;
    filter<<"*.lvl" << "*.lvlx";
    QStringList files = leveldir.entryList(filter);

    filefind_found_files.clear();//Clean up old stuff

    if(files.isEmpty())
    {
        QPair<QString,QString > file;
        file.first = "noworlds";
        file.second = "<episodes not found>";
        filefind_found_files.push_back(file);
    }
    else
    {
        foreach(QString file, files)
        {
            LevelData level = FileFormats::OpenLevelFileHeader(filefind_folder+file);
            if(level.ReadFileValid)
            {
                QString title = level.LevelName;
                QPair<QString,QString > filex;
                filex.first = filefind_folder+file;
                filex.second = (title.isEmpty()?file:title);
                filefind_found_files.push_back(filex);
                //QString title = level.LevelName;
                //menu.addMenuItem(filefind_folder+file, (title.isEmpty()?file:title));
            }
        }
    }

    filefind_finished=true;

    return 0;
}

