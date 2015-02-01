/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "file_formats.h"
#include "file_strlist.h"
#include "pge_x.h"

#ifdef PGE_EDITOR
#include <script/commands/memorycommand.h>
#endif

MetaData FileFormats::ReadNonSMBX64MetaData(QString RawData, QString filePath)
{
    int str_count=0;        //Line Counter
    QString line;           //Current Line data


    MetaData FileData;
    #ifdef PGE_EDITOR
    FileData.script = new ScriptHolder();
    #endif

    QString errorString;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEFile pgeX_Data(RawData);
    if( !pgeX_Data.buildTreeFromRaw() )
    {
        errorString = pgeX_Data.lastError();
        goto badfile;
    }

    for(int section=0; section<pgeX_Data.dataTree.size(); section++) //look sections
    {
        if(pgeX_Data.dataTree[section].name=="JOKES")
        {
            #ifndef PGE_ENGINE
            if(!pgeX_Data.dataTree[section].data.isEmpty())
                if(!pgeX_Data.dataTree[section].data[0].values.isEmpty())
                    QMessageBox::information(nullptr, "Jokes",
                            pgeX_Data.dataTree[section].data[0].values[0].value,
                            QMessageBox::Ok);
            #endif
        }
        else
        if(pgeX_Data.dataTree[section].name=="META_BOOKMARKS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }

                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];

                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName = "";
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                      if(v.marker=="BM") //Bookmark name
                      {
                          if(PGEFile::IsQStr(v.value))
                              meta_bookmark.bookmarkName = PGEFile::X2STR(v.value);
                          else
                              goto badfile;
                      }
                      else
                      if(v.marker=="X") // Position X
                      {
                          if(PGEFile::IsIntS(v.value))
                              meta_bookmark.x = v.value.toInt();
                          else
                              goto badfile;
                      }
                      else
                      if(v.marker=="Y") //Position Y
                      {
                          if(PGEFile::IsIntS(v.value))
                              meta_bookmark.y = v.value.toInt();
                          else
                              goto badfile;
                      }
                }
                FileData.bookmarks.push_back(meta_bookmark);
            }
        }
        #ifdef PGE_EDITOR
        else
        if(pgeX_Data.dataTree[section].name=="META_SCRIPT_EVENTS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").
                            arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            if(pgeX_Data.dataTree[section].subTree.size()>0)
            {
                if(!FileData.script)
                    FileData.script = new ScriptHolder();
            }

            //Read subtree
            for(int subtree=0;subtree<pgeX_Data.dataTree[section].subTree.size();subtree++)
            {
                if(pgeX_Data.dataTree[section].subTree[subtree].name=="EVENT")
                {
                    if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
                    {
                        errorString=QString("Wrong section data syntax:\nSection [%1]\nSubtree [%2]").
                                    arg(pgeX_Data.dataTree[section].name).
                                    arg(pgeX_Data.dataTree[section].subTree[subtree].name);
                        goto badfile;
                    }

                    EventCommand * event = new EventCommand(EventCommand::EVENTTYPE_LOAD);

                    for(int sdata=0;sdata<pgeX_Data.dataTree[section].subTree[subtree].data.size();sdata++)
                    {
                        if(pgeX_Data.dataTree[section].subTree[subtree].data[sdata].type!=PGEFile::PGEX_Struct)
                        {
                            errorString=QString("Wrong data item syntax:\nSubtree [%1]\nData line %2")
                                    .arg(pgeX_Data.dataTree[section].subTree[subtree].name)
                                    .arg(sdata);
                            goto badfile;
                        }

                        PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].subTree[subtree].data[sdata];

                        //Get values
                        for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                        {
                            PGEFile::PGEX_Val v = x.values[sval];
                            errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                                    .arg(pgeX_Data.dataTree[section].name)
                                    .arg(sdata)
                                    .arg(v.marker)
                                    .arg(v.value);

                            if(v.marker=="TL") //Marker
                            {
                                if(PGEFile::IsQStr(v.value))
                                    event->setMarker(PGEFile::X2STR(v.value));
                                else
                                    goto badfile;
                            }
                            else
                            if(v.marker=="ET") //Event type
                            {
                                if(PGEFile::IsIntS(v.value))
                                    event->setEventType( (EventCommand::EventType)v.value.toInt() );
                                else
                                    goto badfile;
                            }
                        }
                    }//Event header

                    //Basic commands subtree
                    if(event->eventType()==EventCommand::EVENTTYPE_LOAD)
                    {
                        for(int subtree2=0;subtree2<pgeX_Data.dataTree[section].subTree[subtree].subTree.size();subtree2++)
                        {
                            if(pgeX_Data.dataTree[section].subTree[subtree2].subTree[subtree2].name=="BASIC_COMMANDS")
                            {
                                if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
                                {
                                    errorString=QString("Wrong section data syntax:\nSection [%1]\nSubtree [%2]\nSubtree [%2]").
                                                arg(pgeX_Data.dataTree[section].name).
                                                arg(pgeX_Data.dataTree[section].subTree[subtree].name).
                                                arg(pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].name);
                                    goto badfile;
                                }

                                for(int sdata=0;sdata<pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].data.size();sdata++)
                                {
                                    if(pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].data[sdata].type!=PGEFile::PGEX_Struct)
                                    {
                                        errorString=QString("Wrong data item syntax:\nSubtree [%1]\nData line %2")
                                                .arg(pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].name)
                                                .arg(sdata);
                                        goto badfile;
                                    }

                                    PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].data[sdata];

                                    QString name="";
                                    QString commandType="";
                                    int hx=0;
                                    int ft=0;
                                    double vf=0.0;

                                    //Get values
                                    for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                                    {
                                        PGEFile::PGEX_Val v = x.values[sval];
                                        errorString=QString("Wrong value syntax\nSubtree [%1]\nData line %2\nMarker %3\nValue %4")
                                                .arg(pgeX_Data.dataTree[section].subTree[subtree2].name)
                                                .arg(sdata)
                                                .arg(v.marker)
                                                .arg(v.value);

                                        if(v.marker=="N") //Command name
                                        {
                                            if(PGEFile::IsQStr(v.value))
                                                name = PGEFile::X2STR(v.value);
                                            else
                                                goto badfile;
                                        }
                                        else
                                        if(v.marker=="CT") //Command type
                                        {
                                            if(PGEFile::IsQStr(v.value))
                                                commandType = PGEFile::X2STR(v.value);
                                            else
                                                goto badfile;
                                        }


                                        //MemoryCommand specific values
                                        else
                                        if(v.marker=="HX") //Heximal value
                                        {
                                            if(PGEFile::IsIntS(v.value))
                                                hx = v.value.toInt();
                                            else
                                                goto badfile;
                                        }
                                        else
                                        if(v.marker=="FT") //Field type
                                        {
                                            if(PGEFile::IsIntS(v.value))
                                                ft = v.value.toInt();
                                            else
                                                goto badfile;
                                        }
                                        else
                                        if(v.marker=="V") //Value
                                        {
                                            if(PGEFile::IsFloat(v.value))
                                                vf = v.value.toDouble();
                                            else
                                                goto badfile;
                                        }
                                    }

                                    if(commandType=="MEMORY")
                                    {
                                        MemoryCommand *cmd =
                                           new MemoryCommand(hx,(MemoryCommand::FieldType)ft, vf);
                                        cmd->setMarker(name);
                                        event->addBasicCommand(cmd);
                                    }
                                }//commands list
                            }//Basic command subtree
                        }//subtrees
                    }

                    FileData.script->revents() << event;
                }//EVENT tree

            }//META_SCRIPT_EVENTS subtree
        }//META_SCRIPT_EVENTS Section
        #endif
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////

    errorString.clear(); //If no errors, clear string;
    return FileData;

    badfile:    //If file format is not correct
    BadFileMsg(filePath+"\nError message: "+errorString, str_count, line);
    FileData.bookmarks.clear();

    return FileData;
}


QString FileFormats::WriteNonSMBX64MetaData(MetaData metaData)
{
    QString TextData;
    long i;

    //Bookmarks
    if(!metaData.bookmarks.isEmpty())
    {
        TextData += "META_BOOKMARKS\n";
        for(i=0;i<metaData.bookmarks.size(); i++)
        {
            //Bookmark name
            TextData += PGEFile::value("BM", PGEFile::qStrS(metaData.bookmarks[i].bookmarkName));
            TextData += PGEFile::value("X", PGEFile::IntS(metaData.bookmarks[i].x));
            TextData += PGEFile::value("Y", PGEFile::IntS(metaData.bookmarks[i].y));
            TextData += "\n";
        }
        TextData += "META_BOOKMARKS_END\n";

        #ifdef PGE_EDITOR
        if(metaData.script)
        {
            if(!metaData.script->events().isEmpty())
            {
                TextData += "META_SCRIPT_EVENTS\n";
                foreach(EventCommand* x, metaData.script->events())
                {
                    TextData += "EVENT\n";
                    if(!x->marker().isEmpty())
                        TextData += PGEFile::value("TL", PGEFile::qStrS( x->marker() ) );
                    TextData += PGEFile::value("ET", PGEFile::IntS( (int)x->eventType() ) );
                    TextData += "\n";

                    if(x->basicCommands().size()>0)
                    {
                        TextData += "BASIC_COMMANDS\n";
                        foreach(BasicCommand *y, x->basicCommands())
                        {
                            TextData += PGEFile::value("N", PGEFile::qStrS( y->marker() ) );
                            if(QString(y->metaObject()->className())=="MemoryCommand")
                            {
                                MemoryCommand *z = dynamic_cast<MemoryCommand*>(y);
                                TextData += PGEFile::value("CT", PGEFile::qStrS( "MEMORY" ) );
                                TextData += PGEFile::value("HX", PGEFile::IntS( z->hexValue() ) );
                                TextData += PGEFile::value("FT", PGEFile::IntS( (int)z->fieldType() ) );
                                TextData += PGEFile::value("V", PGEFile::FloatS( z->getValue() ) );
                            }
                            TextData += "\n";
                        }
                        TextData += "BASIC_COMMANDS_END\n";
                    }
                    TextData += "EVENT_END\n";
                }
                TextData += "META_SCRIPT_EVENTS_END\n";
            }
        }
        #endif
    }
    return TextData;
}
